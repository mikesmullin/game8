import { glob } from 'glob'
import cbFs from 'fs';
import fs from 'fs/promises';
import { createReadStream } from 'fs';
import path from 'path';
import { spawn } from 'child_process';
import { fileURLToPath } from 'url';
import chalk from 'chalk';
import readline from 'readline';

const isWin = process.platform === "win32";
const isMac = process.platform === "darwin";
const isNix = process.platform === "linux";
const __dirname = path.dirname(fileURLToPath(import.meta.url));
const SDHC_PATH = path.join('vendor', 'sokol-tools-bin', 'bin', 'win32', 'sokol-shdc.exe'); // TODO: cross-platform support
const BUILD_PATH = "build";
const C_COMPILER_PATH = 'clang';
const EMCC_COMPILER_PATH = 'emcc.bat';
const EMRUN_PATH = 'emrun.bat';
const OUT_FILE = "compile_commands.json";
const workspaceFolder = path.join(__dirname, '..');
const absBuild = (...args) => path.join(workspaceFolder, BUILD_PATH, ...args);
const absWs = (...args) => path.join(workspaceFolder, ...args);
const relBuild = (...args) => path.relative(path.join(workspaceFolder, BUILD_PATH), path.join(...args));
const relWs = (...args) => path.relative(path.join(workspaceFolder), path.join(...args));
const repeat = (n, s) => new Array(n).fill(s).join('');
const indent = (n, s) => s.replace(/^/gm, repeat(n, '  '));
const isEmpty = (v) => v == null || v == undefined || ('string' == typeof v && '' == v.trim());
const Promise__allBatch = async (a, parallel = 48) => {
  for (let i = 0; i < a.length;) {
    let ps = [];
    for (let j = 0; j < parallel && i < a.length; i++, j++) {
      let t = a[i];
      ps.push(t());
    }
    await Promise.all(ps);
  }
};
const DEBUG_COMPILER_ARGS = [
  '-O0',
  // export debug symbols (x86dbg understands both; turn these on when debugging, leave off for faster compile)
  // '-gdwarf', // DWARF (GDB / Linux compatible)
  '-g', /* '-gcodeview',*/ // CodeView (PDB / windbg / Windows compatible)
];
const C_COMPILER_ARGS = [
  // '-m64', // generate 64-bit executable

  // ignore specific warnings
  // '-Wno-microsoft-enum-forward-reference',
  // '-Wno-deprecated-non-prototype',
  '@clang_options.rsp',
];
if (!isWin) C_COMPILER_ARGS.push('-pthread');
const C_COMPILER_INCLUDES = [
];
const C_ENGINE_COMPILER_FLAGS = [
  '-DSOKOL_GLCORE', // use GL backend
];
const C_DLL_COMPILER_FLAGS = [
  '-DENGINE__COMPILING_DLL',
];
const C_WEB_COMPILER_FLAGS = [
  '-DSOKOL_GLES3', // use GLES3 backend
  '-sUSE_WEBGL2', // use WebGL2
  // see: https://emscripten.org/docs/tools_reference/settings_reference.html?highlight=environment
  '-sTOTAL_MEMORY=512MB', // allow larger memory pool (16MB is default)
  '-sSTACK_OVERFLOW_CHECK=2', // check for stack overflow
  '-sSTACK_SIZE=5MB',
  // see: https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html
  '-sEXPORTED_FUNCTIONS=_main,_libwebsocket_cb',
  '-sEXPORTED_RUNTIME_METHODS=cwrap', // ,ccall
];

const LINKER_LIBS = [];
// if (isWin) {
//   LINKER_LIBS.push('-l', 'Ws2_32.lib');
// }
const LINKER_LIB_PATHS = [];
const COMPILER_TRANSLATION_UNITS = [
  relWs(workspaceFolder, 'src', '**', '*.c'),
];
const COMPILER_TRANSLATION_UNITS_DLL = [
  relWs(workspaceFolder, 'src', '**', '*.c'),
];
const COMPILER_TRANSLATION_UNITS_WEB = [
  relWs(workspaceFolder, 'src', '**', '*.c'),
];
const COMPILER_TRANSLATION_UNITS_WEB_EXCLUDE = [
];
const COMPILER_TRANSLATION_UNITS_WEB_COPY = [
  relWs(workspaceFolder, 'assets', 'web', '*'),
];
const COMPILER_TRANSLATION_UNITS_UNIT_TESTS = [
  relWs(workspaceFolder, 'test', 'unit', '**', '*.c'),
];
const COMPILER_TRANSLATION_UNITS_INTEGRATION_TESTS = [
  relWs(workspaceFolder, 'test', 'integration', '**', 'app.c'),
];
// const COMPILER_TRANSLATION_UNITS_LOGIC_C = [
//   relWs(workspaceFolder, 'src', '**', 'Logic.c'),
//   relWs(workspaceFolder, 'test', 'integration', '**', 'Logic.c'),
// ];

const EDITOR_INCLUDES = [
  // `-I${relBuild(workspaceFolder, 'vendor', 'emsdk', 'upstream', 'emscripten', 'cache', 'sysroot', 'include')}`,
];

const DEFAULT_LOGIC_C = 'src/game/Logic.c';
const DEFAULT_LOGIC_DLL = 'tmp/Logic.dll';

const nixPath = (p) =>
  path.posix.normalize(p.replace(/\\/g, '/'));
const winPath = (p) =>
  path.posix.normalize(p.replace(/\//g, '\\\\'));

const generate_clangd_compile_commands = async () => {
  console.log('scanning directory...');
  const compile_commands = [];
  const unit_files = await glob('{src,tests}/**/*.c');
  console.debug('unit_files: ', unit_files);
  for (const unit_file of unit_files) {
    compile_commands.push({
      directory: path.join(workspaceFolder),
      arguments: [
        C_COMPILER_PATH,
        ...DEBUG_COMPILER_ARGS,
        ...C_COMPILER_ARGS,
        // ...C_COMPILER_INCLUDES,
        ...C_ENGINE_COMPILER_FLAGS,
        ...EDITOR_INCLUDES,
        '-c',
        '-o', `${unit_file}.o`,
        relWs(unit_file),
      ],
      file: relWs(workspaceFolder, unit_file),
    });
  }

  console.log(`writing ${OUT_FILE} ...`)
  await fs.writeFile(OUT_FILE, JSON.stringify(compile_commands, null, 2));

  console.log('done making.');
};

const child_spawn = async (cmd, args = [], opts = {}) => {
  const invocation = `${opts.stdin ? `type ${opts.stdin} | ` : ''}${cmd} ${args.join(' ')}${opts.stdout ? ` > ${opts.stdout}` : ''}`;
  if (opts.printCmd || !opts.buffer) {
    console.log(chalk.gray(indent(1, invocation)));
  }
  let buf = false, outBuf = '', errBuf = '';
  let stdio = ['inherit', 'pipe', 'pipe'];
  if (opts.buffer) {
    buf = true;
    delete opts.buffer;
    stdio = ['ignore', 'pipe', 'pipe'];
  }
  const cwd = opts.cwd || process.cwd();
  const child = opts.child = spawn(cmd, args, { stdio, cwd });
  opts.kill = (signal) => {
    console.log(chalk[opts.color](opts.prefix + ' * ') + 'sending SIGTERM');
    child.kill();
  };
  child.stdout.setEncoding('utf8');
  child.stdout.on('data', (data) => {
    outBuf += data.toString();
    if (!buf) {
      outBuf = outBuf.replace(/(.+\r?\n)/g, (m, m1) => {
        process.stdout.write(chalk[opts.color](opts.prefix + ' | ') + m1);
        return '';
      });
    }
  });
  child.stderr.setEncoding('utf8');
  child.stderr.on('data', (data) => {
    errBuf += data.toString();
    if (!buf) {
      errBuf = errBuf.replace(/(.+\r?\n)/g, (m, m1) => {
        process.stderr.write(chalk[opts.color](opts.prefix + ' | ') + m1);
        return '';
      });
    }
  });
  const code = await new Promise((ok) => {
    child.on('close', (code) => {
      if (code !== 0) {
        if (!buf) {
          console.log(`process exited with code ${code}`);
        }
      }
      ok(code);
    });
  });
  return { cmd: invocation, code, stdout: outBuf, stderr: errBuf };
};

const all = async (autorun) => {
  // console.log("--clean--");
  // await clean();
  await copy_dlls();
  console.log('--shaders--');
  const code1 = await shaders('hlsl5:glsl430');
  if (0 != code1) return code1;
  console.log('--main--');
  const code2 = await compile('main');
  if (0 != code2) return code2;
  console.log('--dll--');
  const code3 = await compile_reload(DEFAULT_LOGIC_C, DEFAULT_LOGIC_DLL);
  if (0 != code3) return code3;
  if (autorun) await run('main');
};

const web = async () => {
  // console.log("--clean--");
  // await clean();
  await copy_dlls();
  const code1 = await shaders('glsl300es');
  if (0 != code1) return code1;
  // for these to work,
  // you may need to enter the python emsdk env first (sets env vars)
  // $ cd vendor/emsdk
  // $ emsdk_env
  const code2 = await compile_web('main');
  if (0 != code2) return code2;
  await run_web('main');
};

const copy_dlls = async () => {
  const srcs = [];
  const dest = path.join(workspaceFolder, BUILD_PATH);
  for (const src of srcs) {
    await fs.copyFile(src, path.join(dest, path.basename(src)));
  }
};

const shaders = async (out_type) => {
  const shaderFiles = await glob(nixPath(path.join(workspaceFolder, 'assets', 'shaders', '*.glsl')));
  const ps = [];
  let code = 0;
  for (const shaderFile of shaderFiles) {
    const relPath = path.relative(path.join(workspaceFolder), shaderFile);
    const h_file = `${relPath}.h`;
    const { exists: glsl_exists, stat: { mtime: glsl_mtime } } = await fs_exists(relPath);
    const { exists: h_exists, stat: { mtime: h_mtime } } = await fs_exists(h_file);
    if (glsl_mtime > h_mtime) {
      ps.push(() =>
        child_spawn(SDHC_PATH, ['-i',
          relPath, '-o', h_file,
          '-l', out_type]).then((r) => {
            if (0 !== r.code) code = r.code;
          }));
    }
  }
  await Promise__allBatch(ps);
  return code;
};

const clean = async () => {
  // TODO: selective clean
  await fs.rm(path.join(workspaceFolder, BUILD_PATH), { recursive: true, force: true });
  await fs.mkdir(path.join(workspaceFolder, BUILD_PATH));

  const shaderFiles = await glob(nixPath(path.join(workspaceFolder, 'assets', 'shaders', '*.glsl.h')));
  for (const shaderFile of shaderFiles) {
    await fs.rm(shaderFile, { force: true });
  }
};

const logic_hotreload_variants = (mode) => (unit) => {
  if ('main' == mode) {
    unit = unit.replace(/Logic.o$/, 'Logic_main.o');
  }
  else if ('dll' == mode) {
    unit = unit.replace(/Logic.o$/, 'Logic_dll.o');
  }
  return unit;
}


const compile = async (basename) => {
  const started = performance.now();
  console.log(`compiling ${basename}...`);
  const unit = 'src/app.c';
  let success = true;
  let code = null;


  // discover included translation unit files
  // ie. we have a naming convention .h may have a matching .c
  const object_files = [], ps = [];
  for await (const m of extract_include_units({ file: unit })) {
    if (m.h_file) continue;
    ps.push(() =>
      recompile_object(relWs(workspaceFolder, m.c_file))
        .then((r) => {
          if (0 != r.r?.code) {
            code = r.r?.code;
            success = false;
          }
          object_files.push(r.object);
        }));
  }
  await Promise__allBatch(ps);

  const executable = relWs(workspaceFolder, BUILD_PATH, `${basename}${isWin ? '.exe' : ''}`);
  if (success) {
    // compile and link in one step
    const r1 = await child_spawn(C_COMPILER_PATH, [
      ...DEBUG_COMPILER_ARGS,
      ...C_COMPILER_ARGS,
      ...C_ENGINE_COMPILER_FLAGS,
      // ...C_COMPILER_INCLUDES,
      ...LINKER_LIBS,
      ...LINKER_LIB_PATHS,
      ...object_files.map(
        logic_hotreload_variants('main'),
      ),
      unit,
      '-o', executable,
    ], { buffer: true });
    code = r1.code;
    success &= 0 == r1.code;
    console.log(chalk.grey(r1.cmd));
    if (r1.stdout) console.log(r1.stdout);
    if (r1.stderr) console.error(chalk.red(r1.stderr));
  }
  const ended = performance.now();

  const color = success ? 'green' : 'red';
  const log = success ? 'log' : 'error';
  console[log](chalk[color](`compiling ${basename} ${success ? 'succeeded' : 'failed'}. file: ${executable}, elapsed: ${((ended - started) / 1000).toFixed(2)}s`));

  return code;
};

function generateRandomString(length) {
  const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  let result = '';
  for (let i = 0; i < length; i++) {
    const randomIndex = Math.floor(Math.random() * chars.length);
    result += chars[randomIndex];
  }
  return result;
}

const compile_reload = async (unit, outname) => {
  const started = performance.now();
  console.log(`recompiling...`);

  await fs.mkdir(path.join(workspaceFolder, BUILD_PATH, 'tmp'), { recursive: true });

  await fs.mkdir(absBuild(path.dirname(unit)), { recursive: true });

  // discover included translation unit files
  // ie. we have a naming convention .h may have a matching .c
  const object_files = [], ps = [];
  for await (const m of extract_include_units({ file: unit })) {
    if (m.h_file) continue;
    ps.push(() =>
      recompile_object(relWs(workspaceFolder, m.c_file), 'dll', C_DLL_COMPILER_FLAGS)
        .then((r) => {
          object_files.push(r.object);
        }));
  }
  await Promise__allBatch(ps);

  // const src = relWs(workspaceFolder, unit);
  const target = outname;
  const dst = relWs(workspaceFolder, BUILD_PATH, target);

  // for (const tu of dsts) {
  //   const started = performance.now();
  //   await child_spawn(C_COMPILER_PATH, [
  //     ...DEBUG_COMPILER_ARGS,
  //     // '-ftime-report', // display compile time stats
  //     ...C_COMPILER_ARGS,
  //     // ...C_COMPILER_INCLUDES,
  //     // ...C_CONDITIONAL_COMPILER_ARGS(dsts.join(',')),
  //     // ...LINKER_LIBS,
  //     // ...LINKER_LIB_PATHS,
  //     // '-shared',
  //     // ...dsts.filter(s => !s.includes('.pb.')),
  //     '-c', tu
  //     // '-o', dst,
  //   ]);
  //   const ended = performance.now();
  //   console.log(`compile unit performance. file: ${tu}, elapsed: ${((ended - started) / 1000).toFixed(2)}s`);
  // }

  const ANALYZE = false;
  const ANALYZER = path.join(process.cwd(), 'ClangBuildAnalyzer.exe');
  const analyzer_out_path = path.join('build', 'src', 'game');
  const analyzer_bin = path.join('build', 'analysis.bin');
  if (ANALYZE) await child_spawn(ANALYZER, ['--start', analyzer_out_path]);

  const r1 = await child_spawn(C_COMPILER_PATH, [
    ...DEBUG_COMPILER_ARGS,
    // '-ftime-report', // display compile time stats
    ...(ANALYZE ? ['-ftime-trace'] : []), // display compile time stats
    ...C_COMPILER_ARGS,
    ...C_ENGINE_COMPILER_FLAGS,
    ...C_DLL_COMPILER_FLAGS,
    ...LINKER_LIBS,
    ...LINKER_LIB_PATHS,
    '-shared',
    ...object_files.map(
      logic_hotreload_variants('dll'),
    ),
    unit,
    '-o', dst,
  ], { buffer: true });
  const ended = performance.now();

  const success = 0 == r1.code;
  const color = success ? 'green' : 'red';
  const log = success ? 'log' : 'error';
  console.log(chalk.grey(r1.cmd));
  if (r1.stdout) console.log(r1.stdout);
  if (r1.stderr) console.error(chalk.red(r1.stderr));


  // TODO: on linux, to analyze with gcc
  // gcc -std=c11 -ftime-report -Wno-implicit-int ... -lm

  // TODO: `-ftime-trace` is JSON-based profiling outputs 
  // that can be loaded into Chrome's `chrome://tracing` visualizer

  if (ANALYZE) await child_spawn(ANALYZER, ['--stop', analyzer_out_path, analyzer_bin]);
  if (ANALYZE) await child_spawn(ANALYZER, ['--analyze', analyzer_bin]);

  // once .dll is finished writing to disk, copy it into place (so it will be loaded)
  let target2 = target.replace('tmp/', '');
  if (target != target2) {
    let out_target = path.join(workspaceFolder, BUILD_PATH, target);
    let out_target2 = path.join(workspaceFolder, BUILD_PATH, target2);
    try {
      await fs.rename(out_target, out_target2);
    } catch (e) {
      console.log(chalk.red('file move failed.'), e);
    }
  }

  console[log](chalk[color](`compiling DLL ${0 == r1.code ? 'succeeded' : 'failed'}. file: ${target2}, elapsed: ${((ended - started) / 1000).toFixed(2)}s`));
  return r1.code;
};

const watch = async (base = 'src') => {
  console.log(chalk.grey('watching ') + chalk.cyan(base + '/') + chalk.grey('...'));

  const logic_c = (await glob(nixPath(relWs(workspaceFolder, base, '**', 'Logic.c'))))[0];
  console.log(chalk.grey('found: ') + chalk.yellow(logic_c));

  const watcher = fs.watch(path.join(workspaceFolder, base), { recursive: true });
  let timer;
  let wait = false;
  for await (const event of watcher) {
    clearTimeout(timer);
    if (!wait) {
      timer = setTimeout(async () => {
        wait = true;
        console.debug(`${event.eventType} ${chalk.yellow(event.filename)}`);
        await compile_reload(logic_c, `tmp/${generateRandomString(16)}.dll`);
        wait = false;
      }, 250);
    }
  }
};

const all_loop = async () => {
  console.log(`watching...`);
  const watcher = fs.watch(path.join(workspaceFolder, 'src'), { recursive: true });
  let timer;
  let wait = false;
  for await (const event of watcher) {
    console.debug('event', event);
    clearTimeout(timer);
    if (!wait) {
      timer = setTimeout(async () => {
        wait = true;
        await all(false);
        wait = false;
      }, 250);
    }
  }
};

const run = async (basename, args = []) => {
  const executable = `${basename}${isWin ? '.exe' : ''}`;
  const exePath = path.join(workspaceFolder, BUILD_PATH, executable);

  try {
    await fs.stat(exePath);
  } catch (e) {
    console.log(".exe is missing. probably failed to compile.", e);
    return;
  }

  if (isNix || isMac) {
    // chmod +x
    await fs.chmod(exePath, 0o755);
  }

  await child_spawn(exePath, args, { cwd: absBuild() });
}

const compile_web = async (basename) => {
  console.log(`compiling w/ emscripten...`);

  const unit_files = [];
  let candidates, noncandidates;
  for (const u of COMPILER_TRANSLATION_UNITS_WEB) {
    candidates = await glob(nixPath(relWs(absWs(u))));
    nextFile:
    for (const file of candidates) {
      for (const nu of COMPILER_TRANSLATION_UNITS_WEB_EXCLUDE) {
        noncandidates = await glob(nixPath(relWs(absWs(nu))));
        for (const nonFile of noncandidates) {
          if (file == nonFile) continue nextFile;
        }
      }

      unit_files.push(file);
    }
  }

  // compile and link in one step
  const r1 = await child_spawn(EMCC_COMPILER_PATH, [
    ...DEBUG_COMPILER_ARGS,
    '-gsource-map',
    // ...C_COMPILER_ARGS,
    ...C_WEB_COMPILER_FLAGS,
    // ...C_COMPILER_INCLUDES,
    //...LINKER_LIBS,
    //...LINKER_LIB_PATHS,
    ...unit_files.map(unit => relWs(workspaceFolder, unit)),
    '-o', relWs(workspaceFolder, BUILD_PATH, `${basename}.html`),
  ]);

  // copy static assets
  const dest = path.join(workspaceFolder, BUILD_PATH);
  for (const u of COMPILER_TRANSLATION_UNITS_WEB_COPY) {
    for (const src of await glob(nixPath(relWs(absWs(u))))) {
      await fs.copyFile(src, path.join(dest, path.basename(src)));
    }
  }

  console.log("done compiling.");
  return r1.code;
};

const run_web = async (basename) => {
  const html = `${basename}.html`;
  const htmlPath = path.join(workspaceFolder, BUILD_PATH, html);

  try {
    await fs.stat(htmlPath);
  } catch (e) {
    console.log(".html is missing. probably failed to compile.", e);
    return;
  }

  // const serve_root = path.relative(path.join(workspaceFolder, BUILD_PATH), process.cwd());
  // console.log(serve_root);
  console.log("http://localhost:9090/build/main.html");
  const r1 = await child_spawn(EMRUN_PATH, [
    '--port', 9090,
    // '--serve-root', serve_root,
    '--no-browser',
    html,
  ]);
  return r1.code;
}

const fs_exists = async (file) => {
  try {
    const stat = await fs.stat(file);
    return { exists: true, stat };
  } catch (e) {
    return { exists: false, stat: { mtime: null } };
  }
};

const extract_include_units = async function* (opts) {
  if (!opts) opts = {};
  if (!opts.file) return;
  if (undefined == opts.depth) opts.depth = 0;
  if (undefined == opts.seen) opts.seen = new Set();
  if (undefined == opts.analyze) opts.analyze = false;
  const dname1 = path.dirname(opts.file);

  opts.seen.add(nixPath(opts.file));
  const rl = readline.createInterface({
    input: createReadStream(opts.file),
    crlfDelay: Infinity, // Handle both \n and \r\n newlines
  });
  const RX_INCLUDE1 = /^#include "([^"]+)"/gm;
  const RX_INCLUDE2 = /^#include <([^"]+)>/gm;
  let m;
  for await (const line of rl) {
    while (null != (m = RX_INCLUDE1.exec(line))) {
      const [, include] = m;
      // if (line.includes('nofollow')) continue;
      const cwd = process.cwd();
      const dname2 = path.dirname(include);
      const bname = path.basename(include, '.h');
      const h_file = relWs(path.resolve(cwd, dname1, dname2, `${bname}.h`));
      const { exists: h_exists, stat: { mtime: h_mtime } } = await fs_exists(h_file);
      const c_file = relWs(path.resolve(cwd, dname1, dname2, `${bname}.c`));
      const { exists: c_exists, stat: { mtime: c_mtime } } = await fs_exists(c_file);
      // console.debug('match', { h_file, c_file });
      if (c_exists) {
        if (!opts.seen.has(nixPath(c_file))) {
          opts.seen.add(nixPath(c_file));
          yield { c_file, c_mtime, depth: opts.depth };
          yield* await extract_include_units({
            file: c_file, depth: opts.depth + 1, seen: opts.seen, analyze: opts.analyze
          });
        }
      }
      if (h_exists) {
        if (!opts.seen.has(nixPath(h_file))) {
          opts.seen.add(nixPath(h_file));
          yield { h_file: h_file, h_mtime, depth: opts.depth };
          yield* await extract_include_units({
            file: h_file, depth: opts.depth + 1, seen: opts.seen, analyze: opts.analyze
          });
        }
      }
    }
    if (opts.analyze) {
      while (null != (m = RX_INCLUDE2.exec(line))) {
        const [, include] = m;
        if (opts.seen.has(nixPath(include))) continue;
        opts.seen.add(nixPath(include));
        yield { c_file: `<${include}>`, depth: opts.depth };
      }
    }
  }
};

const recompile_object = async (unit, mode = 'main', extra_compiler_flags = []) => {
  const basename = path.basename(unit, '.c');
  const out_path = path.join(workspaceFolder, BUILD_PATH, relWs(unit).replace(/\.c$/, ''));
  let object = relWs(path.join(out_path, `${basename}.o`));
  object = logic_hotreload_variants(mode)(object);
  await fs.mkdir(out_path, { recursive: true });
  let u_file = unit;
  let { stat: { mtime: u_mtime } } = await fs_exists(unit);
  for await (const m of extract_include_units({ file: unit })) {
    if (m.h_mtime > u_mtime) { u_file = m.h_file; u_mtime = m.h_mtime; }
  }
  const { stat: { mtime: object_mtime } } = await fs_exists(object);
  let r1 = { code: 0 };
  if (u_mtime > object_mtime) {
    // console.debug({ u_file, u_mtime, object_mtime });
    r1 = await child_spawn(C_COMPILER_PATH, [
      ...DEBUG_COMPILER_ARGS,
      ...C_COMPILER_ARGS,
      ...C_ENGINE_COMPILER_FLAGS,
      ...extra_compiler_flags,
      // ...C_COMPILER_INCLUDES,
      // ...LINKER_LIBS,
      // ...LINKER_LIB_PATHS,
      '-c', unit, // compile without linking
      '-o', object,
    ], { buffer: true });
    console.log(chalk.grey(r1.cmd));
    if (0 != r1.code) {
      // try {
      //   await fs.rm(object); // because it gets half-written, otherwise
      // } catch (e) { }
      console.error(chalk.red(r1.stdout));
      console.error(chalk.red(r1.stderr));
    }
  }
  return { unit, object, r: r1 };
};

// like rspec
//
// usage:
//    # run only jobs with `// @tag net` metadata
//    node build_scripts\\Makefile.mjs test --filter=net
//
const test = async () => {
  const results = [], overall = {};
  overall.filters = [];
  overall.files = [];
  overall.results = results;
  overall.passes = 0, overall.fails = 0, overall.skips = 0;
  overall.compileLap = 0, overall.execLap = 0, overall.lap = 0;
  overall.available = COMPILER_TRANSLATION_UNITS_UNIT_TESTS.length;
  overall.ran = 0;
  for (const arg of process.argv) {
    if (arg.startsWith('--filter=')) {
      const [, filter] = arg.split('=');
      overall.filters.push(filter);
    }
    else if (arg.startsWith('--file=')) {
      const [, file] = arg.split('=');
      overall.files.push(file);
    }
    else if (arg.startsWith('-norun')) {
      overall.norun = true;
    }
  }

  for (const u of [...COMPILER_TRANSLATION_UNITS_UNIT_TESTS, ...COMPILER_TRANSLATION_UNITS_INTEGRATION_TESTS]) {
    for (let unit of await glob(nixPath(relWs(absWs(u))))) {
      const result = {};
      result.execCmd = '';
      result.execCode = 0, result.execOut = '', result.execErr = '';
      result.execLap = 0;
      unit = relWs(workspaceFolder, unit);

      // read file metadata
      const c = await fs.readFile(unit, { encoding: 'utf8' });
      const RX_META = /^\/\/ @(\w+)(?: (.+))?$/gm;
      let m;
      while (null != (m = RX_META.exec(c))) {
        const [, k, v] = m;
        if ('describe' == k) result.description = v;
        else if ('tag' == k) !result.tags && (result.tags = ''), result.tags += ',' + v;
        else if ('skip' == k) result.skip = true;
        else if ('run' == k) !result.runInsts && (result.runInsts = []), result.runInsts.push(v);
      }
      if (
        result.skip ||
        (result.tags && overall.filters.length > 0 && !overall.filters.some(t => result.tags.includes(t))) ||
        (overall.files.length > 0 && !overall.files.some(f => nixPath(f) == nixPath(unit)))
      ) {
        result.pass = false, result.fail = false, result.skip = true;
        results.push(result);
        overall.skips++;
        continue;
      }
      overall.ran++;

      if (result.description) {
        console.log(`\n${overall.ran}) ${result.description}`);
      }
      else {
        console.log(`\n${overall.ran}) ${result.basename} `);
      }

      // discover included translation unit files
      // ie. we have a naming convention .h may have a matching .c
      const object_files = [], ps = [];
      for await (const m of extract_include_units({ file: unit })) {
        if (m.h_file) continue;
        const relFile = (file) => path.relative(
          relWs(workspaceFolder, path.dirname(m.c_file)),
          relWs(workspaceFolder, path.dirname(unit), file)
        );
        ps.push(() =>
          recompile_object(relWs(workspaceFolder, m.c_file), 'main', [
            '-DENGINE__TEST',
            `-DDEPINJ__GAME_H=${JSON.stringify(relFile('game/Game.h'))}`,
          ]).then((r) => {
            object_files.push(r.object);
          }));
      }
      await Promise__allBatch(ps);

      // compile and link in one step
      const started1 = performance.now();
      const dname = path.dirname(unit);

      const isUnit = nixPath(dname).startsWith('test/unit/');
      const isIntegration = nixPath(dname).startsWith('test/integration/');
      const fbasename = path.basename(unit, '.c');
      const basename = path.basename(dname);
      result.basename = basename;
      const test_out_path = path.join(workspaceFolder, BUILD_PATH);
      const executable = relWs(path.join(test_out_path, `${isUnit ? fbasename : basename}${isWin ? '.exe' : ''}`));
      await fs.mkdir(test_out_path, { recursive: true });

      if (isIntegration) {
        // dll
        const logic_c = (await glob(nixPath(relWs(dname, '**', 'Logic.c'))))[0];
        const code3 = await compile_reload(logic_c, DEFAULT_LOGIC_DLL);
      }

      // main

      const r1 = await child_spawn(C_COMPILER_PATH, [
        ...DEBUG_COMPILER_ARGS,
        ...C_COMPILER_ARGS,
        ...C_ENGINE_COMPILER_FLAGS,
        // ...C_COMPILER_INCLUDES,
        // ...LINKER_LIBS,
        // ...LINKER_LIB_PATHS,
        ...object_files.filter(
          logic_hotreload_variants('main'),
        ),
        unit,
        '-o', executable,
      ], { buffer: true });
      result.compileCmd = r1.cmd;
      result.compileCode = r1.code, result.compileOut = r1.stdout, result.compileErr = r1.stderr;
      const ended1 = performance.now();
      result.compileLap = ended1 - started1; // ms      
      overall.compileLap += result.compileLap;

      if (0 != result.compileLap > 1000) {
        console.log(chalk.yellow(indent(1, `Compilation took ${result.compileLap.toFixed(2)} ms!`)));
        console.log(chalk.grey(indent(1, result.compileCmd)));
      }
      else if (0 != result.compileCode) {
        console.error(chalk.grey(indent(1, result.compileCmd)));
      }

      if (0 != result.compileCode) {
        console.error(chalk.red(indent(1, `compilation failed. code: ${result.compileCode} `)));
        if (result.compileOut) {
          console.error(chalk.red(indent(2, result.compileOut)));
        }
        if (result.compileErr) {
          console.error(chalk.red(indent(2, result.compileErr)));
        }
      }

      if (0 == result.compileCode) {
        if (isNix || isMac) {
          await fs.chmod(executable, 0o755); // chmod +x
        }

        if (overall.norun) {
        } else {
          // execute test
          const started2 = performance.now();
          if (result.runInsts?.length == 1) {
            // use args specified in test
            const args2a = result.runInsts[0].split(/\s+/g);
            const p2a = child_spawn(relBuild(executable), args2a, { printCmd: true, buffer: true, cwd: absBuild() });
            const r2a = await p2a;
            result.execCmd = r2a.cmd;
            result.execCode = r2a.code, result.execOut = r2a.stdout, result.execErr = r2a.stderr;
          }
          else if (result.runInsts?.length == 3) {
            // launch three, wait for all, only consider result of third
            const args2a = result.runInsts[0].split(/\s+/g);
            const args2b = result.runInsts[1].split(/\s+/g);
            const args2c = result.runInsts[2].split(/\s+/g);
            let oa, ob, oc;
            const p2a = child_spawn(relBuild(executable), args2a, oa = { printCmd: true, color: 'cyan', prefix: 'proc#0', buffer: false, cwd: absBuild() });
            const p2b = child_spawn(relBuild(executable), args2b, ob = { printCmd: true, color: 'yellow', prefix: 'proc#1', buffer: false, cwd: absBuild() });
            const p2c = child_spawn(relBuild(executable), args2c, oc = { printCmd: true, color: 'magenta', prefix: 'proc#2', buffer: false, cwd: absBuild() });
            let completed = 0, done, p = new Promise((a) => { done = a; });
            let r2a, r2b, r2c;
            p2a.then((v) => {
              r2a = v;
              if (0 !== v.code && null !== v.code) { result.execCode = v.code; ob.kill(); oc.kill(); }
              if (3 == ++completed) done();
            });
            p2b.then((v) => {
              r2b = v;
              if (0 !== v.code && null !== v.code) { result.execCode = v.code; oa.kill(); oc.kill(); }
              if (3 == ++completed) done();
            });
            p2c.then((v) => {
              r2c = v;
              if (null == result.execCode) result.execCode = v.code;
              if (0 !== v.code && null !== v.code) { oa.kill(); ob.kill(); }
              if (3 == ++completed) done();
            });
            await p;

            result.execCmd = r2c.cmd, result.execOut = r2c.stdout, result.execErr = r2c.stderr;
          }
          else {
            // no args
            const r2 = await child_spawn(relBuild(executable), [], { printCmd: true, buffer: true, cwd: absBuild() });
            result.execCmd = r2.cmd;
            result.execCode = r2.code, result.execOut = r2.stdout, result.execErr = r2.stderr;
          }

          const ended2 = performance.now();
          result.execLap = ended2 - started2; // ms
        }
      }

      overall.execLap += result.execLap;
      overall.lap += overall.compileLap + overall.execLap;
      result.pass = true, result.fail = false, result.skip = false;
      if (0 != result.compileCode || 0 != result.execCode) {
        result.pass = false, result.fail = true, result.skip = false;
      }

      if (result.pass) {
        overall.passes++;
        console.log(chalk.green(indent(1, `process succeeded. code: ${result.execCode} `)));
        if (!isEmpty(result.execOut)) {
          console.log(chalk.green(indent(2, result.execOut)));
        }
      }
      else if (result.fail) {
        overall.fails++;
        console.error(chalk.red(indent(1, `process failed. code: ${result.execCode} `)));
        if (!isEmpty(result.execOut)) {
          console.error(chalk.red(indent(2, result.execOut)));
        }
      }

      if (!isEmpty(result.execErr)) {
        console.log(chalk.red(indent(2, result.execErr)));
      }

      results.push(result);

      if (!result.pass) {
        break;
      }
    }
  }

  console.log(`\nFinished in ${overall.lap.toFixed(2)} ms, compiled in ${overall.compileLap.toFixed(2)} ms, execed in ${overall.execLap.toFixed(2)} ms.`);

  const color = overall.fails > 0 ? 'red' : overall.passes > 0 ? 'green' : 'white';
  console['red' == color ? 'error' : 'log'](chalk[color](`${overall.ran} run, ${overall.fails} failures, ${overall.skips} skips. (${overall.available} found)`));

  // console.debug('overall', overall);
  return overall.fails > 0 ? 1 : 0;
};

(async () => {
  const [, , ...cmds] = process.argv;
  const [cmd] = cmds;
  let code;
  switch (cmd) {
    case 'all':
      code = await all(false);
      if (0 != code) process.exit(code);
      break;
    case 'web':
      web();
      break;
    case 'clean':
      console.log("--clean--");
      await clean();
      break;
    case 'copy_dlls':
      await copy_dlls();
      break;
    case 'shaders':
      await shaders('hlsl5:glsl430');
      // await shaders('glsl300es');
      break;
    case 'clang':
      await generate_clangd_compile_commands();
      break;
    case 'reload':
      await compile_reload(DEFAULT_LOGIC_C, DEFAULT_LOGIC_DLL);
      break;
    case 'watch':
      let [, , , base] = process.argv;
      await watch(base);
      break;
    case 'loop':
      await all_loop();
      break;
    case 'run':
      const [, ...args] = cmds;
      await run('main', args);
      break;
    case 'test':
      // await clean();
      // await copy_dlls();
      console.log('--shaders--');
      code = await shaders('hlsl5:glsl430');
      if (0 != code) process.exit(code);
      console.log('--tests--');
      code = await test();
      if (0 != code) process.exit(code);
      break;
    case 'analyze':
      let [, , , file, analyze] = process.argv;
      analyze = '--analyze' == analyze;
      let i = 0;
      for await (const unit of extract_include_units({ file, analyze })) {
        if (unit.h_file) continue;
        console.log(indent(unit.depth, unit.c_file));
        i++;
      }
      break;
    case 'help':
    default:
      console.log(`
Mike's hand-rolled build system.

    USAGE:
node build_scripts\\Makefile.mjs < SUBCOMMAND >

      SUBCOMMANDS:
  all        Clean, rebuild, and launch the default app.
  web        Clean, rebuild, and launch the default app in browser.
  clean      Delete all build output.
  copy_dlls  Copy dynamic libraries to build directory.
  shaders    Compile SPIRV shaders with GLSLC.
  clang      Generate the.json file needed for clangd for vscode extension.
  watch      Watch for changes, recompile.dll.
  loop       Watch for changes, recompile all.
  run        Run the main app
  test       Compile and run the test suite
  analyze    Print include tree for a given file
      `);
      break;
  }
})();
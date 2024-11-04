import { glob } from 'glob'
import cbFs from 'fs';
import fs from 'fs/promises';
import path from 'path';
import { spawn } from 'child_process';
import { fileURLToPath } from 'url';

const isWin = process.platform === "win32";
const isMac = process.platform === "darwin";
const isNix = process.platform === "linux";
const __dirname = path.dirname(fileURLToPath(import.meta.url));
const SDHC_PATH = path.join('vendor', 'sokol-tools-bin', 'bin', 'win32', 'sokol-shdc.exe'); // TODO: multiplatform support
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
const DEBUG_COMPILER_ARGS = [
  '-O0',
  // export debug symbols (x86dbg understands both; turn these on when debugging, leave off for faster compile)
  // '-gdwarf', // DWARF (GDB / Linux compatible)
  '-g', /* '-gcodeview',*/ // CodeView (PDB / windbg / Windows compatible)
];
const C_COMPILER_ARGS = [
  '-m64', // generate 64-bit executable 

  // ignore specific warnings
  '-Wno-microsoft-enum-forward-reference',
  '-Wno-deprecated-non-prototype',
];
const C_COMPILER_INCLUDES = [
];
const C_ENGINE_COMPILER_FLAGS = [
  // '-DSOKOL_IMPL', // use Sokol
  '-DSOKOL_GLCORE', // use GL backend
];
const C_DLL_COMPILER_FLAGS = [
  // '-DSOKOL_API_DECL=static', // headers-only
  '-DSOKOL_GLCORE', // use GL backend
];
const C_WEB_COMPILER_FLAGS = [
  '-DSOKOL_GLES3', // use GLES3 backend
  '-sUSE_WEBGL2', // use WebGL2
  // see: https://emscripten.org/docs/tools_reference/settings_reference.html?highlight=environment
  '-sTOTAL_MEMORY=512MB', // allow larger memory pool (16MB is default)
  '-sSTACK_OVERFLOW_CHECK=2', // check for stack overflow
  '-sSTACK_SIZE=5MB',
];

const ENGINE_ONLY = [
  'src/app.c',
  'src/lib/HotReload.c',
];
const LINKER_LIBS = [];
// if (isWin) {
//   LINKER_LIBS.push('-l', 'Ws2_32.lib');
// }
const LINKER_LIB_PATHS = [];
const COMPILER_TRANSLATION_UNITS = [
  relWs(workspaceFolder, 'src', '*.c'),
  relWs(workspaceFolder, 'src', 'lib', '**', '*.c'),
];
const COMPILER_TRANSLATION_UNITS_DLL = [
  relWs(workspaceFolder, 'src', 'lib', '**', '*.c'),
  relWs(workspaceFolder, 'src', 'game', '**', '*.c'),
];
const COMPILER_TRANSLATION_UNITS_WEB = [
  relWs(workspaceFolder, 'src', '**', '*.c'),
];
const COMPILER_TRANSLATION_UNITS_WEB_EXCLUDE = [
  relWs(workspaceFolder, 'src', 'lib', 'HotReload.c'),
];
const COMPILER_TRANSLATION_UNITS_WEB_COPY = [
  relWs(workspaceFolder, 'assets', 'web', '*'),
];

const nixPath = (p) =>
  path.posix.normalize(p.replace(/\\/g, '/'));

const generate_clangd_compile_commands = async () => {
  console.log('scanning directory...');
  const compile_commands = [];
  const unit_files = await glob('{src,tests}/**/*.c');
  console.debug('unit_files: ', unit_files);
  const dll_files = await glob('{src,tests}/game/**/*.c');
  let is_dll = false;
  for (const unit_file of unit_files) {
    is_dll = dll_files.includes(unit_file);
    compile_commands.push({
      directory: path.join(workspaceFolder, BUILD_PATH),
      arguments: [
        C_COMPILER_PATH,
        ...C_COMPILER_ARGS,
        // ...C_COMPILER_INCLUDES,
        ...(is_dll ? C_DLL_COMPILER_FLAGS : C_ENGINE_COMPILER_FLAGS),
        '-c',
        '-o', `${unit_file}.o`,
        relBuild(unit_file),
      ],
      file: relBuild(workspaceFolder, unit_file),
    });
  }

  console.log(`writing ${OUT_FILE}...`)
  await fs.writeFile(OUT_FILE, JSON.stringify(compile_commands, null, 2));

  console.log('done making.');
};

const child_spawn = async (cmd, args = [], opts = {}) => {
  console.log(`${opts.stdin ? `type ${opts.stdin} | ` : ''}${cmd} ${args.join(' ')}${opts.stdout ? ` > ${opts.stdout}` : ''}`);
  let stdin, stdout;
  const stdio = ['inherit', 'inherit', 'inherit'];
  if (opts.stdin) {
    stdio[0] = 'pipe';
    // stdin = cbFs.createReadStream(path.join(workspaceFolder, stdinfile));
  }
  if (opts.stdout) {
    stdio[1] = 'pipe';
    // stdout = await cbFs.createWriteStream(path.join(workspaceFolder, stdoutfile));
  }
  const child = spawn(cmd, args, { stdio });
  if (opts.stdin) {
    stdin.pipe(child.stdin);
  }
  if (opts.stdout) {
    child.stdout.pipe(stdout);
  }
  const code = await new Promise((ok) => {
    child.on('close', async (code) => {
      if (code !== 0) {
        console.log(`process exited with code ${code}`);
      }
      ok(code);
    });
  });
  return code;
};

const all = async (autorun) => {
  await clean();
  await copy_dlls();
  console.log('shaders');
  const code1 = await shaders('hlsl5:glsl430');
  if (0 != code1) return code1;
  console.log('main');
  const code2 = await compile('main');
  if (0 != code2) return code2;
  console.log('dll');
  const code3 = await compile_reload("src/game/Logic.c.dll");
  if (0 != code3) return code3;
  if (autorun) await run('main');
};

const web = async () => {
  await clean();
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
  const shaderFiles = await glob(path.join(workspaceFolder, 'assets', 'shaders', '*.glsl').replace(/\\/g, '/'));
  for (const shaderFile of shaderFiles) {
    const relPath = path.relative(path.join(workspaceFolder), shaderFile);
    const code = await child_spawn(SDHC_PATH, ['-i',
      relPath, '-o', `${relPath}.h`,
      '-l', out_type]);
    if (0 !== code) return code;
  }
  return 0;
};

const clean = async () => {
  await fs.rm(path.join(workspaceFolder, BUILD_PATH), { recursive: true, force: true });
  await fs.mkdir(path.join(workspaceFolder, BUILD_PATH));

  const shaderFiles = await glob(path.join(workspaceFolder, 'assets', 'shaders', '*.glsl.h').replace(/\\/g, '/'));
  for (const shaderFile of shaderFiles) {
    await fs.rm(shaderFile, { force: true });
  }
};

const compile = async (basename) => {
  console.log(`compiling ${basename}...`);

  const unit_files = [];
  for (const u of COMPILER_TRANSLATION_UNITS) {
    for (const file of await glob(relWs(absWs(u)).replace(/\\/g, '/'))) {
      // if (ENGINE_ONLY.includes(nixPath(file))) {
      unit_files.push(file);
      // }
    }
  }

  // compile and link in one step
  const code = await child_spawn(C_COMPILER_PATH, [
    ...DEBUG_COMPILER_ARGS,
    ...C_COMPILER_ARGS,
    ...C_ENGINE_COMPILER_FLAGS,
    // ...C_COMPILER_INCLUDES,
    ...LINKER_LIBS,
    ...LINKER_LIB_PATHS,
    ...unit_files.map(unit => relWs(workspaceFolder, unit)),
    '-o', relWs(workspaceFolder, BUILD_PATH, `${basename}${isWin ? '.exe' : ''}`),
  ]);

  if (0 == code) console.log("done compiling.");
  else console.log("compile failed.");
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

const compile_reload = async (outname) => {
  console.log(`recompiling...`);

  await fs.mkdir(path.join(workspaceFolder, BUILD_PATH, 'tmp'), { recursive: true });

  const dsts = [];
  for (const u of COMPILER_TRANSLATION_UNITS_DLL) {
    for (const file of await glob(relWs(absWs(u)).replace(/\\/g, '/'))) {
      if (!ENGINE_ONLY.includes(nixPath(file))) {
        dsts.push(relWs(workspaceFolder, file));
      }
    }
  }

  const unit = 'src/game/Logic.c';
  await fs.mkdir(absBuild(path.dirname(unit)), { recursive: true });
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
  if (ANALYZE) await child_spawn(ANALYZER, ['--start', 'src/game']);

  const started = performance.now();
  const code = await child_spawn(C_COMPILER_PATH, [
    ...DEBUG_COMPILER_ARGS,
    ...(ANALYZE ? ['-ftime-trace'] : []), // display compile time stats
    ...C_COMPILER_ARGS,
    ...C_DLL_COMPILER_FLAGS,
    ...LINKER_LIBS,
    ...LINKER_LIB_PATHS,
    '-shared',
    ...dsts,
    '-o', dst,
  ]);
  const ended = performance.now();

  if (ANALYZE) await child_spawn(ANALYZER, ['--stop', 'src/game', 'analysis.bin']);
  if (ANALYZE) await child_spawn(ANALYZER, ['--analyze', 'analysis.bin']);

  // swap lib
  let target2;
  try {
    await fs.stat(path.join(workspaceFolder, BUILD_PATH, target));
    target2 = target.replace('.tmp', '');
    // try {
    //   await fs.rename(path.join(workspaceFolder, BUILD_PATH, target), path.join(workspaceFolder, BUILD_PATH, 'tmp', target2));
    // } catch (e) {
    // }

    if (target != target2) {
      await fs.cp(path.join(workspaceFolder, BUILD_PATH, target), path.join(workspaceFolder, BUILD_PATH, target2));
    }
  } catch (e) {
    console.log('copy failed.', e);
  }

  console.log(`recompilation ${0 == code ? 'succeeded' : 'failed'}. file: ${target2}, elapsed: ${((ended - started) / 1000).toFixed(2)}s`);
  return code;
};

const watch = async () => {
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
        await compile_reload(`src/game/${generateRandomString(16)}.dll.tmp`);
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

const run = async (basename) => {
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

  await child_spawn(exePath);
}

const compile_web = async (basename) => {
  console.log(`compiling w/ emscripten...`);

  const unit_files = [];
  let candidates, noncandidates;
  for (const u of COMPILER_TRANSLATION_UNITS_WEB) {
    candidates = await glob(relWs(absWs(u)).replace(/\\/g, '/'));
    nextFile:
    for (const file of candidates) {
      for (const nu of COMPILER_TRANSLATION_UNITS_WEB_EXCLUDE) {
        noncandidates = await glob(relWs(absWs(nu)).replace(/\\/g, '/'));
        for (const nonFile of noncandidates) {
          if (file == nonFile) continue nextFile;
        }
      }

      unit_files.push(file);
    }
  }

  // compile and link in one step
  const code = await child_spawn(EMCC_COMPILER_PATH, [
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
    for (const src of await glob(relWs(absWs(u)).replace(/\\/g, '/'))) {
      await fs.copyFile(src, path.join(dest, path.basename(src)));
    }
  }

  console.log("done compiling.");
  return code;
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
  const code = await child_spawn(EMRUN_PATH, [
    '--port', 9090,
    // '--serve-root', serve_root,
    '--no-browser',
    html,
  ]);
  return code;
}

(async () => {
  const [, , ...cmds] = process.argv;
  loop:
  for (const cmd of cmds) {
    switch (cmd) {
      case 'all':
        const code = await all(false);
        if (0 != code) process.exit(code);
        break;
      case 'web':
        web();
        break;
      case 'clean':
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
      case 'main':
        await compile(cmd);
        break;
      case 'reload':
        await compile_reload("src/game/Logic.c.dll");
        break;
      case 'watch':
        await watch();
        break;
      case 'loop':
        await all_loop();
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
  watch      Watch for changes, recompile .dll.
  loop       Watch for changes, recompile all.
  main       Compile and run the main app
  `);
        break loop;
    }
  }
})();
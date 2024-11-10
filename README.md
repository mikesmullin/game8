# Game 8

This prototype explores an application of the [Sokol](https://github.com/floooh/sokol) library.

### Features include:
- cross-platform (win, mac, nix, wasm)
- stateful hot-reload (via .DLL + file watcher)
- entity component system (ECS) design pattern
- atlas + PBR shaders
  - Blender-compatible via Wavefront (.obj)
- real-time networked multiplayer
  - client-authoritative
  - lockstep entity replication
    - input synchronization
    - deterministic math
  - websockets
- unit & integration test runner (like rspec)
  - windowed or headless

my related Game prototypes: 
[5](https://github.com/mikesmullin/cpp20-win11-x64-sdl2-vulkan?tab=readme-ov-file)
[6](https://github.com/mikesmullin/c17-sdl2-vulkan)
[7](https://github.com/mikesmullin/game7)

## Prerequisites
- Recent version of **Windows**, Mac, or Linux OS (32-bit)
  - or a Browser supporting [**WebGL2**](https://webgl2fundamentals.org/)
- **GPU** supporting [**OpenGL**](https://www.opengl.org/) or [**Metal**](https://developer.apple.com/metal/) (drivers often included with OS)
- **Node.js** (for build scripts)
- **Clang** (recommended compiler)

## Building

Clone the repo, and fetch dependencies.
```
git clone ...
git submodule update --init --recursive 
```

### ie. on Windows
Start the game.
```
C:\> node build_scripts/Makefile.mjs all
```
Enable hot-reload. (ie. when editing [`Logic.c`](src/game/Logic.c))
```
C:\> node build_scripts/Makefile.mjs watch
```

### ie. in Browser
Setup emscripten compiler (`emcc`) environment.
```
cd vendor/emsdk/
emsdk_env
cd ..
```

Start the web server.
```
C:\> node build_scripts/Makefile.mjs web
```

Navigate to [http://localhost:9090/build/main.html](http://localhost:9090/build/main.html)

**NOTICE:** Web version does not hot-reload, only because desktop affords the superior debugging experience.


## Testing

### ie. on Windows
```
C:\> node build_scripts/Makefile.mjs test
```

Additional build commands can be found with `help`.
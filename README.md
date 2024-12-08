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

## Inspirations

- [Isadora Sophia](https://isadorasophia.com/about/) 🇨🇦 🇧🇷 ([@isainstars](https://x.com/isainstars)) 
  - (Sophia 2024) *"9 Secrets for a Future-Proof Game Engine - Isadora | Vancouver Game Dev Series"* | [youtube](https://www.youtube.com/watch?v=4sc3cm9Kdys)
  - [Murder engine](https://github.com/isadorasophia/murder) game engine
  - [Cold Blood, Inc.](https://coldbloodinc.studio/) studio
- John Carmack 🇺🇸 ([@ID_AA_Carmack](https://x.com/ID_AA_Carmack))
  - (1996) *Quake* | [steam](https://store.steampowered.com/app/2310/Quake/)
    - [Q_rsqrt()](https://stackoverflow.com/questions/1349542/john-carmacks-unusual-fast-inverse-square-root-quake-iii)
    - [QuakeC](https://github.com/erysdren/QCVM?tab=readme-ov-file) ([manual](https://github.com/erysdren/QCVM/blob/main/contrib/quakec-reference-manual.pdf))
    - (1997) *Graphics Programming Black Book* [official free online](https://www.jagregory.com/abrash-black-book/)
- code style
  - NASA "space-safe" https://www.youtube.com/watch?v=GWYhtksrmhE
  - ooga booga https://github.com/alpinestudios/oogabooga

## References

- Game Math
  - dot https://www.youtube.com/watch?v=0iNrGpwZwog
  - cross https://www.youtube.com/watch?v=gPnWm-IXoAY
  - dot & cross visual https://www.youtube.com/watch?v=h0NJK4mEIJU
  - trig approx https://www.ganssle.com/item/approximations-for-trig-c-code.htm
  - nvidia trig approx https://developer.download.nvidia.com/cg/index_stdlib.html
  - interesting idea for 8-bit degrees (limited precision, but lower bandwidth; 
    how precise do object orientations really need to be in a lot of cases?)  
    `allegro_degrees = regular_degrees * 256 / 360;`  
    `allegro_degrees = radians * 128 / PI;`  
  - Cubic B-Splines for corkscrew/curved trajectory and mesh arrays (ie. roads, fences) 
    - https://www.youtube.com/watch?v=RLc20gcc804
    - https://www.youtube.com/watch?v=n-o2e4KxbW4
  - deep mastery https://foundationsofgameenginedev.com/

- Deterministic (Cross-Platform/Portable) Game Math
  - good/latest
    - advice
      - https://box2d.org/posts/2024/08/determinism/
    - examples
      - UE C++ https://github.com/mdragon159/ProjectNomad-Core/tree/main/ProjectNomadCore/Physics
      - Godot C# https://github.com/Fractural/GodotFixedVolatilePhysics
      - Rapier Rust https://rapier.rs/docs/user_guides/rust/determinism  
        (target platforms must strictly comply to the IEEE 754-2008 floating-points standard)
  - bad/old
    - advice
      - https://gafferongames.com/post/floating_point_determinism/
      - https://randomascii.wordpress.com/2013/07/16/floating-point-determinism/
    - examples
      - https://jrouwe.github.io/JoltPhysics/#deterministic-simulation
    - using fixed-point/integer math is 4-10x slower
      - https://www.youtube.com/watch?v=-4Bi1hgAZUI box2d in fixed-point (seems slow)
      - https://www.youtube.com/watch?v=7hK1g8CvbgY unity3d soft floats
        - https://stackoverflow.com/questions/3321468/whats-the-difference-between-hard-and-soft-floating-point-numbers
      - https://github.com/mgetka/fptc-lib
      - https://github.com/PetteriAimonen/libfixmath
      - https://github.com/jcarrano/fixed_point_arith
    - software emulated float
      - https://github.com/ucb-bar/berkeley-softfloat-3
      - https://www.mpfr.org/

- Game Engine > Physics
  - good books
    https://www.amazon.com/Game-Physics-Engine-Development-Commercial-Grade/dp/0123819768
  - box2d author
    - concepts
      - Constraints https://www.youtube.com/watch?v=SHinxAhv1ZE
      - Impulses https://www.youtube.com/watch?v=E13h1E_Pc00
    - first version (box2d-lite) is ok for learning  
      https://github.com/erincatto/box2d-lite/blob/master/docs/HowDoPhysicsEnginesWork.pdf
  - animating a rolling cube != simulating physics + multi-point/edge collision  
    https://www.youtube.com/watch?v=sVg8jnajLSk
  - WARNING! patents exist for algorithms used in physics, audio, etc.  
    which is one reason to use existing libs (ie. Box2D, FMOD, etc.) as they've side-stepped these issues.  
    definitely an ironic pitfall; that by writing your own code from-scratch, if you sell it, you could be subject to patent infringement.  
    https://patents.google.com/patent/ATE247310T1/en
  - active ragdolls (e.g., Gang Beasts, Subrosa)
    - https://www.youtube.com/watch?v=hQub6caVlpY
    - https://x.com/crypticsea/status/1851698285976932851
  - spring physics-based animation
    - e.g., https://www.youtube.com/watch?v=bFOAipGJGA0
  - hinges

- Multiplayer Networking
  - overwatch ecs https://www.gdcvault.com/play/1024001/-Overwatch-Gameplay-Architecture-and
  - overwatch statescript https://www.youtube.com/watch?v=odSBJ49rzDo
  - physics + animation is tricky https://www.youtube.com/live/mKBDhC6-t4s
  - NAT Punch-Through https://keithjohnston.wordpress.com/2014/02/17/nat-punch-through-for-multiplayer-games/
  - WebRTC Punch-Through https://getbananas.net/
  - lockstep replication / input synchronization
    - https://gafferongames.com/post/deterministic_lockstep/
    - https://www.gabrielgambetta.com/client-side-prediction-server-reconciliation.html
    - https://www.youtube.com/watch?v=GqHTNmRspjU
    - unity3d https://docs-multiplayer.unity3d.com/netcode/current/components/networktransform/
    - ue5 https://www.youtube.com/watch?v=OVeo3cVTIcU&t=40s
  - websockets
    - https://www.openmymind.net/WebSocket-Framing-Masking-Fragmentation-and-More/
    - HumbleNet https://github.com/HumbleNet/HumbleNet/blob/master/tests/test_peer.cpp (*wasm!*)
  - serilization / encoding
    - google flatbuffers https://github.com/google/flatbuffers (*fast!*)
  - RPCs
    - https://www.youtube.com/watch?v=EwI8WG5MpYw
    - gRPC https://www.youtube.com/watch?v=gnchfOojMk4
  
- Player Camera
  - https://godotengine.org/asset-library/asset/1822
  - https://github.com/ramokz/phantom-camera

- AI
  - good books
    - https://www.amazon.com/dp/B07PYGNV64
  - A* pathing https://www.youtube.com/watch?v=-L-WgKMFuhE

- Custom Scripting Language Interpreter
  - good books
    - https://github.com/munificent/craftinginterpreters/
  - good examples
    - c vm https://github.com/munificent/craftinginterpreters/blob/master/c/vm.c#L866
    - arm64 asm
      - https://developer.arm.com/documentation/102374/0102/Registers-in-AArch64---general-purpose-registers
      - https://developer.arm.com/documentation/ddi0602/2024-09/Base-Instructions/STR--register---Store-register--register--

- OpenGL API, GLSL Shaders
  - pbr
    - math 
      - https://www.youtube.com/watch?v=MkFS6lw6aEs
      - https://www.youtube.com/watch?v=5p0e7YNONr8
      - https://www.youtube.com/watch?v=j-A0mwsJRmk
      - https://www.youtube.com/watch?v=XK_p2MxGBQs
      - https://www.youtube.com/watch?v=RRE-F57fbXw
    - examples
      - khronos gltf pbr impl https://www.khronos.org/gltf/pbr/
      - adobe substance 3d painter [lib-pbr.glsl](https://helpx.adobe.com/substance-3d-painter/scripting-and-development/api-reference/shader-api/libraries-shader-api/lib-pbr-shader-api.html)
  - pixel shaders
    - https://saint11.art/blog/scaling/
  - deferred rendering pipeline (maximum light sources)
    - https://www.youtube.com/watch?v=ucjVHusVCcU
  - good books / references
    - https://learnopengl.com/
    - https://thebookofshaders.com/
    - https://github.com/septag/glslcc
    - https://github.com/KhronosGroup/glslang    

- Compile-time optimization
  - https://aras-p.info/blog/2019/01/12/Investigating-compile-times-and-Clang-ftime-report/
  - https://www.phoronix.com/news/LLVM-Clang-9.0-Time-Trace

- Cross-platform Threading
  - https://github.com/tinycthread/tinycthread/blob/master/source/tinycthread.h
  - https://web.dev/articles/webassembly-threads
  - https://wasmer.io/posts/wasm-as-universal-binary-format-part-1-native-executables
  - https://ashourics.medium.com/performance-comparison-analysis-wasmer-vs-wasmtime-48c6f51b536f

- Time Travel
  - https://www.youtube.com/watch?v=1V4qoXEa7JU
  - https://www.youtube.com/watch?v=5wifLUOtkJM
  - https://www.youtube.com/watch?v=Vapuu9qe_yE

- Red-Black Trees (used for fast entity zsort)
  - https://www.youtube.com/watch?v=t-oiZnplv7g

- Bezier fns
  - `create()` https://cubic-bezier.com/#.17,1.56,.53,.71

- Arena Allocators (deep!)
  - https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator

- Hash Table impl in c (deep!)
  - https://www.andreinc.net/2021/10/02/implementing-hash-tables-in-c-part-1

- String8
  - Mr. 4th impl https://www.youtube.com/watch?v=2wio9UOFcow
  - Ryan Fleury's impl for Epic > RAD Debugger ([.h](https://github.com/EpicGamesExt/raddebugger/blob/94c4000603010aed97a45ca4d87a501a264f66b0/src/metagen/metagen_base/metagen_base_strings.h#L16) | [.c](https://github.com/EpicGamesExt/raddebugger/blob/94c4000603010aed97a45ca4d87a501a264f66b0/src/metagen/metagen_base/metagen_base_strings.c))

- Game Console Commands
  - [Don't Starve Together](https://dontstarve.fandom.com/wiki/Console/Don%27t_Starve_Together_Commands) console command list 
    - `c_spawn()`: create entity in world (at cursor pos)
    - `c_give()`: add to inventory
    - `c_set(health|sanity|hunger|moisture|temperature)()`: modify stats
    - `c_godmode()`: toggle
    - `c_speedmul()`: scale player speed
    - `c_freecrafting()`: toggle creative mode
    - `c_move()`: teleport to player
    - `c_gonext()`: teleport to prefab
    - `c_despawn()`: kick player
    - `c_skip()`: manipulate time
  - [Unreal Engine 5](https://www.youtube.com/watch?v=bw4zDuH3v54) command list
    - `open 127.0.0.1`: connect to multiplayer server
  - **Half-Life / Source engine** ([cli](https://steamcommunity.com/sharedfiles/filedetails/?id=2906736676) | [console](https://developer.valvesoftware.com/wiki/List_of_Half-Life_console_commands_and_variables)) command list
    - prefix [naming convention](https://counterstrike.fandom.com/wiki/Developer_console#Command_Prefixes) (Unreal Engine copied this too.)
      - `r_`, `cl_` and `sv_` : render, client, and server.
        - (guessing) indicates which netvars are replicated vs. local-only
  - [Quake](https://quake.fandom.com/wiki/Console_Commands_(Q1)#Console_Commands) console command list
    - `bind q "script"`: key to script/cmd
      - where player actions are in scope as symbols like `+use`
    - `clear`: console clear
    - `fov`: change perspective fov
    - `crosshair`: toggle
    - `name`: change player name
    - `pause`: pause or unpause game
    - `say`: chat
    - `skill`: change difficulty mid-game
    - `cl_bob`, `cl_bobcycle`: modify bob animation
    - `noclip`: pass through geometry
    - `god`: invincibility
    - `kill`: suicide player, restart level
    - `fly`: toggle flight ability
    - `demos`, `playdemo`, `record`, `stop`, `stopdemo`, `timedemo`

## Cool Tools
- [quickjs](https://bellard.org/quickjs/) ([regexp impl](https://github.com/bellard/quickjs/blob/master/libregexp.c))
- [cosmopolitan](https://github.com/jart/cosmopolitan)
- [blinkenlights](https://justine.lol/blinkenlights/) debugger
- [rr](https://rr-project.org/) reverse debugger
- [Sonniss' GDC sfx collection](https://sonniss.com/gameaudiogdc) (*immense! high quality! free!*)

## Examples
- Iconic
  - (1993) [doom](https://github.com/id-Software/DOOM) game + engine src
  - (1996) [quake](https://github.com/id-Software/Quake) game + engine src
  - (1997) [duke3d](https://github.com/videogamepreservation/dukenukem3d) game + engine src
  - (2003) [gemrb](https://github.com/gemrb/gemrb) (a.k.a Bioware Infinity) engine src
  - (2007) [valve source](https://github.com/VSES/SourceEngine2007) engine src (*leak!*)
- Potentially useful
  - (2008) Aliens vs Predator ([gold](https://github.com/miohtama/aliens-vs-predator) | [pandora](https://github.com/lonewolf9383/avp_GLES)) game src
  - (2013) [Star Wars Jedi Knight II: Jedi Outcast](https://github.com/grayj/Jedi-Outcast) game src
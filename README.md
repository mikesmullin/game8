# Game 8

This prototype explores an application of the [Sokol](https://github.com/floooh/sokol) library.

### Features include:
- Stateful Hot-reloading (via .DLL + file watcher)

## Building

### on Windows
Start the game.
```
C:\> node build_scripts/Makefile.mjs all
```
Enable hot-reload. (ie. when editing [`Logic.c`](src/game/Logic.c))
```
C:\> node build_scripts/Makefile.mjs watch
```

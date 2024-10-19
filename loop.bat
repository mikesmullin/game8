@echo off
:loop
node build_scripts\\Makefile.mjs all
goto loop
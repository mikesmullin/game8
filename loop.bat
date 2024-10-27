@echo off
:loop
node build_scripts\\Makefile.mjs loop
goto loop
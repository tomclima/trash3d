@echo off

mkdir ..\builds
pushd ..\builds

cl -Zi -FC ..\trash3d\code\win32_trash3d.cpp User32.lib  Gdi32.lib onecore.lib

popd
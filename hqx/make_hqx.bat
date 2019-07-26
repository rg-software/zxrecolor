@echo off
echo Run this script from MSVS Developer Command Prompt

cmake .
devenv /build Release hqx.sln
copy /y Release\test.exe hqx.exe

echo Usage: hqx.exe image-file

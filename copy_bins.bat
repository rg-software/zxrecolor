@echo off
md bin

copy /y hqx\hqx.exe bin\

copy /y ZXColorEmul\UnrealSpeccy\Release\2006.rom bin\
copy /y ZXColorEmul\UnrealSpeccy\Release\bass.dll bin\
copy /y ZXColorEmul\UnrealSpeccy\Release\unreal.exe bin\
copy /y ZXColorEmul\UnrealSpeccy\Release\unreal.ini bin\

copy /y ZXColorTool\ZXColor\bin\Release\ZXColor.exe bin\

ver > nul
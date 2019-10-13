# Deployment

Deploying your remake is very easy: 

1. Prepare a working folder with the minimal emulator environment. It should include the files `2006.rom`, `bass.dll`, `unreal.exe`, and `unreal.ini`.

2. Place the `sna`-snapshot of the original ZX Spectrum game into the working folder. Open `unreal.ini` and make sure that the value of `snapshot` parameter matches the name of your snapshot.

2. You can rename `unreal.exe` and `unreal.ini` into anything else like `MyGame.exe` and `MyGame.ini`, just make sure these names match.

3. Place the content of your `game` folder (with pictures, sounds, and `settings.txt`) into the archive named `game.zip` and move it into your working folder. Make sure the resources are directly in the archive, not inside some subfolder.

Now you can simply convert your working folder to a regular or self-extracting archive and publish it anywhere you like! Just make sure to [drop me a line](mailto:mozgovoy@u-aizu.ac.jp), I'll be happy to know about new remakes :)
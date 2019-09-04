![](Docs_Source/images/zx_recoloring_logo.png)

This toolset will help you to give a more modern look to your favorite ZX Spectrum games and make them attractive even to the new generation of gamers. By following a few simple steps you will be able to:

- substitute any original game sprites with their multicolor double-resolution equivalents;
- add new graphical elements and embellishments;
- provide your own music and graphical effects;
- (more functioins are planned...)

These changes require no coding, and are applied instantly, so it is possible to make just a couple of small changes and leave the rest of the game intact.

![](Docs_Source/images/sb_demo.gif)

Unlike [Spec256](http://www.emulatronia.com/emusdaqui/spec256/index-eng.htm), ZX Recoloring Project does not require you to analyze and modify game snapshots. Everything works by means of pattern matching: if a certain sprite can be found on the screen, it will be substituted in real time.

This approach has certain obvious limitations: it won't work for games based on scalable vector graphics (such as [3D Tank Duel](http://www.worldofspectrum.org/infoseekid.cgi?id=0005136)), and handling overlapping sprites require additional efforts. However, very nice-looking results can be achieved in most cases.

So, grab the tools, check out [documentation] and start recoloring your favorite games!

# Building
The toolset can be built with MS Visual Studio 2017. Open `Developer Command Prompt for VS 2017`, navigate to the project folder and run 

    msbuild ZxRecolor.proj

The resulting files will be stored in the `bin` subfolder. If you have MkDocs installed, you will also get full project documentation accessible via `index.html`.
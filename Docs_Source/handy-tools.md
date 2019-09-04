# Handy tools

While you can manage any recoloring project solely with the help of your favorite graphics editor, there are additional tools aimed to make this job easier. Please check them out.

## UnrealSpeccy Emulator

UnrealSpeccy includes numerous developer-oriented instruments, and some additional means are added to our customized version. Here is a very incomplete list:

- `Ctrl+1`, `Ctrl+2`, `Ctrl+3`: switch between recolored, original, and black-and-white rendering.
- `Alt+F8`: take a screenshot.
- `Ctrl+8`: record a video clip (you will need to have [ffmpeg.exe](files/ffmpeg.zip) in the emulator folder). You can change target video format in `unreal.ini`: search for "ffmpeg encoding options". Also note that the resulting video will be placed into the "current folder", which is `game` if you have already created it.


## hqx

Many emulators improve video output by means of [hqx filter](https://en.wikipedia.org/wiki/Hqx). Hqx-processed screenshots can be a good start for subsequent recoloring work. ZX Recoloring project includes a precompiled version of hqx command line tool. Run it with the original (unscaled) ZX Spectrum screenshot as an input argument to generate a double-sized hqx-improved `output.bmp`

## ZX Color Tool

TODO
<!-- A simple auxiliary instrument called ZX Color Tool is designed to edit sprite pairs conveniently:
The main purpose of the tool is extraction of individual sprites out of a specifically marked-up image. It is quite hard to work with dozens of separate drawings. It is much easier to collect them into a single screen and to draw borders manually. Then ZX Color Tool will be able to save screen fragments as independent sprites automatically. Let's consider elements of the tool's main form:

The Load Image button loads an image with sprites for further processing. In ZX mode the tool doubles the size of the image before displaying it (assuming that a user wants to load a ZX Spectrum screenshot with original-sized sprites).
The button with an arrow (>) adds a new object to the objects collection. The object name should be specified in the textfield to the left of the button.
If a certain object is selected in the Objects list, the bounds of the corresponding screen sprite can be adjusted by left (left-up corner) and right (right-down corner) mouse clicks.
The work with objects list is performed with Del (delete object), New (clear objects list), Load (load another objects list) and Save (save objects list) buttons.
The Split button runs the process of sprites splitting. The ready images will be saved in the subfolder Output.
Since original ZX sprites are rescaled while displaying on the ZX Color Tool's main form, the same objects list can be used both for image with original sprites, and for image with thir recolored counterparts.

In general, the process of game modernization can look as follows:

Place all original Spectrum sprites into zx_game.bmp file.
Double the dimensions of this image and save it as pc_game.bmp.
Using ZX Color Tool, mark-up the original sprites of zx_game.bmp. The name of each object is reasonable to start with prefix zx_. The objects list should be saved in file zx_objects.txt.
Copy the contents of the file zx_objects.txt into the file pc_objects.txt. Edit the file pc_objects.txt by replacing all occurrences of the substring zx_ with the string pc_.
Update the sprites in the file pc_game.bmp.
Now it is possible to extract individual sprites from the collections using Split button.

While updating the graphics, it is neccessary to make sure that each new sprite covers its original counterpart completely. Otherwise some fragments of original sprites would be visible on the screen, too. It is generally not recommended to depart too much from the original palette of moving objects. If the system would be unable to find a sprite due to some reason (for example, if the sprite is heavily covered with other objects), the displayed original sprite would be not so noticeable.

-->
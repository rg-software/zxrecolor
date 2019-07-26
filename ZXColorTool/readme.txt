ZX Color
---

Load Image button: loads a ZX Spectrum screenshot for the future sprite analysis.

X1 mode just shows the screen on the form (used for x2-rescaled screenshots)
X2 mode shows double-sized screen on the form (used for original-sized Speccy screenshots)

> button: adds a new object, specified in the editbox to the left

left mouse click: specify an upper left corner of the current object
right mouse click: specify a lower right corner of the current object
middle mouse click: specify an "anchor" cell for the object

Del button: delete the current object
New button: clear the object list
Load button: load the object list
Save button: save the object list

The format is: every line describes a single object.
Every object is specified as follows:
object-name upper-left-corner lower-right-corner anchor-cell
If anchor-cell is not specified, a value of "far away cell" (5000) is saved

Split button: create a set of sprites in "Output" subdirectory
For each object a sprite (.bmp) file and info (.info) files are created.
The info file contains "anchor" coordinates relative to upper-left corner of the sprite.
If anchor is not specified, its coordinates are (0, 0)
# Tips and tricks

Recoloring can be tricky at times. Here are some ideas to help you to deal with typical situations.

## Block and pixel matching

If you are pretty sure that a certain sprite is always block-aligned, simply use a block-precise rule:

    0 block zx_bow.bmp pc_bow.bmp

| zx_bow.bmp | pc_bow.bmp |
|----------- |-------	  |
| ![](images/zx_bow.png)  | ![](images/pc_bow.png) |   

If you are dealing with a moving object, use a pixel-precise rule, and don't forget about transparency, if the object moves over a non-solid background:

    0 pixel zx_tank.bmp pc_tank.bmp

| zx_tank.bmp | pc_tank.bmp | result  |
|----------- |-------	  |-----	|
| ![](images/zx_tank.png) 	 | ![](images/pc_tank.png) 	  |  ![](images/zx_tank_clip.gif) ![](images/pc_tank_clip.gif) 	|   


<!-- 

Matching partially intersecting sprites

Matching small immovable background sprites

Rearranging screen layout, drawing decoration

-->
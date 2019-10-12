# Writing rules
This section is intended to be a succint yet complete description of rule types and rule syntax. It might be a bit difficult to read it due to lack of practical examples, so make sure to check out [Tips and tricks](tips-and-tricks.md) section as well.

## Settings file

All required game transformations are performed by means of rules, listed in the `game\settings.txt` file. Each line of this file is a separate rule. Empty lines and lines starting with a semicolon symbol are ignored:

```
; This is a comment

<here-comes-my-first-rule>
```

Rules are case-sensitive.

## Sprite replacement rules

Each sprite replacement rule can substitute an original ZX Spectrum sprite found on the screen with its upgraded version. Recolored screen is twice wider and twice higher (512x384) than the original screen (256x192), and imposes no color limits. Sprite replacement rules are described with the following syntax:

```
<layer-number> <rule-type>[|x,y] <original-sprite> <new-sprite> [protected|N]
```

### Layers

If a game has non-blank background, it is necessary to ensure that foreground images (such as game characters) are drawn over the background rather than behind it. This can be achieved with layers: each subsequent layer is drawn only after the previous layer is complete. For example, all the sprites on the layer 1 will be drawn after all the sprites on the layer 0. The `layer-number` element should be an integer.

### Replacement rule types

Currently there are two types of sprite replacement rules: `block` and `pixel`. ZX Spectrum screen is logically divided into 8x8-pixel blocks, having independent color palettes. In practice it means that immovable background elements are typically aligned with the borders of such blocks.

Rules of `block` type search original sprites aligned with block corners only. In contrast, `pixel` rules check every location on the screen. There are two primary reasons to use `block` rules, where it is possible. First, they are much faster, since only block corner locations have to be checked. Second, they help fighting false positive matches. Sometimes, an accidental combination of pixels on the screen can form a "match" for a certain small sprite, causing the rule to trigger. If it is known that the original sprite is always block-aligned, we can minimize the chances of a false positive match.

Rule type can be followed with an optional `|x,y` section, forcing the rule to be applicable to the original sprite located only in the specified position of the screen. This method is handy for detecting static elements displayed in known locations. 

### Original sprite declaration

Original sprite declaration consists of the following elements:

```
<sprite-name>.bmp[|x1,y1,color1[|x2,y2,color2[|...]]]
```

Color palette in ZX Spectrum is specified per 8x8 sprite block rather than per pixel. In practice it often leads to "color clashes" (incorrectly colored areas), apparent in many Spectrum games. For us it means that color matching is unreliable: we cannot be sure that the given sprite will always preserve its color palette.

Therefore, the basic matching procedure assumes that the original sprite does not contain any color information. It should be a 24bpp BMP image, with a width equal to a multiple of 8, and actually containing pixels of these colors only:

- white, `RGB(255, 255, 255)`: background "paper" color;
- black, `RGB(0, 0, 0)`: foreground "ink" color;
- pinkish, `RGB(242, 10, 242)`: transparent color, ignored in matching operations.

Sometimes, however, color information is both reliable and necessary. Thus, there is an option to specify a list of colors of *individual* sprite pixels. For example, the declaration

```
my_sprite.bmp|0,0,red|0,1,yellow
```

specifies that the match for `my_sprite.bmp` must have a black pixel in the top-left corner and a yellow pixel right under it. Color names are taken from the standard Spectrum vocabulary: `black`, `blue`, `red`, `magenta`, `green`, `cyan`, `yellow`, `white`. For brighter colors, add `+` to the color name: `black+`, `blue+`, `red+`, `magenta+`, `green+`, `cyan+`, `yellow+`, `white+`.

### Recolored sprite declaration

Declaring recolored sprites is easier:

```
<new_sprite>.bmp[|dx,dy]
```

Here the optional `|dx,dy` part allows to specify a relative displacement of the new sprite on the screen. For example, if you want to place the new sprite just a bit higher than the original matched fragment, use

```
<new_sprite>.bmp|0,-1
```

Note that the numbers here correspond to the original ZX Spectrum screen dimensions. Thus, "-1" will actually become "two pixels higher" in the final double-size render.

### Protected clause

The `[protected|N]` clause is one of handy tools designed to deal with incomplete matches. It works as follows:

1. The system identifies missing protected sprites, matched on the previous frame.

1. For each protected sprite, the system tries to match at least `N` non-transparent sprite pixels in the original location.

1. If this is operation is successful, the sprite is considered found.

In effect, this technique allows to match immovable background objects when other sprites partially obscure them. Note that both paper and ink pixels participate in matching procedure. Thus, for example, an empty (paper-only) 8x8 block has 32 matching pixels with any half-ink/half-paper block.

## Sound rules

Sound rules let you add your own music and sound effects to the game. Their basic syntax is similar to the syntax of sprite replacement rules:

```
<channel-number> <rule-type>[|x,y] <original-sprite> <sound-sample-file(s)> <event> [flags] 
```

### Channels

Every playback event is associated with an integer channel number. A single channel cannot be shared between two active playback events. If you start playback on the given channel, the previously active playback session of this channel will be interrupted.

Zero channel is "exclusive": initiating a playback on the channel 0 will interrupt all other active playback events.

### Sound rule types

Sound rules are triggered with the same pattern matching routines as sprite replacement rules. Hence, there are two types of sound rules: `sound-block` and `sound-pixel`. The `<original-sprite>` declaration also follows the syntax used in replacement rules.

### Events

Sound playback is initiated when a specified events occurs. Currently, there are two types of sound events:

- `appears`: occurs when the given sprite appears on the screen (it is found now, but was not found on the previous frame);
- `disappears`: occurs when the given sprite disappears from the screen (it is not found now, but was found on the previous frame).

Events are used to initiate a playback; there are no dedicated means for stopping a playback. However, it is possible to play an empty/silent sound file on the given channel to achieve the same outcome.

### Sound samples

A sound sample is a conventional audio file. A variety of formats are supported, including MP3, WAV, and OGG. It is possible to specify a list of `|`-separated samples in one rule. When the rule is triggered, a random sample from the list will be chosen (unless `seq` flag is specified).

### Flags

There are additional flags affecting the work of the sound engine:

- `mute_ay`: AY sound processor will be turned off while the rule is active;
- `mute_beeper`: regular ZX beeper will be turned off while the rule is active;
- `volume|N`: a sound sample will be played with the specified volume (0-100);
- `loop`: a sound sample will be looped until interrupted by another rule; 
- `seq`: samples in a list of samples will be chosen sequentially rather than randomly.

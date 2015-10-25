# CHIP-8 emulator

![pong game](http://i.imgur.com/qZHG5eu.png)

A simple CHIP-8 emulator with basic functionality.

SDL/GL handles the graphics and keyboard controls.

prints a bell character ('\a') to stdout for audio.

## Usage

Run the emulator from the command line, with the filename of a rom as the first argument.

```
c8-emu games/pong.ch8
```

The CHIP-8 hexpad is mapped to a Qwerty keyboard.

| Hexpad  | Qwerty  |
|---------|---------|
| 1 2 3 C | 1 2 3 4 |
| 4 5 6 D | Q W E R |
| 7 8 9 E | A S D F |
| A 0 B F | Z X C V |

## TODO
* Implement Super CHIP-8 instructions
* UI 
  * debugging tools
  * file/rom selection
  * reset
  * screen colour selection 

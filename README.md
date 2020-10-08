# generate_scroller

Create a scrolling animation full of neat little blinkenlights. A small ImageMagick-based script to add some chromatic aberration is included.

* The scrolling pixels represent your given message in binary.
* The current (bottom) line of 8 bits is repeated in a vertical column at the bottom left.
* A blinking scrollbar at the top left shows your progress through the message.
* The top right is an even parity bit for the current byte.
* A binary counter in the bottom right ticks up once per new byte.

```
~/generate_scroller $ gcc scroller_generator.c && ./a.out
Message: Colorless green emotions sleep furiously.
~/generate_scroller $ source coalesce.sh
~/generate_scroller $ rm *frame*
```

<img src="https://github.com/nlc/generate_scroller/raw/master/_example.gif?raw=true">

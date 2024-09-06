# fencli

![fencli example output](https://raw.githubusercontent.com/fgarlin/fencli/master/example_output.png)

**fencli** is a tiny C99 command-line FEN-string visualizer. It takes a chess position in Forsyth-Edwards Notation (FEN) as a command-line argument and prints a board with the given position using Unicode characters and UTF-8 encoding. The program will also detect errors in the first three FEN fields (piece placement, active color and castling availability).

I needed a small command-line utility to review a few FEN-strings, so I built it myself in an afternoon. Yes, I do realize there are (approximately) a million other pieces of software that do the same thing, but this one is mine. :-)

## Cloning, building and usage

This program is only known to work under Linux. The provided Makefile can be used to build the program, although there are no external dependencies and you can manually compile `fencli.c` with a compiler of your choice.

``` sh
git clone https://github.com/fgarlin/fencli && cd fencli
make
./fencli rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

## Customizing

The colors and the Unicode characters that represent the pieces and the board can be easily customized by editing the top of the source file. Colors must be true color (24-bit) RGB values in the form of `{0-255, 0-255, 0-255}`.

## License

This code is released under the MIT license. See LICENSE for more details.

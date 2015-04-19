# Reversi

This is a simple *reversi* game for Windows developed in C++ with [Cinder](http://libcinder.org/).\

## Rules

The rules of the game are quite simple and can be found [here](http://en.wikipedia.org/wiki/Reversi#Rules). It is possible to restart a game at any time by hitting the F4, F6, F8, F10, or F12 keys: hitting the F`n` key will create a new game whose board has size `n`x`n`.

*The cells which are valid candidates for the next move are drawn in a brighter color*, and a thin blue frame is shown around their border as the user moves the mouse cursor over them; the frame is red for cells which are not valid candidates for the next move. By holding the 'Ctrl' key it is also possible to display a semi-transparent token below the mouse cursor:

![Next move screenshot](https://github.com/andyprowl/reversi/blob/basic/screenshots/screenshot02.jpg)

If an invalid cell is picked for placing the next token, a message will appear asking the user to choose a different cell.

The game ends when no player is allowed to perform any move that would results in at least one reversal - the most common reason being that the board is full, as new tokens cannot be placed on cells that are already occupied. When the game is over, no further tokens can be placed and the player who has most tokens on the board wins:  

![Game over screenshot](https://github.com/andyprowl/reversi/blob/basic/screenshots/screenshot01.jpg)

## Limitations

At the moment, neither AI nor online multi-player mode are supported. Both features are planned for future versions.

## Installation instructions

The quickest way to play Reversi is to:

 1. Download the [`reversi.zip`](https://github.com/andyprowl/reversi/raw/basic/deploy/reversi.zip) file from the `/deploy` subfolder in this repository.
 2. Unzip it to some local folder. 
 3. Start the `reversi.exe` executable from the unzipped `/reversi` folder.

Alternatively, you can build the program from the sources. To build the projects you will need Microsoft Visual Studio 2013 and [Cinder 0.8.6](http://libcinder.org/blog/posts/9_cinder-086-released/).

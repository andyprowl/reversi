# Reversi

This is a simple *reversi* game for Windows developed in C++ with [Cinder](http://libcinder.org/).

![Game over screenshot](https://github.com/andyprowl/reversi/blob/basic/screenshots/screenshot01.jpg)

## Rules

The rules of the game are quite simple and can be found [here](http://en.wikipedia.org/wiki/Reversi#Rules). It is possible to restart a game at any time by hitting the F4, F6, F8, F10, or F12 keys: hitting the F`n` key will create a new game whose board has size `n`x`n`.

## Limitations

At the moment, neither AI nor online multi-player mode are supported. Both features are planned for future versions.

## Installation instructions

The quickest way to play Reversi is to:

 1. Download the [`reversi.zip`](https://github.com/andyprowl/reversi/raw/basic/deploy/reversi.zip) file from the `/deploy` subfolder in this repository.
 2. Unzip it to some local folder. 
 3. Start the `reversi.exe` executable from the unzipped `/reversi` folder.

Alternatively, you can build the program from the sources. To build the projects you will need Microsoft Visual Studio 2013 and [Cinder 0.8.6](http://libcinder.org/blog/posts/9_cinder-086-released/).

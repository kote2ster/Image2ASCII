Image2ASCII
===========

Capable of converting .PNG and .PPM images to ASCII art

Put any image into ./images/ folder

This program is purely written in C

Features:
----------
- Resizes CMD to maximum resolution, so a bigger ASCII art is generated
- The ASCII pictures always fit in the CMD window, even when you resize it
- Has a dynamic menu, which lists folders and pic files in ./images/
- Put your .PNG and .PPM images and folders in ./images/
- You can set a loop number which generates more pictures successively
- It is advised to put extracted .gif pictures (PNGs) in one folder and then loop can be used for animation!
- You can redirect the output from Console window to output file
- You can resize the ASCII picture
- It supports Extended and Basic ASCII Character Sets too

For compiling:
---------

- Used external library: PDCURSES (Windows) - http://pdcurses.sourceforge.net/
  * Building instructions for Windows:
  * Download pdcurs34.zip
  * in cmd.exe (or in msys) set PDCURSES_SRCDIR = \<main pdcurses root folder\>
  * then set environment variable (path) to C:\MinGW\bin
  * then in pdcurses root folder: mingw32-make.exe -f .\win32\mingwin32.mak
  * after it is finished you have to include pdcurses.a in your project's linker (and curses.h header is in main pdcurses root folder)

Others:
---------
- Used external functions for decoding .PNG images: LodePNG - http://lodev.org/lodepng/
- Used external exe for creating Character Tables: png2charset - http://www.piclist.com/techref/datafile/charset/extractor/charset_extractor.htm

##### The main OS target was Windows, but should be working on other systems too (the windows only functions are in seperate files)

/**
 * @file Image2ASCIIcolors.h
 * @brief Header file for settings and includes
 */

#ifndef IMAGE2ASCIICOLORS_H_INCLUDED
#define IMAGE2ASCIICOLORS_H_INCLUDED

#include "libtcod.hpp"
#include "lodepng.h"
#include <string>
#include <math.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
/*-----------------------SETTINGS-----------------------------*/
#define ConsoleSize_X 160
#define ConsoleSize_Y 90
#define PATH_LONG 100
#define ROOT_FOLDER "h:/Dropbox/Public/source_codes/c/Image2ASCII/images"
//#define BlockCharsOnly
//#define ChangeBlackWhite
//#define ExtendedASCIICharSet
#define PreciseASCIIgen
/*------------------------------------------------------------*/
#define FontSize_X 8
#define FontSize_Y 8
#define CHAR_X 8
#define CHAR_Y 8
#ifdef ExtendedASCIICharSet
#define CHAR_NUM 255 /* Extended ASCII */
#else
#define CHAR_NUM 95  /* Standard ASCII */
#endif /* ExtendedASCIICharSet */
#include "LibtcodFunctions.h"
#include "Functions.h"
/**
 * @def ConsoleSize_X
 *   @brief Width of the Console Window (in chars)
 * @def ConsoleSize_Y
 *   @brief Heigth of the Console Window (in chars)
 * @def PATH_LONG
 *   @brief Max how long is the path to the file
 * @def ROOT_FOLDER
 *   @brief Root folder of the images
 * @def BlockCharsOnly
 *   @brief Define this to WriteOut every ascii picture with only (whole) block characters
 * @def ChangeBlackWhite
 *   @brief Define this if you want to set the background color to white (instead of black)
 * @def ExtendedASCIICharSet
 *   @brief Define this to enable extended ASCII support
 * @def PreciseASCIIgen
 *   @brief Define this to generate more accurate ASCII pictures (whole sizes are calculated) and CONSOLEINFO.FontSize will be = {CHAR_X,CHAR_Y} \n
            so pictures (to fit in max console screen) should be: CHAR_Y*(Con.CharAmount.Y - 2) high and CHAR_X*(Con.CharAmount.X - 1) wide
 *   @see CalculateImageSizesPrec()
 * @def FontSize_X
 *   @brief Width of a character in Console Window
 * @def FontSize_Y
 *   @brief Height of a character in Console Window
 * @def CHAR_X
 *   @brief Width  of one ASCII Char
 * @def CHAR_Y
 *   @brief Height of one ASCII Char
 * @def CHAR_NUM
 *   @brief Number of ASCII Characters to choose from
**/

#endif // IMAGE2ASCIICOLORS_H_INCLUDED

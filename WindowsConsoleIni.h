/**
 * @file WindowsConsoleIni.h
 * @brief Header of Windows only functions
 * @def WINXP
 * @brief if defined: omitting not for xp function
 * @def BASIGMINGW
 * @brief if defined: you are using basic mingw-w32-0.6.2 with WIN32API 3.17
 */
#ifndef WINDOWSCONSOLEINI_H_INCLUDED
#define WINDOWSCONSOLEINI_H_INCLUDED

/*#define WINXP      if defined: omitting not for xp functions */
#define BASIGMINGW /*if defined: you are using basic mingw-w32-0.6.2 with WIN32API 3.17 */

#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */
/** @brief SIZES struct */
typedef struct
{
    int X; /**< X: width */
    int Y; /**< Y: Height */
} SIZES;
/** @brief CONSOLEINFO struct */
typedef struct
{
    SIZES FontSize;   /**< Console window font sizes */
    SIZES CharAmount; /**< Console window char amount per rows and columns */
    SIZES Size;       /**< Console window size in pixels */
} CONSOLEINFO;

void SetConsoleWindowFullScreen(SIZES *CharAmount);
void GetConsoleDimensions(CONSOLEINFO *Con);
void SetConsoleFont(SIZES FontSize);

#endif /* WINDOWSCONSOLEINI_H_INCLUDED */

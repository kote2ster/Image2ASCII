#ifndef WINDOWSCONSOLEINI_H_INCLUDED
#define WINDOWSCONSOLEINI_H_INCLUDED

//#define WINXP //if defined: omitting not for xp functions
#define BASIGMINGW //if defined: you are using basic mingw-w32-0.6.2 with WIN32API 3.17
#include <windows.h>

typedef struct
{
    int X;
    int Y;
} SIZES;
typedef struct
{
    SIZES FontSize;
    SIZES CharAmount; //Console window char amount per rows and columns
    SIZES Size; //Console window size in pixels
} CONSOLEINFO;

void SetConsoleWindowFullScreen(SIZES *CharAmount);
void GetConsoleDimensions(CONSOLEINFO *Con);
void SetConsoleFont(SIZES FontSize);

#endif // WINDOWSCONSOLEINI_H_INCLUDED

#ifndef TEXTSCREENINI_H_INCLUDED
#define TEXTSCREENINI_H_INCLUDED

#include <curses.h> //External library PDCURSES.DLL for windows
#include <string.h>

typedef struct
{
    char **arr;
    int arr_size;
} CHAR2DARRAY;

WINDOW* Screens();

void InitialisePDCURSES(size_t ConsoleWidth,size_t ConsoleHeight,WINDOW **stdwindow);
void initcolors();

//Returns choice (index) number
int MenuChoices(CHAR2DARRAY choices,CHAR2DARRAY options,int ConSizeY,WINDOW *stdwindow);
int CheckIfExit(WINDOW *stdwindow,int choice);
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

#endif // TEXTSCREENINI_H_INCLUDED

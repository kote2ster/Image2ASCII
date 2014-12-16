/**
 * @file TextScreenIni.h
 * @brief Header of Curses initialisating funtions
 */
#ifndef TEXTSCREENINI_H_INCLUDED
#define TEXTSCREENINI_H_INCLUDED

#include <curses.h> /* External library NCURSES (PDCURSES for Windows) */
#include <string.h>
/** @brief Custom char type for a 2D array */
typedef struct
{
    char **arr;  /**< Dynamic 2D char array */
    int arr_size;/**< Size of the array */
} CHAR2DARRAY;

void InitialisePDCURSES(size_t ConsoleWidth,size_t ConsoleHeight,WINDOW **stdwindow);
void initcolors();

int MenuChoices(CHAR2DARRAY choices,CHAR2DARRAY options,int ConSizeY,WINDOW *stdwindow);
int CheckIfExit(WINDOW *stdwindow,int choice);
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

#endif /* TEXTSCREENINI_H_INCLUDED */

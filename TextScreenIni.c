/**
 * @file TextScreenIni.c
 * @brief Implementations of Curses funtions
 */
#include "TextScreenIni.h"

/*------------------------Initialisers-------------------------*/
/**
* @brief Main initalisation for PDCURSES (Curses.h)
* @param [in] ConsoleWidth Width of Console (in chars)
* @param [in] ConsoleHeight Height of Console (in chars)
* @param [out] stdwindow Outs the standard window output, see [Curses Tutorial](http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)
*/
void InitialisePDCURSES(size_t ConsoleWidth,size_t ConsoleHeight,WINDOW **stdwindow)
{
    (*stdwindow) = initscr(); /* Getting standard ncurses window handler */
    resize_term(ConsoleHeight,ConsoleWidth); /* Resizing terminal... */
    noecho();
    cbreak();    /* getch() waits for one character */
    start_color();
    curs_set(0); /* Cursor blinking OFF */
    keypad((*stdwindow),TRUE); /* Enable all keyboard inputs */
    clear();
    refresh();

    initcolors(); /* Initializing COLOR_PAIRS */
}
/**
* @brief Initialising colors... setting to Black-White default
*/
void initcolors()
{
    init_pair(1,COLOR_WHITE,COLOR_BLACK);
    bkgd(COLOR_PAIR(1)); /* Entire Console screen White-Black */
    /*   init_pair(2,COLOR_BLUE,COLOR_GREEN);
     *   init_pair(3,COLOR_RED,COLOR_WHITE);
     */
}
/*-------------------------------------------------------------*/
/**
* @brief Main listing function which lists all filenames and options in screen
* @param [in] choices File/Foldername choices
* @param [in] options Optional choices
* @param [in] ConSizeY Size of Console in chars
* @return index of the chosen option
*/
int MenuChoices(CHAR2DARRAY choices,CHAR2DARRAY options,int ConSizeY,WINDOW *stdwindow)
{
    int ch; /* Input management (getch()) */
    int i,y,x;
    int arr_relative_index = 0;
    int arr_index = 0;
    int MENU_Y_ADD = 3;      /* Lower whole Choice Menu */
    int LEFT_MENU_BOUND = 3; /* Move right whole Choice Menu */
    int HIGH_MENU_BOUND = MENU_Y_ADD;
    int LOW_MENU_BOUND = MENU_Y_ADD+choices.arr_size-1;
    int MoreChoices = 0; /* Check if there are more options which do not fit in screen */
    y=HIGH_MENU_BOUND;
    x=LEFT_MENU_BOUND;
    if(ConSizeY<=(MENU_Y_ADD+choices.arr_size+options.arr_size))
    {
        LOW_MENU_BOUND = ConSizeY-1-options.arr_size-1;
        MoreChoices = 1;
    }

    /*    stdwindow = create_newwin();  //Create NEW SCREEN (UPDATES stdscr) */

    /*--------------Writing out Menu Choices-------------*/
    /* Writing out Choices */
    for (i=0; i<LOW_MENU_BOUND+1-MENU_Y_ADD; i++)
    {
        mvwprintw(stdwindow,y+i,x,"  %s",choices.arr[i]);
        clrtoeol(); /* Clear to end of line */
    }
    mvwchgat(stdwindow,y, x+2, strlen(choices.arr[y-HIGH_MENU_BOUND]), A_BLINK, 0, NULL);
    mvwprintw(stdwindow,y,x,"->");
    if(MoreChoices)
    {
        mvwprintw(stdwindow,LOW_MENU_BOUND-1,x, "||");
        mvwprintw(stdwindow,LOW_MENU_BOUND  ,x,"\\/");
    }
    /* Writing out OPTIONS */
    for(i=0; i<options.arr_size; i++)
    {
        mvwprintw(stdwindow,LOW_MENU_BOUND+2+i,x,"%s",options.arr[i]);
        clrtoeol(); /* Clear to end of line */
    }
    wrefresh(stdwindow);
    /*---------------------------------------------------*/
    do
    {
        /* Wait for user input... then dehighlight prev. selection, remove '->' then highlight choice, add '->' */
        ch = wgetch(stdwindow);

        mvwchgat(stdwindow,y, x+2, strlen(choices.arr[arr_index+arr_relative_index]), A_NORMAL, 0, NULL);
        mvwprintw(stdwindow,y,x,"  ");
        switch(ch)
        {
        case KEY_DOWN:
            if(y!=LOW_MENU_BOUND)
            {
                y++;
                arr_relative_index++;
            }
            else
            {
                y = HIGH_MENU_BOUND;
                arr_relative_index = 0;
                arr_index = 0;
            }
            break;
        case KEY_UP:
            if(y!=HIGH_MENU_BOUND)
            {
                y--;
                arr_relative_index--;
            }
            else
            {
                y = LOW_MENU_BOUND;
                arr_relative_index = LOW_MENU_BOUND-HIGH_MENU_BOUND;
                arr_index = choices.arr_size-1-LOW_MENU_BOUND+HIGH_MENU_BOUND;
            }
            break;
        }
        if(y==HIGH_MENU_BOUND+2 && arr_index>0)
        {
            arr_index--;
            arr_relative_index++;
            y++;
        }
        if(y==LOW_MENU_BOUND-2 && arr_index+LOW_MENU_BOUND-HIGH_MENU_BOUND<choices.arr_size-1)
        {
            arr_index++;
            arr_relative_index--;
            y--;
        }
        if(arr_index!=0)
        {
            mvwprintw(stdwindow,HIGH_MENU_BOUND  ,x,"/\\");
            mvwprintw(stdwindow,HIGH_MENU_BOUND+1,x,"||" );
        }
        else
        {
            mvwprintw(stdwindow,HIGH_MENU_BOUND  ,x,"  ");
            mvwprintw(stdwindow,HIGH_MENU_BOUND+1,x,"  ");
        }
        if(arr_index+LOW_MENU_BOUND-HIGH_MENU_BOUND!=choices.arr_size-1)
        {
            mvwprintw(stdwindow,LOW_MENU_BOUND -1,x, "||");
            mvwprintw(stdwindow,LOW_MENU_BOUND   ,x,"\\/");
        }
        else
        {
            mvwprintw(stdwindow,LOW_MENU_BOUND -1,x,"  ");
            mvwprintw(stdwindow,LOW_MENU_BOUND   ,x,"  ");
        }
        /* Writing out Choices */
        for (i=0; i<LOW_MENU_BOUND+1-MENU_Y_ADD; i++)
        {
            mvwprintw(stdwindow,HIGH_MENU_BOUND+i,x+2,"%s",choices.arr[arr_index+i]);
            clrtoeol(); /* Clear to end of line */
        }
        mvwchgat(stdwindow,y,x+2, strlen(choices.arr[arr_index+arr_relative_index]), A_BLINK, 0, NULL);
        mvwprintw(stdwindow,y,x,"->");
        wrefresh(stdwindow);
        if(CheckIfExit(stdwindow,ch)) return KEY_EXIT; /* Esc pressed */
    }
    while(ch!='\n' && (ch<'0'||ch>'9') ); /* While ENTER or 0<=ch<=9 */

    wclear(stdwindow);
    wmove(stdwindow,0,0);
    wrefresh(stdwindow);

    if (ch>='0'&&ch<='9')
        return -(ch-'0'); /* returning optional choice (negated) */

    return arr_index+arr_relative_index; /* returning choice number... */
    /* mvwprintw(stdwindow,LOW_MENU_BOUND+2,x,"CHOICE: %i",y+1-HIGH_MENU_BOUND); // Writing out selected choice */
    /* wrefresh(stdwindow);
     * wgetch(stdwindow);
     */
}
/**
* @brief Creates new screen (window)
* @param [in] height
* @param [in] width
* @param [in] starty
* @param [in] startx
*/
WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0); /* 0, 0 gives default characters
                            * for the vertical and horizontal
                            * lines			*/
    wrefresh(local_win);   /* Show that box 		*/

    return local_win;
}
/**
* @brief Destroys screen (window)
* @param [in] local_win
*/
void destroy_win(WINDOW *local_win)
{
    /* box(local_win, ' ', ' '); : This won't produce the desired
     * result of erasing the window. It will leave it's four corners
     * and so an ugly remnant of window.
     */
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    /* The parameters taken are
     * 1. win: the window on which to operate
     * 2. ls: character to be used for the left side of the window
     * 3. rs: character to be used for the right side of the window
     * 4. ts: character to be used for the top side of the window
     * 5. bs: character to be used for the bottom side of the window
     * 6. tl: character to be used for the top left corner of the window
     * 7. tr: character to be used for the top right corner of the window
     * 8. bl: character to be used for the bottom left corner of the window
     * 9. br: character to be used for the bottom right corner of the window
     */
    wrefresh(local_win);
    delwin(local_win);
}
/**
* @brief Checks if Exit was pressed
* @param [in] stdwindow in the standard window
* @param [in] choice which was pressed
* @return true (1) if exit was pressed
*/
int CheckIfExit(WINDOW *stdwindow,int choice)
{
    if(choice==27)
    {
        nodelay(stdwindow,1);
        choice=getch();
        if(choice==ERR)
        {
            nodelay(stdwindow,0);
            return 1; /* Escape button pressed */
        }
    }
    nodelay(stdwindow,0);
    return 0; /* Not Escape button */
}

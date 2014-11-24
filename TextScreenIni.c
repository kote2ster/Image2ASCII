#include "TextScreenIni.h"

WINDOW* Screens()
{
    WINDOW* new_windows;
    new_windows = newwin(LINES/2-10,COLS/2-40,LINES -LINES/2+10 -2,COLS -COLS/2+40 -2); //New Window params:(height,width,stary,starx)
    box(new_windows,' ',' ');
    wrefresh(new_windows);
    keypad(new_windows,TRUE); //Re-Enable all keyboard inputs
    return new_windows;
}
/*------------------------Initialisers-------------------------*/
void InitialisePDCURSES(size_t ConsoleWidth,size_t ConsoleHeight,WINDOW **stdwindow)
{
    (*stdwindow) = initscr();
    resize_term(ConsoleHeight,ConsoleWidth); //Resizing terminal...
    noecho();
    cbreak(); //getch() waits for one character
    start_color();
    curs_set(0); //Cursor blinking OFF
    keypad((*stdwindow),TRUE); //Enable all keyboard inputs
    clear();
    refresh();

    initcolors(); //Initializing COLOR_PAIRS
}
void initcolors()
{
    init_pair(1,COLOR_WHITE,COLOR_BLACK);
    bkgd(COLOR_PAIR(1)); //Entire Console screen White-Black
//    init_pair(2,COLOR_BLUE,COLOR_GREEN);
//    init_pair(3,COLOR_RED,COLOR_WHITE);
}
/*-------------------------------------------------------------*/
int MenuChoices(CHAR2DARRAY choices,CHAR2DARRAY options,int ConSizeY,WINDOW *stdwindow)
{
    int ch;
    int i;
    int arr_relative_index = 0;
    int arr_index = 0;
    int MENU_Y_ADD = 3; //Lower whole Choice Menu
    int HIGH_MENU_BOUND = MENU_Y_ADD;
    int LEFT_MENU_BOUND = 3; //Move right whole Choice Menu
    int LOW_MENU_BOUND = MENU_Y_ADD+choices.arr_size-1;
    int y=HIGH_MENU_BOUND;
    int x=LEFT_MENU_BOUND;
    int MoreChoices = 0;
    if(ConSizeY<=(MENU_Y_ADD+choices.arr_size+options.arr_size))
    {
        LOW_MENU_BOUND = ConSizeY-1-options.arr_size-1;
        MoreChoices = 1;
    }

//    stdwindow = Screens(); //Create NEW SCREEN (UPDATES stdscr)

    /*--------------Writing out Menu Choices-------------*/
    for (i=0; i<LOW_MENU_BOUND+1-MENU_Y_ADD; i++)
    {
        mvwprintw(stdwindow,y+i,x,"  %s",choices.arr[i]); //Writing out Choices
        clrtoeol(); //Clear to end of line
    }
    mvwchgat(stdwindow,y, x+2, strlen(choices.arr[y-HIGH_MENU_BOUND]), A_BLINK, 0, NULL);
    mvwprintw(stdwindow,y,x,"->");
    if(MoreChoices)
    {
        mvwprintw(stdwindow,LOW_MENU_BOUND-1,x, "||");
        mvwprintw(stdwindow,LOW_MENU_BOUND  ,x,"\\/");
    }
    for(i=0; i<options.arr_size; i++)
    {
        mvwprintw(stdwindow,LOW_MENU_BOUND+2+i,x,"%s",options.arr[i]); //Writing out OPTIONS
        clrtoeol(); //Clear to end of line
    }
    wrefresh(stdwindow);
    /*---------------------------------------------------*/
    do
    {
        ch = wgetch(stdwindow); //Wait for user input... then dehighlight prev. selection, remove '->' then highlight choice, add '->'

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
        for (i=0; i<LOW_MENU_BOUND+1-MENU_Y_ADD; i++)
        {
            mvwprintw(stdwindow,HIGH_MENU_BOUND+i,x+2,"%s",choices.arr[arr_index+i]); //Writing out Choices
            clrtoeol(); //Clear to end of line
        }
        mvwchgat(stdwindow,y,x+2, strlen(choices.arr[arr_index+arr_relative_index]), A_BLINK, 0, NULL);
        mvwprintw(stdwindow,y,x,"->");
        wrefresh(stdwindow);
        if(CheckIfExit(stdwindow,ch)) return KEY_EXIT; //Esc pressed
    }
    while(ch!='\n' && (ch<'0'||ch>'9') ); //While ENTER or 0<=ch<=9

    wclear(stdwindow);
    wmove(stdwindow,0,0);
    wrefresh(stdwindow);

    if (ch>='0'&&ch<='9')
        return -(ch-'0'); //returning optional choice (negated)

    return arr_index+arr_relative_index; //returning choice number...
//    mvwprintw(stdwindow,LOW_MENU_BOUND+2,x,"CHOICE: %i",y+1-HIGH_MENU_BOUND); //Writing out selected choice
//    wrefresh(stdwindow);
//    wgetch(stdwindow);
}
WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);		/* 0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
    wrefresh(local_win);		/* Show that box 		*/

    return local_win;
}
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
int CheckIfExit(WINDOW *stdwindow,int choice)
{
    if(choice==27)
    {
        nodelay(stdwindow,1);
        choice=getch();
        if(choice==ERR)
        {
            nodelay(stdwindow,0);
            return 1; //Escape
        }
    }
    nodelay(stdwindow,0);
    return 0; //Not escape
}

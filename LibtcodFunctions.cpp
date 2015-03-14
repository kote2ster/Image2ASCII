#include "Image2ASCIIcolors.h"

/**
* @brief Main listing function which lists all filenames and options in screen
* @param [in] choices File/Foldername choices
* @param [in] options Optional choices
* @param [in] ConSizeY Size of Console in chars
* @return index of the chosen option
*/
int MenuChoicesLibt(CHAR2DARRAY choices,CHAR2DARRAY options,int ConSizeY)
{
    TCOD_key_t ch; /* Input management (getch()) */
    int i;
    int arrRelIndex = 0;
    int arr_index = 0;
    int MENU_Y_ADD = 3;      /* Lower whole Choice Menu */
    int LEFT_MENU_BOUND = 3; /* Move right whole Choice Menu */
    int HIGH_MENU_BOUND = MENU_Y_ADD;
    int LOW_MENU_BOUND = MENU_Y_ADD+choices.arr_size-1;
    int MoreChoices = 0; /* Check if there are more options which do not fit in screen */

    TCODConsole::root->clear();

    if(ConSizeY<=(HIGH_MENU_BOUND+choices.arr_size+options.arr_size))
    {
        LOW_MENU_BOUND = ConSizeY-1-options.arr_size-1;
        MoreChoices = 1;
    }

    /*--------------Writing out Menu Choices-------------*/
    if(MoreChoices)
    {
        TCODConsole::root->print(LEFT_MENU_BOUND,LOW_MENU_BOUND-1, "||");
        TCODConsole::root->print(LEFT_MENU_BOUND,LOW_MENU_BOUND  ,"\\/");
    }
    /* Writing out OPTIONS */
    for(i=0; i<options.arr_size; i++)
    {
        TCODConsole::root->print(LEFT_MENU_BOUND,LOW_MENU_BOUND+2+i,"%s",options.arr[i]);
    }
    /*---------------------------------------------------*/
    do
    {
        TCODConsole::root->print(LEFT_MENU_BOUND,HIGH_MENU_BOUND+arrRelIndex,"->");
        /* Writing out Choices */
        for (i=0; i<LOW_MENU_BOUND-HIGH_MENU_BOUND+1; i++)
        {
            if(i==arrRelIndex) TCODConsole::root->setDefaultForeground(TCODColor::red);
            TCODConsole::root->print(LEFT_MENU_BOUND+2,HIGH_MENU_BOUND+i,"%s",choices.arr[arr_index+i]);
            TCODConsole::root->rect (LEFT_MENU_BOUND+2+strlen(choices.arr[arr_index+i]),HIGH_MENU_BOUND+i,ConsoleSize_X,1,true); /* Clear to end of line */
            TCODConsole::root->setDefaultForeground(TCODColor::white);
        }
        /* Wait for user input... then dehighlight prev. selection, remove '->' then highlight choice, add '->' */
        TCODConsole::root->flush();
        ch = TCODConsole::root->waitForKeypress(false);
        TCODConsole::root->print(LEFT_MENU_BOUND,HIGH_MENU_BOUND+arrRelIndex,"  "); /* Remove previous '->' */
        switch(ch.vk)
        {
        case TCODK_DOWN:
            if(HIGH_MENU_BOUND+arrRelIndex==LOW_MENU_BOUND-3 && arr_index+LOW_MENU_BOUND-HIGH_MENU_BOUND<choices.arr_size-1)
            {
                arr_index++;
            }
            else if(HIGH_MENU_BOUND+arrRelIndex!=LOW_MENU_BOUND)
            {
                arrRelIndex++;
            }
            else
            {
                arrRelIndex = 0;
                arr_index = 0;
            }
            break;
        case TCODK_UP:
            if(HIGH_MENU_BOUND+arrRelIndex==HIGH_MENU_BOUND+3 && arr_index>0)
            {
                arr_index--;
            }
            else if(HIGH_MENU_BOUND+arrRelIndex!=HIGH_MENU_BOUND)
            {
                arrRelIndex--;
            }
            else
            {
                arrRelIndex = LOW_MENU_BOUND-HIGH_MENU_BOUND;
                arr_index = choices.arr_size-1-arrRelIndex;
            }
            break;
        default:
            break;
        }
        if(arr_index>0)
        {
            TCODConsole::root->print(LEFT_MENU_BOUND,HIGH_MENU_BOUND  ,"/\\");
            TCODConsole::root->print(LEFT_MENU_BOUND,HIGH_MENU_BOUND+1,"||" );
        }
        else
        {
            TCODConsole::root->print(LEFT_MENU_BOUND,HIGH_MENU_BOUND  ,"  ");
            TCODConsole::root->print(LEFT_MENU_BOUND,HIGH_MENU_BOUND+1,"  ");
        }
        if(arr_index+LOW_MENU_BOUND-HIGH_MENU_BOUND<choices.arr_size-1)
        {
            TCODConsole::root->print(LEFT_MENU_BOUND,LOW_MENU_BOUND -1, "||");
            TCODConsole::root->print(LEFT_MENU_BOUND,LOW_MENU_BOUND   ,"\\/");
        }
        else
        {
            TCODConsole::root->print(LEFT_MENU_BOUND,LOW_MENU_BOUND -1,"  ");
            TCODConsole::root->print(LEFT_MENU_BOUND,LOW_MENU_BOUND   ,"  ");
        }
        TCODConsole::root->flush();

        if(ch.vk==TCODK_ESCAPE || TCODConsole::isWindowClosed()) return EXIT; /* Esc pressed */
    }
    while(ch.vk!=TCODK_ENTER && (ch.c<'0'||ch.c>'9') ); /* While ENTER or 0<=ch<=9 */

    TCODConsole::root->clear();

    if (ch.c>='0'&&ch.c<='9')
        return -(ch.c-'0'); /* returning optional choice (negated) */

    return arr_index+arrRelIndex; /* returning choice number... */
}

/**
* @brief LIBTCOD input handler function
* @param mode InputModes enum
* @param x Left position of input data
* @param y Top position of input data
* @return String data (string or number)
*/
std::string InputHandler(InputModes mode,int x,int y)
{
    int CharNum=0;
    std::string Data="";
    TCOD_key_t input;
    switch(mode)
    {
    case INPUT_NUMBER:
        do
        {
            input = TCODConsole::root->waitForKeypress(false);
            if(input.c >= '0' && input.c <= '9')
            {
                TCODConsole::root->print(x+CharNum,y,"%c",input.c);
                CharNum++;
                Data += input.c;
            }
            else if(input.vk == TCODK_BACKSPACE)
            {
                Data = Data.substr(0,Data.length() - 1); /* Remove last element */
                CharNum--;
                TCODConsole::root->print(x+CharNum,y," ");
            }
            if(input.vk == TCODK_ENTER)
            {
                return Data;
            }
            TCODConsole::root->flush();
        }
        while(input.vk != TCODK_ESCAPE);
        break;
    case INPUT_STRING:
        do
        {
            input = TCODConsole::root->waitForKeypress(false);
            if(isprint(input.c))
            {
                TCODConsole::root->print(x+CharNum,y,"%c",input.c);
                CharNum++;
                Data += input.c;
            }
            else if(input.vk == TCODK_BACKSPACE)
            {
                Data = Data.substr(0,Data.length() - 1); /* Remove last element */
                CharNum--;
                TCODConsole::root->print(x+CharNum,y," ");
            }
            if(input.vk == TCODK_ENTER)
            {
                return Data;
            }
            TCODConsole::root->flush();
        }
        while(input.vk != TCODK_ESCAPE);
        break;
    default:
        break;
    }
    return "";
}

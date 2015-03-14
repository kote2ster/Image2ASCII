/**
 * @file WindowsConsoleIni.c
 * @brief Implementations of Windows only functions
 */
#include "WindowsConsoleIni.h"

/**
* @brief (Only for Windows) Sets the Console window to full screen
* @param [out] CharAmount how many chars fits in screen (vertically and horizontally)
*/
void SetConsoleWindowFullScreen(SIZES *CharAmount) {
    HANDLE outcon;
    COORD consize;
    SMALL_RECT Rect; /* Rectangle STRUCT for setting Window Size */
#ifndef BASIGMINGW
    HWND consoleHWND;
    RECT r;
#endif /* BASIGMINGW */
    outcon = GetStdHandle(STD_OUTPUT_HANDLE);    /* Get Console Window Handle */
    /* Get console max rows number and max columns number (number of chars fit in window) */
    consize=GetLargestConsoleWindowSize(outcon);
    /* -1 and -2 adjustments to fit in screen perfectly */
    CharAmount->Y = consize.Y-1;
    CharAmount->X = consize.X-2;

    SetConsoleScreenBufferSize(outcon, consize); /* Set Buffer Size */

    Rect.Top = 0;
    Rect.Left = 0;
    /* BufferSize-1 to fit perfectly */
    Rect.Bottom = consize.Y - 1;
    Rect.Right = consize.X - 1;

    SetConsoleWindowInfo(outcon, TRUE, &Rect);   /* Set Window Size */

#ifndef BASIGMINGW
    consoleHWND = GetConsoleWindow(); /* Get Console Window exHandle */

    GetWindowRect(consoleHWND, &r);   /* Get the console's current dimensions (in pixels) */
    /* Move Window to (0;0) position, size: Console Size in pixels */
    MoveWindow(consoleHWND, 0, 0, r.right, r.bottom, TRUE);

    /*    ShowWindow(consoleHWND,SW_MAXIMIZE); */
#endif /* BASIGMINGW */
}
/**
* @brief (Only for Windows) Gets the new window sizes
* @param [out] Con sets in CONSOLEINFO type
*/
void GetConsoleDimensions(CONSOLEINFO *Con) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
#ifndef BASIGMINGW
    HWND consoleHWND;
    RECT r;
#endif /* BASIGMINGW */
    /* Get info, and get the resized console size parameters (columns, rows) */
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    Con->CharAmount.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    Con->CharAmount.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#ifndef BASIGMINGW /* Getting Console size in pixels with WINAPI */
    consoleHWND = GetConsoleWindow(); /* Get Console Window exHandle */

    GetWindowRect(consoleHWND, &r);   /* Get the console's current dimensions (in pixels) */

    Con->Size.X = r.right;
    Con->Size.Y = r.bottom;
#else /* Else calculate with FontSize*Con_CharAmount */
    Con->Size.X = Con->FontSize.X * Con->CharAmount.X;
    Con->Size.Y = Con->FontSize.Y * Con->CharAmount.Y;
#endif /* BASIGMINGW */
}
/**
* @brief (Only for Windows and for newer WINAPI) Sets the Console font size to the defined FontSize_X and FontSize_Y
* @param [in] FontSize defined font size informations
*/
void SetConsoleFont(SIZES FontSize) {
#if !defined(WINXP) && !defined(BASIGMINGW) /* Minimum supported client: Windows Vista */
    HANDLE outcon;
    CONSOLE_FONT_INFOEX font;

    outcon = GetStdHandle(STD_OUTPUT_HANDLE);      /* Getting windows console standard handler */
    font = {sizeof(CONSOLE_FONT_INFOEX)};          /* CONSOLE_FONT_INFOEX is defined in some windows header */
    GetCurrentConsoleFontEx(outcon, false, &font); /* Getting current font settings */
    font.FontFamily = 54;       /* Lucida console font */
    font.dwFontSize.X = FontSize.X;
    font.dwFontSize.Y = FontSize.Y;
    SetCurrentConsoleFontEx(outcon, false, &font); /* Setting font settings */
#endif /* WINXP, BASIGMINGW */
}

#include "WindowsConsoleIni.h"

void SetConsoleWindowFullScreen(SIZES *CharAmount)
{
    HANDLE outcon = GetStdHandle(STD_OUTPUT_HANDLE);      // Get Handle

    COORD consize=GetLargestConsoleWindowSize(outcon);    // Get console max rows number and max columns number (number of chars fit in window)

    CharAmount->Y = consize.Y-1;
    CharAmount->X = consize.X-2;

    SetConsoleScreenBufferSize(outcon, consize);            // Set Buffer Size

    SMALL_RECT Rect; // Rectangle for setting Window Size; BufferSize-1 to fit perfectly
    Rect.Top = 0;
    Rect.Left = 0;
    Rect.Bottom = consize.Y - 1;
    Rect.Right = consize.X - 1;

    SetConsoleWindowInfo(outcon, TRUE, &Rect);            // Set Window Size

#ifndef BASIGMINGW
    HWND consoleHWND = GetConsoleWindow();                // Get Console Window Handle

    RECT r;
    GetWindowRect(consoleHWND, &r); // Get the console's current dimensions (in pixels)

    MoveWindow(consoleHWND, 0, 0, r.right, r.bottom, TRUE);  // Move Window to (0;0) position, size: Console Size in pixels

//    ShowWindow(consoleHWND,SW_MAXIMIZE);
#endif // BASIGMINGW
}
void GetConsoleDimensions(CONSOLEINFO *Con)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi); //Get info, and get the new console size parameters (columns, rows)

    Con->CharAmount.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    Con->CharAmount.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#ifndef BASIGMINGW
    HWND consoleHWND = GetConsoleWindow();                // Get Console Window Handle

    RECT r;
    GetWindowRect(consoleHWND, &r); // Get the console's current dimensions (in pixels)

    Con->Size.X = r.right;
    Con->Size.Y = r.bottom;
#else
    Con->Size.X = Con->FontSize.X * Con->CharAmount.X;
    Con->Size.Y = Con->FontSize.Y * Con->CharAmount.Y;
#endif // BASIGMINGW
}
void SetConsoleFont(SIZES FontSize)
{
#if !defined(WINXP) && !defined(BASIGMINGW) //Minimum supported client: Windows Vista
    HANDLE outcon = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_FONT_INFOEX font= {sizeof(CONSOLE_FONT_INFOEX)}; //CONSOLE_FONT_INFOEX is defined in some windows header
    GetCurrentConsoleFontEx(outcon, false, &font);//PCONSOLE_FONT_INFOEX is the same as CONSOLE_FONT_INFOEX*
    font.FontFamily = 54; //Lucida console font
    font.dwFontSize.X = FontSize.X;
    font.dwFontSize.Y = FontSize.Y;
    SetCurrentConsoleFontEx(outcon, false, &font);
#endif // WINXP, BASIGMINGW
}

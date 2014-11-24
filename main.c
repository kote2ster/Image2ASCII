#include "Functions.h"
#define FontSize_X 7
#define FontSize_Y 12

/********************************************************************************
* IMAGE2ASCII                                                                   *
* Made by Ákos Köte                                                             *
* Last modification: 2014.11.23                                                 *
********************************************************************************/

int main()
{
    WINDOW *stdwindow;
    CONSOLEINFO Con;
    IMAGE PNG;
    IMAGE PPM;
    SUBSECTION subsec;
    CHAR2DARRAY filenames;
    CHAR2DARRAY options;
    CHAR_SET CharSet;
    char path[PATH_LONG];
    char outfilename[40];
    char *filename_dest;
    int ch;
    int choice;
    int choice_first;
    int loops;
    int writetofile;
    int error; //Error management...
    char *point;

    Con.FontSize.X = FontSize_X;
    Con.FontSize.Y = FontSize_Y;
    SetConsoleFont(Con.FontSize); //Default text font
    SetConsoleWindowFullScreen(&Con.CharAmount); //Setting to full screen...

    InitialisePDCURSES(Con.CharAmount.X,Con.CharAmount.Y,&stdwindow); //For Screen management..., out: Standard window buffer (using for active window)

    strcpy(path,"images"); //default folder: "images/"

    options.arr = Allocate2D(8,18+40); //7 Optional choices, Max length: "4 - OUTPUT: FILE: " + length of outfilename
    options.arr_size=8;
    strcpy(options.arr[0],"1 - CHAR_SET SHOWCASE MODE1");
    strcpy(options.arr[1],"2 - CHAR_SET SHOWCASE MODE2");
    strcpy(options.arr[2],"3 - RESIZE TERMINAL/CMD");
    strcpy(options.arr[3],"4 - OUTPUT: TERMINAL");
    strcpy(options.arr[4],"5 - LOOP");
    strcpy(options.arr[5],"6 - THUMBNAIL MODE");
    strcpy(options.arr[6],"7 - DEFAULT INFORMATIONS");
    strcpy(options.arr[7],"8 - EXIT"); //For Optional Functions, Setting optional choices...

    /*--Basic ASCII table font generator (for using with png2charset.exe)--*/
//for (int i=32;i<=126;i++)
//{
//    printw("%c",i);
//    if ((i+1)%16==0)
//        printw("\n");
//}
    /*---------------------------------------------------------------------*/
#ifdef ExtendedASCIICharSet
    CharSetImporter(&CharSet,"8x14ext_asciichars.dat"); //Extended ASCII, Import charset to array
#else
    CharSetImporter(&CharSet,"8x14asciichars.dat"); //Basic ASCII
#endif // ExtendedASCIICharSet
    CalculateWeights(&CharSet); //Calculating weights...

    writetofile=0;

    do
    {
        /*---------------SELECTING PNG FILE-------------*/
        filenames.arr_size=0;
        ListDirectory(&filenames,path,&choice_first); //files in PNGs directory will be Choices, out: number of filenames
        loops=0;
        ch=0;

        do
        {
            choice=MenuChoices(filenames,options,Con.CharAmount.Y,stdwindow); //Write-outs file Choices and Options; Returns: Choice number

            switch(choice)
            {
            case ShowCaseMode1:
                ShowCase(CharSet);
                break;
            case ShowCaseMode2:
                ShowCase2(CharSet);
                break;
            case ResizeCMD:
                echo();
                curs_set(1);
                printw("Current settings:\n1 char's width: %d , height: %d\n",Con.FontSize.X,Con.FontSize.Y);
                printw("Output pixel size will be: %d*[CHARS_IN_A_ROW] * %d*[CHARS_IN_A_COLUMN]\n",Con.FontSize.X,Con.FontSize.Y);
                printw("\nNUMBER OF CHARACTERS IN A ROW?    ");
                scanw("%d",&Con.CharAmount.X);
                printw("\nNUMBER OF CHARACTERS IN A COLUMN? ");
                scanw("%d",&Con.CharAmount.Y);
                resize_term(Con.CharAmount.Y,Con.CharAmount.X); //Resizing terminal for PDCURSES...
                break;
            case SetOutSTREAM:
                if(strcmp(options.arr[3],"4 - OUTPUT: TERMINAL")==0)
                {
                    echo();
                    curs_set(1);
                    printw("Enter File name: ");
                    scanw("%s",outfilename);
                    strcpy(options.arr[2],"3 - RESIZE OUTPUT IMAGE");
                    sprintf(options.arr[3],"4 - OUTPUT: FILE: %s",outfilename);
                    writetofile=1;
                }
                else
                {
                    strcpy(options.arr[3],"4 - OUTPUT: TERMINAL");
                    writetofile=0;
                }
                break;
            case SetLOOP:
                echo();
                curs_set(1);
                printw("How many loops?: ");
                scanw("%d",&loops);
                sprintf(options.arr[4],"5 - LOOPS: %d loops",loops);
                break;
            case ThumbMODE:
                clear();
                mvprintw(0,0,"Loading... Please Wait");
                refresh();
                choice = ThumbnailMode(path,filenames,Con,CharSet);
                break;
            case DEFAULTINFO:
                printw("Optimised for: %dpx wide %dpx high Console Font Characters\n",FontSize_X,FontSize_Y);
#ifdef ExtendedASCIICharSet
                printw("Using: Extended ASCII Char Set\n");
#else
                printw("Using: Basic ASCII Char Set\n");
#endif // ExtendedASCIICharSet
                printw("Total: %d characters\n",CHAR_NUM);
                printw("-----------------------------------------------------------\n");
                printw("The program is able to convert .PNG and .PPM image files\n");
                printw("Folders are supported\n");
                printw("-----------------------------------------------------------\n");
                printw("Written purely in C\n");
                printw("Used external library                     : PDCURSES\n");
                printw("Used external functions for importing PNGs: lodepng \n");
                printw("Used external exe for generating CharTable: png2charset.exe\n");
                printw("===========================================================\n");
                printw("Made by: Akos Kote\nVersion: v1.0");
                refresh();
                getch();
                clear();
                break;
            case EXIT:
            case KEY_EXIT:
                clear();
                endwin();
                exit(0);
                break;
            }
            if(choice>=0 && strchr(filenames.arr[choice],'/') != NULL) //It's a folder
            {
                char *temp;
                temp = strtok(filenames.arr[choice],"[]");
                strcpy(path,temp);
                ListDirectory(&filenames,path,&choice_first); //relist
                choice = -1; //Do not exit from while
            }
            else if (choice>=0 && strcmp(filenames.arr[choice],"..")==0) //It's a one level up sign
            {
                char *temp;
                temp = strrchr(path,'/');
                *temp = '\0'; //Cut off last folder name
                ListDirectory(&filenames,path,&choice_first); //relist
                choice = -1; //Do not exit from while
            }
            noecho();
            curs_set(0);
            clear();
        }
        while(choice<0);  //until choice>=0 (user selected a file)
        /*----------------------------------------------*/

        do
        {
            if (!writetofile)
                GetConsoleDimensions(&Con); //Get console pixel size & number of columns and rows
            if(writetofile&&loops>0)
                sprintf(outfilename,"%s",filenames.arr[choice]); //When writing to files in a LOOP
            /*---------------READING IMAGE FILE-------------*/
            filename_dest=(char*)calloc(strlen(path)+1+strlen(filenames.arr[choice])+1,sizeof(char));
            sprintf(filename_dest,"%s/%s",path,filenames.arr[choice]); //Putting path before filename
            //decode
            if((point = strrchr(filenames.arr[choice],'.')) != NULL)
            {
                if(strcmp(point,".png") == 0)   //ends with png
                {
                    error = lodepng_decode32_file(&PNG.Image, &PNG.Width, &PNG.Height, filename_dest);
                    //if there's an error, display it
                    if(error) printw("decoder error %d: %s", error, lodepng_error_text(error));
                    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
                    CalculateImageWeight(&PNG,&subsec,Con);
                    //ProcessingPNG Parameters:PNGImage,SUBSECTION,OUT: PNG_WEIGHT
                    ProcessPNG(&PNG,subsec);
                    PNG.ASCII_Image = IMAGE2ASCII(PNG,CharSet);
                    Wait(&loops,&ch);
                    WriteOut(PNG,writetofile,outfilename);
                    free(PNG.Image);
                    free(PNG.ASCII_Image);
                    free(PNG.Weight);
                }
                else if(strcmp(point,".ppm") == 0) //ends with ppm
                {
                    DecodePPM(&PPM,filename_dest);
                    CalculateImageWeight(&PPM,&subsec,Con);
                    ProcessPPM(&PPM,subsec);
                    PPM.ASCII_Image = IMAGE2ASCII(PPM,CharSet);
                    Wait(&loops,&ch);
                    WriteOut(PPM,writetofile,outfilename);
                    free(PPM.Image);
                    free(PPM.ASCII_Image);
                    free(PPM.Weight);
                }
            }
            /*----------------------------------------------*/

            free(filename_dest);

            if(loops<=0)
            {
                curs_set(1);
                printw("\n<--- %s --->",filenames.arr[choice]);
                ch=getch();
            }

            if(choice<filenames.arr_size-1 && (loops>0 || ch==KEY_RIGHT))
                choice++;
            else if (loops>0 || ch==KEY_RIGHT)
                choice = choice_first;
            if(choice>choice_first && ch==KEY_LEFT)
                choice--;
            else if (ch==KEY_LEFT)
                choice = filenames.arr_size-1;
        }
        while (loops>0||ch==KEY_LEFT||ch==KEY_RIGHT);

        strcpy(options.arr[2],"3 - RESIZE TERMINAL/CMD");
        strcpy(options.arr[3],"4 - OUTPUT: TERMINAL");
        strcpy(options.arr[4],"5 - LOOP");
        writetofile=0;
        curs_set(0);
        clear();
        move(0,0);
    }
    while(ch!='q');

    endwin();
    return 0;
}
/*
 *  Basic concept is: split image into small subsections (tiles), these sections' size equal to CHAR_SET character sections
 *  Then a scaling weight is measured for both the image's subsections and CHAR_SET's sections
 *  then for each image subsections the best weight matches are measured (to CHAR_SET's weights)
*/

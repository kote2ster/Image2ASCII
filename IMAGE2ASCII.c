/**
 * @file IMAGE2ASCII.c
 * @brief Capable of converting .PNG and .PPM images to ASCII art
 * @def FontSize_X
 * @brief Width of a character in Console Window
 * @def FontSize_Y
 * @brief Height of a character in Console Window
 */
#include "Functions.h"
#define FontSize_X 7
#define FontSize_Y 12

/** [TOC]
 * @mainpage Image to ASCII-ART converter
 * @section intro_sec Introduction
 *  @author    Ákos Köte
 *  @version   1.2
 *  @date      Last Modification: 2014.12.16.
 *
 * [GitHub Project README](https://github.com/kote2ster/Image2ASCII "GitHub Project")
 *
 * @section conc_sec Basic Concept
 *  - First of all [png2charset.exe][1] created [8x14asciichars.dat][2] and [8x14asciichars.dat][3]
 which contains the pixel informations of the characters from a 8*14 font sized ASCII table.
 *  - The program orders a weigth (brightness) value for every character with CalculateWeights()
 *  - The program reads and then splits an input Image (.png or .ppm) into small [SUBSECTIONS][4] (tiles), these sections' size are measured with CalculateImageSizes() \n
 [FONTSIZE_Y][7]*[SUBSECTION.scale][4]*[IMAGE.HeightTile][5]=[IMAGE.Height][5] and [FONTSIZE_X][6]*[SUBSECTION.scale][4]*[IMAGE.WidthTile][5]=[IMAGE.Width][5]
 *  - Then for each [IMAGE][5] SUBSECTION the best weight (brightness) matches are measured (to CHAR_SET's weights) with IMAGE2ASCII()
 *
 * [1]: http://www.piclist.com/techref/datafile/charset/extractor/charset_extractor.htm "Charset Extractor from Images"
 * [2]: https://github.com/kote2ster/Image2ASCII/blob/master/8x14asciichars.dat "Basic ASCII"
 * [3]: https://github.com/kote2ster/Image2ASCII/blob/master/8x14ext_asciichars.dat "Extended ASCII"
 * [4]: struct_s_u_b_s_e_c_t_i_o_n.html "SUBSECTION Struct"
 * [5]: struct_i_m_a_g_e.html "IMAGE Struct"
 * [6]: _i_m_a_g_e2_a_s_c_i_i_8c.html#afcc50585fe8d478fd7ddc6753f92c757 "FONTSIZE_X"
 * [7]: _i_m_a_g_e2_a_s_c_i_i_8c.html#a7139d8293983e2bdbab9094c4c3b4475 "FONTSIZE_Y"
 */

int main(void)
{
    WINDOW *stdwindow; /* The standard window for curses */
    CONSOLEINFO Con;
    IMAGE PNG;
    IMAGE PPM;
    SUBSECTION subsec;
    CHAR2DARRAY filenames;
    CHAR2DARRAY options;
    CHAR_SET CharSet;
    char path[PATH_LONG]; /* The string path for accessing pictures */
    char outfilename[40]; /* The string name of the output (text) file */
    char *filename_dest;  /* The string name of a picture */
    int ch;     /* Input management (getch()) */
    int choice; /* Choice index of a picture */
    int choice_first; /* The first index of a picture in a folder */
    int loops;  /* Number of pictures to generate */
    int writetofile;  /* Boolean int, if TRUE you want to write in a file */
    int error;  /* Error management... */
    char *point;/* Pointer to check the extensions */

    Con.FontSize.X = FontSize_X;
    Con.FontSize.Y = FontSize_Y;
#ifdef _WIN32 /* Windows only functions */
    SetConsoleFont(Con.FontSize);                /* Setting default text font */
    SetConsoleWindowFullScreen(&Con.CharAmount); /* Setting to full screen... */
#else /* Else setting console to fix default value */
    Con.CharAmount.X = 100;
    Con.CharAmount.Y = 50;
#endif /* _WIN32 */
    /*For Screen management..., out: Standard active window */
    InitialisePDCURSES(Con.CharAmount.X,Con.CharAmount.Y,&stdwindow);

    strcpy(path,ROOT_FOLDER); /* default root folder */

    /* 8 Optional choices, Max length: "4 - OUTPUT: FILE: " + length of outfilename */
    options.arr = Allocate2D(8,18+40);
    options.arr_size=8;
    /* Setting optional choices to choose from... */
    strcpy(options.arr[0],"1 - CHAR_SET SHOWCASE MODE1");
    strcpy(options.arr[1],"2 - CHAR_SET SHOWCASE MODE2");
    strcpy(options.arr[2],"3 - RESIZE TERMINAL/CMD");
    strcpy(options.arr[3],"4 - OUTPUT: TERMINAL");
    strcpy(options.arr[4],"5 - LOOP");
    strcpy(options.arr[5],"6 - THUMBNAIL MODE");
    strcpy(options.arr[6],"7 - DEFAULT INFORMATIONS");
    strcpy(options.arr[7],"8 - EXIT");

#ifdef ExtendedASCIICharSet /* Extended ASCII, Import charset to array */
    CharSetImporter(&CharSet,"8x14ext_asciichars.dat");
#else /* Else Basic ASCII */
    CharSetImporter(&CharSet,"8x14asciichars.dat");
#endif /* ExtendedASCIICharSet */
    CalculateWeights(&CharSet); /* Calculating charset weights... */

    writetofile=0;

    do
    {
        /*---------------SELECTING PNG FILE-------------*/
        filenames.arr_size=0;
        /* files in ./ROOT_FOLDER/ directory will be Choices, [out]: filenames, number of filenames */
        ListDirectory(&filenames,path,&choice_first);
        loops=0;
        ch=0;

        do
        {
            /* Writeouts file Choices and Options; [out]: Choice number */
            choice=MenuChoices(filenames,options,Con.CharAmount.Y,stdwindow);

            switch(choice) /* Using enums defined in "Functions.h" */
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
                resize_term(Con.CharAmount.Y,Con.CharAmount.X); /* Resizing terminal for PDCURSES... */
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
#endif /* ExtendedASCIICharSet */
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
                printw("Made by: Akos Kote\nVersion: v1.2");
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
            if(choice>=0 && strchr(filenames.arr[choice],'/') != NULL) /* It's a folder */
            {
                char *temp;
                temp = strtok(filenames.arr[choice],"[]");
                strcpy(path,temp);
                ListDirectory(&filenames,path,&choice_first); /*relist*/
                choice = -1; /* Do not exit from while */
            }
            else if (choice>=0 && strcmp(filenames.arr[choice],"..")==0) /* It's a one level up sign */
            {
                char *temp;
                temp = strrchr(path,'/');
                *temp = '\0'; /* Cut off last folder name */
                ListDirectory(&filenames,path,&choice_first); /*relist*/
                choice = -1; /* Do not exit from while */
            }
            noecho();
            curs_set(0);
            clear();
        }
        while(choice<0);  /* until choice>=0 (user selected a file) */
        /*----------------------------------------------*/

        do
        {
#ifdef _WIN32 /* Windows only functions */
            if (!writetofile)
            {
                /* If not writing to file, get console pixel size & number of columns and rows */
                GetConsoleDimensions(&Con);
            }
#endif /* _WIN32 */
            if(writetofile&&loops>0)
                /* When writing to files in a LOOP */
                sprintf(outfilename,"%s",filenames.arr[choice]);
            /*---------------READING IMAGE FILE-------------*/
            filename_dest=(char*)calloc(strlen(path)+1+strlen(filenames.arr[choice])+1,sizeof(char));
            /* Putting path before filename */
            sprintf(filename_dest,"%s/%s",path,filenames.arr[choice]);
            /* Decode */ /* Check if it has an extension */
            if((point = strrchr(filenames.arr[choice],'.')) != NULL)
            {
                if(strcmp(point,".png") == 0)   /* ends with png */
                {
                    error = lodepng_decode32_file(&PNG.Image, &PNG.Width, &PNG.Height, filename_dest);
                    /* if there's an error, display it */
                    if(error) printw("decoder error %d: %s", error, lodepng_error_text(error));
                    /* the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...*/
#ifdef PreciseASCIIgen /* whole SUBSECTION sizes are generated and Precise ASCII art is generated */
                    CalculateImageSizesPrec(&PNG,&subsec,Con);
                    PreciseProcessPNG(&PNG,subsec,CharSet);
#else /* not whole SUBSECTION sizes */
                    CalculateImageSizes(&PNG,&subsec,Con);
                    ProcessPNG(&PNG,subsec); /*ProcessingPNG [in]:PNGImage,SUBSECTION,[out]: PNG_WEIGHT */
                    IMAGE2ASCII(&PNG,CharSet);
#endif /* PreciseASCIIgen */
                    Wait(&loops,&ch);
                    WriteOut(PNG,writetofile,outfilename);
                    free(PNG.Image);
                    free(PNG.ASCII_Image);
                    free(PNG.Weight);
                }
                else if(strcmp(point,".ppm") == 0) /* ends with ppm */
                {
                    DecodePPM(&PPM,filename_dest);
                    CalculateImageSizes(&PPM,&subsec,Con);
                    ProcessPPM(&PPM,subsec);
                    IMAGE2ASCII(&PPM,CharSet);
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

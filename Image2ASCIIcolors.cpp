/**
 * @file IMAGE2ASCII.c
 * @brief Capable of converting .PNG and .PPM images to ASCII art, uses Libtcod library
 */
#include "Image2ASCIIcolors.h"

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

int main(int argc,char *argv[]) {
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
    TCOD_key_t ch;     /* Input management (getch()) */
    int choice; /* Choice index of a picture */
    int choice_first; /* The first index of a picture in a folder */
    int loops;  /* Number of pictures to generate */
    int writetofile;  /* Boolean int, if TRUE you want to write in a file */
    int error;  /* Error management... */
    char *point;/* Pointer to check the extensions */
    std::string temp; /* for inputs... */

    /*-----------Initializing CONSOLEINFO-----------*/
    Con.FontSize.X = FontSize_X;
    Con.FontSize.Y = FontSize_Y;
    Con.CharAmount.X = ConsoleSize_X;
    Con.CharAmount.Y = ConsoleSize_Y;
    Con.Size.X = Con.FontSize.X * Con.CharAmount.X;
    Con.Size.Y = Con.FontSize.Y * Con.CharAmount.Y;
    /* TCODSystem::getCurrentResolution(&Con.Size.X,&Con.Size.Y); */
    /*----------------------------------------------*/

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
    CharSetImporter(&CharSet,"8x8terminalFull.dat");
#else /* Else Basic ASCII */
    CharSetImporter(&CharSet,"8x8terminal.dat");
#endif /* ExtendedASCIICharSet */
    CalculateWeights(&CharSet); /* Calculating charset weights... */

    /*----------Initializing libtcod root------------*/
    TCODConsole::initRoot(Con.CharAmount.X, Con.CharAmount.Y, "IMAGE2ASCII");
    TCODSystem::setFps(25);
    /* Setting black-white default colors */
    TCODConsole::root->setDefaultBackground(TCODColor::black);
    TCODConsole::root->setDefaultForeground(TCODColor::white);
    /*-----------------------------------------------*/

    writetofile=0;

    do {
        /*---------------SELECTING PNG FILE-------------*/
        filenames.arr_size=0;
        /* files in ./ROOT_FOLDER/ directory will be Choices, [out]: filenames, number of filenames */
        ListDirectory(&filenames,path,&choice_first);
        loops=0;
        PNG.ASCII_Color = NULL;
        PPM.ASCII_Color = NULL;

        do {
            /* Writeouts file Choices and Options; [out]: Choice number */
            choice=MenuChoicesLibt(filenames,options,Con.CharAmount.Y);

            switch(choice) { /* Using enums defined in "Functions.h" */
            case ShowCaseMode1:
                ShowCase(CharSet);
                break;
            case ShowCaseMode2:
                ShowCase2(CharSet);
                break;
            case ResizeCMD:
                TCODConsole::root->print(0,0,"Current settings:\n1 char's width: %d , height: %d",Con.FontSize.X,Con.FontSize.Y);
                TCODConsole::root->print(0,2,"Output pixel size will be: %d*[CHARS_IN_A_ROW] * %d*[CHARS_IN_A_COLUMN]",Con.FontSize.X,Con.FontSize.Y);
                TCODConsole::root->print(0,3,"NUMBER OF CHARACTERS IN A ROW?    ");
                TCODConsole::root->flush();
                temp = InputHandler(INPUT_NUMBER,0,4);
                if(!temp.empty()) Con.CharAmount.X = atoi(temp.c_str());
                TCODConsole::root->print(0,5,"NUMBER OF CHARACTERS IN A COLUMN? ");
                TCODConsole::root->flush();
                temp = InputHandler(INPUT_NUMBER,0,6);
                if(!temp.empty()) Con.CharAmount.Y = atoi(temp.c_str());
                TCODConsole::initRoot(Con.CharAmount.X, Con.CharAmount.Y, "IMAGE2ASCII"); /* Resizing console... */
                Con.Size.X = Con.FontSize.X * Con.CharAmount.X;
                Con.Size.Y = Con.FontSize.Y * Con.CharAmount.Y;
                break;
            case SetOutSTREAM:
                if(strcmp(options.arr[3],"4 - OUTPUT: TERMINAL")==0) {
                    TCODConsole::root->print(0,0,"Enter File name: ");
                    TCODConsole::root->flush();
                    temp = InputHandler(INPUT_STRING,strlen("Enter File name: "),0);
                    if(!temp.empty()) strcpy(outfilename,temp.c_str());
                    strcpy(options.arr[2],"3 - RESIZE OUTPUT IMAGE");
                    sprintf(options.arr[3],"4 - OUTPUT: FILE: %s",outfilename);
                    writetofile=1;
                } else {
                    strcpy(options.arr[3],"4 - OUTPUT: TERMINAL");
                    writetofile=0;
                }
                break;
            case SetLOOP:
                TCODConsole::root->print(0,0,"How many loops?: ");
                TCODConsole::root->flush();
                temp = InputHandler(INPUT_NUMBER,strlen("How many loops?: "),0);
                if(!temp.empty()) loops = atoi(temp.c_str());
                sprintf(options.arr[4],"5 - LOOPS: %d loops",loops);
                break;
            case ThumbMODE:
                TCODConsole::root->print(0,0,"Loading... Please Wait");
                TCODConsole::root->flush();
                choice = ThumbnailMode(path,filenames,Con,CharSet);
                break;
            case DEFAULTINFO:
                TCODConsole::root->print(0,0,"Optimised for: %dpx wide %dpx high Console Font Characters",FontSize_X,FontSize_Y);
#ifdef ExtendedASCIICharSet
                TCODConsole::root->print(0,1,"Using: Extended ASCII Char Set");
#else
                TCODConsole::root->print(0,1,"Using: Basic ASCII Char Set");
#endif /* ExtendedASCIICharSet */
                TCODConsole::root->print(0,2,"Total: %d characters",CHAR_NUM);
                TCODConsole::root->print(0,3,
                                         "-----------------------------------------------------------\n"
                                         "The program is able to convert .PNG and .PPM image files\n"
                                         "Folders are supported\n"
                                         "-----------------------------------------------------------\n"
                                         "Written in C/C++\n"
                                         "Used external library                     : LIBTCOD\n"
                                         "Used external functions for importing PNGs: lodepng \n"
                                         "Used external exe for generating CharTable: png2charset.exe\n"
                                         "===========================================================\n"
                                         "Made by: Akos Kote\nVersion: v1.2");
                TCODConsole::root->flush();
                TCODConsole::root->waitForKeypress(true);
                break;
            case EXIT:
                exit(0);
                break;
            }
            if(choice>=0 && strchr(filenames.arr[choice],'/') != NULL) { /* It's a folder */
                char *temp;
                temp = strtok(filenames.arr[choice],"[]");
                strcpy(path,temp);
                ListDirectory(&filenames,path,&choice_first); /*relist*/
                choice = -1; /* Do not exit from while */
            } else if (choice>=0 && strcmp(filenames.arr[choice],"..")==0) { /* It's a one level up sign */
                char *temp;
                temp = strrchr(path,'/');
                *temp = '\0'; /* Cut off last folder name */
                ListDirectory(&filenames,path,&choice_first); /*relist*/
                choice = -1; /* Do not exit from while */
            }
            TCODConsole::root->clear();
        } while(choice<0); /* until choice>=0 (user selected a file) */
        /*----------------------------------------------*/

        do {
            if(writetofile&&loops>0)
                /* When writing to files in a LOOP */
                sprintf(outfilename,"%s",filenames.arr[choice]);
            /*---------------READING IMAGE FILE-------------*/
            filename_dest=(char*)calloc(strlen(path)+1+strlen(filenames.arr[choice])+1,sizeof(char));
            /* Putting path before filename */
            sprintf(filename_dest,"%s/%s",path,filenames.arr[choice]);
            /* Decode */ /* Check if it has an extension */
            if((point = strrchr(filenames.arr[choice],'.')) != NULL) {
                if(strcmp(point,".png") == 0) { /* ends with png */
                    error = lodepng_decode32_file(&PNG.Image, &PNG.Width, &PNG.Height, filename_dest);
                    /* if there's an error, display it */
                    if(error) TCODConsole::root->print(0,0,"decoder error %d: %s", error, lodepng_error_text(error));
                    /* the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...*/
#ifdef PreciseASCIIgen /* whole SUBSECTION sizes are generated and Precise ASCII art is generated */
//                    CalculateImageSizesPrec(&PNG,&subsec,Con);
                    CalculateImageSizes(&PNG,&subsec,Con);
                    PreciseProcessPNG(&PNG,subsec,CharSet);
#else /* not whole SUBSECTION sizes */
                    CalculateImageSizes(&PNG,&subsec,Con);
                    ProcessPNG(&PNG,subsec); /*ProcessingPNG [in]:PNGImage,SUBSECTION,[out]: PNG_WEIGHT */
                    IMAGE2ASCII(&PNG,CharSet);
#endif /* PreciseASCIIgen */
                    Wait(25,&loops);
                    WriteOut(PNG,writetofile,outfilename);
                    free(PNG.Image);
                    free(PNG.ASCII_Image);
                    free(PNG.Weight);
                } else if(strcmp(point,".ppm") == 0) { /* ends with ppm */
                    DecodePPM(&PPM,filename_dest);
                    CalculateImageSizes(&PPM,&subsec,Con);
                    ProcessPPM(&PPM,subsec);
                    IMAGE2ASCII(&PPM,CharSet);
                    Wait(25,&loops);
                    WriteOut(PPM,writetofile,outfilename);
                    free(PPM.Image);
                    free(PPM.ASCII_Image);
                    free(PPM.Weight);
                }
            }
            /*----------------------------------------------*/

            loops--;

            if(loops<=0) {
                TCODConsole::root->print(0,Con.CharAmount.Y-1,"<--- %s --->",filenames.arr[choice]);
                TCODConsole::root->flush();
                ch=TCODConsole::root->waitForKeypress(false);
            }

            if(choice<filenames.arr_size-1 && (loops>0 || ch.vk==TCODK_RIGHT))
                choice++;
            else if (loops>0 || ch.vk==TCODK_RIGHT)
                choice = choice_first;
            if(choice>choice_first && ch.vk==TCODK_LEFT)
                choice--;
            else if (ch.vk==TCODK_LEFT)
                choice = filenames.arr_size-1;

            /*------Increase or Decrease brightness of the picture------*/
            if(ch.vk==TCODK_UP||ch.vk==TCODK_DOWN) {
                if((point = strrchr(filenames.arr[choice],'.')) != NULL) {
                    if(strcmp(point,".png") == 0) { /* ends with png */
                        error = lodepng_decode32_file(&PNG.Image, &PNG.Width, &PNG.Height, filename_dest);
                        /* if there's an error, display it */
                        if(error) TCODConsole::root->print(0,0,"decoder error %d: %s", error, lodepng_error_text(error));
                        do {
                            for(unsigned i=0; i<PNG.Width*PNG.Height*4; i++) {
                                if(PNG.Image[i]<255) PNG.Image[i]++;
                            }
                            /* the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...*/
#ifdef PreciseASCIIgen /* whole SUBSECTION sizes are generated and Precise ASCII art is generated */
//                    CalculateImageSizesPrec(&PNG,&subsec,Con);
                            CalculateImageSizes(&PNG,&subsec,Con);
                            PreciseProcessPNG(&PNG,subsec,CharSet);
#else /* not whole SUBSECTION sizes */
                            CalculateImageSizes(&PNG,&subsec,Con);
                            ProcessPNG(&PNG,subsec); /*ProcessingPNG [in]:PNGImage,SUBSECTION,[out]: PNG_WEIGHT */
                            IMAGE2ASCII(&PNG,CharSet);
#endif /* PreciseASCIIgen */
//                            Wait(25,&loops);
                            WriteOut(PNG,writetofile,outfilename);
                            ch=TCODConsole::root->waitForKeypress(false);
                        } while(ch.vk==TCODK_UP||ch.vk==TCODK_DOWN);
                        free(PNG.Image);
                        free(PNG.ASCII_Image);
                        free(PNG.Weight);
                    }
                }
            }
            /*----------------------------------------------------------*/

            free(filename_dest);
        } while (loops>0||ch.vk==TCODK_LEFT||ch.vk==TCODK_RIGHT);

        strcpy(options.arr[2],"3 - RESIZE TERMINAL/CMD");
        strcpy(options.arr[3],"4 - OUTPUT: TERMINAL");
        strcpy(options.arr[4],"5 - LOOP");
        writetofile=0;
        TCODConsole::root->clear();
    } while(!TCODConsole::isWindowClosed());

    return 0;
}

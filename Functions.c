#include "Functions.h"

void CharSetImporter(CHAR_SET*CharSet,const char* filename)
{
    FILE *f;
    int i;
    f = fopen(filename,"r");
    char buff[9+2*CHAR_X*CHAR_Y];
    if (f!=NULL)
    {
        int char_index = 0;
        int char_line = 0;
        int char_column = 0;
        while (fgets(buff,9+2*CHAR_X*CHAR_Y,f)!=NULL && char_index<CHAR_NUM)
        {
            for(i=3; i<9+2*CHAR_X*CHAR_Y; i++)
            {
                if(buff[i]=='0'||buff[i]=='1')
                {
                    CharSet->ARR[char_index][char_line][char_column] = buff[i];
                    char_column++;
                }
                if((i-3)!=0 && (i-3)%(CHAR_X*2)==0)
                {
                    char_column=0;
                    char_line++;
                }
            }
            Indexer(CharSet,char_index);
            char_column=0;
            char_line=0;
            char_index++;
        }
    }
    else
    {
        printw("ERROR NOT FOUND %s",filename);
        getch();
        endwin();
        exit(1);
    }
    fclose(f);
}
void Indexer(CHAR_SET* CharSet,int char_index)
{
    if(char_index<=94) //Standard ASCII
    {
        CharSet->CHAR[char_index]=char_index+32;
    }
#ifdef ExtendedASCIICharSet
    else //Extended ASCII
    {
        if(char_index>=95&&char_index<=210) CharSet->CHAR[char_index]=char_index+33;
        if(char_index>=211)                 CharSet->CHAR[char_index]=char_index+34;
    }
#endif // ExtendedASCIICharSet
}
void CalculateWeights(CHAR_SET*CharSet)
{
    //Calculating WEIGHTS...
    int weight=0;
    int weight_min=CHAR_X*CHAR_Y;
    int weight_max=0;
    int k,i,j;
    for(k=0; k<CHAR_NUM; k++)
    {
        weight=0;
        for (i=0; i<CHAR_Y; i++)
        {
            for (j=0; j<CHAR_X; j++)
            {
                if (CharSet->ARR[k][i][j]=='1')
                {
                    weight++;
                }
            }
        }
        if(weight>weight_max)weight_max=weight;
        if(weight<weight_min)weight_min=weight;
        CharSet->WEIGHT[k]=weight;
    }
    //Calculating SCALING WEIGHTS...
    weight=0;
    for(k=0; k<CHAR_NUM; k++)
    {
        weight=CHAR_X*CHAR_Y*(CharSet->WEIGHT[k]-weight_min)/(weight_max-weight_min); //f(x)=TABLE_MAX*(x-MIN)/(MAX-MIN) => distributing from MIN->TABLE_MAX
        CharSet->SCALING_WEIGHT[k]=weight;
    }
}
/*------------------PROCESSING PICS-------------------*/
void CalculateImageWeight(IMAGE* img,SUBSECTION* subsec,CONSOLEINFO Con)
{
    //FONTSIZE.Y*subsection_scale*height_tile=height ; FONTSIZE.X*subsection_scale*width_tile=width
    if((double)img->Height/img->Width >= (double)(Con.Size.Y-2*Con.FontSize.Y)/(Con.Size.X-1*Con.FontSize.X)) //then height_tile is fixed = ConsoleHeight-2 (to fit in console screen)
    {
        img->HeightTile = Con.CharAmount.Y - 2; //Split vertically PNG size, -2 for \n and "REGENERATE?", Remnants not important
        subsec->scale=(double)img->Height/(img->HeightTile*Con.FontSize.Y);
        img->WidthTile=img->Width/(Con.FontSize.X*subsec->scale);
    }
    else //then width_tile is fixed = ConsoleHeight-1 (to fit in console screen)
    {
        img->WidthTile = Con.CharAmount.X - 1; //Split horizontally PNG size, -1 to fit to console window, Remnants not important
        subsec->scale=(double)img->Width/(img->WidthTile*Con.FontSize.X);
        img->HeightTile=img->Height/(Con.FontSize.Y*subsec->scale);
    }

    subsec->height=Con.FontSize.Y*subsec->scale; //In pixels, -2 for \n and "<--- %s --->"
    subsec->width =Con.FontSize.X*subsec->scale; //In pixels, -1 to fit to console window

//        height_tile = height/14; unsigned subsection_height=14;
//        width_tile = width/8;    unsigned subsection_height=8;

    img->Weight = (double*)calloc(img->HeightTile*img->WidthTile,sizeof(double));
}
void DecodePPM(IMAGE* PPM,const char* filename_dest)
{
    FILE* input;
    char buffer[4];
    int Maxval;
    int bytespersample;
    int i;
    input=fopen(filename_dest,"rb");
    PPM->Width=0;
    PPM->Height=0;
    fread(buffer,1,3,input); //Header "P6 "
    do  //Width + WS
    {
        fread(buffer,1,1,input);
        if(buffer[0]!='\n') //If not a WS
        {
            PPM->Width*=10;
            PPM->Width+=(unsigned)(buffer[0]-'0'); //Convert to number
        }
    }
    while(buffer[0]!='\n'); //While not a WS
    do  //Height + WS
    {
        fread(buffer,1,1,input);
        if(buffer[0]!='\n') //If not a WS
        {
            PPM->Height*=10;
            PPM->Height+=(unsigned)(buffer[0]-'0'); //Convert to number
        }
    }
    while(buffer[0]!='\n'); //While not a WS
    Maxval=0;
    do  //Getting MaxVal value
    {
        fread(buffer,1,1,input);
        if(buffer[0]!='\n') //If not a WS
        {
            Maxval*=10;
            Maxval+=(buffer[0]-'0'); //Convert to number
        }
    }
    while(buffer[0]!='\n'); //While not a WS
    if(Maxval<256) bytespersample=1;
    else           bytespersample=2;
    PPM->Image=(unsigned char*)calloc(bytespersample*PPM->Width*PPM->Height*3,sizeof(unsigned char)); //RGB
    i=0;
    while(fread(buffer,bytespersample,1,input)!=0) //Read image
    {
        PPM->Image[i]=buffer[0];
        if(bytespersample==2)
            PPM->Image[i]+=buffer[1];
        i++;
    }
}
void ProcessPPM(IMAGE* PPM,SUBSECTION subsec)
{
    int weight = 0;
    unsigned i,j;
    double k,l;
    for(i=0; i<PPM->HeightTile; i++)
    {
        for(j=0; j<PPM->WidthTile; j++)
        {
            for(k=0+i*subsec.height; k<subsec.height+i*subsec.height; k++) //In subsection_width*subsection_height subsections...
            {
                for(l=0+j*subsec.width; l<subsec.width+j*subsec.width; l++)
                {
                    //l: column, k: row, *4 RGBA, A: Alpha channel (visibility) 0->invisible, 255->fully visible
                    weight +=( PPM->Image[((int)l+(int)k*PPM->Width)*3  ] +   //R
                               PPM->Image[((int)l+(int)k*PPM->Width)*3+1] +   //G
                               PPM->Image[((int)l+(int)k*PPM->Width)*3+2] )/3; //B max 255 (RGB pixel's weight)
                }
            }
            //max 255 (subsection's weight)(ceil is used to reduce overlapping errors)
            PPM->Weight[j+i*PPM->WidthTile] = weight/(ceil(subsec.height)*ceil(subsec.width))
                                              * CHAR_X*CHAR_Y/255; //f(x)=x*CHAR_TABLE_MAX/MAX
            weight=0;
        }
    }
}
void ProcessPNG(IMAGE* PNG,SUBSECTION subsec)
{
    int weight = 0;
    unsigned i,j;
    double k,l;
    for(i=0; i<PNG->HeightTile; i++)
    {
        for(j=0; j<PNG->WidthTile; j++)
        {
            for(k=0+i*subsec.height; k<subsec.height+i*subsec.height; k++) //In subsection_width*subsection_height subsections...
            {
                for(l=0+j*subsec.width; l<subsec.width+j*subsec.width; l++)
                {
                    //l: column, k: row, *4 RGBA, A: Alpha channel (visibility) 0->invisible, 255->fully visible
                    weight +=( PNG->Image[((int)l+(int)k*PNG->Width)*4  ] +   //R
                               PNG->Image[((int)l+(int)k*PNG->Width)*4+1] +   //G
                               PNG->Image[((int)l+(int)k*PNG->Width)*4+2] )/3 //B max 255 (RGB pixel's weight)
                             * PNG->Image[((int)l+(int)k*PNG->Width)*4+3]/255;//A Transparency (0xAlphaChannel/0xFF)
                }
            }
            //max 255 (subsection's weight)(ceil is used to reduce overlapping errors)
            PNG->Weight[j+i*PNG->WidthTile] = weight/(ceil(subsec.height)*ceil(subsec.width))
                                              * CHAR_X*CHAR_Y/255; //f(x)=x*CHAR_TABLE_MAX/MAX
            weight=0;
        }
    }
}
/*----------------------------------------------------*/
/*---------------- IMAGE AS ASCII CHARS----------------------*/
unsigned char *IMAGE2ASCII(IMAGE img,CHAR_SET CharSet)
{
    unsigned char *temp = (unsigned char*)calloc(img.WidthTile*img.HeightTile,sizeof(unsigned char));
    unsigned i,j;
    int index = 0;
    int correction = 0;
    int correction_index = 0;
    for (i=0; i<img.HeightTile; i++)
    {
        for (j=0; j<img.WidthTile; j++)
        {
            while ((int)img.Weight[j+i*img.WidthTile]!=CharSet.SCALING_WEIGHT[index]+correction) //Searching closes weight values in CHAR_SET_SCALING_WEIGHT
            {
                index++;
                if (index==CHAR_NUM) //If exceeds max CHAR_SET
                {
                    index=0;
                    correction=pow(-1,correction_index) * (2+correction_index) * 0.5;
                    correction_index++;
                }
            }
            temp[j+i*img.WidthTile]=CharSet.CHAR[index]; //corresponding char in ASCII

            index=0;
            correction=0;
            correction_index=0;
        }
    }
    return temp;
}
/* How correction works:
 *  (-1)^c_i * (2+c_i)*0,5 ;
 *  (-1)^c_i every second is negative [c_i:0,1,2... (-1)^c_i:1,-1,1...] ;
 *  (2+c_i)*0,5 using int chopping down ability (only whole numbers)
 *   so if c_i:0,1,2,3,4,5... (2+c_i)*0,5:1,1,2,2,3,3...
 *   => correction={1,-1,2,-2,3,-3,4,-4,...} finding closest weight to IMAGE_SCALING_WEIGHT */
/*-----------------------------------------------------------*/
/*---------WRITE OUT ASCII IMAGE TO CONSOLE OR TO FILE-------------*/
void WriteOut(IMAGE img,int writetofile,const char* outfilename)
{
    FILE *output = NULL;
    unsigned i,j;
    if(writetofile)
        output = fopen(outfilename,"w");
    clear();
    move(0,0);
    for (i=0; i<img.HeightTile; i++)
    {
        for (j=0; j<img.WidthTile; j++)
        {
            if(!writetofile)
            {
                printw("%c",img.ASCII_Image[j+i*img.WidthTile]);
//                refresh();
            }
            else //writing to file...
            {
                fputc(img.ASCII_Image[j+i*img.WidthTile],output);
            }
        }
        if(!writetofile)
        {
            printw("\n"); //Done 1 line
//            refresh();
        }
        else //writing to file...
        {
            fputc('\n',output);
        }
    }
    refresh();
    if(writetofile)
        fclose(output);
}
/*-----------------------------------------------------------------*/
void ListDirectory(CHAR2DARRAY* filenames,const char *rootfolder,int *choice_first)
{
    DIR *dir;
    DIR *foldir;
    struct dirent *ent;
    char path[PATH_LONG];
    char *point;
    unsigned i=0;
    unsigned j=0;
    unsigned k=0;
    unsigned l=0;
    FILENAMELIST *directory;
    FILENAMELIST *nextdir;
    FILENAMELIST *file;
    FILENAMELIST *nextfile;
    directory=(FILENAMELIST*)malloc(sizeof(FILENAMELIST));
    nextdir  =(FILENAMELIST*)malloc(sizeof(FILENAMELIST));
    directory->next=nextdir;
    file     =(FILENAMELIST*)malloc(sizeof(FILENAMELIST));
    nextfile =(FILENAMELIST*)malloc(sizeof(FILENAMELIST));
    file->next=nextfile;
    if ((dir = opendir (rootfolder)) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
//            tolower(ent->d_name);
            if( strcmp(rootfolder,"images")!=0 && strcmp(ent->d_name,"..")==0) //If not in root folder, allow one level up sign
            {
                nextdir->name=(char*)calloc(3,sizeof(char));
                strcpy(nextdir->name,"..");
                nextdir->next=(FILENAMELIST*)malloc(sizeof(FILENAMELIST));
                nextdir=nextdir->next;
                j++;
            }
            if( strcmp(ent->d_name,"..")!=0 && strcmp(ent->d_name,".")!=0  )
            {
                if((point = strrchr(ent->d_name,'.')) != NULL)
                {
                    if(strcmp(point,".png") == 0 || strcmp(point,".ppm") == 0)   //ends with png or ppm
                    {
                        nextfile->name=(char*)calloc(strlen(ent->d_name)+1,sizeof(char));
                        strcpy(nextfile->name,ent->d_name);
                        nextfile->next=(FILENAMELIST*)malloc(sizeof(FILENAMELIST));
                        nextfile=nextfile->next;
                        i++;
                    }
                }
                else //check if it's a folder
                {
                    strcpy(path,rootfolder);
                    strcat(path,"/");
                    strcat(path,ent->d_name);
                    if ((foldir = opendir (path)) != NULL)
                    {
                        nextdir->name=(char*)calloc(strlen(path)+1,sizeof(char));
                        strcpy(nextdir->name,path);
                        nextdir->next=(FILENAMELIST*)malloc(sizeof(FILENAMELIST));
                        nextdir=nextdir->next;
                        j++;
                    }
                    closedir (foldir);
                }
            }
        }
        closedir (dir);
        filenames->arr=Allocate2D(i+j,PATH_LONG+3);
        filenames->arr_size=i+j;
        *choice_first=j;
        nextdir=directory;
        for(k=0; k<j; k++) //Folders
        {
            nextdir=nextdir->next;
            if(strcmp(nextdir->name,"..")!=0)
            {
                strcpy(filenames->arr[k],"[");
                strcat(filenames->arr[k],nextdir->name);
                strcat(filenames->arr[k],"]");
            }
            else
            {
                strcpy(filenames->arr[k],nextdir->name);
            }
        }
        nextfile=file;
        for(l=0; l<i; l++) //Files
        {
            nextfile=nextfile->next;
            strcpy(filenames->arr[k+l],nextfile->name);
        }
//        for(l=0;l<i;l++) //Files
//        {
//            strcpy(filenames->arr[l],files[l]);
//        }
//        filenames->arr_size=i;
    }
    else
    {
        /* could not open directory */
        printw("Could not find directory ./images/");
        getch();
        endwin();
        exit(1);
    }
    //Removing lists
    nextdir=directory;
    for(k=0; k<j+2; k++) //Folders
    {
        directory=nextdir;
        nextdir=nextdir->next;
        free(directory);
    }
    nextfile=file;
    for(l=0; l<i+2; l++) //Files
    {
        file=nextfile;
        nextfile=nextfile->next;
        free(file);
    }
}
void Wait(int* loops,int* ch)
{
    if((*loops)>0)
    {
        (*loops)--;
        timeout(100); //getch waits for 100 millisec
        (*ch)=getch();
        if((*ch)!=ERR) //if user pressed something... then stop
            (*loops)=0;
        timeout(-1); //getch waits indefinitely
    }
}
int ThumbnailMode(char path[],CHAR2DARRAY filenames,CONSOLEINFO Con,CHAR_SET CharSet)
{
    IMAGELIST *imagelst;
    IMAGELIST *nextimage;
    SUBSECTION subsec;
    int ch;
    char *point;
    char *filename_dest;
    unsigned i,j;
    int k;
    int choice;
    int error;
    int quit;
    int NUMofTHUMBNAILS=4;
    Con.CharAmount.X = 50; //THUMBNAIL_X
    Con.CharAmount.Y = 30; //THUMBNAIL_Y
    Con.Size.X = Con.FontSize.X * Con.CharAmount.X;
    Con.Size.Y = Con.FontSize.Y * Con.CharAmount.Y;
    imagelst  =(IMAGELIST*)malloc(sizeof(IMAGELIST));
    nextimage =(IMAGELIST*)malloc(sizeof(IMAGELIST));
    imagelst->next=nextimage;
    nextimage->img.ASCII_Image=NULL;
    for(choice=0; choice<filenames.arr_size; choice++)
    {
        /*---------------READING IMAGE FILE-------------*/
        filename_dest=(char*)calloc(strlen(path)+1+strlen(filenames.arr[choice])+1,sizeof(char));
        sprintf(filename_dest,"%s/%s",path,filenames.arr[choice]); //Putting path before filename
        //decode
        if((point = strrchr(filenames.arr[choice],'.')) != NULL)
        {
            if(strcmp(point,".png") == 0)   //ends with png
            {
                error = lodepng_decode32_file(&nextimage->img.Image, &nextimage->img.Width, &nextimage->img.Height, filename_dest);
                //if there's an error, display it
                if(error) printw("decoder error %d: %s", error, lodepng_error_text(error));
                //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
                CalculateImageWeight(&nextimage->img,&subsec,Con);
                //ProcessingPNG Parameters:PNGImage,SUBSECTION,OUT: PNG_WEIGHT
                ProcessPNG(&nextimage->img,subsec);
                nextimage->img.ASCII_Image = IMAGE2ASCII(nextimage->img,CharSet);
            }
            else if(strcmp(point,".ppm") == 0) //ends with ppm
            {
                DecodePPM(&nextimage->img,filename_dest);
                CalculateImageWeight(&nextimage->img,&subsec,Con);
                ProcessPPM(&nextimage->img,subsec);
                nextimage->img.ASCII_Image = IMAGE2ASCII(nextimage->img,CharSet);
            }
        }
        /*----------------------------------------------*/
        nextimage->next=(IMAGELIST*)malloc(sizeof(IMAGELIST));
        nextimage=nextimage->next;
        nextimage->img.ASCII_Image=NULL;
        nextimage->next=NULL;
        free(filename_dest);
    }
    choice=0;
    quit=0;
    nextimage=imagelst->next;
    do
    {
        clear();
        for(k=0; k<NUMofTHUMBNAILS; k++)
        {
            if(nextimage->img.ASCII_Image!=NULL)
            {
                for(i=0; i<nextimage->img.HeightTile; i++)
                {
                    move(i,k*Con.CharAmount.X);
                    for(j=0; j<nextimage->img.WidthTile; j++)
                    {
                        printw("%c",nextimage->img.ASCII_Image[j+i*nextimage->img.WidthTile]);
                    }
                }
            }
            move(Con.CharAmount.Y,k*Con.CharAmount.X);
            if(choice+k<filenames.arr_size)
                printw("%s",filenames.arr[choice+k]);
            if(nextimage->next!=NULL)
                nextimage=nextimage->next;
        }
        mvchgat(Con.CharAmount.Y, 0, strlen(filenames.arr[choice]), A_BLINK, 0, NULL);
        refresh();

        ch=getch();
        if(choice<filenames.arr_size-1 && ch==KEY_RIGHT)
            choice++;
        else if (ch==KEY_RIGHT)
            choice = 0;
        if(choice>0 && ch==KEY_LEFT)
            choice--;
        else if (ch==KEY_LEFT)
            choice = filenames.arr_size-1;

        nextimage=imagelst->next;
        for(i=0; i<(unsigned)choice; i++)
            nextimage=nextimage->next;

        if(CheckIfExit(stdscr,ch))
        {
            quit=1;
            choice=-1;
        }
    }
    while ((ch!='\n'||ch==KEY_LEFT||ch==KEY_RIGHT)&&!quit);

    nextimage=imagelst; //Removing list
    for(i=0; i<(unsigned)filenames.arr_size+2; i++)
    {
        imagelst=nextimage;
        nextimage=nextimage->next;
        free(imagelst);
    }

    return choice;
}
/*------------------SHOWCASE-------------------*/
void ShowCase(CHAR_SET CharSet)
{
    clear();
    int weight;
    int CharIndex=0;
    int ch=0;
    int i,j;
    mvprintw(0,5,"-=SHOWCASE MODE=-");
    while(ch!='\n'&&!CheckIfExit(stdscr,ch))
    {
        weight=0;
        for (i=0; i<14; i++)
        {
            for (j=0; j<8; j++)
            {
                if (CharSet.ARR[CharIndex][i][j]=='1')
                {
                    mvprintw(2+i,10+j,"%c",CharSet.ARR[CharIndex][i][j]);
                    weight++;
                }
                else
                    mvprintw(2+i,10+j," ");
            }
        }
        mvprintw(17,14,"  ");
        mvprintw(17,14,"%c",CharSet.CHAR[CharIndex]);
        mvprintw(19, 9,"WEIGHT: %2d",weight);
        refresh();
        ch=getch();
        if(ch==KEY_RIGHT)
        {
            CharIndex++;
            if(CharIndex==CHAR_NUM)
                CharIndex=0;
        }
        else if (ch==KEY_LEFT)
        {
            CharIndex--;
            if(CharIndex==-1)
                CharIndex=CHAR_NUM-1;
        }
    }
}
void ShowCase2(CHAR_SET CharSet)
{
    clear();
    int weight;
    int CharIndex=0;
    int CharFirstIndex=0;
    int MiddleIndex=0;
    int ch=0;
    int k,i,j;
    mvprintw(0,65,"-=SHOWCASE MODE2=-");
    while(ch!='\n'&&!CheckIfExit(stdscr,ch))
    {
        weight=0;
        for(k=0; k<7; k++)
        {
            if(CharIndex>=CHAR_NUM)
                CharIndex=0;
            if(CharIndex<=-1)
                CharIndex=CHAR_NUM-1;
            for (i=0; i<14; i++)
            {
                for (j=0; j<8; j++)
                {
                    if (CharSet.ARR[CharIndex][i][j]=='1')
                    {
                        mvprintw(2+i,j+(2*k+1)*10,"%c",CharSet.ARR[CharIndex][i][j]);
                        if(k==3) //Calculate only middle char
                            weight++;
                    }
                    else
                        mvprintw(2+i,j+(2*k+1)*10," ");
                }
            }
            if(k==3) MiddleIndex=CharIndex; //Middle char
            CharIndex++;
        }
        mvprintw(17,73,"  ");
        mvprintw(17,73,"%c",CharSet.CHAR[MiddleIndex]);
        mvprintw(19,68,"WEIGHT:%3d",weight);
        mvprintw(21,70,"%03d/%03d",MiddleIndex,CHAR_NUM-1);
        refresh();
        ch=getch();
        if(ch==KEY_RIGHT)
        {
            CharFirstIndex++;
            if(CharFirstIndex==CHAR_NUM)
                CharFirstIndex=0;
        }
        else if (ch==KEY_LEFT)
        {
            CharFirstIndex--;
            if(CharFirstIndex==-1)
                CharFirstIndex=CHAR_NUM-1;
        }
        CharIndex=CharFirstIndex;
    }
}
/*--------------------------------------------*/
char **Allocate2D(int numberof1D,int elementsper1D)
{
    int i;
    char **temp = (char**)calloc(numberof1D,sizeof(char*));
    for (i=0; i<numberof1D; i++)
    {
        temp[i]= (char* )calloc(elementsper1D,sizeof(char));
    }
    return temp;
}
void Free2D(CHAR2DARRAY* array2d)
{
    int i;
    for (i=0; i<array2d->arr_size; i++)
    {
        free(array2d->arr[i]);
    }
    free(array2d->arr);
}

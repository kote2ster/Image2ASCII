/**
 * @file Functions.c
 * @brief Implementations of main functions
 */
#include "Functions.h"

/**
 * @brief Imports and arranges each ASCII CHAR
 * @param [out] CharSet CHAR_SET type, where the ASCII CHAR pixel informations are stored
 * @param [in] filename (8x14asciichars.dat) where the ASCII CHAR informations are, see [mainpage](index.html)
 */
void CharSetImporter(CHAR_SET*CharSet,const char* filename)
{
    FILE *f;
    int i;
    char buff[9+2*CHAR_X*CHAR_Y];
    f = fopen(filename,"r");
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
/**
 * @brief Correspondig CHAR_SETs are calculated for @see CharSetImporter()
 * @param [out] CharSet CHAR_SET type, where the corresponding ASCII CHARs are stored
 * @param [in] char_index Where the iteration was
 * @details if ExtendedASCIICharSet is set, then extended ASCII CHARs are calculated too
 */
void Indexer(CHAR_SET* CharSet,int char_index)
{
    if(char_index<=94) /* Standard ASCII */
    {
        CharSet->CHAR[char_index]=char_index+32;
    }
#ifdef ExtendedASCIICharSet
    else /* Extended ASCII */
    {
        if(char_index>=95&&char_index<=210) CharSet->CHAR[char_index]=char_index+33;
        if(char_index>=211)                 CharSet->CHAR[char_index]=char_index+34;
    }
#endif /* ExtendedASCIICharSet */
}
/**
 * @brief Calculates CHAR_SET WEIGHT and also SCALING_WEIGHT
 * @param [out] CharSet CHAR_SET type, the weight (brightness) values are stored here
 */
void CalculateWeights(CHAR_SET*CharSet)
{
    /* Calculating WEIGHTS... */
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
    /* Calculating SCALING WEIGHTS... */
    weight=0;
    for(k=0; k<CHAR_NUM; k++)
    {
        /* f(x)=TABLE_MAX*(x-MIN)/(MAX-MIN) => distributing from 0->TABLE_MAX */
        weight=CHAR_X*CHAR_Y*(CharSet->WEIGHT[k]-weight_min)/(weight_max-weight_min);
        CharSet->SCALING_WEIGHT[k]=weight;
    }
}
/*------------------PROCESSING PICS-------------------*/
/**
 * @brief Calculates IMAGE dimensions
 * @param [out] img IMAGE type, HeightTile and WidthTile are calculated to fit in Console Screen
 * @param [out] subsec SUBSECTION height, width and scale are measured
 * @param [in] Con Console Screen informations
 */
void CalculateImageSizes(IMAGE* img,SUBSECTION* subsec,CONSOLEINFO Con)
{
    /* FONTSIZE.Y*subsection_scale*height_tile=height ; FONTSIZE.X*subsection_scale*width_tile=width */
    if((double)img->Height/img->Width >= (double)(Con.Size.Y-2*Con.FontSize.Y)/(Con.Size.X-1*Con.FontSize.X))
    {
        /* then height_tile is fixed = ConsoleHeight-2 (to fit in console screen) */
        img->HeightTile = Con.CharAmount.Y - 2; /* -2 for \n and "<--- %s --->" */
        subsec->scale=(double)img->Height/(img->HeightTile*Con.FontSize.Y);
        img->WidthTile=img->Width/(Con.FontSize.X*subsec->scale);
    }
    else
    {
        /* then width_tile is fixed = ConsoleHeight-1 (to fit in console screen) */
        img->WidthTile = Con.CharAmount.X - 1; /* -1 to fit to console window */
        subsec->scale=(double)img->Width/(img->WidthTile*Con.FontSize.X);
        img->HeightTile=img->Height/(Con.FontSize.Y*subsec->scale);
    }

    subsec->height=Con.FontSize.Y*subsec->scale; /* Calculating SUBSECTION in pixels */
    subsec->width =Con.FontSize.X*subsec->scale; /* Calculating SUBSECTION in pixels */

    /*        height_tile = height/14; unsigned subsection_height=14;
     *        width_tile = width/8;    unsigned subsection_height=8; */
}
/**
 * @brief Calculates IMAGE dimensions more precisely if PreciseASCIIgen is defined see [Functions.h](_functions_8h.html)
 * @param [out] img IMAGE type, HeightTile and WidthTile are calculated
 * @param [out] subsec SUBSECTION height, width will be CHAR_Y and CHAR_X
 * @param [in] Con Console Screen informations
 */
void CalculateImageSizesPrec(IMAGE* img,SUBSECTION* subsec,CONSOLEINFO Con)
{
    img->HeightTile = 1;
    img->WidthTile = 1;
    subsec->scale = 1;
    /* FONTSIZE.Y*subsection_scale*height_tile=height ; FONTSIZE.X*subsection_scale*width_tile=width */
    if((double)img->Height/img->Width >= (double)(Con.Size.Y-2*CHAR_Y)/(Con.Size.X-1*CHAR_X))
    {
        while(CHAR_Y*(img->HeightTile+1) <= img->Height && img->HeightTile < Con.CharAmount.Y - 2)
        {
            /*if(img->HeightTile < Con.CharAmount.Y - 2)
            {*/
            img->HeightTile++;
            /*}
            else
            {
                img->HeightTile = 1;
                subsec->scale++;
            }*/
        }
        while(CHAR_X*(img->WidthTile+1) <= img->Width)
        {
            img->WidthTile++;
        }
    }
    else
    {
        while(CHAR_X*(img->WidthTile+1) <= img->Width && img->WidthTile < Con.CharAmount.X - 1)
        {
            /*if(img->WidthTile < Con.CharAmount.X - 1)
            {*/
            img->WidthTile++;
            /*}
            else
            {
                img->WidthTile = 1;
                subsec->scale++;
            }*/
        }
        while(CHAR_Y*(img->HeightTile+1) <= img->Height)
        {
            img->HeightTile++;
        }
    }

    subsec->height=CHAR_Y; /* Calculating SUBSECTION in pixels */
    subsec->width =CHAR_X; /* Calculating SUBSECTION in pixels */
}
/**
 * @brief Decodes a .PPM image file
 * @param [out] PPM IMAGE type, gets the raw pixels and height and width
 * @param [in] filename_dest the image file location
 */
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
    fread(buffer,1,3,input); /* Header "P6 " */
    do  /* Width + WS */
    {
        fread(buffer,1,1,input);
        if(buffer[0]!='\n') /* If not a WS */
        {
            PPM->Width*=10;
            PPM->Width+=(unsigned)(buffer[0]-'0'); /* Convert to number */
        }
    }
    while(buffer[0]!='\n'); /* While not a WS */
    do  /* Height + WS */
    {
        fread(buffer,1,1,input);
        if(buffer[0]!='\n') /* If not a WS */
        {
            PPM->Height*=10;
            PPM->Height+=(unsigned)(buffer[0]-'0'); /* Convert to number */
        }
    }
    while(buffer[0]!='\n'); /* While not a WS */
    Maxval=0;
    do  /* Getting MaxVal value */
    {
        fread(buffer,1,1,input);
        if(buffer[0]!='\n') /* If not a WS */
        {
            Maxval*=10;
            Maxval+=(buffer[0]-'0'); /* Convert to number */
        }
    }
    while(buffer[0]!='\n'); /* While not a WS */
    if(Maxval<256) bytespersample=1;
    else           bytespersample=2;
    PPM->Image=(unsigned char*)calloc(bytespersample*PPM->Width*PPM->Height*3,sizeof(unsigned char)); /*RGB*/
    i=0;
    while(fread(buffer,bytespersample,1,input)!=0) /* Read image */
    {
        PPM->Image[i]=buffer[0];
        if(bytespersample==2)
            PPM->Image[i]+=buffer[1];
        i++;
    }
}
/**
 * @brief Processes (weight calculation of) a .PPM image file
 * @param [out] PPM IMAGE type, gets the weight for each SUBSECTION
 * @param [in] subsec SUBSECTION informations
 */
void ProcessPPM(IMAGE* PPM,SUBSECTION subsec)
{
    int weight,cnt;
    unsigned i,j;
    double k,l;
    PPM->Weight = (double*)calloc(PPM->HeightTile*PPM->WidthTile,sizeof(double));
    for(i=0; i<PPM->HeightTile; i++)
    {
        for(j=0; j<PPM->WidthTile; j++)
        {
            weight=cnt=0;
            /* In subsection_width*subsection_height subsections... */
            for(k=0+i*subsec.height; k<subsec.height+i*subsec.height; k++)
            {
                for(l=0+j*subsec.width; l<subsec.width+j*subsec.width; l++)
                {
                    cnt++;
                    /* l: column, k: row, *3 RGB */
                    weight +=( PPM->Image[((int)l+(int)k*PPM->Width)*3  ] +    /* R */
                               PPM->Image[((int)l+(int)k*PPM->Width)*3+1] +    /* G */
                               PPM->Image[((int)l+(int)k*PPM->Width)*3+2] )/3; /* B max 255 (RGB pixel's weight)*/
                }
            }
            /* weight/cnt is max 255 (subsection's weight); rescaling for later comparison f(x)=x * CHAR_MAXWEIGHT/MAXSUBSECWEIGHT */
            PPM->Weight[j+i*PPM->WidthTile] = weight/cnt * CHAR_X*CHAR_Y/255;
        }
    }
}
/**
 * @brief Processes (weight calculation of) a .PNG image file
 * @param [out] PPM IMAGE type, gets the weight for each SUBSECTION
 * @param [in] subsec SUBSECTION informations
 */
void ProcessPNG(IMAGE* PNG,SUBSECTION subsec)
{
    int weight,cnt;
    unsigned i,j;
    double k,l;
    PNG->Weight = (double*)calloc(PNG->HeightTile*PNG->WidthTile,sizeof(double));
    for(i=0; i<PNG->HeightTile; i++)
    {
        for(j=0; j<PNG->WidthTile; j++)
        {
            weight=cnt=0;
            /* In subsection_width*subsection_height subsections... */
            for(k=0+i*subsec.height; k<subsec.height+i*subsec.height; k++)
            {
                for(l=0+j*subsec.width; l<subsec.width+j*subsec.width; l++)
                {
                    cnt++;
                    /* l: column, k: row, *4 RGBA, A: Alpha channel (visibility) 0->invisible, 255->fully visible */
                    weight +=( PNG->Image[((int)l+(int)k*PNG->Width)*4  ] +   /* R */
                               PNG->Image[((int)l+(int)k*PNG->Width)*4+1] +   /* G */
                               PNG->Image[((int)l+(int)k*PNG->Width)*4+2] )/3 /* B */ /* max 255 (RGB pixel's weight) */
                             * PNG->Image[((int)l+(int)k*PNG->Width)*4+3]/255;/* A Transparency (0xAlphaChannel/0xFF) */
                }
            }
            /* weight/cnt is max 255 (subsection's weight); rescaling for later comparison f(x)=x * CHAR_MAXWEIGHT/MAXSUBSECWEIGHT */
            PNG->Weight[j+i*PNG->WidthTile] = weight/cnt * (CHAR_X*CHAR_Y)/255;
        }
    }
}
/**
 * @brief Precise processing a .PNG image file
 * @param [out] PPM IMAGE type, gets the weight for each SUBSECTION
 * @param [in] subsec SUBSECTION informations
 * @param [in] CharSet CHAR_SET informations
 * @details Tries to contrast up SUBSECTIONS for clearer curves in images see AMPLIFIER,  min((CHAR_SET.ARR[][][] - ImgSubWeight[][])^2)
 */
void PreciseProcessPNG(IMAGE* PNG,SUBSECTION subsec,CHAR_SET CharSet)
{
    int WeightTemp[CHAR_NUM]; /* how many are similar */
    int*ImgSubWeight = calloc(CHAR_Y*CHAR_X,sizeof(int)); /* a pixel brightness of the image */
    int weight,weight_min,weight_max,cnt,minIndex,minPixels;
    unsigned i,j,k,l,m,n;
    PNG->Weight = NULL;
    PNG->ASCII_Image = (unsigned char*)calloc(PNG->WidthTile*PNG->HeightTile,sizeof(unsigned char));
    for(i=0; i<PNG->HeightTile; i++)
    {
        for(j=0; j<PNG->WidthTile; j++)
        {
            minPixels=255*255*CHAR_Y*CHAR_X+1;
            minIndex=cnt=0;
            weight_min=255;
            weight_max=0;
            for(m=0; m<CHAR_NUM; m++)
            {
                WeightTemp[m]=0;
            }
            for(k=0+i*CHAR_Y; k<CHAR_Y+i*CHAR_Y; k++)
            {
                for(l=0+j*CHAR_X; l<CHAR_X+j*CHAR_X; l++)
                {
                    /* for every pixel l: column, k: row, *4 RGBA, A: Alpha channel (visibility) 0->invisible, 255->fully visible */
                    weight = ( PNG->Image[(l+k*PNG->Width)*4  ] +   /* R */
                               PNG->Image[(l+k*PNG->Width)*4+1] +   /* G */
                               PNG->Image[(l+k*PNG->Width)*4+2] )/3 /* B */ /* max 255 (RGB pixel's weight) */
                             * PNG->Image[(l+k*PNG->Width)*4+3]/255;/* A Transparency (0xAlphaChannel/0xFF) */
                    if(weight>weight_max) weight_max=weight;
                    if(weight<weight_min) weight_min=weight;
                    ImgSubWeight[l-j*CHAR_X+(k-i*CHAR_Y)*CHAR_X] = weight;
                }
            }
            if(weight_max==weight_min)
            {
                weight_max=255;
                weight_min=0;
            }
            /* Adjusting SUBSECTION contrast/brightness (distributing it) **WARNING** Pixel errors will be amplified too */
            if(weight_max>AMPLIFIER/*amplify above a value*/)
            {
                for(k=0; k<CHAR_Y; k++)
                {
                    for(l=0; l<CHAR_X; l++)
                    {
                        /* f(x)=MAX_WEIGHT*(x-MIN)/(MAX-MIN) => distributing from 0->TABLE_MAX */
                        weight = 255*(ImgSubWeight[l+k*CHAR_X]-weight_min)/(weight_max-weight_min);
                        ImgSubWeight[l+k*CHAR_X] = weight;
                    }
                }
            }
            for(m=0; m<CHAR_NUM; m++)
            {
                /* In subsection_width*subsection_height subsections... */
                for(k=0+i*CHAR_Y; k<CHAR_Y+i*CHAR_Y; k++)
                {
                    for(l=0+j*CHAR_X; l<CHAR_X+j*CHAR_X; l++)
                    {
                        /* for every pixel l: column, k: row, *4 RGBA, A: Alpha channel (visibility) 0->invisible, 255->fully visible */
                        weight = (CharSet.ARR[cnt][k-i*CHAR_Y][l-j*CHAR_X]-'0')*255 -
                                 ImgSubWeight[l-j*CHAR_X+(k-i*CHAR_Y)*CHAR_X];
                        weight*=weight; /* ^2 */
                        WeightTemp[cnt] += weight;
                    }
                }
                if(WeightTemp[cnt]<minPixels)
                {
                    minPixels=WeightTemp[cnt];
                    minIndex=cnt;
                }
                cnt++;
            }
            PNG->ASCII_Image[j+i*PNG->WidthTile]=CharSet.CHAR[minIndex];
        }
    }
    free(ImgSubWeight);
}
/*----------------------------------------------------*/
/*---------------- IMAGE AS ASCII CHARS----------------------*/
/**
* @brief Calculates for each SUBSECTION the corresponding ASCII CHAR from CHAR_SET
* @param [out] img IMAGE type, best ASCII CHAR match
* @param [in] CharSet CHAR_SET informations
* @details
How correction works:\n
*  (-1)^c_i * (2+c_i)*0,5 ;\n
*  (-1)^c_i every second is negative [c_i:0,1,2... (-1)^c_i:1,-1,1...] ;\n
*  (2+c_i)*0,5 using int chopping down ability (only whole numbers)\n
*   so if c_i:0,1,2,3,4,5... (2+c_i)*0,5:1,1,2,2,3,3...\n
*   => correction={1,-1,2,-2,3,-3,4,-4,...} finding closest weight to IMAGE_SCALING_WEIGHT
*/
void IMAGE2ASCII(IMAGE *img,CHAR_SET CharSet)
{
    unsigned i,j;
    int index = 0;
    int correction = 0;
    int correction_index = 0;
    img->ASCII_Image = (unsigned char*)calloc(img->WidthTile*img->HeightTile,sizeof(unsigned char));
    for (i=0; i<img->HeightTile; i++)
    {
        for (j=0; j<img->WidthTile; j++)
        {
            /* Searching closest weight values in CHAR_SET_SCALING_WEIGHT */
            while ((int)img->Weight[j+i*img->WidthTile]!=CharSet.SCALING_WEIGHT[index]+correction)
            {
                index++;
                if (index==CHAR_NUM) /* If exceeds max CHAR_SET */
                {
                    index=0;
                    correction=pow(-1,correction_index) * (2+correction_index) * 0.5;
                    correction_index++;
                }
            }
            img->ASCII_Image[j+i*img->WidthTile]=CharSet.CHAR[index]; /* corresponding char in ASCII */

            index=0;
            correction=0;
            correction_index=0;
        }
    }
}
/*-----------------------------------------------------------*/
/*---------WRITE OUT ASCII IMAGE TO CONSOLE OR TO FILE-------------*/
/**
* @brief Writing out ASCII CHARs to Console or to File
* @param [in] img IMAGE informations
* @param [in] writetofile Boolean
* @param [in] outfilename Where to write if writetofile==1
*/
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
                /* refresh(); */
            }
            else /* writing to file... */
            {
                fputc(img.ASCII_Image[j+i*img.WidthTile],output);
            }
        }
        if(!writetofile)
        {
            printw("\n"); /* Done 1 line */
            /* refresh(); */
        }
        else /* writing to file... */
        {
            fputc('\n',output);
        }
    }
    refresh();
    if(writetofile)
        fclose(output);
}
/*-----------------------------------------------------------------*/
/**
* @brief Gets all file and folder names
* @param [out] filenames Outs file and folder names
* @param [in] rootfolder The root folder name
* @param [out] choice_first the first choice which is a file
*/
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
            /* If not in root folder, allow one level up sign */
            if( strcmp(rootfolder,ROOT_FOLDER)!=0 && strcmp(ent->d_name,"..")==0)
            {
                nextdir->name=(char*)calloc(3,sizeof(char));
                strcpy(nextdir->name,"..");
                nextdir->next=(FILENAMELIST*)malloc(sizeof(FILENAMELIST));
                nextdir=nextdir->next;
                j++;
            }
            if( strcmp(ent->d_name,"..")!=0 && strcmp(ent->d_name,".")!=0  )
            {
                /* Check if it has an extension */
                if((point = strrchr(ent->d_name,'.')) != NULL)
                {
                    if(strcmp(point,".png") == 0 || strcmp(point,".ppm") == 0)   /* ends with png or ppm */
                    {
                        nextfile->name=(char*)calloc(strlen(ent->d_name)+1,sizeof(char));
                        strcpy(nextfile->name,ent->d_name);
                        nextfile->next=(FILENAMELIST*)malloc(sizeof(FILENAMELIST));
                        nextfile=nextfile->next;
                        i++;
                    }
                }
                else /* check if it's a folder */
                {
                    strcpy(path,rootfolder);
                    strcat(path,"/");
                    strcat(path,ent->d_name);
                    /* try to open it as a dir */
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
        /* Copy folder names and filenames from list to CHAR2DARRAY* filenames */
        for(k=0; k<j; k++) /* Folders */
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
        for(l=0; l<i; l++) /* Files */
        {
            nextfile=nextfile->next;
            strcpy(filenames->arr[k+l],nextfile->name);
        }
    }
    else
    {
        /* could not open directory */
        printw("Could not find directory ./%s/",ROOT_FOLDER);
        getch();
        endwin();
        exit(1);
    }
    /* Removing lists */
    nextdir=directory;
    for(k=0; k<j+2; k++) /* Folders */
    {
        directory=nextdir;
        nextdir=nextdir->next;
        free(directory);
    }
    nextfile=file;
    for(l=0; l<i+2; l++) /* Files */
    {
        file=nextfile;
        nextfile=nextfile->next;
        free(file);
    }
}
/**
* @brief Waits used for PDCURSES
* @param [out] loops function manipulates number of loops if necessary
* @param [out] ch Checks if a button was pressed during wait
*/
void Wait(int* loops,int* ch)
{
    if((*loops)>0)
    {
        (*loops)--;
        timeout(100);  /* getch waits for 100 millisec */
        (*ch)=getch();
        if((*ch)!=ERR) /* if user pressed something... then stop LOOP */
            (*loops)=0;
        timeout(-1);   /* getch waits indefinitely */
    }
}
/**
* @brief Thumbnail mode (all ASCII pics' index pictures will be generated and user can select one)
* @param [in] path the path to the filenames
* @param [in] filenames CHAR2DARRAY type, all file and folder names are stored here
* @param [in] Con Console information
* @param [in] CharSet CHAR_SET information
* @return new choice number
*/
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
    int NUMofTHUMBNAILS=4; /* Number of Thumbnails */
    Con.CharAmount.X = 50; /* Thumbnail X size */
    Con.CharAmount.Y = 30; /* Thumbnail Y size */
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
        sprintf(filename_dest,"%s/%s",path,filenames.arr[choice]); /* Putting path before filename */
        /* Decode */ /* Check if it has an extension */
        if((point = strrchr(filenames.arr[choice],'.')) != NULL)
        {
            if(strcmp(point,".png") == 0)   /* ends with png */
            {
                error = lodepng_decode32_file(&nextimage->img.Image, &nextimage->img.Width, &nextimage->img.Height, filename_dest);
                /* if there's an error, display it */
                if(error) printw("decoder error %d: %s", error, lodepng_error_text(error));
                /* the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ... */
                CalculateImageSizes(&nextimage->img,&subsec,Con);
                /* ProcessingPNG [in]:PNGImage,SUBSECTION,[out]: PNG_WEIGHT */
                ProcessPNG(&nextimage->img,subsec);
                IMAGE2ASCII(&nextimage->img,CharSet);
            }
            else if(strcmp(point,".ppm") == 0) /* ends with ppm */
            {
                DecodePPM(&nextimage->img,filename_dest);
                CalculateImageSizes(&nextimage->img,&subsec,Con);
                ProcessPPM(&nextimage->img,subsec);
                IMAGE2ASCII(&nextimage->img,CharSet);
            }
        }
        /*----------------------------------------------*/
        /* Allocating memory for next list item */
        nextimage->next=(IMAGELIST*)malloc(sizeof(IMAGELIST));
        nextimage=nextimage->next;
        nextimage->img.ASCII_Image=NULL;
        nextimage->next=NULL;
        free(filename_dest);
    }
    choice=0;
    quit=0;
    nextimage=imagelst->next;
    /* Writing out images in thumbnail mode */
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
    while ((ch!='\n'||ch==KEY_LEFT||ch==KEY_RIGHT)&&!quit); /* Until ENTER or QUIT */

    nextimage=imagelst;
    /* Removing list */
    for(i=0; i<(unsigned)filenames.arr_size+2; i++)
    {
        imagelst=nextimage;
        nextimage=nextimage->next;
        free(imagelst);
    }

    return choice;
}
/*------------------SHOWCASE-------------------*/
/**
* @brief ASCII CHAR viewer 1
* @param [in] CharSet CHAR_SET information
*/
void ShowCase(CHAR_SET CharSet)
{
    int weight;
    int CharIndex=0;
    int ch=0;
    int i,j;
    clear();
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
/**
* @brief ASCII CHAR viewer 2
* @param [in] CharSet CHAR_SET information
*/
void ShowCase2(CHAR_SET CharSet)
{
    int weight;
    int CharIndex=0;
    int CharFirstIndex=0;
    int MiddleIndex=0;
    int ch=0;
    int k,i,j;
    clear();
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
                        if(k==3) /* Calculate only middle char */
                            weight++;
                    }
                    else
                        mvprintw(2+i,j+(2*k+1)*10," ");
                }
            }
            if(k==3) MiddleIndex=CharIndex; /* Middle char */
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
/**
* @brief Allocates 2D array
* @param [in] numberof1D number of 1D arrays
* @param [in] elementsper1D how many element per 1D arrays
* @return dynamically allocated 2D array
*/
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
/**
* @brief Destroys a 2D array
* @param [in] array2d CHAR2DARRAY type, which will be destroyed
*/
void Free2D(CHAR2DARRAY* array2d)
{
    int i;
    for (i=0; i<array2d->arr_size; i++)
    {
        free(array2d->arr[i]);
    }
    free(array2d->arr);
}

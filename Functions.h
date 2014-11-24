#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include "TextScreenIni.h"
#include "WindowsConsoleIni.h"
#include "lodepng.h"
#include <math.h>
#include <dirent.h>

#define PATH_LONG 100 //Max how long is the path to the file
//#define ExtendedASCIICharSet //Define this to enable extended ASCII support
#define CHAR_X 8
#define CHAR_Y 14

#ifdef ExtendedASCIICharSet
#define CHAR_NUM 221 //Extended ASCII
#else
#define CHAR_NUM 95 //Standard ASCII
#endif // ExtendedASCIICharSet

typedef struct
{
    char ARR[CHAR_NUM][CHAR_Y][CHAR_X]; //ASCII character number, character lines, character columns
    char CHAR[CHAR_NUM];
    int WEIGHT[CHAR_NUM];
    int SCALING_WEIGHT[CHAR_NUM];
} CHAR_SET;
typedef struct
{
    unsigned char *Image; //the raw pixels for images
    unsigned char *ASCII_Image; //the processed ascii image (contains characters)
    double *Weight; //the weight of processed ASCII image (subsections-by-subsections)
    unsigned Width, Height; //width height of raw images in pixels
    unsigned WidthTile, HeightTile; //width height of raw images in characters
} IMAGE;
typedef struct
{
    double width;
    double height;
    double scale; //a scale for default font SIZES to retain proportion and used for keeping IMAGE on screen
} SUBSECTION; //the size of one subsection in IMAGE which will be converted to chars
typedef struct _FILENAMELIST
{
    char *name;
    struct _FILENAMELIST *next;
} FILENAMELIST;
typedef struct _IMAGELIST
{
    IMAGE img;
    struct _IMAGELIST *next;
} IMAGELIST;
enum {ShowCaseMode1=-1,ShowCaseMode2=-2,ResizeCMD=-3,SetOutSTREAM=-4,SetLOOP=-5,ThumbMODE=-6,DEFAULTINFO=-7,EXIT=-8};

void CharSetImporter(CHAR_SET* CharSet,const char* filename);
void CalculateWeights(CHAR_SET* CharSet);
void Indexer(CHAR_SET* CharSet,int char_index);

void CalculateImageWeight(IMAGE* img,SUBSECTION* subsec,CONSOLEINFO Con);
void DecodePPM(IMAGE* PPM,const char* filename_dest);
void ProcessPPM(IMAGE* PPM,SUBSECTION subsec);
void ProcessPNG(IMAGE* PNG,SUBSECTION subsec); //out: IMAGE.Weight[]

//WriteOut to Console (PNG as ASCII); Parameters:IMAGE_SCALING_WEIGHT,CHAR_SET_SCALING_WEIGHT,width_tile,height_tile,writetofile?
unsigned char *IMAGE2ASCII(IMAGE img,CHAR_SET CharSet);

void WriteOut(IMAGE img,int writetofile,const char* outfilename);

void ListDirectory(CHAR2DARRAY* filenames,const char *rootfolder,int *choice_first); //files in PNGs directory will be Choices
int ThumbnailMode(char path[],CHAR2DARRAY filenames,CONSOLEINFO Con,CHAR_SET CharSet);
void ShowCase(CHAR_SET CharSet);
void ShowCase2(CHAR_SET CharSet);

char **Allocate2D(int numberof1D,int elementsper1D);
void Free2D(CHAR2DARRAY* array2d);
void Wait(int* loops,int* ch);

#endif // FUNCTIONS_H_INCLUDED

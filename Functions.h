/**
 * @file Functions.h
 * @brief Header file for the main functions and settings
 */
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#ifdef _WIN32
#include "WindowsConsoleIni.h"
#endif /* _WIN32 */
#include "TextScreenIni.h"
#include "lodepng.h"
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <stdlib.h>
/*-----------------------SETTINGS-----------------------------*/
#define PATH_LONG 100 /* Max how long is the path to the file */
#define ROOT_FOLDER "images"   /* Root folder */
/*#define ExtendedASCIICharSet // Define this to enable extended ASCII support */
/*#define PreciseASCIIgen // Define this to generate more accurate ASCII pictures (whole sizes are calculated) and CONSOLEINFO.FontSize will be = {CHAR_X,CHAR_Y}*/
#define AMPLIFIER 15 /* Contrast amplifier for PreciseASCIIGen, above this (ampl.) brightness (max) value a SUBSECTION's pixel brightnesses will be distributed */
#define CHAR_X 8  /* Width  of one ASCII Char */
#define CHAR_Y 14 /* Height of one ASCII Char */
/*------------------------------------------------------------*/
#ifdef ExtendedASCIICharSet
#define CHAR_NUM 221 /* Extended ASCII */
#else
#define CHAR_NUM 95  /* Standard ASCII */
#endif /* ExtendedASCIICharSet */

/**
 * @def _WIN32
 * @brief Windows only functions will be enabled
 * @def PATH_LONG
 * @def ROOT_FOLDER
 * @def ExtendedASCIICharSet
 * @brief enable ExtendedASCIICharSet
 * @def PreciseASCIIgen
 * @brief so pictures (to fit in max console screen) should be: CHAR_Y*(Con.CharAmount.Y - 2) high and CHAR_X*(Con.CharAmount.X - 1) wide
 * @see CalculateImageSizesPrec()
 * @def AMPLIFIER
 * @see PreciseProcessPNG()
 * @def CHAR_X
 * @brief Width  of one ASCII Cha
 * @def CHAR_Y
 * @brief Height of one ASCII Char
 * @def CHAR_NUM
 * @brief Number of ASCII Characters to choose from
 */

/** @brief ASCII character number, character lines, character columns */
typedef struct
{
    char ARR[CHAR_NUM][CHAR_Y][CHAR_X]; /**< The matrix for each ASCII CHAR */
    char CHAR[CHAR_NUM];                /**< Corresponding ASCII CHAR */
    int WEIGHT[CHAR_NUM];               /**< The weight (brightness) value of each ASCII CHAR */
    int SCALING_WEIGHT[CHAR_NUM];       /**< The scaled weight value for each ASCII CHAR @see CalculateWeights() */
} CHAR_SET;
/** @brief The struct for (.png .ppm) Images */
typedef struct
{
    unsigned char *Image;       /**< the raw pixels for images */
    unsigned char *ASCII_Image; /**< the processed ascii image (contains characters) */
    double *Weight;     /**< the weight of processed ASCII image (SUBSECTION-by-SUBSECTION) */
    unsigned Width;     /**< width of raw images in pixels */
    unsigned Height;    /**< heigth of raw images in pixels */
    unsigned WidthTile; /**< width of raw images in characters */
    unsigned HeightTile;/**< height of raw images in characters */
} IMAGE;
/** @brief the size of one subsection in IMAGE which will be converted to chars */
typedef struct
{
    double width;
    double height;
    double scale; /**< a scale for default font FontSize_X FontSize_Y to retain proportion and used for keeping IMAGE on screen @see CalculateImageSizes() */
} SUBSECTION;
/** @brief used to create temp list for filenames @see ListDirectory() */
typedef struct _FILENAMELIST
{
    char *name;
    struct _FILENAMELIST *next;
} FILENAMELIST;
/** @brief used to create temp list for images @see ThumbnailMode() */
typedef struct _IMAGELIST
{
    IMAGE img;
    struct _IMAGELIST *next;
} IMAGELIST;
/** @brief enum to use in main() switch-case */
enum {ShowCaseMode1=-1,ShowCaseMode2=-2,ResizeCMD=-3,SetOutSTREAM=-4,SetLOOP=-5,ThumbMODE=-6,DEFAULTINFO=-7,EXIT=-8};

void CharSetImporter(CHAR_SET* CharSet,const char* filename);
void CalculateWeights(CHAR_SET* CharSet);
void Indexer(CHAR_SET* CharSet,int char_index);

void CalculateImageSizes(IMAGE* img,SUBSECTION* subsec,CONSOLEINFO Con);
void CalculateImageSizesPrec(IMAGE* img,SUBSECTION* subsec,CONSOLEINFO Con);
void DecodePPM(IMAGE* PPM,const char* filename_dest);
void ProcessPPM(IMAGE* PPM,SUBSECTION subsec); /* out: IMAGE.Weight[] */
void ProcessPNG(IMAGE* PNG,SUBSECTION subsec); /* out: IMAGE.Weight[] */
void PreciseProcessPNG(IMAGE* PNG,SUBSECTION subsec,CHAR_SET CharSet); /* out: IMAGE.Weight[] */

/* WriteOut to Console (PNG as ASCII); Parameters:IMAGE_SCALING_WEIGHT,CHAR_SET_SCALING_WEIGHT,width_tile,height_tile,writetofile? */
void IMAGE2ASCII(IMAGE* img,CHAR_SET CharSet);

void WriteOut(IMAGE img,int writetofile,const char* outfilename);

void ListDirectory(CHAR2DARRAY* filenames,const char *rootfolder,int *choice_first); /* files in PNGs directory will be Choices */
int ThumbnailMode(char path[],CHAR2DARRAY filenames,CONSOLEINFO Con,CHAR_SET CharSet);
void ShowCase(CHAR_SET CharSet);
void ShowCase2(CHAR_SET CharSet);

char **Allocate2D(int numberof1D,int elementsper1D);
void Free2D(CHAR2DARRAY* array2d);
void Wait(int* loops,int* ch);

#endif /* FUNCTIONS_H_INCLUDED */

/**
 * @file Functions.h
 * @brief Header file for the main functions and settings
 */
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

/** @brief ASCII character number, character lines, character columns */
typedef struct
{
    char ARR[CHAR_NUM][CHAR_Y][CHAR_X]; /**< The matrix for each ASCII CHAR */
    unsigned char CHAR[CHAR_NUM];                /**< Corresponding ASCII CHAR */
    int WEIGHT[CHAR_NUM];               /**< The weight (brightness) value of each ASCII CHAR */
    int SCALING_WEIGHT[CHAR_NUM];       /**< The scaled weight value for each ASCII CHAR @see CalculateWeights() */
} CHAR_SET;
/** @brief The struct for (.png .ppm) Images */
typedef struct
{
    unsigned char *Image;       /**< the raw pixels for images */
    unsigned char *ASCII_Image; /**< the processed ascii image (contains characters) */
    TCODColor *ASCII_Color;     /**< ascii image colors */
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

void CalculateImageSizes(IMAGE* img,SUBSECTION* subsec,CONSOLEINFO& Con);
void CalculateImageSizesPrec(IMAGE* img,SUBSECTION* subsec,CONSOLEINFO& Con);
void DecodePPM(IMAGE* PPM,const char* filename_dest);
void ProcessPPM(IMAGE* PPM,SUBSECTION& subsec); /* out: IMAGE.Weight[] */
void ProcessPNG(IMAGE* PNG,SUBSECTION& subsec); /* out: IMAGE.Weight[] */
void PreciseProcessPNG(IMAGE* PNG,SUBSECTION& subsec,CHAR_SET& CharSet); /* out: IMAGE.Weight[] */

/* WriteOut to Console (PNG as ASCII); Parameters:IMAGE_SCALING_WEIGHT,CHAR_SET_SCALING_WEIGHT,width_tile,height_tile,writetofile? */
void IMAGE2ASCII(IMAGE* img,CHAR_SET& CharSet);

void WriteOut(IMAGE img,int writetofile,const char* outfilename);

void ListDirectory(CHAR2DARRAY* filenames,const char *rootfolder,int *choice_first); /* files in PNGs directory will be Choices */
int ThumbnailMode(char path[],CHAR2DARRAY filenames,CONSOLEINFO Con,CHAR_SET CharSet);
void ShowCase(CHAR_SET CharSet);
void ShowCase2(CHAR_SET CharSet);

char **Allocate2D(int numberof1D,int elementsper1D);
void Free2D(CHAR2DARRAY* array2d);
void Wait(int milliWait,int*loops);

#endif /* FUNCTIONS_H_INCLUDED */

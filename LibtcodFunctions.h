#ifndef LIBTCODFUNCTIONS_H_INCLUDED
#define LIBTCODFUNCTIONS_H_INCLUDED

/** @brief SIZES struct */
typedef struct
{
    int X; /**< X: width */
    int Y; /**< Y: Height */
} SIZES;
/** @brief CONSOLEINFO struct */
typedef struct
{
    SIZES FontSize;   /**< Console window font sizes */
    SIZES CharAmount; /**< Console window char amount per rows and columns */
    SIZES Size;       /**< Console window size in pixels */
} CONSOLEINFO;
/** @brief Custom char type for a 2D array */
typedef struct
{
    char **arr;  /**< Dynamic 2D char array */
    int arr_size;/**< Size of the array */
} CHAR2DARRAY;
/** @brief Input modes */
typedef enum {INPUT_NUMBER,INPUT_STRING} InputModes;

int MenuChoicesLibt(CHAR2DARRAY choices,CHAR2DARRAY options,int ConSizeY);
std::string InputHandler(InputModes mode,int x,int y);

#endif // LIBTCODFUNCTIONS_H_INCLUDED

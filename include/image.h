#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

// BMP header (14 bytes)
#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;      // must be 'BM'
    uint32_t bfSize;      // size of file
    uint16_t bfReserved1; // reserved
    uint16_t bfReserved2; // reserved
    uint32_t bfOffBits;   // offset to pixel data
} BMPHeader;

// DIB header (BITMAPINFOHEADER, 40 bytes)
typedef struct {
    uint32_t biSize;          // header size
    int32_t  biWidth;         // width
    int32_t  biHeight;        // height
    uint16_t biPlanes;        // must be 1
    uint16_t biBitCount;      // bits per pixel
    uint32_t biCompression;   // compression type
    uint32_t biSizeImage;     // image size
    int32_t  biXPelsPerMeter; // horizontal resolution
    int32_t  biYPelsPerMeter; // vertical resolution
    uint32_t biClrUsed;       // colors used
    uint32_t biClrImportant;  // important colors
} DIBHeader;
#pragma pack(pop)

typedef struct {
    BMPHeader header;
    DIBHeader dib;
    unsigned char* data;  // pixel data
} BMPImage;

// Function prototypes
BMPImage* load_bmp(const char* filename);
void free_bmp(BMPImage* image);
void fill_bmp(BMPImage* image, unsigned char color[3]);
int save_bmp(const char* filename, BMPImage* image);
BMPImage* rotate_bmp(const BMPImage* src, double angle_degrees);
BMPImage* scale_bmp(const BMPImage* src, double factor);

int embed_message(BMPImage *img, const char *message, int use_msb);
char *extract_message(BMPImage *img, int use_msb);
void free_message(char *msg);

BMPImage* resize_bmp(const BMPImage* src, int new_width, int new_height);
BMPImage* crop_bmp(const BMPImage* src, int x, int y, int crop_width, int crop_height);

#endif

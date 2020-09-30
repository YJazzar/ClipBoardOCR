#pragma once

//typedef __int32 int32_t;
//typedef unsigned __int32 uint32_t;

#include <cstdint>

typedef struct
{
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t  biPlanes;
    uint16_t  biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} DIB;



typedef struct
{
    uint16_t type;
    uint32_t bfSize;
    uint32_t reserved;
    uint32_t offset;
} HEADER;

typedef struct
{
    HEADER header;
    DIB dib;
} BMP;


namespace ClipBoard
{
    int copyToClipBoard(const char[]);

    int getFromClipBoard(const char* outputBMP);
}

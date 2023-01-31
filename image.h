#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct SImageHeaderBMP {
    char szName[2];
    uint32_t nSizeOfFile;
    uint32_t nReserverForApp;
    uint32_t nStartOfImage;
} __attribute__((packed));

struct SImageHeaderDIP {
    uint32_t nSizeOfHeader;
    uint32_t nWidth;
    uint32_t nHeight;
    uint16_t nNumberOfColorPlanes;
    uint16_t nNumberOfBitsPerPixels;
    char *pRemainingPart;
} __attribute__((packed));

struct SPixel {
    uint8_t r, g, b, a;
} __attribute__((packed));

struct SImage {
    struct SImageHeaderBMP *pHeaderBMP;
    struct SImageHeaderDIP *pHeaderDIP;
    uint32_t nPixelsCount;
    struct SPixel *pPixels;
};

struct SImage *createSImage(u_int32_t pixelsCount, struct SImageHeaderBMP *pHeaderBMP, struct SImageHeaderDIP *pHeaderDIP);

struct SImage *loadImageFromFile(const char *filename);

void clearImage(struct SImage *pImage);

void saveImageToFile(const char *outputFilename, struct SImage *image);

#endif

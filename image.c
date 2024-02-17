#include <assert.h>
#include "image.h"

struct SImage *createSImage(u_int32_t pixelsCount, struct SImageHeaderBMP *pHeaderBMP, struct SImageHeaderDIP *pHeaderDIP) {
    assert(pHeaderBMP != NULL && pHeaderDIP != NULL);
    
    struct SImage *pImage = (struct SImage *)malloc(sizeof(struct SImage));
    assert(pImage != NULL);

    pImage->pHeaderBMP = pHeaderBMP;
    pImage->pHeaderDIP = pHeaderDIP;
    pImage->nPixelsCount = pixelsCount;

    pImage->pPixels = (struct SPixel *)calloc(pixelsCount, sizeof(struct SPixel));
    assert(pImage->pPixels != NULL);

    return pImage;
}

struct SImage *loadImageFromFile(const char *filename) {
    struct SImageHeaderBMP *pBmpHeader = (struct SImageHeaderBMP *)malloc(sizeof(struct SImageHeaderBMP));
    assert(pBmpHeader != NULL);

    struct SImageHeaderDIP *pDipHeader = (struct SImageHeaderDIP *)malloc(sizeof(struct SImageHeaderDIP));
    assert(pDipHeader != NULL);

    FILE *pFile = fopen(filename, "rb");
    assert(pFile != NULL);

    fread(pBmpHeader, sizeof(struct SImageHeaderBMP), 1, pFile);
    fread(pDipHeader, sizeof(struct SImageHeaderDIP), 1, pFile);

    pDipHeader->pRemainingPart = (char *)calloc(pDipHeader->nSizeOfHeader - 16, sizeof(char));
    assert(pDipHeader->pRemainingPart != NULL);

    fseek(pFile, sizeof(char *) * -1, SEEK_CUR);

    fread(pDipHeader->pRemainingPart, sizeof(char), pDipHeader->nSizeOfHeader - 16, pFile);

    uint32_t nPixelsCount = pDipHeader->nWidth * pDipHeader->nHeight;

    struct SImage *pImage = createSImage(nPixelsCount, pBmpHeader, pDipHeader);
    assert(pImage != NULL);

    for (uint32_t i = 0; i < nPixelsCount; i++)
        fread(pImage->pPixels + i, pImage->pHeaderDIP->nNumberOfBitsPerPixels / 8, 1, pFile);

    fclose(pFile);
    return pImage;
}

void clearImage(struct SImage *pImage) {
    assert(pImage != NULL);
    assert(pImage->pHeaderBMP != NULL);
    assert(pImage->pHeaderDIP != NULL);
    assert(pImage->pHeaderDIP->pRemainingPart != NULL);
    assert(pImage->pPixels);

    free(pImage->pPixels);
    free(pImage->pHeaderDIP->pRemainingPart);
    free(pImage->pHeaderDIP);
    free(pImage->pHeaderBMP);
    free(pImage);
}

void saveImageToFile(const char *outputFilename, const struct SImage *const image) {
    assert(outputFilename != NULL && image != NULL);
    assert(image->pHeaderBMP != NULL);
    assert(image->pHeaderDIP != NULL);
    assert(image->pPixels != NULL);

    FILE *pFile = fopen(outputFilename, "wb");
    assert(pFile != NULL);

    fwrite(image->pHeaderBMP, sizeof(struct SImageHeaderBMP), 1, pFile);
    fwrite(image->pHeaderDIP, sizeof(struct SImageHeaderDIP), 1, pFile);
    fseek(pFile, sizeof(char *) * -1, SEEK_CUR);
    fwrite(image->pHeaderDIP->pRemainingPart, sizeof(char), image->pHeaderDIP->nSizeOfHeader - 16, pFile);

    for (uint32_t i = 0; i < image->nPixelsCount; i++)
        fwrite(image->pPixels + i, image->pHeaderDIP->nNumberOfBitsPerPixels / 8, 1, pFile);
    
    uint32_t nPadding = 
        image->pHeaderBMP->nSizeOfFile - image->pHeaderBMP->nStartOfImage - image->nPixelsCount * image->pHeaderDIP->nNumberOfBitsPerPixels / 8;

    fwrite("", sizeof(char), nPadding, pFile);
    fclose(pFile);
}

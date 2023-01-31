#include "image.h"


struct SImage *createSImage(u_int32_t pixelsCount, struct SImageHeaderBMP *pHeaderBMP, struct SImageHeaderDIP *pHeaderDIP) {
    if (pHeaderBMP == NULL || pHeaderDIP == NULL)
        return NULL;
    
    struct SImage *pImage = (struct SImage *)malloc(sizeof(struct SImage));

    if (pImage == NULL)
        return NULL;


    pImage->pHeaderBMP = pHeaderBMP;
    pImage->pHeaderDIP = pHeaderDIP;
    pImage->nPixelsCount = pixelsCount;

    pImage->pPixels = (struct SPixel *)calloc(pixelsCount, sizeof(struct SPixel));
    if (pImage->pPixels == NULL) {
        free(pImage);
        return NULL;
    }

    return pImage;
}

struct SImage *loadImageFromFile(const char *filename) {
    struct SImageHeaderBMP *pBmpHeader = (struct SImageHeaderBMP *)malloc(sizeof(struct SImageHeaderBMP));
    if (pBmpHeader == NULL)
        return NULL;

    struct SImageHeaderDIP *pDipHeader = (struct SImageHeaderDIP *)malloc(sizeof(struct SImageHeaderDIP));
    if (pDipHeader == NULL) {
        free(pBmpHeader);
        return NULL;
    }

    FILE *pFile = fopen(filename, "rb");

    fread(pBmpHeader, sizeof(struct SImageHeaderBMP), 1, pFile);
    fread(pDipHeader, sizeof(struct SImageHeaderDIP), 1, pFile);

    pDipHeader->pRemainingPart = (char *)calloc(pDipHeader->nSizeOfHeader - 16, sizeof(char));
    

    if (pDipHeader->pRemainingPart == NULL) {
        free(pBmpHeader);
        free(pDipHeader);
        fclose(pFile);
        return NULL;
    }

    fseek(pFile, sizeof(char *) * -1, SEEK_CUR);

    fread(pDipHeader->pRemainingPart, sizeof(char), pDipHeader->nSizeOfHeader - 16, pFile);

    uint32_t nPixelsCount = pDipHeader->nWidth * pDipHeader->nHeight;

    struct SImage *pImage = createSImage(nPixelsCount, pBmpHeader, pDipHeader);

    if (pImage == NULL) {
        free(pBmpHeader);
        free(pDipHeader->pRemainingPart);
        free(pDipHeader);
        fclose(pFile);
        return NULL;
    }

    for (uint32_t i = 0; i < nPixelsCount; i++)
        fread(pImage->pPixels + i, pImage->pHeaderDIP->nNumberOfBitsPerPixels / 8, 1, pFile);
        // fread(pImage->pPixels + i, sizeof(struct SPixel), 1, pFile);

    return pImage;
}

void clearImage(struct SImage *pImage) {
    if (pImage == NULL)
        return;

    if (pImage->pHeaderBMP != NULL)
        free(pImage->pHeaderBMP);

    if (pImage->pHeaderDIP != NULL) {
        if (pImage->pHeaderDIP->pRemainingPart != NULL)
            free(pImage->pHeaderDIP->pRemainingPart);
        free(pImage->pHeaderDIP);
    }

    if (pImage->pPixels != NULL)
        free(pImage->pPixels);

    free(pImage);
}

void saveImageToFile(const char *outputFilename, struct SImage *image) {
    if (outputFilename == NULL || image == NULL)
        return;
    
    if (image->pHeaderBMP == NULL || image->pHeaderDIP == NULL || image->pPixels == NULL)
        return;

    FILE *pFile = fopen(outputFilename, "wb");

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

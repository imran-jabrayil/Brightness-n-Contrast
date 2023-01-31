#include "autoadjust.h"


void imageAutoadjust(struct SImage *image) {
    if (image == NULL || image->pPixels == NULL)
        return;
    
    uint8_t redMin = image->pPixels[0].r;
    uint8_t greenMin = image->pPixels[0].g;
    uint8_t blueMin = image->pPixels[0].b;

    uint8_t redMax = image->pPixels[0].r;
    uint8_t greenMax = image->pPixels[0].g;
    uint8_t blueMax = image->pPixels[0].b;

    for (uint32_t i = 1; i < image->nPixelsCount; i++) {
        if (redMin > image->pPixels[i].r)
            redMin = image->pPixels[i].r;
        if (redMax < image->pPixels[i].r)
            redMax = image->pPixels[i].r;

        if (greenMin > image->pPixels[i].g)
            greenMin = image->pPixels[i].g;
        if (greenMax < image->pPixels[i].g)
            greenMax = image->pPixels[i].g;

        if (blueMin > image->pPixels[i].b)
            blueMin = image->pPixels[i].b;
        if (blueMax < image->pPixels[i].b)
            blueMax = image->pPixels[i].b;
    }

    redMax -= redMin;
    greenMax -= greenMin;
    blueMax -= blueMin;

    for (uint32_t i = 0; i < image->nPixelsCount; i++) {
        image->pPixels[i].r -= redMin;
        if (redMax != 0)
            image->pPixels[i].r *= 255.f / redMax;
        
        image->pPixels[i].g -= greenMin;
        if (greenMax != 0)
            image->pPixels[i].g *= 255.f / greenMax;

        image->pPixels[i].b -= blueMin;
        if (blueMax != 0)
            image->pPixels[i].b *= 255.f / blueMax;
    }
}

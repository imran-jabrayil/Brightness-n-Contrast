Introduction
============

The main purpose of this project was to process images and auto adjust
its brightness and contrast. The format of the images is **BMP**.

I decided to divide the project in three parts: **main**, **image** and
**autoadjust**.

**image** handles the operations on files, including import and export.
**autoadjust** is in charge of changing the brightness and the constrast
of an image. **main** interacts with a user and calls corresponding
functions.

Codes
=====

Libraries
---------

Different libraries were used to implement the project. Here are the
important ones:

-   **stdio.h**: Besides basic input-output calls, it provides *FILE*
    structure, which is used for handling files in the project.

-   **stdint.h**: I had to store unsigned integer values of different
    sizes. This library gives different data types including **uint8**,
    **uint16**, **uint32**, etc\...

-   **stdlib.h**: This library is used to work with heap memory.

-   **getopt.h**: Automates parsing of command-line arguments.

-   **string.h**: Eases interaction with strings.

image.h and image.c
-------------------

Now let's understand how the project works with **BMP** files. I
considered that this type of files consists of three parts: BMP-header,
DIP-header and all pixels. For each part I have implemented its own
structure. SImage combines them in one structure.

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

Some important notes here:

-   'packed' attribute is used for structures describing parts of the
    file. It makes reading them from file much easier. These structures
    will have almost the same size as the parts of the file. The only
    two differences are pointer used in *SImageHeaderDIP* and the size
    of *SPixel* in bits, but the solution will be a little later.

-   In the code, the order of color components is RGBA, whereas in
    example it's ABGR. To make this code work with the example it's
    enough to reverse the order and everything will work fine.

-   *SImage* structure contains the number of pixels of the image, even
    if it can be simply computated by multiplying width and height
    wheneverwe need pixels count but it makes coding more productive.

Here we also have functions to interact with files and these structures,
which are:

-   **createSImage**: makes creation of *SImage* object much easier. It
    takes the number of pixels and pointers to the headers as arguments
    and returns SImage file. The pixels themselves are not loaded yet
    here.

-   **loadImageFromFile**: by calling previous function it creates
    *SImage* object and then filles it with pixels data. Considering the
    *packed* attribute, the structures are read entirely, not
    part-by-part. There was a problem with pointer, which was previously
    filled by part of the first pixel. To solve it, I overwrote its
    value and moved the stream backwards with corresponding offset. At
    the end of execution, this function returns the *SImage* object with
    the whole image data inside.

        fread(pBmpHeader, sizeof(struct SImageHeaderBMP), 1, pFile);
        fread(pDipHeader, sizeof(struct SImageHeaderDIP), 1, pFile);

        pDipHeader->pRemainingPart = (char *)calloc(pDipHeader->nSizeOfHeader - 16, sizeof(char));

        fseek(pFile, sizeof(char *) * -1, SEEK_CUR);
        fread(pDipHeader->pRemainingPart, sizeof(char), pDipHeader->nSizeOfHeader - 16, pFile);

-   **saveImageToFile**: It's quite similar to the previous one, just
    all actions are reversed. It returns nothing.

-   **clearImage**: Used to clear all memory holded by a loaded image.

autoadjust.h and autoadjust.c
-----------------------------

Let's move on to the auto adjustment algorithm implementation. The main
idea is the same as it was written in the project description. Just to
mention, here we have *imageAutoadjust* function, which takes the
*SImage* object's address as a parameter and implements the algorithm on
its pixels. It modifies the object's data that's why it doesn't need to
return anything. The image passed as an argument will be affected
itself.

main.c
------

Besides the *main* function, the main file has two helping functions, so
I'll start from them.

-   **showHelp**: The program is user-friendly, that's why it tells the
    user how to interact with it. All help messages are taken out to
    this function. It prints out all ways to interact with the program
    and shows the user the flags which can be passed to the program.

-   **endsWith**: A simple program to ensure that the file is in proper
    *BMP* format.

Now about the main function itself.

Firstly, it parses all arguments to give the proper result.

    while ((opt = getopt(argc, argv, options)) != -1) {
        switch (opt) {
            case 'h':
                showHelp();
                return 0;
            case 'o':
                output = optarg;
                break;
            case 1:
                input = optarg;
                break;
        }
    }

The program also automatically handles the errors.

    if (input == NULL) {
        fprintf(stderr, "Not a correct input.\n");
        return -1;
    }

    if (!endsWith(input, ".bmp")) {
        fprintf(stderr, "%s is not a BMP file.\n", input);
        return -1;
    }

    struct SImage *pImage = loadImageFromFile(input);
    if (pImage == NULL) {
        fprintf(stderr, "Couldn't load the file.\n");
        return -1;
    }

And finally, it calls the necessary functions to gain the processed
image.

    imageAutoadjust(pImage);
    saveImageToFile(output, pImage);
    clearImage(pImage);

**Running the program**

Arguments:

-h (optional) : Show help

-o (optional) : Define output filename (output.bmp by default)

Usage:

./program name input filename -o output filename

./program name -h

Results of the process are in folder with the code.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdint.h>
#include "image.h"
#include "autoadjust.h"

void showHelp();
bool endsWith(const char *string, const char *suffix);

int main(int argc, char *argv[]) {
    const char *input = NULL, *output = "output.bmp";

    int opt;
    const char *options = "-:o:h";
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
    imageAutoadjust(pImage);
    saveImageToFile(output, pImage);

    clearImage(pImage);

    return 0;
}

void showHelp() {
    printf("\tArguments: \n");
    printf("-h (optional) => Show help\n");
    printf("-o (optional) => Define output filename (output.bmp by default)\n");
    
    printf("\tUsage: \n");
    printf("./{program_name} {input_filename} -o {output_filename}\n");
    printf("./{program_name} -h\n");
}

bool endsWith(const char *string, const char *suffix) {
    if (string == NULL && suffix == NULL)
        return true;
    if (string == NULL || suffix == NULL)
        return false;

    uint32_t stringLen = strlen(string);
    uint32_t suffixLen = strlen(suffix);

    if (suffixLen > stringLen)
        return false;
    
    return strncmp(string + stringLen - suffixLen, suffix, suffixLen) == 0;
}
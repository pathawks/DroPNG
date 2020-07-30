/**
 * Display info from PNG file
 *
 * Author:      Pat Hawks
 * Created on:  5 Aug 2016
 * Source File: dropngs.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "dropngs.h"

const char *const VERSION_STRING = "0.0.0";

const char *COLOR_TYPE[] = {
    "grayscale",
    NULL,
    "RGB",
    "indexed",
    "grayscale+alpha",
    NULL,
    "RGB+alpha",
    NULL
};

const char *RENDERING_INTENT[] = {
    "Perceptual",
    "Relative colorimetric",
    "Saturation",
    "Absolute colorimetric"
};

const char *COMPRESSION_METHOD[] = {
    "Fastest",
    "Fast",
    "Default",
    "Maximum"
};

void help(){}

static inline uint32_t fgetint(FILE* image) {
    uint32_t a;
    fread(&a, 4, 1, image);
    return ntohl(a);
}

static inline unsigned int dataToInt(unsigned char *d) {
    return htonl(*(uint32_t *)d);
}

static inline unsigned int dataToShort(unsigned char *d) {
    return htons(*(uint16_t *)d);
}

static uint32_t calculateCrc(PngChunk* chunk) {
    uint32_t crc = 0xffffffff;
    crc = update_crc(crc, &chunk->type, 4);
    crc = update_crc(crc, chunk->data, chunk->length);
    return ~crc;
}

int validChunkCrc(PngChunk* chunk) {
    return chunk->crc == calculateCrc(chunk);
}

void readChunkInfo(FILE* image, PngChunk* chunk) {
    chunk->length = fgetint(image);
    chunk->data = malloc(chunk->length);

    fread(chunk->type, 1, 4, image);
    fread(chunk->data, 1, chunk->length, image);

    chunk->crc = fgetint(image);
    chunk->badcrc = !validChunkCrc(chunk);
}

void displayChunkInfo(PngChunk* chunk) {
    size_t i;
    printf("%4s size:%8d\tcrc: %08X %s\n", chunk->type, chunk->length, chunk->crc, chunk->badcrc?"BAD CRC!":"");

    switch (*(int *)chunk->type) {
    case PNG_CHUNKTYPE_IHDR:
        printf("     Width:\t\t%u\n     Height:\t\t%u\n     Bit depth:\t\t%u\n     Color type:\t%s\n     Interlace:\t\t%s\n",
                dataToInt(chunk->data),
                dataToInt(chunk->data+4),
                chunk->data[8],
                COLOR_TYPE[chunk->data[9]&7],
                chunk->data[12] ? "Adam7" : "none"
        );
        break;
    case PNG_CHUNKTYPE_tEXt:
    case PNG_CHUNKTYPE_iTXt:
        printf("     ");
        for(i=0; i<chunk->length; ++i)
            if (chunk->data[i])
                printf("%c", chunk->data[i]);
            else
                printf("\t");
        printf("\n");
        break;
    case PNG_CHUNKTYPE_tIME:
        printf("     Last modified:\t%4d-%2d-%2d at %2d:%2d:%2d\n",
                dataToShort(chunk->data),
                *(char *)(chunk->data + 2),
                *(char *)(chunk->data + 3),
                *(char *)(chunk->data + 4),
                *(char *)(chunk->data + 5),
                *(char *)(chunk->data + 6)
        );
        break;
    case PNG_CHUNKTYPE_gAMA:
        printf("     Gamma:\t\t%u\t(%.1f)\n", dataToInt(chunk->data), 100000./dataToInt(chunk->data));
        break;
    case PNG_CHUNKTYPE_cHRM:
        printf("     White Point x:\t%u\n     White Point y:\t%u\n     Red x:\t\t%u\n     Red y:\t\t%u\n     Green x:\t\t%u\n     Green y:\t\t%u\n     Blue x:\t\t%u\n     Blue y:\t\t%u\n",
                dataToInt(chunk->data),
                dataToInt(chunk->data+4),
                dataToInt(chunk->data+8),
                dataToInt(chunk->data+12),
                dataToInt(chunk->data+16),
                dataToInt(chunk->data+20),
                dataToInt(chunk->data+24),
                dataToInt(chunk->data+28)
        );
        break;
    case PNG_CHUNKTYPE_iCCP:
        printf("     Profile name:\t%s\n", chunk->data);
        break;
    case PNG_CHUNKTYPE_pHYs:
        printf("     Pixels per %s:\t%ux%u\n",
                chunk->data[8]?"meter":"unit",
                dataToInt(chunk->data),
                dataToInt(chunk->data+4)
        );
        break;
    case PNG_CHUNKTYPE_sRGB:
        printf("     Rendering intent:\t%s\n",
                RENDERING_INTENT[*chunk->data&3]
        );
        break;
    case PNG_CHUNKTYPE_IDAT:
    /* Since the zlib stream can span multiple chunks, this only
     * makes sense for the first chunk.

        printf("     Compression method:\t%s\n",
                COMPRESSION_METHOD[(chunk->data[1]&3)],
        );
    */
        break;
    case PNG_CHUNKTYPE_PLTE:
        if (chunk->length % 3) {
            printf("Chunk length must be divisible by 3!");
            break;
        }
        for(i=0; i<chunk->length; i+=3) {
            int color = dataToInt(chunk->data + i) & 0x00ffffff;
            printf("     #%06x\n", color);
        }
        break;
    }
    return;
}

void releaseChunk(PngChunk* chunk) {
    free(chunk->data);
    chunk->data = 0;
    chunk->length = 0;
    *chunk->type = 0;
}

int main(int argc, char **argv) {
    FILE *file;
    PngChunk chunk;
    int invalidPng = 0;

    file = fopen(argv[1], "r");
    if (! (argc > 1 && file)) {
        help();
        return EXIT_FAILURE;
    }

    fseek(file, 8, SEEK_SET);

    fgetc(file);
    while (!feof(file) && !ferror(file)) {
        fseek(file, -1, SEEK_CUR);
        readChunkInfo(file, &chunk);
        displayChunkInfo(&chunk);
        invalidPng |= chunk.badcrc;
        releaseChunk(&chunk);
        fgetc(file);
    }

    return invalidPng;
}

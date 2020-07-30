#ifndef DROPNGS_H
#define DROPNGS_H

typedef struct {
    unsigned int length;
    char type[5];
    unsigned char *data;
    unsigned int crc : 32;
    int badcrc;
} PngChunk;

typedef struct {
    int width;
    int height;
    int depth;
    int color_type;
    int interlaced;
} PngImage;

enum {
    PNG_COLORTYPE_GREYSCALE = 0,
    PNG_COLORTYPE_TRUECOLOR = 2,
    PNG_COLORTYPE_INDEXED   = 3,
    PNG_COLORTYPE_GREYALPHA = 4,
    PNG_COLORTYPE_TRUEALPHA = 6,
};

void displayChunkInfo(PngChunk*);
void readChunkInfo(FILE*, PngChunk*);
int main(int argc, char **argv);

#define PNG_CHUNKTYPE_tEXt (int) 'tEXt'
#define PNG_CHUNKTYPE_PLTE (int) 'PLTE'
#define PNG_CHUNKTYPE_tRNS (int) 'tRNS'
#define PNG_CHUNKTYPE_bKGD (int) 'bKGD'
#define PNG_CHUNKTYPE_tIME (int) 'tIME'
#define PNG_CHUNKTYPE_gAMA (int) 'gAMA'
#define PNG_CHUNKTYPE_cHRM (int) 'cHRM'
#define PNG_CHUNKTYPE_IHDR (int) 'IHDR'
#define PNG_CHUNKTYPE_iTXt (int) 'iTXt'
#define PNG_CHUNKTYPE_pHYs (int) 'pHYs'
#define PNG_CHUNKTYPE_iCCP (int) 'iCCP'
#define PNG_CHUNKTYPE_sRGB (int) 'sRGB'
#define PNG_CHUNKTYPE_IDAT (int) 'IDAT'

uint32_t update_crc(unsigned long crc, void *buf, int len);

#endif

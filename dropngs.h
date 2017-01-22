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

#define PNG_CHUNKTYPE_tEXt 0x74584574
#define PNG_CHUNKTYPE_PLTE 0x45544c50
#define PNG_CHUNKTYPE_tRNS 0x534e5274
#define PNG_CHUNKTYPE_bKGD 0x44474b62
#define PNG_CHUNKTYPE_tIME 0x454d4974
#define PNG_CHUNKTYPE_gAMA 0x414d4167
#define PNG_CHUNKTYPE_cHRM 0x4d524863
#define PNG_CHUNKTYPE_IHDR 0x52444849
#define PNG_CHUNKTYPE_iTXt 0x74585469
#define PNG_CHUNKTYPE_pHYs 0x73594870
#define PNG_CHUNKTYPE_iCCP 0x50434369
#define PNG_CHUNKTYPE_sRGB 0x42475273
#define PNG_CHUNKTYPE_IDAT 0x54414449

uint32_t update_crc(unsigned long crc, void *buf, int len);

#endif

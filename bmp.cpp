
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

typedef unsigned char byte;
typedef unsigned short word;

#ifdef WIN32
#include <pshpack2.h>
#endif

typedef struct
{
	word bfType; 
    int bfSize;
	word bfReserved1; 
	word bfReserved2;
	int bfOffBits;
} BitmapFileHeader;

typedef struct tagBitmapFileHeader
{
	int biSize; 
	int biWidth; 
	int biHeight; 
	word biPlanes; 
	word biBitCount;
	int biCompression; 
	int biSizeImage; 
	int biXPelsPerMeter; 
	int biYPelsPerMeter; 
	int biClrUsed; 
	int biClrImportant; 
} BitmapInfoHeader; 
 
typedef struct
{
	byte rgbBlue;
	byte rgbGreen; 
    byte rgbRed;
	byte rgbReserved;
} BitmapRGBQuad; 

#ifdef WIN32
#include <poppack.h>
#endif

void Image::ConvertColors() {
	byte * pl;
	unsigned int i;
	Color *cl;

	for (i = 0; i < height; i++) {
		pl = &data[trueWidth * i];
		cl = (Color*) pl;
		unsigned int j;
		for (j = 0; j < width; j++) {
			byte temp;
			temp = cl->r;
			cl->r = cl->b;
			cl->b = temp;
			cl = (Color *) ((byte*) cl + stride);
		}
	}
}

Color &Image::Pixels(unsigned int x, unsigned int y) {
	return *(Color *) &(data[y*trueWidth+x*stride]);
}

bool Image::LoadFromBmpFile(char * filename) {
	FILE *file = 0;
	BitmapFileHeader bmfh;
	BitmapInfoHeader bmih;
	int cbBmpBits;
	byte * pb;
	bool rez = false;

	// File exists?
	if ((file = fopen (filename, "rb")) == 0)
		return false;
	
	// Read file header
	if (fread (&bmfh, sizeof bmfh, 1/*count*/, file) != 1)
		goto GetOut;

	// Bogus file header check
	if (!(bmfh.bfReserved1 == 0 && bmfh.bfReserved2 == 0))
		goto GetOut;

	// Read info header
	if (fread (&bmih, sizeof bmih, 1/*count*/, file) != 1)
		goto GetOut;

	// Bogus info header check
	if (!(bmih.biSize == sizeof bmih && bmih.biPlanes == 1))
		goto GetOut;

	// Bogus bit depth?  Only 24-bit && 32-bit supported.
	if (!(bmih.biBitCount == 24 || bmih.biBitCount == 32))
		goto GetOut;

	// Bogus compression?  Only non-compressed supported.
	if (bmih.biCompression != 0) //BI_RGB)
		goto GetOut;


	width = bmih.biWidth;
	height = bmih.biHeight;

	// Read bitmap bits (remainder of file)
	cbBmpBits = bmfh.bfSize - ftell (file);

	pb = (byte *) malloc (cbBmpBits);
	if (pb == 0) {
		goto GetOut;
	}

	if (fread (pb, cbBmpBits, 1/*count*/, file) != 1) {
		free (pb);
		goto GetOut;
	}

	// data is actually stored with the width being rounded up to a multiple of 4
	int biTrueWidth;
	stride = bmih.biBitCount/8;
	biTrueWidth = bmih.biWidth*stride;
	trueWidth = biTrueWidth = (biTrueWidth + 3) & ~3;

	// reverse the order of the data.
	data = (byte *) malloc (cbBmpBits);
	if (data == 0){
		free(pb);
		goto GetOut;
	}

	int i;
	pb += (bmih.biHeight - 1) * biTrueWidth;
	for(i = 0; i < bmih.biHeight; i++) {
		memmove ((void *) &data[biTrueWidth * i], pb, biTrueWidth);
		pb -= biTrueWidth;
	}

	pb += biTrueWidth;
	free (pb);

	rez = true;
	ConvertColors();
GetOut:
	if (file) 
		fclose (file);
	return rez;
}

void Image::Clear() {
	free(data);
	width = height = 0;
	stride = 0;
}

Color White(255, 255, 255), Black(0, 0, 0);

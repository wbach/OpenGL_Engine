#pragma once
#include <string>
#include <vector>

#ifdef _WIN32
#define PACKED
#pragma pack(push,1)
#elif
#define PACKED __attribute__ ((__packed__))
#endif



// for Linux platform, plz make sure the size of data type is correct for BMP spec.
// if you use this on Windows or other platforms, plz pay attention to this.
typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

typedef struct tagBITMAPFILEHEADER
{
	WORD    bfType; // 2  /* Magic identifier */
	DWORD   bfSize; // 4  /* File size in bytes */
	WORD    bfReserved1; // 2
	WORD    bfReserved2; // 2
	DWORD   bfOffBits; // 4 /* Offset to image data, bytes */
} PACKED BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	DWORD    biSize; // 4 /* Header size in bytes */
	LONG     biWidth; // 4 /* Width of image */
	LONG     biHeight; // 4 /* Height of image */
	WORD     biPlanes; // 2 /* Number of colour planes */
	WORD     biBitCount; // 2 /* Bits per pixel */
	DWORD    biCompress; // 4 /* Compression type */
	DWORD    biSizeImage; // 4 /* Image size in bytes */
	LONG     biXPelsPerMeter; // 4
	LONG     biYPelsPerMeter; // 4 /* Pixels per meter */
	DWORD    biClrUsed; // 4 /* Number of colours */
	DWORD    biClrImportant; // 4 /* Important colours */
}  PACKED BITMAPINFOHEADER;

typedef struct
{
	BYTE    b;
	BYTE    g;
	BYTE    r;
} RGB_data; // RGB TYPE, plz also make sure the order

int bmp_generator(const std::string& filename, int width, int height, const unsigned char *data);

void writeRawData(const std::string& filename, std::vector<unsigned char> data);
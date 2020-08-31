/*目前仅支持24位或32位彩色BMP文件*/
#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#ifndef BYTE
#	define BYTE unsigned char
#endif // !BYTE
#ifndef WORD
#	define WORD unsigned short
#endif // !WORD
#ifndef DWORD
#	define DWORD unsigned long
#endif // !DWORD
#ifndef QWORD
#	define QWORD unsigned long long
#endif // !QWORD

enum COLOR_MODE {
	//目前仅支持RGB和RGBA两种格式
	RGB, RGBA, GRAY, BINARY
};
class Pixel {
public:
	BYTE red, green, blue, alpha;
	unsigned int grayLevel();
	static bool pixelCmp(Pixel &a, Pixel &b);
};
class myImage {
	friend class linearFilter;
	friend class OSFilter;
	friend class PADDING;
public:
	myImage() {};
	myImage(myImage &img);
	myImage(const char* p);
	~myImage();
	void discolor();
	void invertColor();
	const unsigned int const *generate_histogram();
	void save_histogram(const char *p);
	void equalize_histogram();
	int open(const char* p);
	int save();
	int save(const char* path);
	const char *path();
	unsigned int depth();
	unsigned int width();
	unsigned int height();
	const unsigned int const *histogram();
	void printHeader();
	bool isOpened();

	static int superposeImage(myImage &a, myImage &b, myImage &c);
	static int mergeImage(myImage &a, myImage &b, myImage &c);
#pragma pack(2)
	struct BMPFileHeader {
		WORD bfType;         //2字节，文件类型；总应是0x4d42('BM')
		DWORD bfSize;           //4字节，文件大小；字节为单位。
		WORD bfReserved1;    //2字节，保留，必须设置为0
		WORD bfReserved2;    //2字节，保留，必须设置为0
		DWORD bfOffBits;        //4字节的偏移，表示从文件头到位图数据的偏移
	};
	struct BMPInfoHeader {
		DWORD biSize;           //4字节，信息头的大小，即40；
		DWORD biWidth;                   //4字节，以像素为单位说明图像的宽度；
		DWORD biHeight;                  //4字节，以像素为单位说明图像的高度；如果为正，说明位图倒立  
		WORD biPlanes;       //2字节，为目标设备说明颜色平面数，总被设置为1；
		WORD biBitCount;     //2字节，说明比特数/像素数，值有1、2、4、8、16、24、32；
		DWORD biCompression;    //4字节，说明图像的压缩类型，0(BI_RGB)表示不压缩；
		DWORD biSizeImage;      //4字节，说明位图数据的大小，当用BI_RGB格式时，可以设置为0；
		DWORD biXPelsPerMeter;           //4字节，表示水平分辨率，单位是像素/米；
		DWORD biYPelsPerMeter;           //4字节，表示垂直分辨率，单位是像素/米；
		DWORD biClrUsed;        //4字节，位图使用的调色板中的颜色索引数，为0说明使用所有；
		DWORD biClrImportant;
	};
#pragma pack()
	
private:
	bool Opened = false;
	BMPFileHeader FileHeader;
	BMPInfoHeader InfoHeader;
	std::string Path;
	std::vector<std::vector<Pixel> > Data;
	const unsigned int *Histogram = nullptr;
	std::vector<Pixel> LookupTable;
	COLOR_MODE Mode;
	unsigned int Depth, Width, Height;
};


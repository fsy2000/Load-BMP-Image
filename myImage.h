/*Ŀǰ��֧��24λ��32λ��ɫBMP�ļ�*/
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
	//Ŀǰ��֧��RGB��RGBA���ָ�ʽ
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
		WORD bfType;         //2�ֽڣ��ļ����ͣ���Ӧ��0x4d42('BM')
		DWORD bfSize;           //4�ֽڣ��ļ���С���ֽ�Ϊ��λ��
		WORD bfReserved1;    //2�ֽڣ���������������Ϊ0
		WORD bfReserved2;    //2�ֽڣ���������������Ϊ0
		DWORD bfOffBits;        //4�ֽڵ�ƫ�ƣ���ʾ���ļ�ͷ��λͼ���ݵ�ƫ��
	};
	struct BMPInfoHeader {
		DWORD biSize;           //4�ֽڣ���Ϣͷ�Ĵ�С����40��
		DWORD biWidth;                   //4�ֽڣ�������Ϊ��λ˵��ͼ��Ŀ�ȣ�
		DWORD biHeight;                  //4�ֽڣ�������Ϊ��λ˵��ͼ��ĸ߶ȣ����Ϊ����˵��λͼ����  
		WORD biPlanes;       //2�ֽڣ�ΪĿ���豸˵����ɫƽ�������ܱ�����Ϊ1��
		WORD biBitCount;     //2�ֽڣ�˵��������/��������ֵ��1��2��4��8��16��24��32��
		DWORD biCompression;    //4�ֽڣ�˵��ͼ���ѹ�����ͣ�0(BI_RGB)��ʾ��ѹ����
		DWORD biSizeImage;      //4�ֽڣ�˵��λͼ���ݵĴ�С������BI_RGB��ʽʱ����������Ϊ0��
		DWORD biXPelsPerMeter;           //4�ֽڣ���ʾˮƽ�ֱ��ʣ���λ������/�ף�
		DWORD biYPelsPerMeter;           //4�ֽڣ���ʾ��ֱ�ֱ��ʣ���λ������/�ף�
		DWORD biClrUsed;        //4�ֽڣ�λͼʹ�õĵ�ɫ���е���ɫ��������Ϊ0˵��ʹ�����У�
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


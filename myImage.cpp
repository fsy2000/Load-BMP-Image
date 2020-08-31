#include "myImage.h"

myImage::myImage(myImage &img) :Path(img.Path), Depth(img.Depth), Width(img.Width), Height(img.Height), FileHeader(img.FileHeader), InfoHeader(img.InfoHeader) {
	if (!img.isOpened()) {
		throw std::exception("Error! Input file isn't opened.");
	}
	Opened = true;
	Data = img.Data;
}

myImage::myImage(const char* p) {
	open(p);
}
int myImage::open(const char* p) {
	Path = p;
	std::ifstream imgFile;
	imgFile.open(p, std::ios::in | std::ios::binary);
	if (!imgFile.is_open()) {
		std::cerr << "Error! File: " << p << " doesn't exist!" << std::endl;
		return -1;
	}
	Opened = true;
	imgFile.read((char *)(&FileHeader), sizeof(FileHeader));
	imgFile.read((char *)(&InfoHeader), sizeof(InfoHeader));
	if (FileHeader.bfType != 0x4d42) {
		std::cerr << "Error! Unsupported format. Required: BMP." << std::endl;
		return -1;
	}
	Width = InfoHeader.biWidth;
	Height = InfoHeader.biHeight;
	Depth = InfoHeader.biBitCount;
	switch (Depth)
	{
	case 24:
		Mode = RGB;
		break;
	case 32:
		Mode = RGBA;
		break;
	default:
		std::cerr << "Error! Unsupported depth. Required: 24, 32." << std::endl;
		return -1;
		break;
	}
	imgFile.seekg(FileHeader.bfOffBits);
	BYTE redBuf, greenBuf, blueBuf, alphaBuf;
	Pixel pixelBuf;
	switch (Mode)
	{
	case RGB:
		while (!imgFile.eof()) {
			std::vector<Pixel>tmp;
			for (size_t i = 0; i < InfoHeader.biWidth; i++)
			{
				imgFile.read((char *)&blueBuf, sizeof(BYTE));
				imgFile.read((char *)&greenBuf, sizeof(BYTE));
				imgFile.read((char *)&redBuf, sizeof(BYTE));
				pixelBuf = { redBuf,greenBuf,blueBuf, (BYTE)255 };
				tmp.push_back(pixelBuf);
			}
			Data.push_back(tmp);
		}
		break;
	case RGBA:
		while (!imgFile.eof()) {
			std::vector<Pixel>tmp;
			for (size_t i = 0; i < InfoHeader.biWidth; i++)
			{
				imgFile.read((char *)&blueBuf, sizeof(BYTE));
				imgFile.read((char *)&greenBuf, sizeof(BYTE));
				imgFile.read((char *)&redBuf, sizeof(BYTE));
				imgFile.read((char *)&alphaBuf, sizeof(BYTE));
				pixelBuf = { redBuf,greenBuf,blueBuf,alphaBuf };
				tmp.push_back(pixelBuf);
			}
			Data.push_back(tmp);
		}
		break;
	}
	imgFile.close();
	return 0;
}
const unsigned int const *myImage::generate_histogram() {
	if (!Histogram) {
		int size;
		if (Mode == GRAY)
			size = pow(2, Depth);
		if (Mode == BINARY)
			size = 2;
		if (Mode == RGBA)
			size = pow(2, Depth / 4);
		else
			size = pow(2, Depth / 3);
		unsigned *hist = new unsigned[size]();
		for (size_t i = 0; i < Height; i++) {
			for (std::vector<Pixel>::iterator iter2 = Data[i].begin(); iter2 != Data[i].end(); iter2++) {
				unsigned tmp = iter2->grayLevel();
				hist[tmp]++;
			}
		}
		Histogram = hist;
	}
	return Histogram;
}
const unsigned int const *myImage::histogram() {
	return Histogram;
}
void myImage::save_histogram(const char *p) {
	std::ofstream outFile;
	outFile.open(p, std::ios::out);
	int size;
	if (Mode == GRAY)
		size = pow(2, Depth);
	if (Mode == BINARY)
		size = 2;
	if (Mode == RGBA)
		size = pow(2, Depth / 4);
	else
		size = pow(2, Depth / 3);
	outFile << "gray level" << ',' << "amount" << std::endl;
	for (int i = 0; i < size; i++)
		outFile << i << ',' << Histogram[i] << std::endl;
	outFile.close();
}

void myImage::discolor() {
	for (size_t i = 0; i < Height; i++) {
		for (std::vector<Pixel>::iterator iter2 = Data[i].begin(); iter2 != Data[i].end(); iter2++) {
			BYTE colorBuf = (iter2->red + iter2->green + iter2->blue) / 3;
			iter2->red = colorBuf;
			iter2->green = colorBuf;
			iter2->blue = colorBuf;
		}
	}
	//InfoHeader.biClrUsed = 0;
	//InfoHeader.biClrImportant = 0;
}
void myImage::invertColor() {
	for (size_t i = 0; i < Height; i++) {
		for (std::vector<Pixel>::iterator iter2 = Data[i].begin(); iter2 != Data[i].end(); iter2++) {
			iter2->red = 255 - iter2->red;
			iter2->green = 255 - iter2->green;
			iter2->blue = 255 - iter2->blue;
		}
	}
}
void myImage::equalize_histogram() {
	generate_histogram();
	int size;
	if (Mode == GRAY)
		size = pow(2, Depth);
	if (Mode == BINARY)
		size = 2;
	if (Mode == RGBA)
		size = pow(2, Depth / 4);
	else
		size = pow(2, Depth / 3);
	unsigned total = InfoHeader.biWidth*InfoHeader.biHeight;
	double *hist = new double[size];
	for (int i = 0; i < size; i++) {
		long long sum = 0;
		for (int j = 0; j < i; j++)
			sum += Histogram[j];
		hist[i] = (double)sum / total;
	}
	for (size_t i = 0; i < Height; i++) {
		for (std::vector<Pixel>::iterator iter2 = Data[i].begin(); iter2 != Data[i].end(); iter2++) {
			int tmp = (iter2->red + iter2->green + iter2->blue) / 3;
			int newlevel = round((size - 1)*hist[tmp]);
			double ratio = (double)newlevel / tmp;
			iter2->red *= ratio;
			iter2->green *= ratio;
			iter2->blue *= ratio;
		}
	}
	delete[] Histogram;
	Histogram = nullptr;
	generate_histogram();
	//InfoHeader.biClrUsed = 0;
	//InfoHeader.biClrImportant = 0;
}
int myImage::save() {
	int i = save(Path.c_str());
	return i;
}
int myImage::save(const char* p) {
	std::ofstream outFile(p, std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		std::cerr << "Writing file error!" << std::endl;
		return -1;
	}
	outFile.write((char *)(&FileHeader), sizeof(FileHeader));
	outFile.write((char *)(&InfoHeader), sizeof(InfoHeader));
	outFile.seekp(FileHeader.bfOffBits);
	switch (Mode)
	{
	case RGB:
		for (std::vector<std::vector<Pixel> >::iterator iter1 = Data.begin(); iter1 != Data.end(); iter1++)
		{
			for (std::vector<Pixel>::iterator iter2 = (*iter1).begin(); iter2 != (*iter1).end(); iter2++) {
				outFile.write((char *)&iter2->blue, sizeof(BYTE));
				outFile.write((char *)&iter2->green, sizeof(BYTE));
				outFile.write((char *)&iter2->red, sizeof(BYTE));
			}
		}
		break;
	case RGBA:
		for (std::vector<std::vector<Pixel> >::iterator iter1 = Data.begin(); iter1 != Data.end(); iter1++)
		{
			for (std::vector<Pixel>::iterator iter2 = (*iter1).begin(); iter2 != (*iter1).end(); iter2++) {
				outFile.write((char *)&iter2->blue, sizeof(BYTE));
				outFile.write((char *)&iter2->green, sizeof(BYTE));
				outFile.write((char *)&iter2->red, sizeof(BYTE));
				outFile.write((char *)&iter2->alpha, sizeof(BYTE));
			}
		}
		break;
	case GRAY:
		break;
	default:
		break;
	}
	outFile.close();
	return 0;
}

const char *myImage::path() {
	return Path.c_str();
}
unsigned int myImage::depth() {
	return Depth;
}
unsigned int myImage::width() {
	return Width;
}
unsigned int myImage::height() {
	return Height;
}
bool myImage::isOpened() {
	return Opened;
}
int myImage::superposeImage(myImage & a, myImage & b, myImage &tmp) {
	if (a.Width != b.Width || a.Height != b.Height) {
		throw std::invalid_argument("Images size error!");
		return -1;
	}
	tmp=a;
	int redBuf, greenBuf, blueBuf;
	for (size_t i = 0; i < a.Height; i++) {
		for (size_t j = 0; j < a.Width; j++) {
			redBuf = tmp.Data[i][j].red + b.Data[i][j].red;
			greenBuf = tmp.Data[i][j].green + b.Data[i][j].green;
			blueBuf = tmp.Data[i][j].blue + b.Data[i][j].blue;
			if (redBuf <= 255)
				tmp.Data[i][j].red = (BYTE)redBuf;
			else
				tmp.Data[i][j].red = (BYTE)255;
			if (greenBuf <= 255)
				tmp.Data[i][j].green = (BYTE)greenBuf;
			else
				tmp.Data[i][j].green = (BYTE)255;
			if (blueBuf <= 255)
				tmp.Data[i][j].blue = (BYTE)blueBuf;
			else
				tmp.Data[i][j].blue = (BYTE)255;
		}
	}
	return 0;
}
int myImage::mergeImage(myImage & a, myImage & b, myImage &tmp) {
	if (a.Width != b.Width || a.Height != b.Height) {
		throw std::invalid_argument("Images size error!");
		return -1;
	}
	tmp = a;
	double redBuf, greenBuf, blueBuf;
	for (size_t i = 0; i < a.Height; i++) {
		for (size_t j = 0; j < a.Width; j++) {
			redBuf = sqrt(tmp.Data[i][j].red*tmp.Data[i][j].red + b.Data[i][j].red*b.Data[i][j].red);
			greenBuf = sqrt(tmp.Data[i][j].green*tmp.Data[i][j].green + b.Data[i][j].green*b.Data[i][j].green);
			blueBuf = sqrt(tmp.Data[i][j].blue*tmp.Data[i][j].blue + b.Data[i][j].blue*b.Data[i][j].blue);
			if (redBuf <= 255)
				tmp.Data[i][j].red = (BYTE)redBuf;
			else
				tmp.Data[i][j].red = (BYTE)255;
			if (greenBuf <= 255)
				tmp.Data[i][j].green = (BYTE)greenBuf;
			else
				tmp.Data[i][j].green = (BYTE)255;
			if (blueBuf <= 255)
				tmp.Data[i][j].blue = (BYTE)blueBuf;
			else
				tmp.Data[i][j].blue = (BYTE)255;
		}
	}
	return 0;
}
void myImage::printHeader() {
	std::cout <<
		"*****FILE HEADER*****\n" <<
		"bfType: " << FileHeader.bfType << "\n" <<
		"bfSize: " << FileHeader.bfSize << "\n" <<
		"bfReserved1: " << FileHeader.bfReserved1 << "\n" <<
		"bfReserved2: " << FileHeader.bfReserved2 << "\n" <<
		"bfOffBits: " << FileHeader.bfOffBits << "\n" <<
		"*****INFO HEADER*****\n" <<
		"biSize: " << InfoHeader.biSize << "\n" <<
		"biWidth: " << InfoHeader.biWidth << "\n" <<
		"biHeight: " << InfoHeader.biHeight << "\n" <<
		"biPlanes: " << InfoHeader.biPlanes << "\n" <<
		"biBitCount: " << InfoHeader.biBitCount << "\n" <<
		"biCompression: " << InfoHeader.biCompression << "\n" <<
		"biSizeImage: " << InfoHeader.biSizeImage << "\n" <<
		"biXPelsPerMeter: " << InfoHeader.biXPelsPerMeter << "\n" <<
		"biYPelsPerMeter: " << InfoHeader.biYPelsPerMeter << "\n" <<
		"biClrUsed: " << InfoHeader.biClrUsed << "\n" <<
		"biClrImportant: " << InfoHeader.biClrImportant << "\n";
}
myImage::~myImage() {
	if (Histogram)
		delete[] Histogram;
}

unsigned int Pixel::grayLevel() {
	return floor((red + green + blue) / 3);
}

bool Pixel::pixelCmp(Pixel &a, Pixel &b) {
	return a.grayLevel() < b.grayLevel();
}

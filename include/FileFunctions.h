#pragma once


#include <iostream>

#include "lodepng.h"



namespace FileFunctions
{
	int convertBMPtoPNG(const char* inputBMP, const char* outputPNG);
	unsigned decodeBMP(std::vector<unsigned char>& image, unsigned& w, unsigned& h, const std::vector<unsigned char>& bmp);

	int execTesseract(const char* inputPNG, const char* outputTXT);
	std::string readFileContents(const char* filePath);
}

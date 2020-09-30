
#include "FileFunctions.h"

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <codecvt>
#include <locale>
#include <atlbase.h>
#include <fstream>

int FileFunctions::convertBMPtoPNG(const char* inputBMP, const char* outputPNG)
{
	std::vector<unsigned char> bmp;
	lodepng::load_file(bmp, inputBMP);
	std::vector<unsigned char> image;
	unsigned w, h;
	unsigned error = decodeBMP(image, w, h, bmp);

	if (error) {
		std::cout << "BMP decoding error " << error << std::endl;
		return 0;
	}

	std::vector<unsigned char> png;
	error = lodepng::encode(png, image, w, h);

	if (error) {
		std::cout << "PNG encoding error " << error << ": " << lodepng_error_text(error) << std::endl;
		return 0;
	}

	lodepng::save_file(png, outputPNG);
	return 0;
}


// The function was from an example given by the developers of lodepng
//	returns 0 if all went ok, non-0 if error
//	output image is always given in RGBA (with alpha channel), even if it's a BMP without alpha channel
unsigned FileFunctions::decodeBMP(std::vector<unsigned char>& image, unsigned& w, unsigned& h, const std::vector<unsigned char>& bmp) {
	static const unsigned MINHEADER = 54; //minimum BMP header size

	if (bmp.size() < MINHEADER) return -1;
	if (bmp[0] != 'B' || bmp[1] != 'M') return 1; //It's not a BMP file if it doesn't start with marker 'BM'
	unsigned pixeloffset = bmp[10] + 256 * bmp[11]; //where the pixel data starts
	//read width and height from BMP header
	w = bmp[18] + bmp[19] * 256;
	h = bmp[22] + bmp[23] * 256;
	//read number of channels from BMP header
	if (bmp[28] != 24 && bmp[28] != 32) return 2; //only 24-bit and 32-bit BMPs are supported.
	unsigned numChannels = bmp[28] / 8;

	//The amount of scanline bytes is width of image times channels, with extra bytes added if needed
	//to make it a multiple of 4 bytes.
	unsigned scanlineBytes = w * numChannels;
	if (scanlineBytes % 4 != 0) scanlineBytes = (scanlineBytes / 4) * 4 + 4;

	unsigned dataSize = scanlineBytes * h;
	if (bmp.size() < dataSize + pixeloffset) return 3; //BMP file too small to contain all pixels

	image.resize(w * h * 4);

	/*
	There are 3 differences between BMP and the raw image buffer for LodePNG:
	-it's upside down
	-it's in BGR instead of RGB format (or BRGA instead of RGBA)
	-each scanline has padding bytes to make it a multiple of 4 if needed
	The 2D for loop below does all these 3 conversions at once.
	*/
	for (unsigned y = 0; y < h; y++)
		for (unsigned x = 0; x < w; x++) {
			//pixel start byte position in the BMP
			unsigned bmpos = pixeloffset + (h - y - 1) * scanlineBytes + numChannels * x;
			//pixel start byte position in the new raw image
			unsigned newpos = 4 * y * w + 4 * x;
			if (numChannels == 3) {
				image[newpos + 0] = bmp[bmpos + 2]; //R
				image[newpos + 1] = bmp[bmpos + 1]; //G
				image[newpos + 2] = bmp[bmpos + 0]; //B
				image[newpos + 3] = 255;            //A
			}
			else {
				image[newpos + 0] = bmp[bmpos + 2]; //R
				image[newpos + 1] = bmp[bmpos + 1]; //G
				image[newpos + 2] = bmp[bmpos + 0]; //B
				image[newpos + 3] = bmp[bmpos + 3]; //A
			}
		}
	return 0;
}

int FileFunctions::execTesseract(const char* inputPNG, const char* outputTXT)
{
	
	std::wstring tesseractPath = L"C:\\Program Files\\Tesseract-OCR\\tesseract.exe";
	std::string params = " " +  std::string(inputPNG) + " " + std::string(outputTXT);
	std::wstring wparams = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(params);

	//LPSTR command = (LPSTR)(tesseractPath + " " + params).c_str();

	std::cout << "Executing:: " << tesseractPath.c_str() << " " << params.c_str() << std::endl;

	/*HINSTANCE r = ShellExecuteA(GetConsoleWindow(), "open", (LPSTR)tesseractPath.c_str(), (LPSTR)params.c_str(), NULL, 0);
	std::cout << "REsult: " << r << std::endl;
	std::cout << "REsult: " << static_cast<int>(reinterpret_cast<uintptr_t>(r)) << std::endl;*/


	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = tesseractPath.c_str();
	ShExecInfo.lpParameters = wparams.c_str();
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	CloseHandle(ShExecInfo.hProcess);

	std::cout << "Finished executing!" << std::endl;

	return 0;
}

std::string FileFunctions::readFileContents(const char* filePath)
{
	std::cout << "Opening the file: " << filePath << std::endl;
	
	std::string output;
	std::string line;
	std::ifstream myfile(filePath);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			std::cout << line << '\n';
			output += line;
		}
		myfile.close();
	}

	else std::cout << "Unable to open file" << std::endl;

	return output;
}
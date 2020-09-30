// ClipboardOCR2.cpp : Defines the entry point for the application.
//

#include <ClipBoard.h>
#include <FileFunctions.h>

#include "main.h"


void extractTextFromImage()
{

	// Save a BMP image from the clipboard: 
	int r = ClipBoard::getFromClipBoard("D:\\dev\\temp\\image.bmp");
	//output += "Result of saving BMP from clipboard: " + r + std::string("\n");


	// Convert the BMP image into a PNG:
	r = FileFunctions::convertBMPtoPNG("D:\\dev\\temp\\image.bmp", "D:\\dev\\temp\\image.png");
	//output += "Result of converting BMP image to PNG: " + r + std::string("\n");


	// Execute tesseract on the image:
	r = FileFunctions::execTesseract("D:\\dev\\temp\\image.png", "D:\\dev\\temp\\image");
	//output += "Result of converting the PNG to text: " + r + std::string("\n");


	// Read the file contents:
	std::string temp = FileFunctions::readFileContents("D:\\dev\\temp\\image.txt");
	//output += "Result of converting reading the text image: " + temp + "haha\n";


	// Copy the text extracted from the image to the clipboard
	ClipBoard::copyToClipBoard(temp.c_str());
	//output = temp;


	//std::cout << "Done!\n\n";
}

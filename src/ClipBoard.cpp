#include "ClipBoard.h"

#include <windows.h>
#include <iostream>
#include <fstream>

int ClipBoard::copyToClipBoard(const char data[]) {
    // Find lendth of string
    int nStrLen = strlen(data);


    // Get handle to memory to hold phrase
    HANDLE hData;
    hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, nStrLen + 1);

    // Pointer to allow char copying
    char* ptrData = NULL;
    ptrData = (char*)GlobalLock(hData);     // Get pointer from handle
    memcpy(ptrData, data, nStrLen + 1);     // Copy over the phrase

    GlobalUnlock(hData); // Free the handle

    if (!OpenClipboard(GetConsoleWindow()))
    {
        std::cout << "Could Not open the clipboard\n";
        return -1;
    }
    else
    {
        std::cout << "Clipboard was opened\n";
    }

    if (!EmptyClipboard())
    {
        std::cout << "Could not empty out the clipboard\n";
        return -1;
    }
    else
    {
        std::cout << "Emptied clipboard\n";
    }

    if (SetClipboardData(CF_TEXT, hData) == NULL)
    {
        std::cout << "Could not copy data to the clipboard\n";
    }
    else
    {
        std::cout << "Data copied to the clipboard:" << data << std::endl;
    }

    CloseClipboard();//finished!!

    return 0;
}



int ClipBoard::getFromClipBoard(const char* outputBMP)
{
    std::cout << "Format Bitmap: " << IsClipboardFormatAvailable(CF_BITMAP) << "\n";
    std::cout << "Format DIB: " << IsClipboardFormatAvailable(CF_DIB) << "\n";
    std::cout << "Format DIBv5: " << IsClipboardFormatAvailable(CF_DIBV5) << "\n";

    if (IsClipboardFormatAvailable(CF_BITMAP) || IsClipboardFormatAvailable(CF_DIB) || IsClipboardFormatAvailable(CF_DIBV5))
    {
        if (OpenClipboard(NULL))
        {
            HANDLE hClipboard = GetClipboardData(CF_DIB);

            if (!hClipboard)
            {
                hClipboard = GetClipboardData(CF_DIBV5);
            }

            if (hClipboard != NULL && hClipboard != INVALID_HANDLE_VALUE)
            {
                void* dib = GlobalLock(hClipboard);

                if (dib)
                {
                    DIB* info = reinterpret_cast<DIB*>(dib);
                    BMP bmp = { 0 };
                    bmp.header.type = 0x4D42;
                    bmp.header.offset = 54;
                    bmp.header.bfSize = info->biSizeImage + bmp.header.offset;
                    bmp.dib = *info;

                    std::cout << "Type: " << std::hex << bmp.header.type << std::dec << "\n";
                    std::cout << "bfSize: " << bmp.header.bfSize << "\n";
                    std::cout << "Reserved: " << bmp.header.reserved << "\n";
                    std::cout << "Offset: " << bmp.header.offset << "\n";
                    std::cout << "biSize: " << bmp.dib.biSize << "\n";
                    std::cout << "Width: " << bmp.dib.biWidth << "\n";
                    std::cout << "Height: " << bmp.dib.biHeight << "\n";
                    std::cout << "Planes: " << bmp.dib.biPlanes << "\n";
                    std::cout << "Bits: " << bmp.dib.biBitCount << "\n";
                    std::cout << "Compression: " << bmp.dib.biCompression << "\n";
                    std::cout << "Size: " << bmp.dib.biSizeImage << "\n";
                    std::cout << "X-res: " << bmp.dib.biXPelsPerMeter << "\n";
                    std::cout << "Y-res: " << bmp.dib.biYPelsPerMeter << "\n";
                    std::cout << "ClrUsed: " << bmp.dib.biClrUsed << "\n";
                    std::cout << "ClrImportant: " << bmp.dib.biClrImportant << "\n";

                    std::ofstream file(outputBMP, std::ios::out | std::ios::binary);
                    if (file)
                    {
                        file.write(reinterpret_cast<char*>(&bmp.header.type), sizeof(bmp.header.type));
                        file.write(reinterpret_cast<char*>(&bmp.header.bfSize), sizeof(bmp.header.bfSize));
                        file.write(reinterpret_cast<char*>(&bmp.header.reserved), sizeof(bmp.header.reserved));
                        file.write(reinterpret_cast<char*>(&bmp.header.offset), sizeof(bmp.header.offset));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biSize), sizeof(bmp.dib.biSize));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biWidth), sizeof(bmp.dib.biWidth));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biHeight), sizeof(bmp.dib.biHeight));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biPlanes), sizeof(bmp.dib.biPlanes));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biBitCount), sizeof(bmp.dib.biBitCount));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biCompression), sizeof(bmp.dib.biCompression));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biSizeImage), sizeof(bmp.dib.biSizeImage));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biXPelsPerMeter), sizeof(bmp.dib.biXPelsPerMeter));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biYPelsPerMeter), sizeof(bmp.dib.biYPelsPerMeter));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biClrUsed), sizeof(bmp.dib.biClrUsed));
                        file.write(reinterpret_cast<char*>(&bmp.dib.biClrImportant), sizeof(bmp.dib.biClrImportant));
                        file.write(reinterpret_cast<char*>(info + 1), bmp.dib.biSizeImage);
                        file.close();
                    }

                    GlobalUnlock(dib);
                }
            }

            CloseClipboard();
        }
    }

    return -1;
}
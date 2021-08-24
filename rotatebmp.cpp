#include<iostream>
#include<fstream>
#include<windows.h>
using namespace std;


BITMAPFILEHEADER Header;                                               // BMP file header
BITMAPINFOHEADER Info;                                                 // BMP info header
int Height, Width;                                                     // height and width of image


int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "Too less command line arguments." << endl;            // number of command line arguments should be no less than 3
        return -1;
    }
    ifstream src(argv[1], ios_base::binary);                           // open src.bmp for reading
    if (src)
    {
        src.read(reinterpret_cast<char*>(&Header), sizeof(Header));    // read BMP file header
        src.read(reinterpret_cast<char*>(&Info), sizeof(Info));        // read BMP info header
        if (Header.bfType != 0x4d42)
        {
            cout << "Wrong format." << endl;                           // the file format should be BMP
            return -1;
        }
        Height = Info.biHeight;
        Width = Info.biWidth;
        RGBTRIPLE* srcData = new RGBTRIPLE[Height * Width * 3];        // source image data
        int srcCompletion = (4 - Width * 3 % 4) % 4;                   // bytes needed for completion of every row of the original image
        for (int i = 0; i < Height; i++)                               // read BMP data
        {
            src.read(reinterpret_cast<char*>(srcData) + i * Width * 3, Width * 3);
            src.seekg(srcCompletion, ios::cur);                        // skip bytes for completion
        }
        src.close();
        ofstream dest(argv[2], ios_base::binary);                      // open dest.bmp for writing
        LONG tmp;
        Info.biHeight = Width;
        Info.biWidth = Height;                                         // exchange the height and width
        tmp = Info.biXPelsPerMeter;
        Info.biXPelsPerMeter = Info.biYPelsPerMeter;
        Info.biYPelsPerMeter = tmp;                                    // exchange the vertical and horizontal resolution
        int destCompletion = (4 - Height * 3 % 4) % 4;                 // bytes needed for completion of every row of the rotated image
        Info.biSizeImage = (Height * 3 + destCompletion) * Width;      // new image size
        Header.bfSize = Info.biSizeImage + 54;                         // new file size
        dest.write(reinterpret_cast<char*>(&Header), sizeof(Header));  // write BMP file header
        dest.write(reinterpret_cast<char*>(&Info), sizeof(Info));      // write BMP info header
        Height = Info.biHeight;
        Width = Info.biWidth;
        RGBTRIPLE* destData = new RGBTRIPLE[Height * Width * 3];       // new BMP data
        for (int i = 0; i < Height; i++)                               // compute new BMP data
            for (int j = 0; j < Width; j++)
                *(destData + i * Width + j) = *(srcData + j * Height + Height - i - 1);
        char* zeros = new char[destCompletion];                        // bytes for completion
        memset(zeros, 0, destCompletion);                              // complete with 0
        for (int i = 0; i < Height; i++)                               // write BMP image data
        {
            dest.write(reinterpret_cast<char*>(destData) + i * Width * 3, Width * 3);
            dest.write(zeros, destCompletion);                         // write bytes for completion
        }
        dest.close();
        delete[] destData;
        delete[] srcData;
        delete[] zeros;
    }
    else
    {
        cout << "File not found." << endl;
        return -1;
    }
	return 0;
}
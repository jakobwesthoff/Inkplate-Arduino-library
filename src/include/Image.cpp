/*
Image.cpp
Inkplate 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "Image.h"

#include "../libs/TJpeg/TJpg_Decoder.h"
#include "defines.h"
#include "pgmspace.h"

Image *_imagePtrJpeg = nullptr;
Image *_imagePtrPng = nullptr;

Image::Image(int16_t w, int16_t h) : Adafruit_GFX(w, h)
{
    _imagePtrJpeg = this;
    _imagePtrPng = this;
}

bool Image::drawImage(const String path, int x, int y, bool dither, bool invert)
{
    return drawImage(path.c_str(), x, y, dither, invert);
};

bool Image::drawImage(const char *path, int x, int y, bool dither, bool invert)
{
    if (strncmp(path, "http://", 7) == 0 || strncmp(path, "https://", 8) == 0)
    {
        if (strstr(path, ".bmp") != NULL || strstr(path, ".dib") != NULL)
            return drawBitmapFromWeb(path, x, y, dither, invert);
        if (strstr(path, ".jpg") != NULL || strstr(path, ".jpeg") != NULL)
            return drawJpegFromWeb(path, x, y, dither, invert);
        if (strstr(path, ".png") != NULL)
            return drawPngFromWeb(path, x, y, dither, invert);
    }
    else
    {
        if (strstr(path, ".bmp") != NULL || strstr(path, ".dib") != NULL)
            return drawBitmapFromSd(path, x, y, dither, invert);
        if (strstr(path, ".jpg") != NULL || strstr(path, ".jpeg") != NULL)
            return drawJpegFromSd(path, x, y, dither, invert);
        if (strstr(path, ".png") != NULL)
            return drawPngFromSd(path, x, y, dither, invert);
    }
    return 0;
};

bool Image::drawImage(const uint8_t *buf, int x, int y, int16_t w, int16_t h, uint8_t c, uint8_t bg)
{
    if (getDisplayMode() == INKPLATE_1BIT && bg == 0xFF)
        drawBitmap(x, y, buf, w, h, c);
    else if (getDisplayMode() == INKPLATE_1BIT && bg != 0xFF)
        drawBitmap(x, y, buf, w, h, c, bg);
    else if (getDisplayMode() == INKPLATE_3BIT)
        drawBitmap3Bit(x, y, buf, w, h);
    return 1;
}

void Image::drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char *_p, int16_t _w, int16_t _h)
{
    if (getDisplayMode() != INKPLATE_3BIT)
        return;
    uint8_t _rem = _w & 1;
    int i, j;
    int xSize = (_w >> 1) + _rem;

    startWrite();
    for (i = 0; i < _h; i++)
    {
        for (j = 0; j < xSize - 1; j++)
        {
            writePixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4) >> 1);
            writePixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff) >> 1);
        }
        writePixel((j * 2) + _x, i + _y, (*(_p + xSize * (i) + j) >> 4) >> 1);
        if (_rem == 0)
            writePixel((j * 2) + 1 + _x, i + _y, (*(_p + xSize * (i) + j) & 0xff) >> 1);
    }
    endWrite();
}

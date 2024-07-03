#include "tgaimage.h"

#include <math.h>
#include <string.h>
#include <time.h>

#include <fstream>
#include <iostream>

TGAImage::TGAImage() : data(NULL), width(0), height(0), byteSpp(0) {}

TGAImage::TGAImage(int w, int h, int bpp)
    : data(NULL), width(w), height(h), byteSpp(bpp) {
    unsigned long nBytes = width * height * byteSpp;
    data = new unsigned char[nBytes];
    memset(data, 0, nBytes);
}

TGAImage::TGAImage(const TGAImage &img) {
    width = img.width;
    height = img.height;
    byteSpp = img.byteSpp;
    unsigned long nBytes = width * height * byteSpp;
    data = new unsigned char[nBytes];
    memcpy(data, img.data, nBytes);
}

TGAImage::~TGAImage() {
    if (data) delete[] data;
}

TGAImage &TGAImage::operator=(const TGAImage &img) {
    if (this != &img) {
        if (data) delete[] data;
        width = img.width;
        height = img.height;
        byteSpp = img.byteSpp;
        unsigned long nBytes = width * height * byteSpp;
        data = new unsigned char[nBytes];
        memcpy(data, img.data, nBytes);
    }
    return *this;
}

bool TGAImage::readTGAFile(const char *filename) {
    if (data) delete[] data;
    data = NULL;
    std::ifstream in;
    in.open(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "can't open file" << filename << "\n";
        in.close();
        return false;
    }
    TGA_Header header;
    in.read((char *)&header, sizeof(header));
    if (!in.good()) {
        in.close();
        std::cerr << "an error occured while reading the header\n";
        return false;
    }
    width = header.width;
    height = header.height;
    byteSpp = header.bitSperPixel >> 3;
    if (width <= 0 || height <= 0 ||
        (byteSpp != GRAYSCALE && byteSpp != RGB && byteSpp != RGBA)) {
        in.close();
        std::cerr << "bad bpp (or width/height) value\n";
        return false;
    }
    unsigned long nBytes = byteSpp * width * height;
    data = new unsigned char[nBytes];
    if (3 == header.dataTypeCode || 2 == header.dataTypeCode) {
        in.read((char *)data, nBytes);
        if (!in.good()) {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    } else if (10 == header.dataTypeCode || 11 == header.dataTypeCode) {
        if (!loadRleData(in)) {
            in.close();
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
    } else {
        in.close();
        std::cerr << "unknown file format " << (int)header.dataTypeCode << "\n";
        return false;
    }
    if (!(header.imageDescriptor & 0x20)) {
        flipVertically();
    }
    if (!(header.imageDescriptor & 0x10)) {
        flipHorizontally();
    }
    std::cerr << width << "x" << height << "/" << byteSpp * 8 << "\n";
    in.close();

    return true;
}

bool TGAImage::loadRleData(std::ifstream &in) {
    unsigned long pixelCnt = width * height;
    unsigned long curPixel = 0;
    unsigned long curByte = 0;
    TGAColor colorBuffer;
    do {
        unsigned char chunkHeader = 0;
        chunkHeader = in.get();
        if (!in.good()) {
            std::cerr << "an error occured while reading the data\n";
            return false;
        }
        if (chunkHeader < 128) {
            chunkHeader++;
            for (int i = 0; i < chunkHeader; i++) {
                in.read((char *)colorBuffer.raw, byteSpp);
                if (!in.good()) {
                    std::cerr << "an error occured while reading the header\n";
                    return false;
                }
                for (int t = 0; t < byteSpp; t++) {
                    data[curByte++] = colorBuffer.raw[t];
                }
                curPixel++;
                if (curPixel > pixelCnt) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        } else {
            chunkHeader -= 127;
            in.read((char *)colorBuffer.raw, byteSpp);
            if (!in.good()) {
                std::cerr << "an error occured while reading the header\n";
                return false;
            }
            for (int i = 0; i < chunkHeader; i++) {
                for (int t = 0; t < byteSpp; t++) {
                    data[curByte++] = colorBuffer.raw[t];
                }
                curPixel++;
                if (curPixel > pixelCnt) {
                    std::cerr << "Too many pixels read\n";
                    return false;
                }
            }
        }
    } while (curPixel < pixelCnt);
    return true;
}

bool TGAImage::unloadRleData(std::ofstream &out) {
    const unsigned char max_chunk_length = 128;
    unsigned long npixels = width * height;
    unsigned long curpix = 0;
    while (curpix < npixels) {
        unsigned long chunkstart = curpix * byteSpp;
        unsigned long curbyte = curpix * byteSpp;
        unsigned char run_length = 1;
        bool raw = true;
        while (curpix + run_length < npixels && run_length < max_chunk_length) {
            bool succ_eq = true;
            for (int t = 0; succ_eq && t < byteSpp; t++) {
                succ_eq = (data[curbyte + t] == data[curbyte + t + byteSpp]);
            }
            curbyte += byteSpp;
            if (1 == run_length) {
                raw = !succ_eq;
            }
            if (raw && succ_eq) {
                run_length--;
                break;
            }
            if (!raw && !succ_eq) {
                break;
            }
            run_length++;
        }
        curpix += run_length;
        out.put(raw ? run_length - 1 : run_length + 127);
        if (!out.good()) {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
        out.write((char *)(data + chunkstart),
                  (raw ? run_length * byteSpp : byteSpp));
        if (!out.good()) {
            std::cerr << "can't dump the tga file\n";
            return false;
        }
    }
    return true;
}

bool TGAImage::writeTGAFile(const char *filename, bool rle) {
    unsigned char developer_area_ref[4] = {0, 0, 0, 0};
    unsigned char extension_area_ref[4] = {0, 0, 0, 0};
    unsigned char footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O',
                                'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};
    std::ofstream out;
    out.open(filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        out.close();
        return false;
    }
    TGA_Header header;
    memset((void *)&header, 0, sizeof(header));
    header.bitSperPixel = byteSpp << 3;
    header.width = width;
    header.height = height;
    header.dataTypeCode =
        (byteSpp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.imageDescriptor = 0x20;  // top-left origin
    out.write((char *)&header, sizeof(header));
    if (!out.good()) {
        out.close();
        std::cerr << "can't dump the tga file\n";
        return false;
    }
    if (!rle) {
        out.write((char *)data, width * height * byteSpp);
        if (!out.good()) {
            std::cerr << "can't unload raw data\n";
            out.close();
            return false;
        }
    } else {
        if (!unloadRleData(out)) {
            out.close();
            std::cerr << "can't unload rle data\n";
            return false;
        }
    }
    out.write((char *)developer_area_ref, sizeof(developer_area_ref));
    if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char *)extension_area_ref, sizeof(extension_area_ref));
    if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.write((char *)footer, sizeof(footer));
    if (!out.good()) {
        std::cerr << "can't dump the tga file\n";
        out.close();
        return false;
    }
    out.close();
    return true;
}

TGAColor TGAImage::get(int x, int y) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {
        return TGAColor();
    }
    return TGAColor(data + (x + y * width) * byteSpp, byteSpp);
}

bool TGAImage::set(int x, int y, TGAColor c) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {
        return false;
    }
    memcpy(data + (x + y * width) * byteSpp, c.raw, byteSpp);
    return true;
}

int TGAImage::getByteSpp() { return byteSpp; }

int TGAImage::getWidth() { return width; }

int TGAImage::getHeight() { return height; }

bool TGAImage::flipHorizontally() {
    if (!data) return false;
    int half = width >> 1;
    for (int i = 0; i < half; i++) {
        for (int j = 0; j < height; j++) {
            TGAColor c1 = get(i, j);
            TGAColor c2 = get(width - 1 - i, j);
            set(i, j, c2);
            set(width - 1 - i, j, c1);
        }
    }
    return true;
}

bool TGAImage::flipVertically() {
    if (!data) return false;
    unsigned long bytes_per_line = width * byteSpp;
    unsigned char *line = new unsigned char[bytes_per_line];
    int half = height >> 1;
    for (int j = 0; j < half; j++) {
        unsigned long l1 = j * bytes_per_line;
        unsigned long l2 = (height - 1 - j) * bytes_per_line;
        memmove((void *)line, (void *)(data + l1), bytes_per_line);
        memmove((void *)(data + l1), (void *)(data + l2), bytes_per_line);
        memmove((void *)(data + l2), (void *)line, bytes_per_line);
    }
    delete[] line;
    return true;
}

unsigned char *TGAImage::buffer() { return data; }

void TGAImage::clear() { memset((void *)data, 0, width * height * byteSpp); }

bool TGAImage::scale(int w, int h) {
    if (w <= 0 || h <= 0 || !data) return false;
    unsigned char *tdata = new unsigned char[w * h * byteSpp];
    int nscanline = 0;
    int oscanline = 0;
    int erry = 0;
    unsigned long nlinebytes = w * byteSpp;
    unsigned long olinebytes = width * byteSpp;
    for (int j = 0; j < height; j++) {
        int errx = width - w;
        int nx = -byteSpp;
        int ox = -byteSpp;
        for (int i = 0; i < width; i++) {
            ox += byteSpp;
            errx += w;
            while (errx >= (int)width) {
                errx -= width;
                nx += byteSpp;
                memcpy(tdata + nscanline + nx, data + oscanline + ox, byteSpp);
            }
        }
        erry += h;
        oscanline += olinebytes;
        while (erry >= (int)height) {
            if (erry >= (int)height << 1)  // it means we jump over a scanline
                memcpy(tdata + nscanline + nlinebytes, tdata + nscanline,
                       nlinebytes);
            erry -= height;
            nscanline += nlinebytes;
        }
    }
    delete[] data;
    data = tdata;
    width = w;
    height = h;
    return true;
}

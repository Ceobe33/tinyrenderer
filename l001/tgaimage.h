#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

// 这里是什么意思 难道是把这个结构打包起来，然后方便外界调用？
// 这样猜测是因为下面看到了pop
#pragma pack(push, 1)

struct TGA_Header {
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLenth;
    char colorMapDepth;
    short x_origin;
    short y_origin;
    short width;
    short height;
    char bitSperPixel;
    char imageDescriptor;
};

#pragma pack(pop)

struct TGAColor {
    union {
        struct {
            unsigned char b, g, r, a;
        };

        unsigned char raw[4];
        unsigned int val;
    };

    // 不太理解 byte spp 是什么东西, 打算继续看逻辑,后面应该有用这个的地方
    // 然后再进行猜测
    int byteSpp;

    TGAColor() : val(0), byteSpp(1) {}

    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
        : b(B), g(G), r(R), a(A), byteSpp(4) {}

    TGAColor(int v, int bpp) : val(v), byteSpp(bpp) {}

    TGAColor(const TGAColor &c) : val(c.val), byteSpp(c.byteSpp) {}

    TGAColor(const unsigned char *p, int bpp) : val(0), byteSpp(bpp) {
        for (int i = 0; i < bpp; i++) {
            raw[i] = p[i];
        }
    }

    TGAColor &operator=(const TGAColor &c) {
        if (this != &c) {
            byteSpp = c.byteSpp;
            val = c.val;
        }
        return *this;
    }
};

class TGAImage {
   protected:
    unsigned char *data;
    int width;
    int height;
    int byteSpp;

    bool loadRleData(std::ifstream &in);
    bool unloadRleData(std::ofstream &out);

   public:
    enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };

    TGAImage();
    TGAImage(int w, int h, int bpp);
    TGAImage(const TGAImage &img);
    bool readTGAFile(const char *filename);
    bool writeTGAFile(const char *filename, bool rle = true);
    bool flipHorizontally();
    bool flipVertically();
    bool scale(int x, int y);
    TGAColor get(int x, int y);
    bool set(int x, int y, TGAColor c);
    ~TGAImage();
    TGAImage &operator=(const TGAImage &img);
    int getWidth();
    int getHeight();
    int getByteSpp();
    unsigned char *buffer();
    void clear();
};

#endif  // !__IMAGE_H__

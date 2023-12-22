#ifndef Texture_H
#define Texture_H

#include <string>

#pragma once

struct Texture {
    unsigned int ID;

    unsigned int Width, Height;

    unsigned int Internal_Format;
    unsigned int Image_Format;

    unsigned int Wrap_S;
    unsigned int Wrap_T;
    unsigned int Filter_Min;
    unsigned int Filter_Max;

    std::string type;
    std::string path;

    Texture();

    void Generate(unsigned int width, unsigned int height, unsigned char* data);

    void Bind() const;
};

#endif

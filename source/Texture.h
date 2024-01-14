#ifndef Texture_H
#define Texture_H

#include <string>
#include "Constants.h" 

#pragma once

struct Texture {
    u32 ID;

    u32 Width, Height;

    u32 Internal_Format;
    u32 Image_Format;

    u32 Wrap_S;
    u32 Wrap_T;
    u32 Filter_Min;
    u32 Filter_Max;

    std::string type;
    std::string path;

    Texture();

    void Generate(u32 width, u32 height, unsigned char* data);

    void Bind() const;
};

#endif

#pragma once
#include <raylib.h>
#include <iostream>

#include <boost/format.hpp>

#define MOUSE_COEF 350.0f
#define SHADOW_QUALITY 2048
#define SELECT_DISTANCE 5.0f

#define WAVE_COUNT 6

typedef struct {
    int chunkX;
    int chunkZ;
    float internalX;
    float internalZ;
} ChunkData;

typedef enum {
    ITEM_SIMPLE,
    ITEM_STANDARD = 0,
    ITEM_COMPLEX,
    ITEM_ADAPTIVE,
} ItemComplexity;

typedef enum {
    POSITION_RELATIVE,
    POSITION_ABSOLUTE,
} UIPosition;

#define CAMERA 0
#define MOUSE 1

#define SCREEN_WIDTH 1900
#define SCREEN_HEIGHT 1000

template<typename... Args>
void print(std::string text, Args&&... args)
{
    std::cout<<(boost::format(text) % ... % args);
}

template<typename T>
void println(T message)
{
    std::cout<< message <<std::endl;
}

extern int IS_ENGINE;
#pragma once
#include <raylib.h>

#include <utility>


/**
 * @brief This is essentially gutted, only used interally in GlobalWater
 */
class Water 
{
private:


    /*
    const int maxParticles = 1000;
    typedef struct {
        Vector3 position;
        Vector3 velocity;
        Vector3 force;
        float influence;
    } Particle;
    std::vector<Particle> particles;
    */

    int size;
    float cellSize;

    Vector2 position;
    float chunkSize;

    float waveHeight(float x, float z, float time);

public:
    static Texture2D blackMap;

    Mesh mesh = { 0 };
    int index;

    Water(Vector2 position_, float cellSize_, int size_);
    void render();
    ~Water();
};


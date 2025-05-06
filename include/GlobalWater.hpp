#pragma once
#include <map>
#include <raylib.h>
#include <string>
#include <utility>
#include "glUtils.hpp"
#include <memory>

class Player;
class Water;
class GLssbo;
class AItems;
class Item;

typedef std::shared_ptr<Item> ItemPtr;

class GlobalWater
{
private:

    //std::unordered_map<std::pair<int, int>, Water*, boost::hash<std::pair<int, int>>> water;
    std::map<std::pair<int, int>, Water*> water;

    int size;
    float cellSize;
    float viewDistance;
    float chunkSize;
    unsigned int* frameCount;
    Shader shader;
    std::map<std::string, int> locs;

    typedef struct {
        float amplitude;
        float frequency;
        float phase;
        Vector2 dir;
        float steepness;
    } Wave;


    Wave waves[6] = {
        // Large primary wave
        { 0.7f, 1.0f, 0.3f, { 0.6f, 0.8f }, 0.5f},
        
        // Medium waves with different directions
        { 0.35f, 0.4f, 0.4f, { 1.0f, 0.0f }, 0.15f },
        { 0.23f, 0.35f, 0.25f, { -0.7f, 0.7f }, 0.12f },
        
        // Smaller, higher frequency detail waves
        { 0.15f, 0.7f, 0.7f, { -0.3f, 0.95f }, 0.1f },
        { 0.15f, 1.2f, 0.3f, { 0.2f, -0.98f }, 0.05f },
        { 0.07f, 1.5f, 0.6f, { -0.8f, -0.6f }, 0.04f }
    };

    GLssbo vertexData;
    void updateVertexData(std::pair<int, int> waterToAdd);
    AItems* items;

    Material material;
public:
    /**
     * @brief cellSize * (size - 1) = 16 must be true for this to work predictably
     * @param size_ number of cells in a chunk
     * @param cellSize_ size of each cell
     * @param viewDistance_ side length of the square of water chunks surrounding the player
     * @param player initial location of the player to set up initial water state
     */
    GlobalWater(int size_, float cellSize_, float viewDistance_, AItems* items);
    void updateWater(ItemPtr player);
    ~GlobalWater();
};

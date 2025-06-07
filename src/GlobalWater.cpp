
#include "GlobalWater.hpp"
#include <vector>
#include <rlgl.h>
#include <fstream>
#include <sstream>
#include <raymath.h>
#include <queue>
#include <cstring>

#include "Items/Player.hpp"
#include "Water.hpp"
#include "AItems.hpp"

/**
 * @brief cellSize * (size - 1) = 16 must be true for this to work predictably
 * @param size_ number of cells in a chunk
 * @param cellSize_ size of each cell
 * @param viewDistance_ side length of the square of water chunks surrounding the player
*/
GlobalWater::GlobalWater(int size_, float cellSize_, float viewDistance_, AItems* items)
    : size(size_), cellSize(cellSize_), viewDistance(viewDistance_), items(items)
{
    chunkSize = cellSize * (size - 1);

    //set up shader
    shader = LoadShader("shaders/water.vs", "shaders/water.fs");

    shader.locs[SHADER_LOC_VERTEX_POSITION] = rlGetLocationAttrib(shader.id, "vertexPosition");
    shader.locs[SHADER_LOC_VERTEX_TEXCOORD01] = rlGetLocationAttrib(shader.id, "vertexTexCoord");
    shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");

    locs["time"] = GetShaderLocation(shader, "time");
    locs["enviorment"] = GetShaderLocation(shader, "enviorment");
    locs["irradiance"] = GetShaderLocation(shader, "irradiance");
    locs["viewPos"] = GetShaderLocation(shader, "viewPos");
    locs["isSkybox"] = GetShaderLocation(shader, "isSkybox");
    for (int i = 0; i < WAVE_COUNT; i++)
    {
        locs[TextFormat("waveDir[%d]", i)] = GetShaderLocation(shader, TextFormat("waveDir[%d]", i));
        locs[TextFormat("amplitude[%d]", i)] = GetShaderLocation(shader, TextFormat("amplitude[%d]", i));
        locs[TextFormat("frequency[%d]", i)] = GetShaderLocation(shader, TextFormat("frequency[%d]", i));
        locs[TextFormat("phase[%d]", i)] = GetShaderLocation(shader, TextFormat("phase[%d]", i));
        locs[TextFormat("steepness[%d]", i)] = GetShaderLocation(shader, TextFormat("steepness[%d]", i));

        SetShaderValue(shader, locs[TextFormat("waveDir[%d]", i)], &waves[i].dir, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, locs[TextFormat("amplitude[%d]", i)], &waves[i].amplitude, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, locs[TextFormat("frequency[%d]", i)], &waves[i].frequency, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, locs[TextFormat("phase[%d]", i)], &waves[i].phase, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, locs[TextFormat("steepness[%d]", i)], &waves[i].steepness, SHADER_UNIFORM_FLOAT);
    }

    material = LoadMaterialDefault();    
    material.shader = shader;
    RLG_Skybox* skybox = items->getCurrentSkybox();
    if (skybox)
    {
        material.maps[MATERIAL_MAP_CUBEMAP].texture = skybox->cubemap;
        material.maps[MATERIAL_MAP_IRRADIANCE].texture = skybox->irradiance;
        SetShaderValue(shader, locs["enviorment"], &skybox->cubemap, SHADER_UNIFORM_INT);
        SetShaderValue(shader, locs["irradiance"], &skybox->irradiance, SHADER_UNIFORM_INT);
    }
    println("INFO: Water successfully initialized");
}

void GlobalWater::updateWater(ItemPtr player)
{
    int xMin = player->getChunkData().chunkX - (.5 * viewDistance);
    int zMin = player->getChunkData().chunkZ - (.5 * viewDistance);
    int xMax = player->getChunkData().chunkX + (.5 * viewDistance);
    int zMax = player->getChunkData().chunkZ + (.5 * viewDistance);
    
    std::vector<std::pair<int, int>> toRemove;
    for (auto& [position, waterObj] : water)
    {
        if (position.first < xMin || position.first >= xMax || position.second < zMin || position.second >= zMax)
        {
            toRemove.push_back(position);
        }
    }
    for (auto position : toRemove)
    {
        delete water[position];
        water.erase(position);
    }

    //add new water if neccessary
    for (int x = xMin; x < xMax; x++)
    {
        for (int z = zMin; z < zMax; z++)
        {
            if (water.find({x, z}) == water.end())
            {
                //WARNING: int to float is apparnetly narrowing, so for very large ints this could be an issue 
                //Might be worth to redefine Vector2 as doubles rather than floats 
                water[{x, z}] = new Water((Vector2){x, z}, cellSize, size);
            }
        }
    }

    //update water and mark irrelivant ones for termination
    BeginShaderMode(shader);
        float time = GetTime();
        SetShaderValue(shader, locs["time"], &time, SHADER_UNIFORM_FLOAT);
        Vector3 camPosition = player->getCamera().position;
        SetShaderValue(shader, locs["viewPos"], &camPosition, SHADER_UNIFORM_VEC3);
        
        int isSkybox;
        
        RLG_Skybox* skybox = items->getCurrentSkybox();
        if (skybox)
        {
            isSkybox = 1;

            int i = 2;
            rlActiveTextureSlot(i);
            rlEnableTextureCubemap(skybox->cubemap.id);
            SetShaderValue(shader, locs["enviorment"], &i, SHADER_UNIFORM_INT);
            
            i++;
            rlActiveTextureSlot(i);
            rlEnableTextureCubemap(skybox->irradiance.id);
            SetShaderValue(shader, locs["irradiance"], &i, SHADER_UNIFORM_INT);
        } else {
            isSkybox = 0;
        }
        SetShaderValue(shader, locs["isSkybox"], &isSkybox, SHADER_UNIFORM_INT);

        for (auto& [position, waterObj] : water)
        {
            vertexData.bind(shader.id);
            DrawMesh(waterObj->mesh, material, MatrixIdentity());
        }

        if (skybox)
        {
            int i = 2;
            rlActiveTextureSlot(i);
            rlDisableTextureCubemap();
            
            i++;
            rlActiveTextureSlot(i);
            rlDisableTextureCubemap();
        }

    EndShaderMode();
}

GlobalWater::~GlobalWater()
{
    UnloadShader(shader);
    for (auto& [position, waterObj] : water)
    {
        delete waterObj;
    }
}
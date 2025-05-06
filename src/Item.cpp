#include "Item.hpp"


#include <raymath.h>

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#include <cstdio>
#include <iostream>
#include <boost/format.hpp>

#include "AItems.hpp"

using boost::format;


Item::Item(AItems* items, Vector3 pos, Vector3 scale, Vector3 rotationAxis, float rotationAngle, std::string model) 
    : items(items), position(pos), scale(scale), rotationAxis(rotationAxis), rotationAngle(rotationAngle), name(model)
{
    initChunk();
    if (model != "")
    {
        if (!items->isModelLoaded(name))
        {
            items->loadModel(name);
        }
    }
}

Item::Item(AItems* items, std::string name_, Vector3 pos, Vector3 modelScale)
    : items(items), position(pos), scale(modelScale), name(name_)
{
    initChunk();

    if (!items->isModelLoaded(name))
    {
        items->loadModel(name);
    }

}

Item::Item(AItems* items, Vector3 pos, Vector3 modelScale)
    : items(items), position(pos), scale(modelScale)
{
    //runs if initialzed in createInitialState, where id is local to client rather than global to the state of the server
    initChunk();

    hasModel = false;
}

Item::Item(AItems* items, json& item) : items(items)
{
    try
    {
        name = item.at("model").get<std::string>();

        position.x = item.at("position").at("x").get<float>();
        position.y = item.at("position").at("y").get<float>();
        position.z = item.at("position").at("z").get<float>();

        rotationAxis.x = item.at("rotationAxis").at("x").get<float>();
        rotationAxis.y = item.at("rotationAxis").at("y").get<float>();
        rotationAxis.z = item.at("rotationAxis").at("z").get<float>();
        rotationAngle = item.at("rotationAngle").get<float>();

        scale.x = item.at("scale").at("x").get<float>();
        scale.y = item.at("scale").at("y").get<float>();
        scale.z = item.at("scale").at("z").get<float>();
    }
    catch (const std::exception& e)
    {
        print("ERROR: Item failed to read complete data from save, error: %1%\n", e.what());
    }

    initChunk();
    if (name != "")
    {
        println(items->isModelLoaded(name));
        if (!items->isModelLoaded(name))
        {
            items->loadModel(name);
        }
    }

    isSaved = true;
}

json Item::saveItem()
{
    json item = {
        {"class", getClass()},
        {"model", name},
        {"position", { {"x", position.x}, {"y", position.y}, {"z", position.z} }},
        {"scale", { {"x", scale.x}, {"y", scale.y}, {"z", scale.z} }},    
        {"rotationAxis", { {"x", rotationAxis.x}, {"y", rotationAxis.y}, {"z", rotationAxis.z} }},
        {"rotationAngle", rotationAngle},
    };
    auto childSaveData = saveItemOverride();
    if (childSaveData != nullptr)
    {
        item.update(childSaveData);
    }

    return item;
}



void Item::render()
{
    renderingLogic();
    if (name != "") RLG_DrawModelEx(items->getModel(name)->model, position, rotationAxis, rotationAngle, scale, WHITE);
}

void Item::initChunk()
{
    int chunkSize = items->getChunkSize();
    for (int i = 0; ; i++)
    {
        if (i * chunkSize > fabs(position.x))
        {
            int absChunkX = i - 1;
            float absInternalX = fabs(position.x) - (float)((i - 1) * chunkSize);
            int sign = position.x > 0.0f ? 1 : -1;
            chunkData.chunkX = sign * absChunkX;
            chunkData.internalX = (float)sign * absInternalX;
            break;
        }
    }
    for (int i = 0; ; i++)
    {
        if (i * chunkSize > fabs(position.z))
        {
            int absChunkZ = i - 1;
            float absInternalZ = fabs(position.z) - (float)((i - 1) * chunkSize);
            int sign = position.z > 0.0f ? 1 : -1;
            chunkData.chunkZ = sign * absChunkZ;
            chunkData.internalZ = (float)sign * absInternalZ;
            break;
        }
    }

}

Item::~Item()
{

}

AModel::AModel(std::string name_, AItems* items) : name(name_)
{
    std::string path = std::string("models/") + name + "/" + name + ".glb";
    model = LoadModel(path.c_str());
    for (int i = 0; i < model.meshCount; i++)
    {
        if (!model.meshes[i].tangents) GenMeshTangents(&model.meshes[i]);
        UploadMesh(&model.meshes[i], false);
    }
    if (items->getCurrentSkybox())
    {
        for (int i = 0; i < model.materialCount; i++)
        {
            model.materials[i].maps[MATERIAL_MAP_CUBEMAP].texture = items->getCurrentSkybox()->cubemap;
            model.materials[i].maps[MATERIAL_MAP_IRRADIANCE].texture = items->getCurrentSkybox()->irradiance;
        }
    }

    std::cout << format("INFO: Model %1% initialized successfully\n") % name;
}

AModel::~AModel()
{
    UnloadModel(model);
    std::cout << format("INFO: Model %1% destroyed\n") % name;
}
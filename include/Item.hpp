#pragma once
#include <raylib.h>
#include "definitions.hpp"
#include "rlights.h"


#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <unordered_map>
#include <functional>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class AItems;

class AModel 
{
private:
    std::string name;
public:
    Model model;
    AModel(std::string name_, AItems* items);
    ~AModel();
};

/// @brief Base class for all 3d objects, or can be used directly for static objects
class Item
{
protected:

    void initChunk();
    virtual void renderingLogic() {}
    AItems* items;
    ChunkData chunkData;

    /// @brief override this to add additional save logic, if your have more than one layer of this and want to save the parent classes data you must explicitly call the parents saveItemOverride()
    virtual json saveItemOverride() { return {}; }

public:

    bool isSaved = true;

    Vector3 position;
    Vector3 scale;
    Vector3 rotationAxis;
    float rotationAngle;

    unsigned int id;
    bool hasModel = true;
    std::string name;

    const ChunkData& getChunkData() { return chunkData; }

    virtual std::string getClass() const { return "Item"; }

    //frame-by-frame functions
    virtual void update() {}
    virtual void render();

    //child class methods
    virtual RLG_Skybox* getSkybox() { return nullptr; }
    virtual Camera getCamera() { return { 0 }; }

    json saveItem();

    Item(AItems* items, std::string name, Vector3 pos, Vector3 modelScale); 
    Item(AItems* items, Vector3 pos, Vector3 modelScale); 

    Item(AItems* items, Vector3 pos, Vector3 scale, Vector3 rotationAxis, float rotationAngle, std::string model = "");
    Item(AItems* items, json& itemObject);
    Item() {};


    ~Item();
};

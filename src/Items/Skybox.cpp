
#include "Items/Skybox.hpp"
#include "AItems.hpp"
#include <stdexcept>

Skybox::Skybox(AItems* items, Vector3 pos, Vector3 scale, Vector3 rotationAxis, float rotationAngle, std::string model)
    : Item(items, pos, scale, rotationAxis, rotationAngle)
{
    std::string path = std::string("skybox/") + model;
    skybox = RLG_LoadSkybox(path.c_str());
    isSaved = false;


}

Skybox::Skybox(AItems* items, json& item) : Item(items, item)
{
    std::string path = std::string("skybox/") + name;
    skybox = RLG_LoadSkybox(path.c_str());
}




RLG_Skybox* Skybox::getSkybox()
{
    return &skybox;
}

void Skybox::render()
{
    RLG_DrawSkybox(skybox);
}

Skybox::~Skybox()
{
    RLG_UnloadSkybox(skybox);
}


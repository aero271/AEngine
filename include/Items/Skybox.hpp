#pragma once
#include "Item.hpp"
#include "rlights.h"

class AItems;

class Skybox : public Item
{
private:
public:
    Skybox(AItems* items, Vector3 pos, Vector3 scale, Vector3 rotationAxis, float rotationAngle, std::string model = "");
    Skybox(AItems* items, json& item);
    virtual std::string getClass() const override { return "Skybox"; }

    RLG_Skybox skybox;    
    RLG_Skybox* getSkybox() override;
    void render() override;
    ~Skybox();
};

typedef std::shared_ptr<Skybox> SkyboxPtr;
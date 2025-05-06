#pragma once

#include <raylib.h>
#include "Item.hpp"

#include "definitions.hpp"

class AItems;

class Player : public Item
{
private:
    
    void cameraUpdate();

public:

    Player(AItems* items, Vector3 pos, Vector3 scale, Vector3 rotationAxis, float rotationAngle, std::string model = "");
    Player(AItems* items, json& item);
    virtual std::string getClass() const override { return "Player"; }
    Camera getCamera() override;

    Camera3D camera;
    int observedObject;


    ///needs to be called before BeginMode3D to acuratly update camera, thus this is public and called outside of renderingLogic

    Player(AItems* items, Vector3 position);
    void renderingLogic() override;
    void update() override;
    ~Player() {}
};

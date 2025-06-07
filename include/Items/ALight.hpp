#pragma once

#include "Item.hpp"
#include "rlights.h"
#include <vector>
#include <memory>


class AItems;

class ALight : public Item
{
private:
    ///setter must be used
    bool hasModel = false;
    bool shadow = false;
    ///setter must be used
    float energy;

public:
    
    ALight(Vector3 pos, Vector3 target, Color color, RLG_LightType type, bool hasShadow, AItems* items_);
    ALight(AItems* items, json& item);
    virtual std::string getClass() const override { return "ALight"; }
    
    void updatePosition(Vector3 newPosistion);
    void updateTarget(Vector3 newTarget);
    void updateEnergy(float newEnergy);
    void updateDistance(float newDistance);

    /**
     * @brief Updates shadow map and draws model, if there is one. Must be called every frame
     */
    void renderLighting();

    ~ALight();
};


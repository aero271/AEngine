
#include "Items/ALight.hpp"
#include "Water.hpp"
#include "definitions.hpp"
#include <raymath.h>
#include <iostream>
#include <boost/format.hpp>
#include "AItems.hpp"

using boost::format;


AItems* items;

void cast(Shader shader)
{
    for (auto& [id, item] : items->getCurrentSceneItems())
    {
        if (!item->hasModel) continue;
        RLG_CastModelEx(shader, items->getModel(item->name)->model, item->position, item->rotationAxis, item->rotationAngle, item->scale);
    }
}

ALight::ALight(Vector3 pos, Vector3 target, Color color, RLG_LightType type, bool hasShadow, AItems* items_)
{
    items = items_;
    position = pos;


    //rlights config
    RLG_UseLight(id, true);
    RLG_SetLightType(id, type);
    RLG_SetLightVec3(id, RLG_LIGHT_POSITION, position);
    RLG_SetLightColor(id, color);
    RLG_SetLightTargetV(id, target);
    
    if (hasShadow)
    {
        RLG_EnableShadow(id, SHADOW_QUALITY);
        shadow = true;
    }
    std::cout << format("INFO: Light %1% initialized\n") % id;
}


void ALight::renderLighting()
{
    if (shadow)
    {
        RLG_UpdateShadowMap(id, cast);
    }
}

void ALight::updatePosition(Vector3 newPosition)
{
    position = newPosition;
    RLG_SetLightVec3(id, RLG_LIGHT_POSITION, newPosition);
}

void ALight::updateTarget(Vector3 newTarget)
{
    RLG_SetLightTargetV(id, newTarget);
}

void ALight::updateEnergy(float newEnergy)
{
    RLG_SetLightValue(id, RLG_LIGHT_ENERGY, newEnergy);
    energy = newEnergy;
}

void ALight::updateDistance(float newDistance)
{
    RLG_SetLightValue(id, RLG_LIGHT_DISTANCE, newDistance);
}

ALight::~ALight()
{
}
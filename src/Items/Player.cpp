
#include "Items/Player.hpp"
#include <raymath.h>
#include "rlights.h"
#include <cmath>
#include "AItems.hpp"
#include "AUI.hpp"

// Runs during object all object construction 
Player::Player(AItems* items, Vector3 pos, Vector3 scale, Vector3 rotationAxis, float rotationAngle, std::string model) 
    : Item(items, pos, scale, rotationAxis, rotationAngle, model) 
{

}

Player::Player(AItems* items, json& item) : Item(items, item)
{
    camera.position = position;
    camera.target = { 2.0f, 2.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.up = { 0.0f, 1.0f, 0.0f };
}

Player::Player(AItems* items, Vector3 position) : Item(items, position, Vector3One())
{
    camera.position = position;
    camera.target = { 2.0f, 2.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.up = { 0.0f, 1.0f, 0.0f };
}

void Player::renderingLogic()
{
    
}

float angleOffXYPlane(Vector3 vec)
{
    return atan2(vec.z, vec.x);
}

float angleOffXZPlane(Vector3 vec)
{
    return asin((vec.y / Vector3Length(vec)));
}

void updateVector3ByAngle(Vector3& vec, Vector2 angles)
{
    //do some math, if you dont understand go back to precalculus, nah jk i dont blame you this code is pretty hard to read
    //but fr if you dont get trig this shit wont make a shred of sense
    Vector3 intermediate;
    intermediate.y = sin(angles.y);
    intermediate.x = sqrt(1 - pow(intermediate.y, 2)) * cos(angles.x);
    intermediate.z = sqrt(1 - pow(intermediate.y, 2)) * sin(angles.x);
    //should already be normalized, but you never know
    vec = Vector3Normalize(intermediate);

}

void Player::update()
{
    if (items->getUI()->isCameraUpdated())
    {
        cameraUpdate();
    }
}

Camera Player::getCamera()
{
    return camera;
}

void Player::cameraUpdate() 
{
    //computing look direction change
    Vector3 lookingDirection = Vector3Subtract(camera.target, camera.position);
    Vector3 unitLooking = Vector3Normalize(lookingDirection);
    Vector2 angles;
    angles.x = angleOffXYPlane(unitLooking);
    angles.y = angleOffXZPlane(unitLooking);

    Vector2 mouseDelta = Vector2Scale(GetMouseDelta(), (1.0f / MOUSE_COEF));
    angles.x += mouseDelta.x;
    angles.y += -1 * mouseDelta.y;

    //converts unit vector to the vector created by the angles
    updateVector3ByAngle(unitLooking, angles);

    //normalized so that looking direction doesnt change walking speed
    Vector2 walkingVector = Vector2Normalize((Vector2){ unitLooking.x, unitLooking.z });

    float SPEED_COEF = 0.05f;

    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        SPEED_COEF = 0.1f;
    }
    if (IsKeyDown(KEY_W)) 
    {
        camera.position.x += walkingVector.x * SPEED_COEF;
        camera.position.z += walkingVector.y * SPEED_COEF;
    }
    if (IsKeyDown(KEY_S))
    {
        camera.position.x += -1.0f * walkingVector.x * SPEED_COEF;
        camera.position.z += -1.0f * walkingVector.y * SPEED_COEF;
    }
    if (IsKeyDown(KEY_A))
    {
        camera.position.x += walkingVector.y * SPEED_COEF;
        camera.position.z += -1.0f * walkingVector.x * SPEED_COEF;
    }
    if (IsKeyDown(KEY_D))
    {
        camera.position.x += -1.0f * walkingVector.y * SPEED_COEF;
        camera.position.z += walkingVector.x * SPEED_COEF;
    }
    // Uncomment for vertical movement 
    
    if (IsKeyDown(KEY_SPACE))
    {
        camera.position.y += SPEED_COEF;
    }
    if (IsKeyDown(KEY_C))
    {
        camera.position.y += -1.0f * SPEED_COEF;
    }
     
    camera.target = Vector3Add(camera.position, unitLooking);

    //figure out what mesh its coliding with
    Ray lineOfSight = { camera.position, unitLooking };
    Matrix identityMat = MatrixIdentity();
    std::vector<std::pair<RayCollision, int>> collidingMeshs;

    auto sceneItems = items->getCurrentSceneItems();
    collidingMeshs.reserve(sceneItems.size());

    /*
    for (auto& [id, object] : sceneItems)
    {
        if (object->name == "") continue;

        RayCollision collision;
        for (int i = 0; i < items->getModel(object->name)->model.meshCount; i++)
        {
            collision = GetRayCollisionMesh(lineOfSight, items->getModel(object->name)->model.meshes[i], identityMat);
            if (collision.hit && collision.distance < SELECT_DISTANCE)
            {
                collidingMeshs.emplace_back(collision, object->id);
                break;
            }
        }
    }


    //since there could be multiple colliding meshes, this verifies that the closest one is returned
    if (collidingMeshs.size() != 0)
    {
        int min = collidingMeshs[0].first.distance;
        int index = 0;
        for (int i = 1; i < collidingMeshs.size(); i++) 
        {
            if (collidingMeshs[i].first.distance < min) {
                min = collidingMeshs[i].first.distance;
                index = i;
            }
        }
        observedObject = collidingMeshs[index].second;
    }
    else
    {
        observedObject = 0;
    }
    */

    initChunk();
    position = camera.position;
    RLG_SetViewPositionV(camera.position);
}
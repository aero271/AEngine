
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#ifndef RLIGHTS_IMPLEMENTATION
    #ifdef PLATFORM_WEB
        #define GLSL_VERSION 300
    #else
        #define GLSL_VERSION 330
    #endif

    #define RLIGHTS_IMPLEMENTATION
    #include "rlights.h" //lighting library
#endif

#if __cplusplus < 201402L
    #error "cpp14 or later is required"
#endif

#include "definitions.hpp"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "Item.hpp"
#include "ALight.hpp"
// #include "WebSocketClient.hpp"
#include "CreateInitialState.hpp"
#include "Water.hpp"
#include "GlobalWater.hpp"
#include "Items/Player.hpp"
#include "AUI.hpp"
#include "AItems.hpp"

#include "litehtml.h"

#include <boost/format.hpp>

#include <cmath>
#include <iostream>
#include <stdexcept>

int IS_ENGINE = 0;



using boost::format;
using std::cout;

int main(int argc, char** argv)
{

    if (argc > 1) 
    {
        if (std::string(argv[1]) == "engine") 
        {
            IS_ENGINE = 1;
        }
    }
    // Tell the window to use vsync and work on high DPI displays
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

    // Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
    SearchAndSetResourceDir("resources");

    // Create the window and OpenGL context
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3d Game");

    RLG_Context context = RLG_CreateContext();
    RLG_SetContext(context);
    RLG_UseMap(MATERIAL_MAP_NORMAL, true);
    RLG_UseMap(MATERIAL_MAP_METALNESS, true);
    RLG_UseMap(MATERIAL_MAP_ROUGHNESS, true);
    RLG_UseDefaultMap(MATERIAL_MAP_NORMAL, false);
    RLG_UseMap(MATERIAL_MAP_CUBEMAP, true);
    RLG_UseMap(MATERIAL_MAP_IRRADIANCE, true);

    { //scope block to destroy all objects needing opengl resources before destroying the opengl context
    AUI ui;
    AItems items;
    items.setChunkSize(32);
    items.setUI(&ui);
    items.loadSaveFile("save.json");

    // init backend
    // asio::io_context ioc;
    // WebSocketClient ws(ioc, "127.0.0.1", "8080");
    // ioc.poll();

    Font roboto = LoadFont("fonts/roboto/static/Roboto-Regular.ttf");

    const float cellSize = 0.25f;
    const float viewCoef = 10 / items.getChunkSize();

    IID skybox = items.createItem(std::make_shared<Skybox>(&items, Vector3Zero(), Vector3Zero(), Vector3Zero(), 0.0f, "StandardCubeMap.png"));
    items.setCurrentSkybox(skybox);

    GlobalWater water((items.getChunkSize() / cellSize) + 1, cellSize, 60, &items);

    SetTargetFPS(120);

    bool tabbedState = true;

    

    // game loop
    while (AUI::isGame)
    {
        ui.update();
        items.update();

        if (ui.mode == CAMERA)
        {

        }

        // ioc.poll();
        //  cout << format("Object Count: %1%\n") % Item::items.size();

        //shadow pass(s)
        //TODO: refactor lights into an Item child class 
        for (auto &light : ALight::lights)
        {
            light->renderLighting();
        }

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(items.getCurrentPlayer()->getCamera());

                items.draw();

                water.updateWater(items.getCurrentPlayer());

            EndMode3D();

            ui.drawUI();

            //DrawTextEx(roboto, TextFormat("x: %f, y: %f, z: %f", player->position.x, player->position.y, player->position.z), {0.0f, 0.0f}, roboto.baseSize, 2, WHITE);
            //DrawTextEx(roboto, TextFormat("%f FPS", (1 / GetFrameTime())), {0.0f, 30.0f}, roboto.baseSize, 2, WHITE);

        EndDrawing();
    }
    items.saveState();

    } //scope block to destroy all objects needing opengl resouces before cleaning up context
    println("djsakldjskl");

    // destroy the window and cleanup the OpenGL context
    RLG_DestroyContext(context);
    CloseWindow();
    return 0;
}

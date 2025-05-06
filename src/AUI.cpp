
#include "AUI.hpp"
#include "definitions.hpp"

#include <raylib.h>
#include <litehtml.h>
#include "glad.h"

#include <filesystem>

namespace fs = std::filesystem;

bool AUI::isGame = true;

AUI::AUI() : container(this)
{
    //get all files in UI directory
    std::vector<std::string> files;
    try
    {
        for (const auto& entry : fs::directory_iterator("UI"))
        {
            files.push_back(entry.path().filename());
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << e.what() << '\n';
    }

    for (auto& file : files)
    {
        if (IsFileExtension(file.c_str(), ".html"))
        { 
            std::string path = std::string("UI/") + file;
            char* html = LoadFileText(path.c_str());
            std::string name;
            for (int i = 0; i < file.length(); i++)
            {
                if (file[i] == '.') 
                {
                    name = file.substr(0, i);
                    break;
                }
            }
            docs[name] = litehtml::document::createFromString(html, &container);
            docs[name]->render(SCREEN_WIDTH);

            if (name == "engine" && IS_ENGINE)
            {
                isDrawn[name] = true;
            }
            else
            {
                isDrawn[name] = false;
            }

            UnloadFileText(html);
        }
    }

    if (IS_ENGINE) 
    {
        mode = MOUSE;
        EnableCursor();
    }
    else 
    {
        mode = CAMERA;
        DisableCursor();
    }

    addEventListener("testtest", [](AUI* ui) {
        println("djsaiokd");
        ui->setCondition("iftest", !ui->getCondition("iftest"));
        println(ui->getCondition("iftest"));
    });


    print("INFO: Loaded UI\n");
}


/// @brief poll inputs, update if ui is to be drawn (runs before camera update and drawing)
void AUI::update()
{
    //manage game state

    if (IsKeyPressed(KEY_ESCAPE))
    {
        isGame = false;
    }
    if (IsKeyPressed(KEY_TAB))
    {
        mode = !mode;
        if (mode == CAMERA) DisableCursor();
        if (mode == MOUSE) EnableCursor();
    }
    if (mode == MOUSE)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) isCamera = true;
        else isCamera = false;
    }
    else if (mode == CAMERA) 
    {
        isCamera = true;
    }
}

/// @brief called after drawing 3d scene, draws ui
void AUI::drawUI() 
{

    litehtml::position pos;
    pos.x = 0;
    pos.y = 0;
    pos.width = SCREEN_WIDTH;
    pos.height = SCREEN_HEIGHT;

    for (auto& [name, doc] : docs)
    {
        if (isDrawn[name])
        {
            doc->draw((litehtml::uint_ptr)this, 0, 0, &pos);
        }
    }

}

void AUI::enableUI(std::string name) { isDrawn[name] = true; }

void AUI::disableUI(std::string name) { isDrawn[name] = false; }

void AUI::addEventListener(std::string id, std::function<void(AUI*)> callback, bool overwrite)
{
    if (!overwrite)
    {
        if (eventListeners.find(id) != eventListeners.end())
        {
            print("ERROR: html element %1% already has an event listener. If overwrite is desired, call with overwrite = true\n", id);
            return;
        }
    }
    eventListeners[id] = callback;
}

const std::function<void(AUI*)>& AUI::getEventListener(std::string id)
{
    if (eventListeners.find(id) != eventListeners.end())
    {
        return eventListeners[id];
    }
    else
    {
        print("ERROR: No event listener for %1%\n", id);
        throw std::logic_error("ERROR: event listner error\n");
    }
}

void AUI::removeEventListener(std::string id)
{
    if (eventListeners.find(id) == eventListeners.end())
    {
        print("ERROR: No event listener for %1%\n", id);
    }
    else
    {
        eventListeners.erase(id);
    }
}

bool AUI::getCondition(std::string id)
{
    return conditions[id];
}

void AUI::setCondition(std::string id, bool cond)
{
    conditions[id] = cond;
}

AUI::~AUI()
{
    for (auto [_, doc] : docs)
    {
        doc.reset();
    }
    println("INFO: UI destroyed successfully");
}
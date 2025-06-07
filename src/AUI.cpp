
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
    loadDirectoryUI("");

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

    if (IS_ENGINE)
    {
        loadEngineUI();
    }

    addEventListener("testtest", [](AUI* ui) {
        ui->setCondition("iftest", !ui->getCondition("iftest"));
        println(ui->getCondition("iftest"));
    });


    print("INFO: Loaded UI\n");
}

void AUI::loadDirectoryUI(std::string directory)
{
    std::vector<std::string> files;
    try
    {
        for (const auto& entry : fs::directory_iterator(std::string("UI/") + directory))
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
            addUI(directory + "/" + file);
        }
    }
}

void AUI::loadEngineUI()
{
    loadDirectoryUI("engine");
    addUI("engine.html");
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

    buttonPressListeners();
}

void AUI::buttonPressListeners()
{
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_A))
    {
        if (docs.count("engine/add")) enableUI("engine/add");
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (docs.count("engine/add")) disableUI("engine/add");
    }
}

/// @brief called after drawing 3d scene, draws ui
void AUI::drawUI() 
{
    litehtml::position pos(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    for (auto& [name, doc] : docs)
    {
        if (doc->enabled)
        {
            doc->draw(&pos);
        }
    }
}

void AUI::enableUI(std::string name) 
{ 
    if (docs[name]->enabled == false) docs[name]->stateChanged = true;
    docs[name]->enabled = true; 
}

void AUI::disableUI(std::string name) 
{ 
    docs[name]->enabled = false; 
    docs[name]->stateChanged = true;
}

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

void AUI::addUI(std::string file)
{
    auto doc = std::make_shared<DocData>(this, nullptr, 0, 0);
    loadUI(file, doc);
}

void AUI::addUI(std::string file, int x, int y)
{
    auto doc = std::make_shared<DocData>(this, nullptr, x, y);
    loadUI(file, doc);
}

void AUI::addUI(std::string file, std::function<Vector2()> callback)
{
    auto doc = std::make_shared<DocData>(this, nullptr, callback);
    loadUI(file, doc);
}

void AUI::loadUI(std::string file, std::shared_ptr<DocData> docData)
{
    std::string path = std::string("UI/") + file;
    char* html = LoadFileText(path.c_str());
    if (!html)
    {
        print("ERROR: failed to load UI file %1%\n", file);
        return;
    }

    std::string name;
    for (int i = 0; i < file.length(); i++)
    {
        if (file[i] == '.') 
        {
            name = file.substr(0, i);
            break;
        }
    }
    docData->doc = litehtml::document::createFromString(html, &container);
    docData->doc->render(SCREEN_WIDTH);
    docs[name] = docData;

    if (name == "engine" && IS_ENGINE)
    {
        docs[name]->enabled= true;
    }
    else
    {
        docs[name]->enabled = false;
    }

    UnloadFileText(html);
    print("INFO: UI %1% loaded\n", name);
}

AUI::~AUI()
{
    docs.clear();
    println("INFO: UI destroyed successfully");
}
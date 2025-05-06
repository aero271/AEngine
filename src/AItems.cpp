#include "AItems.hpp"

#include "rlights.h"
#include <raylib.h>

#include <fstream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

AItems::AItems() : currentScene(scenes["root"])
{
}

void AItems::update()
{
    for (auto& [_, item] : currentScene)
    {
        item->update();
    }
}

void AItems::draw()
{
    for (auto& [_, item] : currentScene)
    {
        item->render();
    }
}

void AItems::loadModel(std::string name)
{
    models[name] = std::make_shared<AModel>(name, this);
}

RLG_Skybox* AItems::getCurrentSkybox() 
{ 
    if (currentSkybox) return currentSkybox->getSkybox();
    else return nullptr;
}

unsigned int AItems::newItemID()
{
    itemID++;
    return itemID;
}

IID AItems::createItem(ItemPtr item, std::string scene)
{
    IID id = newItemID();
    scenes[scene][id] = item;
    items[id] = item;
    print("INFO: Created item %1%\n", id);
    return id;
}


ItemPtr AItems::getItemWithModel(std::string model)
{
    for (auto& [id, item] : items)
    {
        if (item->name == model) return item;
    }
    return nullptr;
}

void AItems::saveState(std::string name)
{
    json save;

    for (auto& [name, scene] : scenes)
    {
        save[name] = json::array();
        for (auto& [id, item] : scene)
        {
            if (item->isSaved) 
            {
                save[name].push_back(item->saveItem());
            }
        }
    }
    std::string filepath = std::string("saves/") + name + ".json";
    std::ofstream file(filepath);
    file << std::setw(4) << save << std::endl;
    print("INFO: Saved state to %1%\n", name + ".json");
}

void AItems::loadSaveFile(std::string file)
{
    std::ifstream saveFile((std::string("saves/") + file));
    json save = json::parse(saveFile);

    for (auto& [name, scene] : save.items())
    {
        for (auto& item : scene)
        {
            //load data from save
            std::string className;

            std::cout<< "INFO: Loaded Item: " << item << "\n";

            className = item.at("class").get<std::string>();
            ItemPtr newItem = nullptr;

            if (className == "Item")
            {
                newItem = std::make_shared<Item>(this, item);
            }
            else if (className == "Player")
            {
                newItem = std::make_shared<Player>(this, item);
            }
            else if (className == "Skybox")
            {
                newItem = std::make_shared<Skybox>(this, item);
            }
            /*APPEND HERE*/

            
            if (!newItem) 
            {
                println("ERROR: Item failed to load due to invalid saved class name");
                continue;
            }
            createItem(newItem, name);
        }
    }
    
    if (!currentPlayer)
    {
        for (auto [id, item] : currentScene)
        {
            if (item->getClass() == "Player") 
            {
                currentPlayer = item;
                break;
            }
        }
    }


}

AItems::~AItems()
{
    for (auto& [_, item] : items)
    {
        item.reset();
    }
    for (auto& [_, model] : models)
    {
        model.reset();
    }
    println("INFO: Items destroyed successfully");
}
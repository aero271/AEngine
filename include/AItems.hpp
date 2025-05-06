#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include "Item.hpp"
#include "Items/Skybox.hpp"
#include "Items/Player.hpp"


typedef std::shared_ptr<Item> ItemPtr;
typedef std::shared_ptr<AModel> ModelPtr;
typedef std::shared_ptr<Player> PlayerPtr;
typedef unsigned int IID;

class AUI;

class AItems
{
private:

    AUI* uiRef = nullptr;

    struct Scene : std::unordered_map<unsigned int, ItemPtr>
    {
        Scene() : std::unordered_map<unsigned int, ItemPtr>() {}
        bool isEnabled;
    };

    std::unordered_map<unsigned int, ItemPtr> items;
    std::unordered_map<std::string, ModelPtr> models;
    std::unordered_map<std::string, Scene> scenes;
   
    Scene& currentScene;
    ItemPtr currentSkybox = nullptr;

    ItemPtr currentPlayer = nullptr;

    int chunkSize;
    IID itemID = 0;
    IID newItemID();

public:
    static std::string vec3ToString(Vector3 vec);

    void loadSaveFile(std::string file);

    int getChunkSize() { return chunkSize; }
    void setChunkSize(int chunkSize_) { chunkSize = chunkSize_; }

    IID createItem(ItemPtr item, std::string scene = "root");
    ItemPtr getItemWithModel(std::string name);
    ItemPtr getItem(IID id) { return items[id]; }

    std::unordered_map<IID, ItemPtr>& getAllItems() { return items; }
    Scene& getSceneItems(std::string scene) { return scenes[scene]; }
    Scene& getCurrentSceneItems() { return currentScene; }

    void setCurrentSkybox(unsigned int item) { currentSkybox = items[item]; }
    RLG_Skybox* getCurrentSkybox();
    
    void setPlayer(ItemPtr player) { currentPlayer = player; }
    ItemPtr getCurrentPlayer() { return currentPlayer; }

    void destroyItem();
    bool isModelLoaded(std::string model) { return (models.find(model) == models.end()) ? false : true; }
    void loadModel(std::string name);
    ModelPtr getModel(std::string name) { return models[name]; }
    void enableScene(std::string scene) { currentScene = scenes[scene]; }
    void saveState(std::string name = "save");

    void setUI(AUI* ui) { uiRef = ui; }
    AUI* getUI() { return uiRef; }

    void draw();
    void update();

    AItems();
    ~AItems();
};

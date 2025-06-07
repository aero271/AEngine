#pragma once

#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "definitions.hpp"

#include "UIContainer.hpp"


class AUI
{
private:


    DocumentContainer container;

    std::unordered_map<std::string, std::function<void(AUI*)>> eventListeners;
    std::unordered_map<std::string, bool> conditions;
    std::unordered_map<std::string, std::string*> inputs;

    struct DocData {
        bool enabled = false;
        bool stateChanged = false;

        int x;
        int y;
        bool functionBased;
        bool stateChangeOnly = false;
        std::shared_ptr<litehtml::document> doc;
        std::function<Vector2()> positionFunc;
        AUI* ui;

        DocData(AUI* ui, std::shared_ptr<litehtml::document> doc, std::function<Vector2()> positionFunc)
            : doc(doc), positionFunc(positionFunc), functionBased(true), ui(ui) {}

        DocData(AUI* ui, std::shared_ptr<litehtml::document> doc, int x, int y)
            : doc(doc), x(x), y(y), functionBased(false), ui(ui) {}

        void draw(litehtml::position* pos)
        {
            if (functionBased)    
            {
                if (!stateChangeOnly || stateChanged)
                {
                    Vector2 position = positionFunc();
                    x = position.x;
                    y = position.y;
                }
            }
            doc->draw((litehtml::uint_ptr)ui, x, y, pos);
            if (stateChanged) stateChanged = false;
        }
    };

    std::unordered_map<std::string, std::shared_ptr<DocData>> docs;
    
    bool isCamera;

    void loadUI(std::string file, std::shared_ptr<DocData> docData);
    void buttonPressListeners();
    void loadEngineUI();

public:
    static bool isGame;


    int mode;

    void update();
    void drawUI();

    void enableUI(std::string name);
    void disableUI(std::string name);

    void addUI(std::string file);
    void addUI(std::string file, int x, int y);
    void addUI(std::string file, std::function<Vector2()> callback);

    void addEventListener(std::string id, std::function<void(AUI*)> callback, bool overwrite = false);
    const std::function<void(AUI*)>& getEventListener(std::string id);
    void removeEventListener(std::string id);

    void initInput(std::string id, std::string* text) { inputs[id] = text; }
    void deleteInput(std::string id) 
    { 
        if (inputs.count(id)) inputs.erase(id);
        else print("ERROR: attempted to delete nonexistant input %1%\n", id);
    }
    std::string* getInput(std::string id) 
    { 
        if (inputs.count(id)) return inputs[id]; 
        else 
        {
            print("ERROR: tryed to get non-existant input %1%\n", id);
            return nullptr;
        }
    }

    bool getCondition(std::string id);
    void setCondition(std::string id, bool cond);

    bool isCameraUpdated() { return isCamera; }

    void loadDirectoryUI(std::string directory);

    AUI();
    ~AUI();
};

/*
    Inline Definitions
*/

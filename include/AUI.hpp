#pragma once

#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "UIContainer.hpp"


class AUI
{
private:


    std::map<std::string, std::shared_ptr<litehtml::document>> docs;
    std::map<std::string, bool> isDrawn;
    DocumentContainer container;
    std::string html;

    std::unordered_map<std::string, std::function<void(AUI*)>> eventListeners;
    std::unordered_map<std::string, bool> conditions;
    
    bool isCamera;

public:
    static bool isGame;


    int mode;

    void update();
    void drawUI();

    void enableUI(std::string name);
    void disableUI(std::string name);

    void addEventListener(std::string id, std::function<void(AUI*)> callback, bool overwrite = false);
    const std::function<void(AUI*)>& getEventListener(std::string id);
    void removeEventListener(std::string id);

    bool getCondition(std::string id);
    void setCondition(std::string id, bool cond);

    bool isCameraUpdated() { return isCamera; }

    AUI();
    ~AUI();
};

/*
    Inline Definitions
*/

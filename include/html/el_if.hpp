#pragma once

#include "litehtml.h"
#include "litehtml/html_tag.h"
#include "litehtml/render_item.h"
#include <memory>
#include <list>

class AUI;

class el_if : public litehtml::html_tag
{
private:
    std::string id;
    AUI* ui;

    std::list<std::shared_ptr<litehtml::render_item>> backupChildren;
    std::shared_ptr<litehtml::element> emptyElement;
    std::shared_ptr<litehtml::render_item> emptyElementRi;

public:
    el_if(const std::shared_ptr<litehtml::document>& doc, AUI* ui);

    void parse_attributes() override;

    void draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position *clip, const std::shared_ptr<litehtml::render_item> &ri) override;
};

class el_test : public litehtml::html_tag
{
public:
    el_test(const std::shared_ptr<litehtml::document>& doc, AUI* ui);

};
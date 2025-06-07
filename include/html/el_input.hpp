#pragma once
#include "litehtml.h"
#include "litehtml/html_tag.h"

class AUI;

class el_input : public litehtml::html_tag
{
private:

    AUI* ui;
    bool isClicked;
    std::string text;
    std::string id;

public:
    el_input(const std::shared_ptr<litehtml::document>& doc, AUI* ui);
	void draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position *clip, const std::shared_ptr<litehtml::render_item> &ri) override;
	void parse_attributes() override;
    ~el_input();
};

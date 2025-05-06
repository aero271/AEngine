#pragma once
#include <litehtml.h>
#include <litehtml/html_tag.h>
#include <memory>

class AUI;
class AItems;

class el_button : public litehtml::html_tag
{
private:
    std::string id;
public:
    explicit    el_button(const std::shared_ptr<litehtml::document>& doc);
    void        draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position *clip, const std::shared_ptr<litehtml::render_item> &ri) override;

};

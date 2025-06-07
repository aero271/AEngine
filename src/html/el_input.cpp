#include "html/el_input.hpp"
#include "litehtml/render_item.h"
#include "AUI.hpp"
#include <raylib.h>

el_input::el_input(const std::shared_ptr<litehtml::document>& doc, AUI* ui) 
    : html_tag(doc), ui(ui)
{
    text = "fucl";
}

void el_input::parse_attributes() 
{
    html_tag::parse_attributes();

    if (m_attrs.count("id"))
    {
        id = m_attrs["id"];
        ui->initInput(id, &text);
    }
}

void el_input::draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position* clip, const std::shared_ptr<litehtml::render_item>& ri)
{
    litehtml::position pos = ri->pos();
    pos.x += x;
    pos.y += y;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mp = GetMousePosition();
        if (mp.x > pos.left() && mp.x < pos.right() && mp.y > pos.top() && mp.y < pos.bottom())
        {
            isClicked = true;
        }
        else 
        {
            isClicked = false;
        }
    }

    if (isClicked)
    {
        int key = GetKeyPressed();
        if (key)
        {
            if (key == KEY_BACKSPACE)
            {
                text.erase(text.end() - 1);
            }
            else if (key == KEY_SPACE)
            {
                text += " ";
            }
            else 
            {
                text += GetKeyName(key);
            }
        }
    }

    html_tag::draw(hdc, x, y, clip, ri);

    get_document()->container()->draw_text(hdc, text.c_str(), m_css.get_font(), m_css.get_color(), pos);
}

el_input::~el_input()
{
    ui->deleteInput(id);
}
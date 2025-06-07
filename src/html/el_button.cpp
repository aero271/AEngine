#include "html/el_button.hpp"
#include "litehtml/render_item.h"
#include "AItems.hpp"
#include "AUI.hpp"

el_button::el_button(const std::shared_ptr<litehtml::document>& doc) 
    : html_tag(doc) 
{
}

void el_button::draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position *clip, const std::shared_ptr<litehtml::render_item> &ri)
{
    // standard element draw behavior
    html_tag::draw(hdc, x, y, clip, ri);

    if (m_attrs.find("id") == m_attrs.end())
    {
        return;
    }

    litehtml::position pos = ri->pos();
    pos.x += x;
    pos.y += y;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mp = GetMousePosition();
        if (mp.x > pos.left() && mp.x < pos.right() && mp.y > pos.top() && mp.y < pos.bottom())
        {
            AUI* ui = (AUI*)hdc;
            try
            {
                ui->getEventListener(m_attrs["id"])(ui);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            
        }
    }
}
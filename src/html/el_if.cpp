
#include "html/el_if.hpp"
#include <stdexcept>
#include "AUI.hpp"
#include "definitions.hpp"

el_if::el_if(const std::shared_ptr<litehtml::document>& doc, AUI* ui)
    : html_tag(doc), ui(ui)
{
}

void el_if::parse_attributes()
{
    html_tag::parse_attributes();

    if (m_attrs.find("id") == m_attrs.end())
    {
        throw std::logic_error("ERROR: if element must have id attribute\n");
    }
    id = m_attrs["id"];

    if (m_attrs.find("default") != m_attrs.end())
    {
        bool cond = (m_attrs["default"] == "true") ? true : false;
        ui->setCondition(id, cond);
    }
    else
    {
        ui->setCondition(id, true);
    }
}

void el_if::draw(litehtml::uint_ptr hdc, int x, int y, const litehtml::position *clip, const std::shared_ptr<litehtml::render_item> &ri)
{
    std::shared_ptr<litehtml::render_item>& el = const_cast<std::shared_ptr<litehtml::render_item>&>(ri);
    if (ui->getCondition(id))
    {
        println("ran");
        html_tag::draw(hdc, x, y, clip, ri);
    }
}
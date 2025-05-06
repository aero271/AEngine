
#include "UIContainer.hpp"
#include <stdexcept>
#include "definitions.hpp"
#include <cstdlib>
#include "glad.h"
#include "html/el_button.hpp"
#include "html/el_if.hpp"
#include "AUI.hpp"

using namespace litehtml;

#ifdef __linux__ 
    void openUrl(std::string url)
    {
        std::string command = "xdg-open " + url + " &";
        system(command.c_str());
    }
#else 
    void openUrl(std::string url)
    {
        print("ERROR: platform link opening not supported");
    }
#endif


DocumentContainer::DocumentContainer(AUI* ui) : ui(ui)
{
    if(FT_Init_FreeType(&ftInstance))
    {
        throw std::runtime_error("ERROR: FreeType failed to initialize\n");
    }
}

DocumentContainer::~DocumentContainer()
{
    for (auto& [_, image] : images)
    {
        UnloadTexture(image);
    }
    for (auto& [_, font] : fonts)
    {
        UnloadFont(font->font);
    }
}

litehtml::uint_ptr  DocumentContainer::create_font(const font_description& descr, const document* doc, litehtml::font_metrics* fm)
{
    std::string fontPath = std::string("fonts/") + descr.family + ".ttf";
    auto font = make_shared<FontData>(LoadFontEx(fontPath.c_str(), descr.size, 0, 0), descr.size);
    fontID++;

    // FreeType used to extract font data, not actually used to draw anything
    if (FT_New_Face(ftInstance, fontPath.c_str(), 0, &font->face))
    {
        print("ERROR: font %1% failed to load\n", fontPath);
        return (uint_ptr)nullptr;
    }
    FT_Set_Pixel_Sizes(font->face, 0, font->size);

    fm->ascent   = font->face->size->metrics.ascender >> 6;
    fm->descent  = - (font->face->size->metrics.descender >> 6);  
    fm->height   = font->face->size->metrics.height >> 6;

    fonts[(uint_ptr)fontID] = font;
    return (uint_ptr)fontID;
}

void DocumentContainer::delete_font(uint_ptr hFont) 
{
    fonts.erase(hFont);
}

int DocumentContainer::text_width(const char* text, uint_ptr hFont)
{
    FT_Face& f = fonts[hFont]->face;
    int width = 0;
    int length = strlen(text);
    for (int i = 0; i < length; i++)
    {
        FT_Load_Char(f, text[i], FT_LOAD_DEFAULT);
        width += f->glyph->advance.x / 64;
    }
    return width;
}

void DocumentContainer::draw_text(uint_ptr hdc, const char* text, uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) 
{
    DrawTextEx(fonts[hFont]->font, text, {(float)pos.x, (float)pos.y}, fonts[hFont]->size, 2, (Color){color.red, color.green, color.blue, color.alpha});
}

int DocumentContainer::pt_to_px(int pt) const { return pt; }
int	DocumentContainer::get_default_font_size() const { return 12; }
const char*	DocumentContainer::get_default_font_name() const { return "Roboto"; }

void DocumentContainer::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) 
{

}

void DocumentContainer::load_image(const char* src, const char* baseurl, bool redraw_on_ready)
{
    std::string imagePath;
    if (baseurl) imagePath = std::string(baseurl) + src;
    else imagePath = std::string(src);

    if (images.find(imagePath) != images.end()) return;

    images[imagePath] = LoadTexture(imagePath.c_str());
}


void DocumentContainer::get_image_size(const char* src, const char* baseurl, litehtml::size& sz)
{
    std::string imagePath;
    if (baseurl) imagePath = std::string(baseurl) + src;
    else imagePath = std::string(src);

    sz.height = images[imagePath].height;
    sz.width = images[imagePath].width;
}

void DocumentContainer::draw_image(litehtml::uint_ptr hdc, const background_layer& layer, const std::string& url, const std::string& base_url)
{
    std::string imagePath = base_url + url;

    int x = layer.origin_box.x;
    int y = layer.origin_box.y;

    DrawTexture(images[imagePath], x, y, WHITE);
}

void DocumentContainer::draw_solid_fill(litehtml::uint_ptr hdc, const background_layer& layer, const web_color& color) 
{
    Color castColor = (Color){ color.red, color.green, color.blue, color.alpha };
    //print("Trying to draw a rectangle with at %1%, %2%, with size %3%, %4%\n" );

    DrawRectangle(layer.clip_box.x, layer.clip_box.y, layer.clip_box.width, layer.clip_box.height, castColor);
}

void DocumentContainer::draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer& layer, const background_layer::linear_gradient& gradient) {}
void DocumentContainer::draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer& layer, const background_layer::radial_gradient& gradient) {}
void DocumentContainer::draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer& layer, const background_layer::conic_gradient& gradient) {}

void DocumentContainer::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root)
{
    // Draw top border
    if(borders.top.width > 0) {
        Color color = {borders.top.color.red, borders.top.color.green, borders.top.color.blue, borders.top.color.alpha};
        DrawRectangle(draw_pos.x, draw_pos.y, draw_pos.width, borders.top.width, color);
    }
    
    // Draw right border
    if(borders.right.width > 0) {
        Color color = {borders.right.color.red, borders.right.color.green, borders.right.color.blue, borders.right.color.alpha};
        DrawRectangle(draw_pos.x + draw_pos.width - borders.right.width, draw_pos.y, borders.right.width, draw_pos.height, color);
    }
    
    // Draw bottom border
    if(borders.bottom.width > 0) {
        Color color = {borders.bottom.color.red, borders.bottom.color.green, borders.bottom.color.blue, borders.bottom.color.alpha};
        DrawRectangle(draw_pos.x, draw_pos.y + draw_pos.height - borders.bottom.width, draw_pos.width, borders.bottom.width, color);
    }
    
    // Draw left border
    if(borders.left.width > 0) {
        Color color = {borders.left.color.red, borders.left.color.green, borders.left.color.blue, borders.left.color.alpha};
        DrawRectangle(draw_pos.x, draw_pos.y, borders.left.width, draw_pos.height, color);
    }
}

void DocumentContainer::set_caption(const char* caption) {}
void DocumentContainer::set_base_url(const char* base_url) {}
void DocumentContainer::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) {}

void DocumentContainer::on_anchor_click(const char* url, const litehtml::element::ptr& el)
{
    openUrl(url);
}

void DocumentContainer::on_mouse_event(const litehtml::element::ptr& el, litehtml::mouse_event event) 
{
    //perhaps make a callback system here
}

void DocumentContainer::set_cursor(const char* cursor) {}
void DocumentContainer::transform_text(litehtml::string& text, litehtml::text_transform tt) {}

void DocumentContainer::import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) 
{
    std::string path = std::string("UI/") + baseurl + url;
    char* css = LoadFileText(path.c_str()); 
    text = std::string(css);
    UnloadFileText(css);
}


void DocumentContainer::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) 
{
    BeginScissorMode(pos.x, pos.y, pos.width, pos.height);
}

void DocumentContainer::del_clip() 
{
    EndScissorMode();
}

void DocumentContainer::get_client_rect(litehtml::position& client) const 
{
    client.x = 0;
    client.y = 0;
    client.width = SCREEN_WIDTH;
    client.height = SCREEN_HEIGHT;
}

litehtml::element::ptr DocumentContainer::create_element(const char* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc) 
{
    std::string tag = tag_name;
    if (tag == "button")
    {
        return std::make_shared<el_button>(doc);
    }
    else if (tag == "if")
    {
        return std::make_shared<el_if>(doc, ui);
    }
    return nullptr;
}

void DocumentContainer::get_media_features(litehtml::media_features& media) const {}
void DocumentContainer::get_language(litehtml::string& language, litehtml::string& culture) const {}

void DocumentContainer::split_text(const char* text, const std::function<void(const char*)>& on_word, const std::function<void(const char*)>& on_space) 
{
    const char* ptr = text;
    while (*ptr) {
        // Process word
        const char* start = ptr;
        while (*ptr && !isspace(*ptr)) {
            ptr++;
        }
        if (ptr > start) {
            std::string word(start, ptr);
            on_word(word.c_str());
        }
        // Process space
        start = ptr;
        while (*ptr && isspace(*ptr)) {
            ptr++;
        }
        if (ptr > start) {
            std::string space(start, ptr);
            
            on_space(space.c_str());
        }
    }
} 

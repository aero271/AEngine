#pragma once

#include <litehtml.h>
#include <map>
#include <string>
#include <raylib.h>
#include <ft2build.h>
#include FT_FREETYPE_H

class AUI;

using namespace litehtml;

class DocumentContainer : public litehtml::document_container
{
public:
    DocumentContainer(AUI* ui);
    virtual ~DocumentContainer();

	litehtml::uint_ptr  create_font(const font_description& descr, const document* doc, litehtml::font_metrics* fm) override;
    void                delete_font(uint_ptr hFont) override;
    int                 text_width(const char* text, uint_ptr hFont) override;
    void                draw_text(uint_ptr hdc, const char* text, uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override;
    int                 pt_to_px(int pt) const override;
    int					get_default_font_size() const override;
    const char*			get_default_font_name() const override;
    void				draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;
    void				load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;
    void				get_image_size(const char* src, const char* baseurl, litehtml::size& sz) override;
    void				draw_image(litehtml::uint_ptr hdc, const background_layer& layer, const std::string& url, const std::string& base_url) override;
    void				draw_solid_fill(litehtml::uint_ptr hdc, const background_layer& layer, const web_color& color) override;
    void				draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer& layer, const background_layer::linear_gradient& gradient) override;
    void				draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer& layer, const background_layer::radial_gradient& gradient) override;
    void				draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer& layer, const background_layer::conic_gradient& gradient) override;
    void				draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) override;

    void				set_caption(const char* caption) override;
    void				set_base_url(const char* base_url) override;
    void				link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) override;
    void				on_anchor_click(const char* url, const litehtml::element::ptr& el) override;
    void				on_mouse_event(const litehtml::element::ptr& el, litehtml::mouse_event event) override;
    void			    set_cursor(const char* cursor) override;
    void				transform_text(litehtml::string& text, litehtml::text_transform tt) override;
    void				import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) override;
    void			    set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) override;
    void				del_clip() override;
    void			    get_client_rect(litehtml::position& client) const override;
    litehtml::element::ptr	create_element( const char* tag_name,
                                                    const litehtml::string_map& attributes,
                                                    const std::shared_ptr<litehtml::document>& doc) override;

    void				get_media_features(litehtml::media_features& media) const override;
    void				get_language(litehtml::string& language, litehtml::string& culture) const override;
    litehtml::string	resolve_color(const litehtml::string& /*color*/) const { return litehtml::string(); }
    void				split_text(const char* text, const std::function<void(const char*)>& on_word, const std::function<void(const char*)>& on_space) override; 

private:

    struct FontData
    {
        Font font;
        int size;
        FT_Face face;

        FontData(Font font, int size) : font(font), size(size) {}

    };
    


    std::map<uint_ptr, std::shared_ptr<FontData>> fonts;
    FT_Library ftInstance;
    int fontID = 0;

    std::map<std::string, Texture> images;

    AUI* ui;

};
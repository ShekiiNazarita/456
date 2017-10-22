#ifdef USE_TILE_LOCAL
#pragma once

#include <vector>

#include "fixedvector.h"
#include "format.h"
#include "menu.h"
#include "tilebuf.h"
#include "tilereg.h"

class MenuRegion : public ControlRegion
{
public:
    MenuRegion(ImageManager *im, FontWrapper *entry);

    virtual int handle_mouse(MouseEvent &event) override;
    virtual void render() override;
    virtual void clear() override;

    void set_entry(int index, const string &s, int colour, const MenuEntry *me,
                   bool mark_selected = true);
    void set_offset(int lines);
    void set_more(const formatted_string &more);
    void set_num_columns(int columns);

    void get_visible_items_range(int &first, int &last);

    int cur_page;
    int num_pages;

    virtual void place_entries();
protected:
    virtual void on_resize() override;
    int mouse_entry(int x, int y);
    int vis_item_first, vis_item_last;

    struct MenuRegionEntry
    {
        formatted_string text;
        int colour; // keep it separate from text
        int sx, ex, sy, ey;
        int page;
        bool selected;
        bool heading;
        char key;
        bool valid;
        vector<tile_def> tiles;
    };

    ImageManager *m_image;
    FontWrapper *m_font_entry;
    formatted_string m_more;
    int m_mouse_idx;
    int m_max_columns;
    bool m_dirty;
#ifdef TOUCH_UI
    int m_more_region_start;
#endif

    virtual void run() override {};
    void _place_entries(const int left_offset, const int top_offset,
                        const int menu_width);
    void _clear_buffers();

    vector<MenuRegionEntry> m_entries;

    // TODO enne - remove this?
    ShapeBuffer m_shape_buf;
    LineBuffer m_line_buf;
    FontBuffer m_font_buf;
    FixedVector<TileBuffer, TEX_MAX> m_tile_buf;
};

#endif

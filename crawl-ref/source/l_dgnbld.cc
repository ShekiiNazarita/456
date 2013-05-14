/**
 * @file
 * @brief Building routines (library "dgn").
**/

#include "AppHdr.h"

#include <cmath>
#include <vector>

#include "dungeon.h"
#include "dgn-delve.h"
#include "dgn-irregular-box.h"
#include "dgn-shoals.h"
#include "dgn-swamp.h"
#include "dgn-layouts.h"
#include "cluautil.h"
#include "coord.h"
#include "coordit.h"
#include "l_libs.h"
#include "mapdef.h"
#include "random.h"

static const char *exit_glyphs = "{}()[]<>@";

// Return the integer stored in the table (on the stack) with the key name.
// If the key doesn't exist or the value is the wrong type, return defval.
static int _table_int(lua_State *ls, int idx, const char *name, int defval)
{
    if (!lua_istable(ls, idx))
        return defval;
    lua_pushstring(ls, name);
    lua_gettable(ls, idx < 0 ? idx - 1 : idx);
    bool nil = lua_isnil(ls, idx);
    bool valid = lua_isnumber(ls, idx);
    if (!nil && !valid)
        luaL_error(ls, "'%s' in table, but not an int.", name);
    int ret = (!nil && valid ? luaL_checkint(ls, idx) : defval);
    lua_pop(ls, 1);
    return ret;
}

// Return the character stored in the table (on the stack) with the key name.
// If the key doesn't exist or the value is the wrong type, return defval.
static char _table_char(lua_State *ls, int idx, const char *name, char defval)
{
    if (!lua_istable(ls, idx))
        return defval;
    lua_pushstring(ls, name);
    lua_gettable(ls, idx < 0 ? idx - 1 : idx);
    bool nil = lua_isnil(ls, idx);
    bool valid = lua_isstring(ls, idx);
    if (!nil && !valid)
        luaL_error(ls, "'%s' in table, but not a string.", name);

    char ret = defval;
    if (!nil && valid)
    {
        const char *str = lua_tostring(ls, idx);
        if (str[0] && !str[1])
            ret = str[0];
        else
            luaL_error(ls, "'%s' has more than one character.", name);
    }
    lua_pop(ls, 1);
    return ret;
}

// Return the string stored in the table (on the stack) with the key name.
// If the key doesn't exist or the value is the wrong type, return defval.
static const char* _table_str(lua_State *ls, int idx, const char *name, const char *defval)
{
    if (!lua_istable(ls, idx))
        return defval;
    lua_pushstring(ls, name);
    lua_gettable(ls, idx < 0 ? idx - 1 : idx);
    bool nil = lua_isnil(ls, idx);
    bool valid = lua_isstring(ls, idx);
    if (!nil && !valid)
        luaL_error(ls, "'%s' in table, but not a string.", name);
    const char *ret = (!nil && valid ? lua_tostring(ls, idx) : defval);
    lua_pop(ls, 1);
    return ret;
}

// Return the boolean stored in the table (on the stack) with the key name.
// If the key doesn't exist or the value is the wrong type, return defval.
static bool _table_bool(lua_State *ls, int idx, const char *name, bool defval)
{
    if (!lua_istable(ls, idx))
        return defval;
    lua_pushstring(ls, name);
    lua_gettable(ls, idx < 0 ? idx - 1 : idx);
    bool nil = lua_isnil(ls, idx);
    bool valid = lua_isboolean(ls, idx);
    if (!nil && !valid)
        luaL_error(ls, "'%s' in table, but not a bool.", name);
    bool ret = (!nil && valid ? lua_toboolean(ls, idx) : defval);
    lua_pop(ls, 1);
    return ret;
}

// These macros all assume the table is on the top of the lua stack.
#define TABLE_INT(ls, val, def) int val = _table_int(ls, -1, #val, def);
#define TABLE_CHAR(ls, val, def) char val = _table_char(ls, -1, #val, def);
#define TABLE_STR(ls, val, def) const char *val = _table_str(ls, -1, #val, def);
#define TABLE_BOOL(ls, val, def) bool val = _table_bool(ls, -1, #val, def);

#define ARG_INT(ls, num, val, def) int val = lua_isnone(ls, num) ? \
                                             def : lua_tointeger(ls, num)

// Read a set of box coords (x1, y1, x2, y2) from the table.
// Return true if coords are valid.
static bool _coords(lua_State *ls, map_lines &lines,
                    int &x1, int &y1, int &x2, int &y2, int border = 0)
{
    const int idx = -1;
    x1 = _table_int(ls, idx, "x1", 0);
    y1 = _table_int(ls, idx, "y1", 0);
    x2 = _table_int(ls, idx, "x2", lines.width() - 1);
    y2 = _table_int(ls, idx, "y2", lines.height() - 1);

    if (x2 < x1)
        swap(x1, x2);
    if (y2 < y1)
        swap(y1, y2);

    return (x1 + border <= x2 - border && y1 + border <= y2 - border);
}

// Check if a given coordiante is valid for lines.
static bool _valid_coord(lua_State *ls, map_lines &lines, int x, int y, bool error = true)
{
    if (x < 0 || x >= lines.width())
    {
        if (error)
            luaL_error(ls, "Invalid x coordinate: %d", x);
        return false;
    }

    if (y < 0 || y >= lines.height())
    {
        if (error)
            luaL_error(ls, "Invalid y coordinate: %d", y);
        return false;
    }

    return true;
}

// Does what fill_area did, but here, so that it can be used through
// multiple functions (including make_box).
static int _fill_area(lua_State *ls, map_lines &lines, int x1, int y1, int x2, int y2, char fill)
{
    for (int y = y1; y <= y2; ++y)
        for (int x = x1; x <= x2; ++x)
            lines(x, y) = fill;

    return 0;
}

static void _border_area(map_lines &lines, int x1, int y1, int x2, int y2, char border)
{
    for (int x = x1 + 1; x < x2; ++x)
        lines(x, y1) = border, lines(x, y2) = border;
    for (int y = y1; y <= y2; ++y)
        lines(x1, y) = border, lines(x2, y) = border;
}

// Specifically only deals with horizontal lines.
static vector<coord_def> _box_side(int x1, int y1, int x2, int y2, int side)
{
    vector<coord_def> line;

    int start_x, start_y, stop_x, stop_y, x, y;

    switch (side)
    {
    case 0: start_x = x1; start_y = y1; stop_x = x2; stop_y = y1; break;
    case 1: start_x = x2; start_y = y1; stop_x = x2; stop_y = y2; break;
    case 2: start_x = x1; start_y = y2; stop_x = x2; stop_y = y2; break;
    case 3: start_x = x1; start_y = y1; stop_x = x1; stop_y = y2; break;
    default: die("invalid _box_side");
    }

    x = start_x; y = start_y;

    if (start_x == stop_x)
    {
        for (y = start_y+1; y < stop_y; y++)
            line.push_back(coord_def(x, y));
    }
    else
    {
        for (x = start_x+1; x < stop_x; x++)
            line.push_back(coord_def(x, y));
    }

    return line;
}

// Does what count_passable_neighbors does, but in C++ form.
static int _count_passable_neighbors(lua_State *ls, map_lines &lines, int x,
                                     int y, const char *passable = traversable_glyphs)
{
    coord_def tl(x, y);
    int count = 0;

    for (adjacent_iterator ai(tl); ai; ++ai)
    {
        if (_valid_coord(ls, lines, ai->x, ai->y, false))
            if (strchr(passable, lines(*ai)))
                count++;
    }

    return count;
}

static int _count_passable_neighbors(lua_State *ls, map_lines &lines, coord_def point,
                                     const char *passable = traversable_glyphs)
{
    return _count_passable_neighbors(ls, lines, point.x, point.y, passable);
}

static vector<coord_def> _get_pool_seed_positions(
                                                vector<vector<int> > pool_index,
                                                int pool_size,
                                                int min_seperation)
{
    const int NO_POOL   = 999997; // must match dgn_add_pools

    if (pool_size < 1)
        pool_size = 1;

    // 1. Find all floor positions

    vector<coord_def> floor_positions;

    for (unsigned int x = 0; x < pool_index.size(); x++)
        for (unsigned int y = 0; y < pool_index[x].size(); y++)
        {
            if (pool_index[x][y] == NO_POOL)
                floor_positions.push_back(coord_def(x, y));
        }

    // 2. Choose the pool seed positions

    int min_seperation_squared = min_seperation * min_seperation;
    int pool_count_goal = (floor_positions.size() + random2(pool_size))
                          / pool_size;

    vector<coord_def> seeds;

    for (int i = 0; i < pool_count_goal; i++)
    {
        if (floor_positions.empty())
        {
            // give up if no more positions
            break;
        }

        // choose a random position
        int chosen_index = random2(floor_positions.size());
        coord_def chosen_coord = floor_positions[chosen_index];
        floor_positions[chosen_index] = floor_positions.back();
        floor_positions.pop_back();

        // check if it is too close to another seed
        bool too_close = false;
        for (unsigned int j = 0; j < seeds.size(); j++)
        {
            int diff_x = chosen_coord.x - seeds[j].x;
            int diff_y = chosen_coord.y - seeds[j].y;
            int distance_squared = diff_x * diff_x + diff_y * diff_y;

            if (distance_squared < min_seperation_squared)
            {
                too_close = true;
                break;
            }
        }

        // if not too close, add it to the list
        if (!too_close)
            seeds.push_back(chosen_coord);
    }

    // 3. Return the pool seeds

    return seeds;
}

// This function assumes the table is on the top of the lua stack.
static vector<char> _pool_fill_glyphs_from_table(lua_State *ls,
                                                 const char *name)
{
    // We will go through the table and put each possible pool
    //  fill glyph in a vector once for each weight.  This will
    //  make it easy to select random ones with the correct
    //  distribution when we need to.
    vector<char> fill_glyphs;

    lua_pushstring(ls, name);
    lua_gettable(ls, -2);
    if (!lua_isnil(ls, -1) && lua_istable(ls, -1))
    {
        // For some reason, LUA requires us to have a dummy
        //  value to remove from the stack whenever we do a
        //  table lookup.  Here is the first one
        lua_pushnil(ls);

        // table is now at -2
        while (lua_next(ls, -2) != 0)
        {
            // uses 'key' (at index -2) and 'value' (at index -1)
            if (lua_type(ls, -2) == LUA_TSTRING
                && lua_type(ls, -1) == LUA_TNUMBER)
            {
                // we use first character of string as glyph
                char glyph = (lua_tostring(ls, -2))[0];

                int count = lua_tointeger(ls, -1);
                // sanity-check
                if (count > 10000)
                    count = 10000;

                if (glyph != '\0')
                {
                    for (int i = 0; i < count; i++)
                        fill_glyphs.push_back(glyph);
                }
            }

            // removes 'value'; keeps 'key' for next iteration
            lua_pop(ls, 1);
        }
    }
    lua_pop(ls, 1);

    // We might have not got anything, if so, use floor
    if (fill_glyphs.size() == 0)
        fill_glyphs.push_back('.');

    return fill_glyphs;
}

// These functions check for irregularities before the first
//  corner along a wall in the indicated direction.
static bool _wall_is_empty(map_lines &lines,
                           int x, int y,
                           const char* wall, const char* floor,
                           bool horiz = false)
{
    coord_def normal(horiz ? 0 : 1, horiz ? 1 : 0);
    for (int d = 1; d >= -1; d-=2)
    {
        coord_def length(horiz ? d : 0, horiz ? 0 : d);
        int n = 1;

        while (true)
        {
            coord_def pos(x + length.x*n,y + length.y*n);
            if (!lines.in_bounds(coord_def(pos.x + normal.x, pos.y + normal.y))
                || !strchr(floor, lines(pos.x + normal.x, pos.y + normal.y)))
            {
                break;
            }
            if (!lines.in_bounds(coord_def(pos.x - normal.x, pos.y - normal.y))
                || !strchr(floor, lines(pos.x - normal.x, pos.y - normal.y)))
            {
                break;
            }

            if (!strchr(wall, lines(pos.x, pos.y)))
                return false;

            n++;
        }
    }

    // hit the edge of the map, so this is good
    return true;
}

LUAFN(dgn_count_feature_in_box)
{
    LINES(ls, 1, lines);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    TABLE_STR(ls, feat, "");

    coord_def tl(x1, y1);
    coord_def br(x2, y2);

    PLUARET(number, lines.count_feature_in_box(tl, br, feat));
}

LUAFN(dgn_count_antifeature_in_box)
{
    LINES(ls, 1, lines);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    TABLE_STR(ls, feat, "");

    coord_def tl(x1, y1);
    coord_def br(x2, y2);

    int sum = (br.x - tl.x + 1) * (br.y - tl.y + 1);
    PLUARET(number, sum - lines.count_feature_in_box(tl, br, feat));
}

LUAFN(dgn_count_neighbors)
{
    LINES(ls, 1, lines);

    TABLE_STR(ls, feat, "");
    TABLE_INT(ls, x, -1);
    TABLE_INT(ls, y, -1);

    if (!_valid_coord(ls, lines, x, y))
        return 0;

    coord_def tl(x-1, y-1);
    coord_def br(x+1, y+1);

    PLUARET(number, lines.count_feature_in_box(tl, br, feat));
}

LUAFN(dgn_count_passable_neighbors)
{
    LINES(ls, 1, lines);

    TABLE_STR(ls, passable, traversable_glyphs);
    TABLE_INT(ls, x, -1);
    TABLE_INT(ls, y, -1);

    if (!_valid_coord(ls, lines, x, y))
    {
        lua_pushnumber(ls, 0);
        return 1;
    }

    lua_pushnumber(ls, _count_passable_neighbors(ls, lines, x, y, passable));
    return 1;
}


LUAFN(dgn_is_valid_coord)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x, -1);
    TABLE_INT(ls, y, -1);

    if (x < 0 || x >= lines.width())
    {
        lua_pushboolean(ls, false);
        return 1;
    }

    if (y < 0 || y >= lines.height())
    {
        lua_pushboolean(ls, false);
        return 1;
    }

    lua_pushboolean(ls, true);
    return 1;
}

LUAFN(dgn_extend_map)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, height, 1);
    TABLE_INT(ls, width, 1);
    TABLE_CHAR(ls, fill, 'x');

    lines.extend(width, height, fill);

    return 0;
}

LUAFN(dgn_fill_area)
{
    LINES(ls, 1, lines);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    TABLE_CHAR(ls, fill, 'x');
    TABLE_CHAR(ls, border, fill);

    _fill_area(ls, lines, x1, y1, x2, y2, fill);
    if (border != fill)
        _border_area(lines, x1, y1, x2, y2, border);

    return 0;
}

LUAFN(dgn_fill_disconnected)
{
    LINES(ls, 1, lines);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    TABLE_CHAR(ls, fill, 'x');
    TABLE_STR(ls, passable, traversable_glyphs);
    TABLE_STR(ls, wanted, exit_glyphs);

    coord_def tl(x1, y1);
    coord_def br(x2, y2);

    travel_distance_grid_t tpd;
    memset(tpd, 0, sizeof(tpd));

    int nzones = 0;
    for (rectangle_iterator ri(tl, br); ri; ++ri)
    {
        const coord_def c = *ri;
        if (tpd[c.x][c.y] || passable && !strchr(passable, lines(c)))
            continue;

        if (lines.fill_zone(tpd, c, tl, br, ++nzones, wanted, passable))
            continue;

        // If wanted wasn't found, fill every passable square that
        // we just found with the 'fill' glyph.
        for (rectangle_iterator f(tl, br); f; ++f)
        {
            const coord_def fc = *f;
            if (tpd[fc.x][fc.y] == nzones)
                lines(fc) = fill;
        }
    }

    return 0;
}

LUAFN(dgn_is_passable_coord)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x, -1);
    TABLE_INT(ls, y, -1);
    TABLE_STR(ls, passable, traversable_glyphs);

    if (!_valid_coord(ls, lines, x, y))
        return 0;

    if (strchr(passable, lines(x, y)))
        lua_pushboolean(ls, true);
    else
        lua_pushboolean(ls, false);

    return 1;
}

LUAFN(dgn_find_in_area)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x1, -1);
    TABLE_INT(ls, y1, -1);
    TABLE_INT(ls, x2, -1);
    TABLE_INT(ls, y2, -1);
    TABLE_BOOL(ls, find_vault, false);

    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    TABLE_STR(ls, find, "x");

    int x, y;

    for (x = x1; x <= x2; x++)
        for (y = y1; y <= y2; y++)
            if (strchr(find, lines(x, y))
                || (find_vault && (env.level_map_mask(coord_def(x,y))
                                   & MMT_VAULT)))
            {
                lua_pushboolean(ls, true);
                return 1;
            }

    lua_pushboolean(ls, false);
    return 1;
}

LUAFN(dgn_height)
{
    LINES(ls, 1, lines);
    PLUARET(number, lines.height());
}

LUAFN(dgn_join_the_dots)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x1, -1);
    TABLE_INT(ls, y1, -1);
    TABLE_INT(ls, x2, -1);
    TABLE_INT(ls, y2, -1);
    TABLE_STR(ls, passable, traversable_glyphs);
    TABLE_CHAR(ls, fill, '.');

    if (!_valid_coord(ls, lines, x1, y1))
        return 0;
    if (!_valid_coord(ls, lines, x2, y2))
        return 0;

    coord_def from(x1, y1);
    coord_def to(x2, y2);

    if (from == to)
        return 0;

    coord_def at = from;
    do
    {
        char glyph = lines(at);

        if (!strchr(passable, glyph))
            lines(at) = fill;

        if (at == to)
            break;

        if (at.x < to.x)
        {
            at.x++;
            continue;
        }

        if (at.x > to.x)
        {
            at.x--;
            continue;
        }

        if (at.y > to.y)
        {
            at.y--;
            continue;
        }

        if (at.y < to.y)
        {
            at.y++;
            continue;
        }
    }
    while (true);

    return 0;
}

LUAFN(dgn_make_circle)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x, -1);
    TABLE_INT(ls, y, -1);
    TABLE_INT(ls, radius, 1);
    TABLE_CHAR(ls, fill, 'x');

    if (!_valid_coord(ls, lines, x, y))
        return 0;

    float radius_squared_max = (radius + 0.5f) * (radius + 0.5f);
    for (int ry = -radius; ry <= radius; ++ry)
        for (int rx = -radius; rx <= radius; ++rx)
            if (rx * rx + ry * ry < radius_squared_max)
                lines(x + rx, y + ry) = fill;

    return 0;
}

LUAFN(dgn_make_diamond)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x, -1);
    TABLE_INT(ls, y, -1);
    TABLE_INT(ls, radius, 1);
    TABLE_CHAR(ls, fill, 'x');

    if (!_valid_coord(ls, lines, x, y))
        return 0;

    for (int ry = -radius; ry <= radius; ++ry)
        for (int rx = -radius; rx <= radius; ++rx)
            if (abs(rx) + abs(ry) <= radius)
                lines(x + rx, y + ry) = fill;

    return 0;
}

LUAFN(dgn_make_rounded_square)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x, -1);
    TABLE_INT(ls, y, -1);
    TABLE_INT(ls, radius, 1);
    TABLE_CHAR(ls, fill, 'x');

    if (!_valid_coord(ls, lines, x, y))
        return 0;

    for (int ry = -radius; ry <= radius; ++ry)
        for (int rx = -radius; rx <= radius; ++rx)
            if (abs(rx) != radius || abs(ry) != radius)
                lines(x + rx, y + ry) = fill;

    return 0;
}

LUAFN(dgn_make_square)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x, -1);
    TABLE_INT(ls, y, -1);
    TABLE_INT(ls, radius, 1);
    TABLE_CHAR(ls, fill, 'x');

    if (!_valid_coord(ls, lines, x, y))
        return 0;

    for (int ry = -radius; ry <= radius; ++ry)
        for (int rx = -radius; rx <= radius; ++rx)
            lines(x + rx, y + ry) = fill;

    return 0;
}

LUAFN(dgn_make_box)
{
    LINES(ls, 1, lines);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    TABLE_CHAR(ls, floor, '.');
    TABLE_CHAR(ls, wall, 'x');
    TABLE_INT(ls, width, 1);

    _fill_area(ls, lines, x1, y1, x2, y2, wall);
    _fill_area(ls, lines, x1+width, y1+width, x2-width, y2-width, floor);

    return 0;
}

LUAFN(dgn_make_box_doors)
{
    LINES(ls, 1, lines);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    TABLE_INT(ls, number, 1);

    int sides[4] = {0, 0, 0, 0};

    int door_count;

    for (door_count = 0; door_count < number; door_count++)
    {
        int current_side = random2(4);
        if (sides[current_side] >= 2)
            current_side = random2(4);

        vector<coord_def> points = _box_side(x1, y1, x2, y2, current_side);

        int total_points = int(points.size());

        int index = random2avg(total_points, 2 + random2(number));

        int tries = 50;

        while (_count_passable_neighbors(ls, lines, points[index]) <= 3)
        {
            tries--;
            index = random2(total_points);

            if (tries == 0)
                break;
        }

        if (tries == 0)
        {
            door_count--;
            continue;
        }

        sides[current_side]++;
        lines(points[index]) = '+';
    }

    lua_pushnumber(ls, door_count);
    return 1;
}

LUAFN(dgn_make_irregular_box)
{
    LINES(ls, 1, lines);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    TABLE_CHAR(ls, floor, '.');
    TABLE_CHAR(ls, wall, 'x');
    TABLE_CHAR(ls, door, '+');
    TABLE_INT(ls, door_count, 1);
    TABLE_INT(ls, div_x, 1);
    TABLE_INT(ls, div_y, 1);
    TABLE_INT(ls, in_x, 10000);
    TABLE_INT(ls, in_y, 10000);

    make_irregular_box(lines, x1, y1, x2, y2,
                       div_x, div_y, in_x, in_y,
                       floor, wall, door, door_count);
    return 0;
}

// Return a metatable for a point on the map_lines grid.
LUAFN(dgn_mapgrd_table)
{
    MAP(ls, 1, map);

    map_def **mapref = clua_new_userdata<map_def *>(ls, MAPGRD_METATABLE);
    *mapref = map;

    return 1;
}

LUAFN(dgn_octa_room)
{
    LINES(ls, 1, lines);

    int default_oblique = min(lines.width(), lines.height()) / 2 - 1;
    TABLE_INT(ls, oblique, default_oblique);
    TABLE_CHAR(ls, outside, 'x');
    TABLE_CHAR(ls, inside, '.');
    TABLE_STR(ls, replace, "");

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    coord_def tl(x1, y1);
    coord_def br(x2, y2);

    for (rectangle_iterator ri(tl, br); ri; ++ri)
    {
        const coord_def mc = *ri;
        char glyph = lines(mc);
        if (replace[0] && !strchr(replace, glyph))
            continue;

        int ob = 0;
        ob += max(oblique + tl.x - mc.x, 0);
        ob += max(oblique + mc.x - br.x, 0);

        bool is_inside = (mc.y >= tl.y + ob && mc.y <= br.y - ob);
        lines(mc) = is_inside ? inside : outside;
    }

    return 0;
}

LUAFN(dgn_remove_isolated_glyphs)
{
    LINES(ls, 1, lines);

    TABLE_STR(ls, find, "");
    TABLE_CHAR(ls, replace, '.');
    TABLE_INT(ls, percent, 100);
    TABLE_BOOL(ls, boxy, false);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    // we never change the border
    if (x1 < 1)
        x1 = 1;
    if (x2 >= lines.width() - 1)
        x2 = lines.width() - 2;
    if (y1 < 1)
        y1 = 1;
    if (y2 >= lines.height() - 1)
        y2 = lines.height() - 2;

    for (int y = y1; y <= y2; ++y)
        for (int x = x1; x <= x2; ++x)
            if (strchr(find, lines(x, y)) && x_chance_in_y(percent, 100))
            {
                bool do_replace = true;
                for (radius_iterator ri(coord_def(x, y), 1,
                                        (boxy ? C_SQUARE : C_POINTY),
                                        NULL, true);                 ri; ++ri)
                {
                    if (_valid_coord(ls, lines, ri->x, ri->y, false))
                        if (strchr(find, lines(*ri)))
                        {
                            do_replace = false;
                            break;
                        }
                }
                if (do_replace)
                    lines(x, y) = replace;
            }

    return 0;
}

LUAFN(dgn_widen_paths)
{
    LINES(ls, 1, lines);

    TABLE_STR(ls, find, "");
    TABLE_CHAR(ls, replace, '.');
    TABLE_STR(ls, passable, traversable_glyphs);
    TABLE_INT(ls, percent, 100);
    TABLE_BOOL(ls, boxy, false);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    // we never change the border
    if (x1 < 1)
        x1 = 1;
    if (x2 >= lines.width() - 1)
        x2 = lines.width() - 2;
    if (y1 < 1)
        y1 = 1;
    if (y2 >= lines.height() - 1)
        y2 = lines.height() - 2;

    float antifraction_each = 1.0 - percent / 100.0f;
    float antifraction_current = 1.0;
    int percent_for_neighbours[9];
    for (int i = 0; i < 9; i++)
    {
        percent_for_neighbours[i] = 100 - (int)(antifraction_current * 100);
        antifraction_current *= antifraction_each;
    }

    // We do not replace this as we go to avoid favouring some directions.
    vector<coord_def> coord_to_replace;

    for (int y = y1; y <= y2; ++y)
        for (int x = x1; x <= x2; ++x)
            if (strchr(find, lines(x, y)))
            {
                int neighbour_count = 0;
                for (radius_iterator ri(coord_def(x, y), 1,
                                        (boxy ? C_SQUARE : C_POINTY),
                                        NULL, true);                 ri; ++ri)
                {
                    if (_valid_coord(ls, lines, ri->x, ri->y, false))
                        if (strchr(passable, lines(*ri)))
                            neighbour_count++;
                }

                // store this coordinate if needed
                if (x_chance_in_y(percent_for_neighbours[neighbour_count], 100))
                    coord_to_replace.push_back(coord_def(x, y));
            }

    // now go through and actually replace the positions
    for (unsigned int i = 0; i < coord_to_replace.size(); i++)
        lines(coord_to_replace[i]) = replace;

    return 0;
}

LUAFN(dgn_connect_adjacent_rooms)
{
    LINES(ls, 1, lines);

    TABLE_STR(ls, wall, "x");
    TABLE_STR(ls, floor, ".");
    TABLE_CHAR(ls, replace, '.');
    TABLE_INT(ls, max, 1);
    TABLE_INT(ls, min, max);
    TABLE_BOOL(ls, check_empty, false);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    // we never go right up to the border to avoid looking off the map edge
    if (x1 < 1)
        x1 = 1;
    if (x2 >= lines.width() - 1)
        x2 = lines.width() - 2;
    if (y1 < 1)
        y1 = 1;
    if (y2 >= lines.height() - 1)
        y2 = lines.height() - 2;

    if (min < 0)
        return luaL_error(ls, "Invalid min connections: %i", min);
    if (max < min)
    {
        return luaL_error(ls, "Invalid max connections: %i (min is %i)",
                          max, min);
    }

    int count = min + random2(max - min + 1);
    for (random_rectangle_iterator ri(coord_def(x1, y1),
                                      coord_def(x2, y2)); ri; ++ri)
    {
        if (count <= 0)
        {
            // stop when have checked enough spots
            return 0;
        }

        int x = ri->x;
        int y = ri->y;

        if (strchr(wall, lines(*ri)))
        {
            if (strchr(floor, lines(x, y - 1))
                && strchr(floor, lines(x, y + 1))
                && (check_empty ? _wall_is_empty(lines, x, y, wall, floor, true)
                   : (strchr(wall, lines(x - 1, y))
                      && strchr(wall, lines(x + 1, y)))))
            {
                lines(*ri) = replace;
            }
            else if (strchr(floor, lines(x - 1, y))
                     && strchr(floor, lines(x + 1, y))
                     && (check_empty ? _wall_is_empty(lines, x, y, wall, floor, false)
                        : (strchr(wall, lines(x, y - 1))
                           && strchr(wall, lines(x, y + 1)))))
            {
                lines(*ri) = replace;
            }
        }
        count--;
    }

    return 0;
}

LUAFN(dgn_replace_area)
{
    LINES(ls, 1, lines);

    TABLE_STR(ls, find, 0);
    TABLE_CHAR(ls, replace, '\0');

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    for (int y = y1; y <= y2; ++y)
        for (int x = x1; x <= x2; ++x)
            if (strchr(find, lines(x, y)))
                lines(x, y) = replace;

    return 0;
}

LUAFN(dgn_replace_first)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x, 0);
    TABLE_INT(ls, y, 0);
    TABLE_INT(ls, xdir, 2);
    TABLE_INT(ls, ydir, 2);
    TABLE_CHAR(ls, find, '\0');
    TABLE_CHAR(ls, replace, '\0');
    TABLE_BOOL(ls, required, false);

    if (!_valid_coord(ls, lines, x, y))
        return 0;

    if (xdir < -1 || xdir > 1)
        return luaL_error(ls, "Invalid xdir: %d", xdir);

    if (ydir < -1 || ydir > 1)
        return luaL_error(ls, "Invalid ydir: %d", ydir);

    while (lines.in_bounds(coord_def(x, y)))
    {
        if (lines(x, y) == find)
        {
            lines(x, y) = replace;
            return 0;
        }

        x += xdir;
        y += ydir;
    }

    if (required)
        return luaL_error(ls, "Could not find feature '%c' to replace", find);

    return 0;
}

LUAFN(dgn_replace_random)
{
    LINES(ls, 1, lines);

    TABLE_CHAR(ls, find, '\0');
    TABLE_CHAR(ls, replace, '\0');
    TABLE_BOOL(ls, required, false);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    int count = (x2 - x1 + 1) * (y2 - y1 + 1);
    if (!count)
    {
        if (required)
            luaL_error(ls, "%s", "No elements to replace");
        return 0;
    }

    vector<coord_def> loc;
    loc.reserve(count);

    for (int y = y1; y <= y2; ++y)
        for (int x = x1; x <= x2; ++x)
            if (lines(x, y) == find)
                loc.push_back(coord_def(x, y));

    if (loc.empty())
    {
        if (required)
            return luaL_error(ls, "Could not find '%c'", find);
    }

    int idx = random2(loc.size());
    lines(loc[idx]) = replace;

    return 0;
}

LUAFN(dgn_replace_closest)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, x, 0);
    TABLE_INT(ls, y, 0);
    TABLE_CHAR(ls, find, '\0');
    TABLE_CHAR(ls, replace, '\0');
    TABLE_BOOL(ls, required, false);

    coord_def center(x, y);

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    int count = (x2 - x1 + 1) * (y2 - y1 + 1);
    if (!count)
    {
        if (required)
            luaL_error(ls, "%s", "No elements to replace");
        return 0;
    }

    vector<coord_def> loc;
    loc.reserve(count);

    int best_distance = 10000;
    unsigned int best_count = 0;
    coord_def best_coord;

    for (int this_y = y1; this_y <= y2; ++this_y)
        for (int this_x = x1; this_x <= x2; ++this_x)
            if (lines(this_x, this_y) == find)
            {
                coord_def here(this_x, this_y);
                int distance = here.distance_from(center);
                if (distance < best_distance)
                {
                    best_distance = distance;
                    best_count = 1;
                    best_coord = here;
                }
                else if (distance == best_distance)
                {
                    best_count++;
                    if (one_chance_in(best_count))
                        best_coord = here;
                }
            }

    if (best_count == 0)
    {
        if (required)
            return luaL_error(ls, "Could not find '%c'", find);
        return 0;
    }

    lines(best_coord) = replace;

    return 0;
}

LUAFN(dgn_smear_map)
{
    LINES(ls, 1, lines);

    TABLE_INT(ls, iterations, 1);
    TABLE_CHAR(ls, smear, 'x');
    TABLE_STR(ls, onto, ".");
    TABLE_BOOL(ls, boxy, false);

    const int border = 1;
    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2, border))
        return 0;

    const int max_test_per_iteration = 10;
    int sanity = 0;
    int max_sanity = iterations * max_test_per_iteration;

    for (int i = 0; i < iterations; i++)
    {
        bool diagonals, straights;
        coord_def mc;

        do
        {
            do
            {
                if (sanity++ > max_sanity)
                    return 0;

                mc.x = random_range(x1+border, y2-border);
                mc.y = random_range(y1+border, y2-border);
            }
            while (onto[0] && !strchr(onto, lines(mc)));

            // Is there a "smear" feature along the diagonal from mc?
            diagonals = lines(mc.x + 1, mc.y + 1) == smear
                        || lines(mc.x - 1, mc.y + 1) == smear
                        || lines(mc.x - 1, mc.y - 1) == smear
                        || lines(mc.x + 1, mc.y - 1) == smear;

            // Is there a "smear" feature up, down, left, or right from mc?
            straights = lines(mc.x + 1, mc.y) == smear
                        || lines(mc.x - 1, mc.y) == smear
                        || lines(mc.x, mc.y + 1) == smear
                        || lines(mc.x, mc.y - 1) == smear;
        }
        while (!straights && (boxy || !diagonals));

        lines(mc) = smear;
    }

    return 0;
}

LUAFN(dgn_spotty_map)
{
    LINES(ls, 1, lines);

    TABLE_STR(ls, replace, "x");
    TABLE_CHAR(ls, fill, '.');
    TABLE_BOOL(ls, boxy, true);
    TABLE_INT(ls, iterations, random2(boxy ? 750 : 1500));

    const int border = 4;
    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2, border))
        return 0;

    const int max_test_per_iteration = 10;
    int sanity = 0;
    int max_sanity = iterations * max_test_per_iteration;

    for (int i = 0; i < iterations; i++)
    {
        int x, y;
        do
        {
            if (sanity++ > max_sanity)
                return 0;

            x = random_range(x1 + border, x2 - border);
            y = random_range(y1 + border, y2 - border);
        }
        while (strchr(replace, lines(x, y))
               && strchr(replace, lines(x-1, y))
               && strchr(replace, lines(x+1, y))
               && strchr(replace, lines(x, y-1))
               && strchr(replace, lines(x, y+1))
               && strchr(replace, lines(x-2, y))
               && strchr(replace, lines(x+2, y))
               && strchr(replace, lines(x, y-2))
               && strchr(replace, lines(x, y+2)));

        for (radius_iterator ai(coord_def(x, y), boxy ? 2 : 1, C_CIRCLE,
                                NULL, false); ai; ++ai)
        {
            if (strchr(replace, lines(*ai)))
                lines(*ai) = fill;
        }
    }

    return 0;
}

LUAFN(dgn_add_pools)
{
    LINES(ls, 1, lines);

    TABLE_STR(ls, replace, ".");
    TABLE_CHAR(ls, border, '.');
    TABLE_INT(ls, pool_size, 100);
    TABLE_INT(ls, seed_seperation, 2);

    vector<char> fill_glyphs = _pool_fill_glyphs_from_table(ls, "contents");

    int x1, y1, x2, y2;
    if (!_coords(ls, lines, x1, y1, x2, y2))
        return 0;

    int size_x = x2 - x1 + 1;
    int size_y = y2 - y1 + 1;

    //
    //  The basic ideas here is that we place a number of
    //    pool "seeds" on the map and spread them outward
    //    randomly until they run into each other.  We never
    //    fill in the last cell, so they never actually
    //    touch and we get paths between them.
    //
    //  The algorithm we use to spread the pools is like a
    //    depth-/breadth-first search, except that:
    //      1. We choose a random element from the open list
    //      2. We have multiple starting locations, each
    //         with its own "closed" value
    //      3. We mark all cells bordered by 2 (or more)
    //         distinct non-BORDER closed values with special
    //         closed value BORDER
    //
    //  In the end, we used the "closed" values to determine
    //    which pool we are in.  The BORDER value indicates
    //    a path between pools.
    //

    // NO_POOL
    //  -> must be lowest constant
    //  -> must match _get_pool_seed_positions
    const int NO_POOL   = 999997;
    const int IN_LIST   = 999998;
    const int BORDER    = 999999;
    const int FORBIDDEN = 1000000;

    // Step 1: Make a 2D array to store which pool each cell is part of
    //    -> We use nested vectors to store this.  We cannot use
    //       a fixedarray because we don't know the size at
    //       compile time.

    vector<vector<int> > pool_index(size_x, vector<int>(size_y, FORBIDDEN));
    for (int x = 0; x < size_x; x++)
        for (int y = 0; y < size_y; y++)
        {
            if (strchr(replace, lines(x + x1, y + y1)))
                pool_index[x][y] = NO_POOL;
        }

    // Step 2: Place the pool seeds and add their neighbours to the open list

    vector<coord_def> pool_seeds = _get_pool_seed_positions(pool_index,
                                                            pool_size,
                                                            seed_seperation);
    vector<coord_def> open_list;

    for (unsigned int pool = 0; pool < pool_seeds.size(); pool++)
    {
        int x = pool_seeds[pool].x;
        int y = pool_seeds[pool].y;

        pool_index[x][y] = pool;

        // add neighbours to open list
        for (orth_adjacent_iterator ai(pool_seeds[pool]); ai; ++ai)
            if (_valid_coord(ls, lines, ai->x, ai->y, false))
            {
                pool_index[ai->x][ai->y] = IN_LIST;
                open_list.push_back(*ai);
            }
    }

    // Step 3: Spread out pools as far as possible

    while (!open_list.empty())
    {
        // remove a random position from the open list
        int index_chosen = random2(open_list.size());
        coord_def chosen_coord = open_list[index_chosen];
        open_list[index_chosen] = open_list.back();
        open_list.pop_back();

        // choose which neighbouring pool to join
        int chosen_pool = NO_POOL;
        for (adjacent_iterator ai(chosen_coord); ai; ++ai)
            if (_valid_coord(ls, lines, ai->x, ai->y, false))
            {
                int neighbour_pool = pool_index[ai->x][ai->y];
                if (neighbour_pool < NO_POOL)
                {
                    // this is a valid pool, consider it
                    if (chosen_pool == NO_POOL)
                        chosen_pool = neighbour_pool;
                    else if (chosen_pool == neighbour_pool)
                        ; // already correct
                    else
                    {
                        // this is the path between pools
                        chosen_pool = BORDER;
                        break;
                    }
                }
                else if (neighbour_pool == FORBIDDEN)
                {
                    // next to a wall
                    chosen_pool = BORDER;
                    break;
                }
            }

        if (chosen_pool != NO_POOL)
        {
            // add this cell to the appropriate pool
            pool_index[chosen_coord.x][chosen_coord.y] = chosen_pool;

            // add neighbours to open list
            for (orth_adjacent_iterator ai(chosen_coord); ai; ++ai)
                if (_valid_coord(ls, lines, ai->x, ai->y, false)
                    && pool_index[ai->x][ai->y] == NO_POOL)
                {
                    pool_index[ai->x][ai->y] = IN_LIST;
                    open_list.push_back(*ai);
                }
        }
        else
        {
            // a default, although I do not know why we ever get here
            pool_index[chosen_coord.x][chosen_coord.y] = NO_POOL;
        }
    }

    // Step 4: Add the pools to the map

    vector<char> pool_glyphs(pool_seeds.size(), '\0');
    for (unsigned int i = 0; i < pool_glyphs.size(); i++)
        pool_glyphs[i] = fill_glyphs[random2(fill_glyphs.size())];

    for (int x = 0; x < size_x; x++)
        for (int y = 0; y < size_y; y++)
            {
            int index = pool_index[x][y];
            if (index < (int)(pool_glyphs.size()))
                lines(x + x1, y + y1) = pool_glyphs[index];
            else if (index == NO_POOL || index == BORDER)
                lines(x + x1, y + y1) = border;
            else if (index == FORBIDDEN)
                ; // leave it alone
            else
            {
                return luaL_error(ls, "Invalid pool index %i/%i at (%i, %i)",
                                  index, pool_glyphs.size(), x + x1, y + y1);
            }
        }

    return 0;
}

static int dgn_width(lua_State *ls)
{
    LINES(ls, 1, lines);
    PLUARET(number, lines.width());
}

LUAFN(dgn_delve)
{
    LINES(ls, 1, lines);

    ARG_INT(ls, 2, ngb_min, 2);
    ARG_INT(ls, 3, ngb_max, 3);
    ARG_INT(ls, 4, connchance, 0);
    ARG_INT(ls, 5, cellnum, -1);
    ARG_INT(ls, 6, top, 125);

    delve(&lines, ngb_min, ngb_max, connchance, cellnum, top);
    return 0;
}

LUAFN(dgn_farthest_from)
{
    LINES(ls, 1, lines);
    const char *beacons = luaL_checkstring(ls, 2);

    ASSERT(lines.width() <= GXM);
    ASSERT(lines.height() <= GYM);
    FixedArray<bool, GXM, GYM> visited;
    visited.init(false);
    vector<coord_def> queue;
    unsigned int dc_prev = 0, dc_next; // indices where dist changes to the next value

    for (int x = lines.width(); x >= 0; x--)
        for (int y = lines.height(); y >= 0; y--)
        {
            coord_def c(x, y);
            if (lines.in_map(c) && strchr(beacons, lines(c)))
            {
                queue.push_back(c);
                visited(c) = true;
            }
        }

    dc_next = queue.size();
    if (!dc_next)
    {
        // Not a single beacon, nowhere to go.
        lua_pushnil(ls);
        lua_pushnil(ls);
        return 2;
    }

    for (unsigned int dc = 0; dc < queue.size(); dc++)
    {
        if (dc >= dc_next)
        {
            dc_prev = dc_next;
            dc_next = dc;
        }

        coord_def c = queue[dc];
        for (adjacent_iterator ai(c); ai; ++ai)
            if (lines.in_map(*ai) && !visited(*ai)
                && strchr(traversable_glyphs, lines(*ai)))
            {
                queue.push_back(*ai);
                visited(*ai) = true;
            }
    }

    ASSERT(dc_next > dc_prev);
    // There may be multiple farthest cells, pick one at random.
    coord_def loc = queue[dc_prev + random2(dc_next - dc_prev)];
    lua_pushnumber(ls, loc.x);
    lua_pushnumber(ls, loc.y);
    return 2;
}

/* Wrappers for C++ layouts, to facilitate choosing of layouts by weight and
 * depth */

LUAFN(dgn_layout_basic)
{
    dgn_build_basic_level();
    return 0;
}

LUAFN(dgn_layout_bigger_room)
{
    dgn_build_bigger_room_level();
    return 0;
}

LUAFN(dgn_layout_chaotic_city)
{
    const dungeon_feature_type feature = check_lua_feature(ls, 2, true);
    dgn_build_chaotic_city_level(feature == DNGN_UNSEEN ? NUM_FEATURES : feature);
    return 0;
}

LUAFN(dgn_layout_shoals)
{
    dgn_build_shoals_level();
    return 0;
}

LUAFN(dgn_layout_swamp)
{
    dgn_build_swamp_level();
    return 0;
}

const struct luaL_reg dgn_build_dlib[] =
{
    { "count_feature_in_box", &dgn_count_feature_in_box },
    { "count_antifeature_in_box", &dgn_count_antifeature_in_box },
    { "count_neighbors", &dgn_count_neighbors },
    { "count_passable_neighbors", &dgn_count_passable_neighbors },
    { "is_valid_coord", &dgn_is_valid_coord },
    { "is_passable_coord", &dgn_is_passable_coord },
    { "extend_map", &dgn_extend_map },
    { "fill_area", &dgn_fill_area },
    { "fill_disconnected", &dgn_fill_disconnected },
    { "find_in_area", &dgn_find_in_area },
    { "height", dgn_height },
    { "join_the_dots", &dgn_join_the_dots },
    { "make_circle", &dgn_make_circle },
    { "make_diamond", &dgn_make_diamond },
    { "make_rounded_square", &dgn_make_rounded_square },
    { "make_square", &dgn_make_square },
    { "make_box", &dgn_make_box },
    { "make_box_doors", &dgn_make_box_doors },
    { "make_irregular_box", &dgn_make_irregular_box },
    { "mapgrd_table", dgn_mapgrd_table },
    { "octa_room", &dgn_octa_room },
    { "remove_isolated_glyphs", &dgn_remove_isolated_glyphs },
    { "widen_paths", &dgn_widen_paths },
    { "connect_adjacent_rooms", &dgn_connect_adjacent_rooms },
    { "replace_area", &dgn_replace_area },
    { "replace_first", &dgn_replace_first },
    { "replace_random", &dgn_replace_random },
    { "replace_closest", &dgn_replace_closest },
    { "smear_map", &dgn_smear_map },
    { "spotty_map", &dgn_spotty_map },
    { "add_pools", &dgn_add_pools },
    { "delve", &dgn_delve },
    { "width", dgn_width },
    { "farthest_from", &dgn_farthest_from },

    { "layout_basic", &dgn_layout_basic },
    { "layout_bigger_room", &dgn_layout_bigger_room },
    { "layout_chaotic_city", &dgn_layout_chaotic_city },
    { "layout_shoals", &dgn_layout_shoals },
    { "layout_swamp", &dgn_layout_swamp },

    { NULL, NULL }
};

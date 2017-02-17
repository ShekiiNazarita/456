#pragma once

enum tile_inventory_flags
{
    TILEI_FLAG_SELECT  = 0x0100,
                       //0x0200, // was: TILEI_FLAG_TRIED
    TILEI_FLAG_EQUIP   = 0x0400,
    TILEI_FLAG_FLOOR   = 0x0800,
    TILEI_FLAG_CURSE   = 0x1000,
    TILEI_FLAG_CURSOR  = 0x2000,
    TILEI_FLAG_MELDED  = 0x4000,
    TILEI_FLAG_INVALID = 0x8000,
};

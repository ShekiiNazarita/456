#ifndef TAG_VERSION_H
#define TAG_VERSION_H

// Let CDO updaters know if the syntax changes.
#define TAG_MAJOR_VERSION  31

// Minor version will be reset to zero when major version changes.
enum tag_minor_version
{
    TAG_MINOR_RESET          = 0, // Minor tags were reset
    TAG_MINOR_DIAG_COUNTERS  = 1, // Counters for diag/ortho moves.
    TAG_MINOR_FISHTAIL       = 2, // Merfolk's tail state.
    TAG_MINOR_DENSITY        = 3, // Count of level's explorable area.
    TAG_MINOR_MALIGN         = 4, // Keep malign gateway markers around for longer.
    TAG_MINOR_GOD_GIFTS      = 5, // Track current as well as total god gifts.
    TAG_MINOR_ATT_SWAP       = 6, // Sort attitude enums by friendliness.
    TAG_MINOR_GRATE          = 7, // Iron grates.
    TAG_MINOR_XP_STEALING    = 8, // Block kill sniping.
    TAG_MINOR_EARTH_ATTUNE   = 9, // [G]nomes earth attunement.
    TAG_MINOR_CROSSTRAIN     = 10, // Track skill points gained with crosstraining.
    TAG_MINOR_ADD_ID_INFO    = 11, // Additional identification info
    TAG_MINOR_SLOW_RESKILL   = 12, // Ashenzari progressive reskilling
    TAG_MINOR_RUNE_SUBST     = 13, // Removal of stored rune_subst.
    TAG_MINOR_NO_CSTRINGS    = 14, // No const-length strings in saves.
    TAG_MINOR_YOU_FORM       = 15, // Give transformations a field on their own.
    TAG_MINOR_NEMELEX_TOGGLE = 16, // Store Nemelex' sacrificing toggle values.
    TAG_MINOR_ZOTDEF_NAME    = 17, // ZotDef wave name.
    TAG_MINOR_LAYOUT_TYPES   = 18, // Layout type turned into a string_set.
    TAG_MINOR_RANGE_CHANCES  = 19, // Map depths and chances are ranges.
    TAG_MINOR_MONSTER_ID     = 20, // Monsters get globally unique serial ids.
    TAG_MINOR_VERSION        = 20, // Current version.  (Keep equal to max.)
};

#endif

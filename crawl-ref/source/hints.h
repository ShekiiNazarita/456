/*
 *  File:       hints.h
 *  Summary:    Stuff needed for hints mode
 *  Written by: j-p-e-g
 *
 *  Created on 2007-01-11.
 */

#ifndef HINTS_H
#define HINTS_H

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "externs.h"
#include "mon-info.h"

class formatted_string;
class writer;
class reader;

enum hints_types
{
    HINT_BERSERK_CHAR,
    HINT_MAGIC_CHAR,
    HINT_RANGER_CHAR,
    HINT_TYPES_NUM   // 3
};

void save_hints(writer& outf);
void load_hints(reader& inf);
void init_hints_options(void);

enum hints_event_type
{
    HINT_SEEN_FIRST_OBJECT,
    // seen certain items
    HINT_SEEN_POTION,
    HINT_SEEN_SCROLL,
    HINT_SEEN_WAND,
    HINT_SEEN_SPBOOK,
    HINT_SEEN_JEWELLERY,
    HINT_SEEN_MISC,
    HINT_SEEN_STAFF,
    HINT_SEEN_WEAPON,
    HINT_SEEN_MISSILES,
    HINT_SEEN_ARMOUR,
    HINT_SEEN_RANDART,
    HINT_SEEN_FOOD,
    HINT_SEEN_CARRION,
    HINT_SEEN_GOLD,
    // encountered dungeon features
    HINT_SEEN_STAIRS,
    HINT_SEEN_ESCAPE_HATCH,
    HINT_SEEN_BRANCH,
    HINT_SEEN_PORTAL,
    HINT_SEEN_TRAP,
    HINT_SEEN_ALTAR,
    HINT_SEEN_SHOP,
    HINT_SEEN_DOOR,
    HINT_FOUND_SECRET_DOOR,
    // other 'first events'
    HINT_SEEN_MONSTER,
    HINT_SEEN_ZERO_EXP_MON,
    HINT_SEEN_TOADSTOOL,
    HINT_MONSTER_BRAND,
    HINT_MONSTER_FRIENDLY,
    HINT_MONSTER_SHOUT,
    HINT_MONSTER_LEFT_LOS,
    HINT_KILLED_MONSTER,
    HINT_NEW_LEVEL,
    HINT_SKILL_RAISE,
    HINT_GAINED_MAGICAL_SKILL,
    HINT_GAINED_MELEE_SKILL,
    HINT_GAINED_RANGED_SKILL,
    HINT_CHOOSE_STAT,
    HINT_MAKE_CHUNKS,
    HINT_OFFER_CORPSE,
    HINT_NEW_ABILITY_GOD,
    HINT_NEW_ABILITY_MUT,
    HINT_NEW_ABILITY_ITEM,
    HINT_FLEEING_MONSTER,
    HINT_ROTTEN_FOOD,
    HINT_CONVERT,
    HINT_GOD_DISPLEASED,
    HINT_EXCOMMUNICATE,
    HINT_SPELL_MISCAST,
    HINT_SPELL_HUNGER,
    HINT_GLOWING,
    HINT_YOU_RESIST,
    // status changes
    HINT_YOU_ENCHANTED,
    HINT_YOU_SICK,
    HINT_YOU_POISON,
    HINT_YOU_ROTTING,
    HINT_YOU_CURSED,
    HINT_YOU_HUNGRY,
    HINT_YOU_STARVING,
    HINT_YOU_MUTATED,
    HINT_CAN_BERSERK,
    HINT_POSTBERSERK,
    HINT_CAUGHT_IN_NET,
    HINT_YOU_SILENCE,
    // warning
    HINT_RUN_AWAY,
    HINT_RETREAT_CASTER,
    HINT_WIELD_WEAPON,
    HINT_NEED_HEALING,
    HINT_NEED_POISON_HEALING,
    HINT_INVISIBLE_DANGER,
    HINT_NEED_HEALING_INVIS,
    HINT_ABYSS,
    // interface
    HINT_MULTI_PICKUP,
    HINT_HEAVY_LOAD,
    HINT_SHIFT_RUN,
    HINT_MAP_VIEW,
    HINT_AUTO_EXPLORE,
    HINT_DONE_EXPLORE,
    HINT_AUTO_EXCLUSION,
    HINT_STAIR_BRAND,
    HINT_HEAP_BRAND,
    HINT_TRAP_BRAND,
    HINT_LOAD_SAVED_GAME,
    HINT_EVENTS_NUM
};

struct newgame_def;
void init_hints();
void pick_hints(newgame_def* choice);
void hints_load_game(void);
void print_hints_menu(unsigned int type);
void hints_zap_secret_doors(void);

formatted_string hints_starting_info2();
void hints_starting_screen(void);
void hints_new_turn();
void hints_death_screen(void);
void hints_finished(void);

void hints_dissection_reminder(bool healthy);
void hints_healing_reminder(void);

void taken_new_item(object_class_type item_type);
void hints_gained_new_skill(skill_type skill);
void hints_monster_seen(const monster& mon);
void hints_first_item(const item_def& item);
void learned_something_new(hints_event_type seen_what,
                           coord_def gc = coord_def());
formatted_string hints_abilities_info(void);
void print_hints_skills_info(void);
void print_hints_skills_description_info(void);

// Additional information for tutorial players.
void hints_describe_item(const item_def &item);
void hints_inscription_info(bool autoinscribe, std::string prompt);
bool hints_pos_interesting(int x, int y);
void hints_describe_pos(int x, int y);
bool hints_monster_interesting(const monster* mons);
void hints_describe_monster(const monster_info& mi, bool has_stat_desc);

void hints_observe_cell(const coord_def& gc);

struct hints_state
{
    FixedVector<bool, HINT_EVENTS_NUM> hints_events;
    bool hints_explored;
    bool hints_stashes;
    bool hints_travel;
    unsigned int hints_spell_counter;
    unsigned int hints_throw_counter;
    unsigned int hints_berserk_counter;
    unsigned int hints_melee_counter;
    unsigned int hints_last_healed;
    unsigned int hints_seen_invisible;

    bool hints_just_triggered;
    unsigned int hints_type;
    unsigned int hints_left;
};

extern hints_state Hints;

#endif

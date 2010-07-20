/*
 *  File:       state.h
 *  Summary:    Game state.
 *  Written by: Linley Henzell
 */

#ifndef STATE_H
#define STATE_H

#include "player.h"
#include <vector>

class monsters;
class mon_acting;

struct god_act_state
{
public:

    god_act_state();
    void reset();

    god_type which_god;
    bool     retribution;
    int      depth;
};

// Track various aspects of Crawl game state.
struct game_state
{
    std::vector<std::string> command_line_arguments;

    bool game_crashed;      // The game crashed and is now in the process of
                            // dumping crash info.

    // An assert was triggered, but an emergency save here should be possible
    // without corruption.
    bool game_wants_emergency_save;

    bool mouse_enabled;     // True if mouse input is currently relevant.

    bool waiting_for_command; // True when the game is waiting for a command.
    bool terminal_resized;   // True if the term was resized and we need to
                             // take action to handle it.

    bool io_inited;         // Is curses or the equivalent initialised?
    bool need_save;         // Set to true when game has started.
    bool saving_game;       // Set to true while in save_game.
    bool updating_scores;   // Set to true while updating hiscores.

    int  seen_hups;         // Set to true if SIGHUP received.

    bool map_stat_gen;      // Set if we're generating stats on maps.

    game_type type;
    bool arena_suspended;   // Set if the arena has been temporarily
                            // suspended.

    bool test;              // Set if we want to run self-tests and exit.
    bool script;            // Set if we want to run a Lua script and exit.
    bool build_db;          // Set if we want to rebuild the db and exit.
    std::vector<std::string> tests_selected; // Tests to be run.
    std::vector<std::string> script_args;    // Arguments to scripts.

    bool unicode_ok;        // Is unicode support available?

    bool show_more_prompt;  // Set to false to disable --more-- prompts.

    std::string sprint_map; // Sprint map set on command line, if any.

    std::string (*glyph2strfn)(unsigned glyph);
    int  (*multibyte_strlen)(const std::string &s);
    void (*terminal_resize_handler)();
    void (*terminal_resize_check)();

    bool            doing_prev_cmd_again;
    command_type    prev_cmd;
    std::deque<int> prev_cmd_keys;

    command_type    repeat_cmd;
    bool            cmd_repeat_start;
    command_type    prev_repeat_cmd;
    int             prev_cmd_repeat_goal;
    bool            cmd_repeat_started_unsafe;

    std::vector<std::string> startup_errors;

    bool level_annotation_shown;

#ifndef USE_TILE
    // Are we currently targeting using the mlist?
    // This is global because the monster pane uses this when
    // drawing.
    bool mlist_targeting;
#endif

    // Range beyond which view should be darkend, -1 == disabled.
    int darken_range;

    // Any changes to macros that need to be changed?
    bool unsaved_macros;

protected:
    void reset_cmd_repeat();
    void reset_cmd_again();

    god_act_state              god_act;
    std::vector<god_act_state> god_act_stack;

    monsters*              mon_act;
    std::vector<monsters*> mon_act_stack;

public:
    game_state();

    void add_startup_error(const std::string &error);
    void show_startup_errors();

    bool is_replaying_keys() const;

    bool is_repeating_cmd() const;

    void cancel_cmd_repeat(std::string reason = "");
    void cancel_cmd_again(std::string reason = "");
    void cancel_cmd_all(std::string reason = "");

    void cant_cmd_repeat(std::string reason = "");
    void cant_cmd_again(std::string reason = "");
    void cant_cmd_any(std::string reason = "");

    void zero_turns_taken();

    void check_term_size() const
    {
        if (terminal_resize_check)
            (*terminal_resize_check)();
    }

    bool     is_god_acting() const;
    bool     is_god_retribution() const;
    god_type which_god_acting() const;
    void     inc_god_acting(bool is_retribution = false);
    void     inc_god_acting(god_type which_god, bool is_retribution = false);
    void     dec_god_acting();
    void     dec_god_acting(god_type which_god);
    void     clear_god_acting();

    std::vector<god_act_state> other_gods_acting() const;

    bool      is_mon_acting() const;
    monsters* which_mon_acting() const;
    void      inc_mon_acting(monsters* mon);
    void      dec_mon_acting(monsters* mon);
    void      clear_mon_acting();
    void      mon_gone(monsters* mon);

    void dump();
    bool player_is_dead() const;

    bool game_is_normal() const;
    bool game_is_tutorial() const;
    bool game_is_arena() const;
    bool game_is_sprint() const;
    bool game_is_hints() const;

    // Save subdirectory used for games such as Sprint.
    std::string game_type_name() const;
    std::string game_savedir_path() const;
    std::string game_type_qualifier() const;

    static std::string game_type_name_for(game_type gt);

    friend class mon_acting;
};

#ifdef DEBUG_GLOBALS
#define crawl_state (*real_crawl_state)
#endif
extern game_state crawl_state;

class god_acting
{
public:
    god_acting(bool is_retribution = false)
        : god(you.religion)
    {
        crawl_state.inc_god_acting(god, is_retribution);
    }
    god_acting(god_type who, bool is_retribution = false)
        : god(who)
    {
        crawl_state.inc_god_acting(god, is_retribution);
    }
    ~god_acting()
    {
        crawl_state.dec_god_acting(god);
    }
private:
    god_type god;
};

class mon_acting
{
public:
    mon_acting(monsters* _mon) : mon(_mon)
    {
        crawl_state.inc_mon_acting(_mon);
    }

    ~mon_acting()
    {
        // Monster might have died in the meantime.
        if (mon == crawl_state.mon_act)
            crawl_state.dec_mon_acting(mon);
    }

private:
    monsters *mon;
};


#endif

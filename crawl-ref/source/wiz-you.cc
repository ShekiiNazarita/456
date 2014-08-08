/**
 * @file
 * @brief Player related debugging functions.
**/

#include "AppHdr.h"

#include "wiz-you.h"

#include <algorithm>
#include <functional>

#include "abyss.h"

#include "chardump.h"
#include "cio.h"
#include "dbg-util.h"
#include "food.h"
#include "godabil.h"
#include "godprayer.h"
#include "godwrath.h"
#include "libutil.h"
#include "macro.h"
#include "message.h"
#include "mutation.h"
#include "newgame.h"
#include "ng-setup.h"
#include "output.h"
#include "player.h"
#include "prompt.h"
#include "religion.h"
#include "skills.h"
#include "skills2.h"
#include "spl-book.h"
#include "spl-cast.h"
#include "spl-util.h"
#include "state.h"
#include "status.h"
#include "strings.h"
#include "terrain.h"
#include "transform.h"
#include "view.h"
#include "unicode.h"
#include "xom.h"

#ifdef WIZARD
static void _swap_equip(equipment_type a, equipment_type b)
{
    swap(you.equip[a], you.equip[b]);
    bool tmp = you.melded[a];
    you.melded.set(a, you.melded[b]);
    you.melded.set(b, tmp);
}

static species_type _find_species(string species)
{
    string spec = lowercase_string(species);

    species_type sp = SP_UNKNOWN;

    for (int i = 0; i < NUM_SPECIES; ++i)
    {
        const species_type si = static_cast<species_type>(i);
        const string sp_name = lowercase_string(species_name(si));

        string::size_type pos = sp_name.find(spec);
        if (pos != string::npos)
        {
            if (pos == 0)
            {
                // We prefer prefixes over partial matches.
                sp = si;
                break;
            }
            else
                sp = si;
        }
    }

    return sp;
}

static void _wizard_change_species_to(species_type sp)
{
    // Can't use magic cookies or placeholder species.
    if (!is_valid_species(sp))
    {
        mpr("That species isn't available.");
        return;
    }

    // Re-scale skill-points.
    for (int i = SK_FIRST_SKILL; i < NUM_SKILLS; ++i)
    {
        skill_type sk = static_cast<skill_type>(i);
        you.skill_points[i] *= species_apt_factor(sk, sp)
                               / species_apt_factor(sk);
    }

    species_type old_sp = you.species;
    you.species = sp;
    you.is_undead = get_undead_state(sp);

    // Change permanent mutations, but preserve non-permanent ones.
    uint8_t prev_muts[NUM_MUTATIONS];
    for (int i = 0; i < NUM_MUTATIONS; ++i)
    {
        if (you.innate_mutation[i] > 0)
        {
            if (you.innate_mutation[i] > you.mutation[i])
                you.mutation[i] = 0;
            else
                you.mutation[i] -= you.innate_mutation[i];

            you.innate_mutation[i] = 0;
        }
        prev_muts[i] = you.mutation[i];
    }
    give_basic_mutations(sp);
    for (int i = 0; i < NUM_MUTATIONS; ++i)
    {
        if (prev_muts[i] > you.innate_mutation[i])
            you.innate_mutation[i] = 0;
        else
            you.innate_mutation[i] -= prev_muts[i];
    }

    switch (sp)
    {
    case SP_RED_DRACONIAN:
        if (you.experience_level >= 7)
            perma_mutate(MUT_HEAT_RESISTANCE, 1, "wizard race change");
        break;

    case SP_WHITE_DRACONIAN:
        if (you.experience_level >= 7)
            perma_mutate(MUT_COLD_RESISTANCE, 1, "wizard race change");
        break;

    case SP_GREEN_DRACONIAN:
        if (you.experience_level >= 7)
            perma_mutate(MUT_POISON_RESISTANCE, 1, "wizard race change");
        if (you.experience_level >= 14)
            perma_mutate(MUT_STINGER, 1, "wizard race change");
        break;

    case SP_YELLOW_DRACONIAN:
        if (you.experience_level >= 14)
            perma_mutate(MUT_ACIDIC_BITE, 1, "wizard race change");
        break;

    case SP_GREY_DRACONIAN:
        if (you.experience_level >= 7)
            perma_mutate(MUT_UNBREATHING, 1, "wizard race change");
        break;

    case SP_BLACK_DRACONIAN:
        if (you.experience_level >= 7)
            perma_mutate(MUT_SHOCK_RESISTANCE, 1, "wizard race change");
        if (you.experience_level >= 14)
            perma_mutate(MUT_BIG_WINGS, 1, "wizard race change");
        break;

    case SP_DEMONSPAWN:
    {
        roll_demonspawn_mutations();
        for (int i = 0; i < int(you.demonic_traits.size()); ++i)
        {
            mutation_type m = you.demonic_traits[i].mutation;

            if (you.demonic_traits[i].level_gained > you.experience_level)
                continue;

            ++you.mutation[m];
            ++you.innate_mutation[m];
        }
        break;
    }

    case SP_DEEP_DWARF:
        if (you.experience_level >= 9)
            perma_mutate(MUT_PASSIVE_MAPPING, 1, "wizard race change");
        if (you.experience_level >= 14)
            perma_mutate(MUT_NEGATIVE_ENERGY_RESISTANCE, 1, "wizard race change");
        if (you.experience_level >= 18)
            perma_mutate(MUT_PASSIVE_MAPPING, 1, "wizard race change");
        break;

    case SP_FELID:
        if (you.experience_level >= 6)
            perma_mutate(MUT_SHAGGY_FUR, 1, "wizard race change");
        if (you.experience_level >= 12)
            perma_mutate(MUT_SHAGGY_FUR, 1, "wizard race change");
        break;

    default:
        break;
    }

    if ((old_sp == SP_OCTOPODE) != (sp == SP_OCTOPODE))
    {
        _swap_equip(EQ_LEFT_RING, EQ_RING_ONE);
        _swap_equip(EQ_RIGHT_RING, EQ_RING_TWO);
        // All species allow exactly one amulet.  When (and knowing you guys,
        // that's "when" not "if") ettins go in, you'll need handle the Macabre
        // Finger Necklace on neck 2 here.
    }

    // FIXME: this checks only for valid slots, not for suitability of the
    // item in question.  This is enough to make assertions happy, though.
    for (int i = 0; i < NUM_EQUIP; ++i)
        if (!you_can_wear(i, true) && you.equip[i] != -1)
        {
            mprf("%s falls away.", you.inv[you.equip[i]].name(DESC_YOUR).c_str());
            // Unwear items without the usual processing.
            you.equip[i] = -1;
            you.melded.set(i, false);
        }

    // Sanitize skills.
    fixup_skills();

    calc_hp();
    calc_mp();

    // The player symbol depends on species.
    update_player_symbol();
#ifdef USE_TILE
    init_player_doll();
#endif
    redraw_screen();
}

void wizard_change_species()
{
    char specs[80];

    msgwin_get_line("What species would you like to be now? " ,
                    specs, sizeof(specs));

    if (specs[0] == '\0')
        return;

    species_type sp = _find_species(specs);

    _wizard_change_species_to(sp);
}
#endif

#ifdef WIZARD
// Casts a specific spell by number or name.
void wizard_cast_spec_spell()
{
    char specs[80], *end;
    int spell;

    mprf(MSGCH_PROMPT, "Cast which spell? ");
    if (cancellable_get_line_autohist(specs, sizeof(specs))
        || specs[0] == '\0')
    {
        canned_msg(MSG_OK);
        crawl_state.cancel_cmd_repeat();
        return;
    }

    spell = strtol(specs, &end, 10);

    if (spell < 0 || end == specs)
    {
        if ((spell = spell_by_name(specs, true)) == SPELL_NO_SPELL)
        {
            mpr("Cannot find that spell.");
            crawl_state.cancel_cmd_repeat();
            return;
        }
    }

    if (your_spells(static_cast<spell_type>(spell), 0, false) == SPRET_ABORT)
        crawl_state.cancel_cmd_repeat();
}

void wizard_memorise_spec_spell()
{
    char specs[80], *end;
    int spell;

    mprf(MSGCH_PROMPT, "Memorise which spell? ");
    if (cancellable_get_line_autohist(specs, sizeof(specs))
        || specs[0] == '\0')
    {
        canned_msg(MSG_OK);
        crawl_state.cancel_cmd_repeat();
        return;
    }

    spell = strtol(specs, &end, 10);

    if (spell < 0 || end == specs)
    {
        if ((spell = spell_by_name(specs, true)) == SPELL_NO_SPELL)
        {
            mpr("Cannot find that spell.");
            crawl_state.cancel_cmd_repeat();
            return;
        }
    }

    if (!learn_spell(static_cast<spell_type>(spell)))
        crawl_state.cancel_cmd_repeat();
}
#endif

void wizard_heal(bool super_heal)
{
    if (super_heal)
    {
        // Clear more stuff and give a HP boost.
        unrot_hp(9999);
        you.magic_contamination = 0;
        you.duration[DUR_LIQUID_FLAMES] = 0;
        you.clear_beholders();
        inc_max_hp(10);
        you.attribute[ATTR_XP_DRAIN] = 0;
        you.duration[DUR_PETRIFIED] = 0;
        you.duration[DUR_PETRIFYING] = 0;
        you.duration[DUR_CORROSION] = 0;
    }

    // Clear most status ailments.
    you.rotting = 0;
    you.disease = 0;
    you.duration[DUR_CONF]      = 0;
    you.duration[DUR_POISONING] = 0;
    you.duration[DUR_EXHAUSTED] = 0;
    set_hp(you.hp_max);
    set_mp(you.max_magic_points);
    set_hunger(HUNGER_VERY_FULL + 100, true);
    you.redraw_hit_points = true;
    you.redraw_armour_class = true;
    you.redraw_evasion = true;
}

void wizard_set_hunger_state()
{
    string hunger_prompt =
        "Set hunger state to s(T)arving, (N)ear starving, (H)ungry";
    if (you.species == SP_GHOUL)
        hunger_prompt += " or (S)atiated";
    else
        hunger_prompt += ", (S)atiated, (F)ull or (E)ngorged";
    hunger_prompt += "? ";

    mprf(MSGCH_PROMPT, "%s", hunger_prompt.c_str());

    const int c = toalower(getchk());

    // Values taken from food.cc.
    switch (c)
    {
    case 't': you.hunger = HUNGER_STARVING / 2;   break;
    case 'n': you.hunger = 1100;  break;
    case 'h': you.hunger = 2300;  break;
    case 's': you.hunger = 4900;  break;
    case 'f': you.hunger = 7900;  break;
    case 'e': you.hunger = HUNGER_MAXIMUM; break;
    default:  canned_msg(MSG_OK); break;
    }

    food_change();

    if (you.species == SP_GHOUL && you.hunger_state >= HS_SATIATED)
        mpr("Ghouls can never be full or above!");
}

static void _wizard_set_piety_to(int newpiety, bool force = false)
{
    if (newpiety < 0 || newpiety > MAX_PIETY)
    {
        mprf("Piety needs to be between 0 and %d.", MAX_PIETY);
        return;
    }
    if (newpiety > piety_breakpoint(5) && you_worship(GOD_RU))
    {
        mprf("Ru piety can't be greater than %d.", piety_breakpoint(5));
        return;
    }

    if (you_worship(GOD_XOM))
    {
        you.piety = newpiety;

        int newinterest;
        if (!force)
        {
            char buf[30];

            // For Xom, also allow setting interest.
            mprf(MSGCH_PROMPT,
                 "Enter new interest (current = %d, Enter for 0): ",
                 you.gift_timeout);

            if (cancellable_get_line_autohist(buf, sizeof buf))
            {
                canned_msg(MSG_OK);
                return;
            }

            newinterest = atoi(buf);
        }
        else
            newinterest = newpiety;

        if (newinterest >= 0 && newinterest < 256)
            you.gift_timeout = newinterest;
        else
            mpr("Interest must be between 0 and 255.");

        mprf("Set piety to %d, interest to %d.", you.piety, newinterest);

        const string new_xom_favour = describe_xom_favour();
        const string msg = "You are now " + new_xom_favour;
        god_speaks(you.religion, msg.c_str());
        return;
    }

    if (newpiety < 1 && !force)
    {
        if (yesno("Are you sure you want to be excommunicated?", false, 'n'))
        {
            you.piety = 0;
            excommunication();
        }
        else
            canned_msg(MSG_OK);
        return;
    }
    mprf("Setting piety to %d.", newpiety);
    set_piety(newpiety);

    // Automatically reduce penance to 0.
    if (player_under_penance())
        dec_penance(you.penance[you.religion]);
}

void wizard_set_piety()
{
    if (you_worship(GOD_NO_GOD))
    {
        mpr("You are not religious!");
        return;
    }

    mprf(MSGCH_PROMPT, "Enter new piety value (current = %d, Enter for 0): ",
         you.piety);
    char buf[30];
    if (cancellable_get_line_autohist(buf, sizeof buf))
    {
        canned_msg(MSG_OK);
        return;
    }

    _wizard_set_piety_to(atoi(buf));
}

//---------------------------------------------------------------
//
// debug_add_skills
//
//---------------------------------------------------------------
#ifdef WIZARD
void wizard_exercise_skill()
{
    skill_type skill = debug_prompt_for_skill("Which skill (by name)? ");

    if (skill == SK_NONE)
        mpr("That skill doesn't seem to exist.");
    else
    {
        mpr("Exercising...");
        exercise(skill, 10);
    }
}
#endif

#ifdef WIZARD
void wizard_set_skill_level(skill_type skill)
{
    if (skill == SK_NONE)
        skill = debug_prompt_for_skill("Which skill (by name)? ");

    if (skill == SK_NONE)
    {
        mpr("That skill doesn't seem to exist.");
        return;
    }

    mpr(skill_name(skill));
    double amount = prompt_for_float("To what level? ");

    if (amount < 0 || amount > 27)
    {
        canned_msg(MSG_OK);
        return;
    }

    const int old_amount = you.skills[skill];

    set_skill_level(skill, amount);

    if (amount == 27)
    {
        you.train[skill] = 0;
        you.train_alt[skill] = 0;
        reset_training();
        check_selected_skills();
    }

    redraw_skill(skill);

    mprf("%s %s to skill level %.1f.", (old_amount < amount ? "Increased" :
                                      old_amount > amount ? "Lowered"
                                                          : "Reset"),
         skill_name(skill), amount);
}
#endif

#ifdef WIZARD
void wizard_set_all_skills()
{
    double amount = prompt_for_float("Set all skills to what level? ");

    if (amount < 0)             // cancel returns -1 -- bwr
        canned_msg(MSG_OK);
    else
    {
        if (amount > 27)
            amount = 27;

        for (int i = SK_FIRST_SKILL; i < NUM_SKILLS; ++i)
        {
            skill_type sk = static_cast<skill_type>(i);
            if (is_invalid_skill(sk) || is_useless_skill(sk))
                continue;

            set_skill_level(sk, amount);

            if (amount == 27)
            {
                you.train[sk] = 0;
                you.training[sk] = 0;
            }
        }

        you.redraw_title = true;

        // We're not updating skill cost here since XP hasn't changed.

        calc_hp();
        calc_mp();

        you.redraw_armour_class = true;
        you.redraw_evasion = true;
    }
}
#endif

#ifdef WIZARD
bool wizard_add_mutation()
{
    bool success = false;
    char specs[80];

    if (player_mutation_level(MUT_MUTATION_RESISTANCE) > 0
        && !crawl_state.is_replaying_keys())
    {
        const char* msg;

        if (you.mutation[MUT_MUTATION_RESISTANCE] == 3)
            msg = "You are immune to mutations; remove immunity?";
        else
            msg = "You are resistant to mutations; remove resistance?";

        if (yesno(msg, true, 'n'))
        {
            you.mutation[MUT_MUTATION_RESISTANCE] = 0;
            crawl_state.cancel_cmd_repeat();
        }
    }

    int answer = yesnoquit("Force mutation to happen?", true, 'n');
    if (answer == -1)
    {
        canned_msg(MSG_OK);
        return false;
    }
    const bool force = (answer == 1);

    if (player_mutation_level(MUT_MUTATION_RESISTANCE) == 3 && !force)
    {
        mpr("Can't mutate when immune to mutations without forcing it.");
        crawl_state.cancel_cmd_repeat();
        return false;
    }

    answer = yesnoquit("Treat mutation as god gift?", true, 'n');
    if (answer == -1)
    {
        canned_msg(MSG_OK);
        return false;
    }
    const bool god_gift = (answer == 1);

    msgwin_get_line("Which mutation (name, 'good', 'bad', 'any', "
                    "'xom', 'slime', 'corrupt', 'qazlal')? ",
                    specs, sizeof(specs));

    if (specs[0] == '\0')
        return false;

    string spec = lowercase_string(specs);

    mutation_type mutat = NUM_MUTATIONS;

    if (spec == "good")
        mutat = RANDOM_GOOD_MUTATION;
    else if (spec == "bad")
        mutat = RANDOM_BAD_MUTATION;
    else if (spec == "any")
        mutat = RANDOM_MUTATION;
    else if (spec == "xom")
        mutat = RANDOM_XOM_MUTATION;
    else if (spec == "slime")
        mutat = RANDOM_SLIME_MUTATION;
    else if (spec == "corrupt")
        mutat = RANDOM_CORRUPT_MUTATION;
    else if (spec == "qazlal")
        mutat = RANDOM_QAZLAL_MUTATION;

    if (mutat != NUM_MUTATIONS)
    {
        int old_resist = player_mutation_level(MUT_MUTATION_RESISTANCE);

        success = mutate(mutat, "wizard power", true, force, god_gift);

        if (old_resist < player_mutation_level(MUT_MUTATION_RESISTANCE)
            && !force)
        {
            crawl_state.cancel_cmd_repeat("Your mutation resistance has "
                                          "increased.");
        }
        return success;
    }

    vector<mutation_type> partial_matches;

    for (int i = 0; i < NUM_MUTATIONS; ++i)
    {
        mutation_type mut = static_cast<mutation_type>(i);
        const char* wizname = mutation_name(mut);
        if (!wizname)
            continue;

        if (spec == wizname)
        {
            mutat = mut;
            break;
        }

        if (strstr(wizname, spec.c_str()))
            partial_matches.push_back(mut);
    }

    // If only one matching mutation, use that.
    if (mutat == NUM_MUTATIONS && partial_matches.size() == 1)
        mutat = partial_matches[0];

    if (mutat == NUM_MUTATIONS)
    {
        crawl_state.cancel_cmd_repeat();

        if (partial_matches.empty())
            mpr("No matching mutation names.");
        else
        {
            vector<string> matches;

            for (unsigned int i = 0; i < partial_matches.size(); ++i)
                matches.push_back(mutation_name(partial_matches[i]));

            string prefix = "No exact match for mutation '" +
                            spec +  "', possible matches are: ";

            // Use mpr_comma_separated_list() because the list
            // might be *LONG*.
            mpr_comma_separated_list(prefix, matches, " and ", ", ",
                                     MSGCH_DIAGNOSTICS);
        }

        return false;
    }
    else
    {
        mprf("Found #%d: %s (\"%s\")", (int) mutat,
             mutation_name(mutat),
             mutation_desc(mutat, 1, false).c_str());

        const int levels =
            prompt_for_int("How many levels to increase or decrease? ",
                                  false);

        if (levels == 0)
        {
            canned_msg(MSG_OK);
            success = false;
        }
        else if (levels > 0)
        {
            for (int i = 0; i < levels; ++i)
                if (mutate(mutat, "wizard power", true, force, god_gift))
                    success = true;
        }
        else
        {
            for (int i = 0; i < -levels; ++i)
                if (delete_mutation(mutat, "wizard power", true, force, god_gift))
                    success = true;
        }
    }

    return success;
}
#endif

void wizard_set_abyss()
{
    char buf[80];
    mprf(MSGCH_PROMPT, "Enter values for X, Y, Z (space separated) or return: ");
    if (!cancellable_get_line_autohist(buf, sizeof buf))
        abyss_teleport();

    uint32_t x = 0, y = 0, z = 0;
    sscanf(buf, "%d %d %d", &x, &y, &z);
    set_abyss_state(coord_def(x,y), z);
}

void wizard_set_stats()
{
    char buf[80];
    mprf(MSGCH_PROMPT, "Enter values for Str, Int, Dex (space separated): ");
    if (cancellable_get_line_autohist(buf, sizeof buf))
        return;

    int sstr = you.strength(false),
        sdex = you.dex(false),
        sint = you.intel(false);

    sscanf(buf, "%d %d %d", &sstr, &sint, &sdex);

    you.base_stats[STAT_STR] = debug_cap_stat(sstr);
    you.base_stats[STAT_INT] = debug_cap_stat(sint);
    you.base_stats[STAT_DEX] = debug_cap_stat(sdex);
    you.stat_loss.init(0);
    you.redraw_stats.init(true);
    you.redraw_evasion = true;
}

void wizard_edit_durations()
{
    vector<duration_type> durs;
    size_t max_len = 0;

    for (int i = 0; i < NUM_DURATIONS; ++i)
    {
        const duration_type dur = static_cast<duration_type>(i);

        if (!you.duration[i])
            continue;

        max_len = max(strlen(duration_name(dur)), max_len);
        durs.push_back(dur);
    }

    if (!durs.empty())
    {
        for (size_t i = 0; i < durs.size(); ++i)
        {
            const duration_type dur = durs[i];
            const char ch = i >= 26 ? ' ' : 'a' + i;
            mprf_nocap(MSGCH_PROMPT, "%c%c %-*s : %d",
                 ch, ch == ' ' ? ' ' : ')',
                 (int)max_len, duration_name(dur), you.duration[dur]);
        }
        mprf(MSGCH_PROMPT, "\nEdit which duration (letter or name)? ");
    }
    else
        mprf(MSGCH_PROMPT, "Edit which duration (name)? ");

    char buf[80];

    if (cancellable_get_line_autohist(buf, sizeof buf) || !*buf)
    {
        canned_msg(MSG_OK);
        return;
    }

    if (!strlcpy(buf, lowercase_string(trimmed_string(buf)).c_str(), sizeof(buf)))
    {
        canned_msg(MSG_OK);
        return;
    }

    duration_type choice = NUM_DURATIONS;

    if (strlen(buf) == 1 && isalower(buf[0]))
    {
        if (durs.empty())
        {
            mprf(MSGCH_PROMPT, "No existing durations to choose from.");
            return;
        }
        const int dchoice = buf[0] - 'a';

        if (dchoice < 0 || dchoice >= (int) durs.size())
        {
            mprf(MSGCH_PROMPT, "Invalid choice.");
            return;
        }
        choice = durs[dchoice];
    }
    else
    {
        vector<duration_type> matches;
        vector<string> match_names;

        for (int i = 0; i < NUM_DURATIONS; ++i)
        {
            const duration_type dur = static_cast<duration_type>(i);
            if (strcmp(duration_name(dur), buf) == 0)
            {
                choice = dur;
                break;
            }
            if (strstr(duration_name(dur), buf) != NULL)
            {
                matches.push_back(dur);
                match_names.push_back(duration_name(dur));
            }
        }
        if (choice != NUM_DURATIONS)
            ;
        else if (matches.size() == 1)
            choice = matches[0];
        else if (matches.empty())
        {
            mprf(MSGCH_PROMPT, "No durations matching '%s'.", buf);
            return;
        }
        else
        {
            string prefix = "No exact match for duration '";
            prefix += buf;
            prefix += "', possible matches are: ";

            mpr_comma_separated_list(prefix, match_names, " and ", ", ",
                                     MSGCH_DIAGNOSTICS);
            return;
        }
    }

    snprintf(buf, sizeof(buf), "Set '%s' to: ", duration_name(choice));
    int num = prompt_for_int(buf, false);

    if (num == 0)
    {
        mprf(MSGCH_PROMPT, "Can't set duration directly to 0, setting it to 1 instead.");
        num = 1;
    }
    you.duration[choice] = num;
}

void wizard_list_props()
{
    mprf(MSGCH_DIAGNOSTICS, "props: %s",
         you.describe_props().c_str());
}

static void debug_uptick_xl(int newxl, bool train)
{
    if (train)
    {
        you.exp_available += exp_needed(newxl) - you.experience;
        train_skills();
    }
    you.experience = exp_needed(newxl);
    level_change(NON_MONSTER, NULL, true);
}

/**
 * Set the player's XL to a new value.
 * @param newxl  The new experience level.
 */
static void debug_downtick_xl(int newxl)
{
    set_hp(you.hp_max);
    // boost maxhp so we don't die if heavily rotted
    you.hp_max_adj_perm += 1000;
    you.experience = exp_needed(newxl);
    level_change();
    you.skill_cost_level = 0;
    check_skill_cost_change();
    // restore maxhp loss
    you.hp_max_adj_perm -= 1000;
    calc_hp();
    if (you.hp_max <= 0)
    {
        // ... but remove it completely if unviable
        you.hp_max_adj_temp = max(you.hp_max_adj_temp, 0);
        you.hp_max_adj_perm = max(you.hp_max_adj_perm, 0);
        calc_hp();
    }

    set_hp(max(1, you.hp));
}

void wizard_set_xl()
{
    mprf(MSGCH_PROMPT, "Enter new experience level: ");
    char buf[30];
    if (cancellable_get_line_autohist(buf, sizeof buf))
    {
        canned_msg(MSG_OK);
        return;
    }

    const int newxl = atoi(buf);
    if (newxl < 1 || newxl > 27 || newxl == you.experience_level)
    {
        canned_msg(MSG_OK);
        return;
    }

    set_xl(newxl, yesno("Train skills?", true, 'n'));
    mprf("Experience level set to %d.", newxl);
}

void set_xl(const int newxl, const bool train)
{
    no_messages mx;
    if (newxl < you.experience_level)
        debug_downtick_xl(newxl);
    else
        debug_uptick_xl(newxl, train);
}

void wizard_get_god_gift()
{
    if (you_worship(GOD_NO_GOD))
    {
        mpr("You are not religious!");
        return;
    }

    if (!do_god_gift(true))
        mpr("Nothing happens.");
}

void wizard_toggle_xray_vision()
{
    you.xray_vision = !you.xray_vision;
    viewwindow(true);
}

void wizard_god_wrath()
{
    if (you_worship(GOD_NO_GOD))
    {
        mpr("You suffer the terrible wrath of No God.");
        return;
    }

    if (!divine_retribution(you.religion, true, true))
        // Currently only dead Jiyva.
        mpr("You're not eligible for wrath.");
}

void wizard_god_mollify()
{
    for (int i = GOD_NO_GOD; i < NUM_GODS; ++i)
    {
        if (player_under_penance((god_type) i))
            dec_penance((god_type) i, you.penance[i]);
    }
}

void wizard_transform()
{
    transformation_type form;

    while (true)
    {
        string line;
        for (int i = 0; i <= LAST_FORM; i++)
        {
#if TAG_MAJOR_VERSION == 34
            if (i == TRAN_JELLY)
                continue;
#endif
            line += make_stringf("[%c] %-10s ", i + 'a',
                                 transform_name((transformation_type)i));
            if (i % 5 == 4 || i == LAST_FORM)
            {
                mprf(MSGCH_PROMPT, "%s", line.c_str());
                line.clear();
            }
        }
        mprf(MSGCH_PROMPT, "Which form (ESC to exit)? ");

        int keyin = toalower(get_ch());

        if (key_is_escape(keyin) || keyin == ' '
            || keyin == '\r' || keyin == '\n')
        {
            canned_msg(MSG_OK);
            return;
        }

        if (keyin < 'a' || keyin > 'a' + LAST_FORM)
            continue;

#if TAG_MAJOR_VERSION == 34
        if ((transformation_type)(keyin - 'a') == TRAN_JELLY)
            continue;
#endif

        form = (transformation_type)(keyin - 'a');

        break;
    }

    you.transform_uncancellable = false;
    if (!transform(200, form) && you.form != form)
        mpr("Transformation failed.");
}

static bool _chardump_check_skill(const vector<string> &tokens)
{
    size_t size = tokens.size();
    // * Level 25.0(24.4) Dodging
    if (size <= 3 || tokens[1] != "Level")
        return false;

    skill_type skill = skill_from_name(lowercase_string(tokens[3]).c_str());
    double amount = atof(tokens[2].c_str());
    set_skill_level(skill, amount);
    if (tokens[0] == "+")
        you.train[skill] = 1;
    else if (tokens[0] == "*")
        you.train[skill] = 2;
    else
        you.train[skill] = 0;

    redraw_skill(skill);

    return true;
}

static bool _chardump_check_stats1(const vector<string> &tokens)
{
    size_t size = tokens.size();
    // HP 121/199   AC 75   Str 35   XL: 27
    if (size <= 7 || tokens[0] != "HP")
        return false;

    bool found = false;
    for (size_t k = 1; k < size; k++)
    {
        if (tokens[k] == "Str")
        {
            you.base_stats[STAT_STR] = debug_cap_stat(atoi(tokens[k+1].c_str()));
            you.redraw_stats.init(true);
            you.redraw_evasion = true;
            found = true;
        }
        else if (tokens[k] == "XL:")
        {
            set_xl(atoi(tokens[k+1].c_str()), false);
            found = true;
        }
    }

    return found;
}

static bool _chardump_check_stats2(const vector<string> &tokens)
{
    size_t size = tokens.size();
    // MP  45/45   EV 13   Int 12   God: Makhleb [******]
    if (size <= 8 || tokens[0] != "MP")
        return false;

    bool found = false;
    for (size_t k = 1; k < size; k++)
    {
        if (tokens[k] == "Int")
        {
            you.base_stats[STAT_INT] = debug_cap_stat(atoi(tokens[k+1].c_str()));
            you.redraw_stats.init(true);
            you.redraw_evasion = true;
            found = true;
        }
        else if (tokens[k] == "God:")
        {
            god_type god = find_earliest_match(lowercase_string(tokens[k+1]),
                                               (god_type) 1, NUM_GODS,
                                               _always_true<god_type>,
                                               bind2nd(ptr_fun(god_name),
                                                       false));
            join_religion(god, true);

            string piety = tokens[k+2];
            int piety_levels = std::count(piety.begin(), piety.end(), '*');
            _wizard_set_piety_to(piety_levels > 0
                                 ? piety_breakpoint(piety_levels - 1)
                                 : 15);
        }
    }

    return found;
}

static bool _chardump_check_stats3(const vector<string> &tokens)
{
    size_t size = tokens.size();
    // Gold 15872   SH 59   Dex  9   Spells:  0 memorised, 26 levels left
    if (size <= 5 || tokens[0] != "Gold")
        return false;

    bool found;
    for (size_t k = 0; k < size; k++)
    {
        if (tokens[k] == "Dex")
        {
            you.base_stats[STAT_DEX] = debug_cap_stat(atoi(tokens[k+1].c_str()));
            you.redraw_stats.init(true);
            you.redraw_evasion = true;
            found = true;
        }
        else if (tokens[k] == "Gold")
            you.set_gold(atoi(tokens[k+1].c_str()));
    }

    return found;
}

static bool _chardump_check_char(const vector<string> &tokens)
{
    size_t size = tokens.size();

    if (size <= 8)
        return false;

    for (size_t k = 1; k < size; k++)
    {
        if (tokens[k] == "Turns:")
        {
            string race = tokens[k-2].substr(1);
            string role = tokens[k-1].substr(0, tokens[k-1].length() - 1);

            _wizard_change_species_to(_find_species(race));
            // XXX role
            return true;
        }
    }

    return false;
}

static void _wizard_modify_character(string inputdata)
{
    vector<string> tokens = split_string(" ", inputdata);

    if (_chardump_check_skill(tokens))
        return;
    if (_chardump_check_stats1(tokens))
        return;
    if (_chardump_check_stats2(tokens))
        return;
    if (_chardump_check_stats3(tokens))
        return;
    if (_chardump_check_char(tokens))
        return;
}

/**
 * Load a character from a dump file.
 *
 * @param filename The name of the file to open.
 * @pre The file either does not exist, or is a complete
 *      dump or morgue file.
 * @returns True if the file existed and could be opened.
 * @post The player's stats, level, skills, training, and gold are
 *       those listed in the dump file.
 */
static bool _load_dump_file(const char *filename)
{
    FileLineInput f(filename);
    if (f.eof())
        return false;

    you.init_skills();

    while (!f.eof())
        _wizard_modify_character(f.get_line());

    init_skill_order();
    init_can_train();
    init_train();
    init_training();
    return true;
}

void wizard_load_dump_file()
{
    char filename[80];
    msgwin_get_line_autohist("Which dump file? ", filename, sizeof(filename));

    if (filename[0] == '\0')
        canned_msg(MSG_OK);
    else if (!_load_dump_file(filename)
             && !_load_dump_file((morgue_directory() + filename).c_str()))
    {
        canned_msg(MSG_NOTHING_THERE);
    }
}

void wizard_offer_new_ru_sacrifices()
{
    if (you_worship(GOD_RU))
    {
        ru_offer_new_sacrifices();
        simple_god_message(" believes you are ready to make a new sacrifice.");
        more();
        you.props["ru_sacrifice_delay"] = 70;
        you.props["ru_progress_to_next_sacrifice"] = 0;
    }
    else
        mpr("You don't worship Ru, so no sacrifices for you.");
}

void wizard_join_religion()
{
    god_type god = choose_god();
    if (god == NUM_GODS)
        mpr("That god doesn't seem to exist!");
    else if (god == GOD_NO_GOD)
    {
        if (you_worship(GOD_NO_GOD))
            mpr("You already have no god!");
        else
            excommunication();
    }
    else if (you_worship(god))
        mpr("You already worship that god!");
    else
        join_religion(god, true);
}

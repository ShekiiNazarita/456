/*
 *  File:       mutation.cc
 *  Summary:    Functions for handling player mutations.
 *  Written by: Linley Henzell
 */

#include "AppHdr.h"
#include "mutation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>

#if defined(UNIX) && !defined(USE_TILE)
#include "libunix.h"
#endif

#include "externs.h"

#include "abl-show.h"
#include "cio.h"
#include "coordit.h"
#include "delay.h"
#include "defines.h"
#include "effects.h"
#include "env.h"
#include "files.h"
#include "format.h"
#include "godabil.h"
#include "itemprop.h"
#include "macro.h"
#include "menu.h"
#include "mgen_data.h"
#include "mon-place.h"
#include "mon-util.h"
#include "notes.h"
#include "ouch.h"
#include "player.h"
#include "player-stats.h"
#include "religion.h"
#include "random.h"
#include "skills2.h"
#include "stuff.h"
#include "transform.h"
#include "tutorial.h"
#include "view.h"
#include "xom.h"

static int _body_covered();

const char *troll_claw_descrip[4] = {
    "You have claws for hands.",
    "You have sharp claws for hands.",
    "You have very sharp claws for hands.",
    "You have claws sharper than steel for hands."
};

const char *troll_claw_gain[3] = {
    "Your claws sharpen.",
    "Your claws sharpen.",
    "Your claws steel!"
};

const char *troll_claw_lose[3] = {
    "Your claws look duller.",
    "Your claws look duller.",
    "Your claws feel softer."
};

const char *naga_speed_descrip[4] = {
    "You cover ground very slowly.",    // 10*14/10 = 14
    "You cover ground rather slowly.",  //  8*14/10 = 11
    "You cover ground rather quickly.", //  7*14/10 =  9
    "You cover ground quickly."         //  6*14/10 =  8
};

const char *centaur_deformed_descrip[3] = {
    "Armour fits poorly on your equine body.",
    "Armour fits poorly on your deformed equine body.",
    "Armour fits poorly on your badly deformed equine body."
};

const char *naga_deformed_descrip[3] = {
    "Armour fits poorly on your serpentine body.",
    "Armour fits poorly on your deformed serpentine body.",
    "Armour fits poorly on your badly deformed serpentine body."
};

mutation_def mut_data[] = {

#include "mutation-data.h"

};

#define MUTDATASIZE (sizeof(mut_data)/sizeof(mutation_def))

static int mut_index[NUM_MUTATIONS];

void init_mut_index()
{
    for (int i = 0; i < NUM_MUTATIONS; ++i)
        mut_index[i] = -1;

    for (unsigned int i = 0; i < MUTDATASIZE; ++i)
    {
        const mutation_type mut = mut_data[i].mutation;
        ASSERT(mut >= 0 && mut < NUM_MUTATIONS);
        ASSERT(mut_index[mut] == -1);
        mut_index[mut] = i;
    }
}

static mutation_def* _seek_mutation(mutation_type mut)
{
    ASSERT(mut >= 0 && mut < NUM_MUTATIONS);
    if (mut_index[mut] == -1)
        return (NULL);
    else
        return (&mut_data[mut_index[mut]]);
}

bool is_valid_mutation(mutation_type mut)
{
    return (mut >= 0 && mut < NUM_MUTATIONS
            && _seek_mutation(mut));
}

const mutation_def& get_mutation_def(mutation_type mut)
{
    ASSERT(is_valid_mutation(mut));
    return (*_seek_mutation(mut));
}

void fixup_mutations()
{
    if (player_genus(GENPC_DRACONIAN))
    {
        ASSERT(is_valid_mutation(MUT_STINGER));
        _seek_mutation(MUT_STINGER)->rarity = 1;
        ASSERT(is_valid_mutation(MUT_BIG_WINGS));
        _seek_mutation(MUT_BIG_WINGS)->rarity = 1;
    }

    if (you.species == SP_TROLL)
    {
        mutation_def* mdef = _seek_mutation(MUT_CLAWS);
        ASSERT(mdef);
        for (int j = 0; j < 3; ++j)
        {
            mdef->gain[j] = troll_claw_gain[j];
            mdef->lose[j] = troll_claw_lose[j];
        }
    }

    if (you.species == SP_NAGA)
    {
        ASSERT(is_valid_mutation(MUT_STINGER));
        _seek_mutation(MUT_STINGER)->rarity = 1;

        ASSERT(is_valid_mutation(MUT_DEFORMED));
        for (int j = 0; j < 3; ++j)
            _seek_mutation(MUT_DEFORMED)->have[j] = naga_deformed_descrip[j];
    }

    if (you.species == SP_CENTAUR)
    {
        ASSERT(is_valid_mutation(MUT_DEFORMED));
        for (int j = 0; j < 3; ++j)
            _seek_mutation(MUT_DEFORMED)->have[j] = centaur_deformed_descrip[j];
    }
}

bool mutation_is_fully_active(mutation_type mut)
{
    // For all except the semi-undead, mutations always apply.
    if (you.is_undead != US_SEMI_UNDEAD)
        return (true);

    // Innate mutations are always active
    if (you.demon_pow[mut])
        return (true);

    // ... as are all mutations for semi-undead who are fully alive
    if (you.hunger_state == HS_ENGORGED)
        return (true);

    // ... as are physical mutations.
    if (get_mutation_def(mut).physical)
        return (true);

    return (false);
}

static bool _mutation_is_fully_inactive(mutation_type mut)
{
    const mutation_def& mdef = get_mutation_def(mut);
    return (you.is_undead == US_SEMI_UNDEAD && you.hunger_state < HS_SATIATED
            && !you.demon_pow[mut] && !mdef.physical);
}

formatted_string describe_mutations()
{
    std::string result;
    bool have_any = false;
    const char *mut_title = "Innate Abilities, Weirdness & Mutations";

    // center title
    int offset = 39 - strlen(mut_title) / 2;
    if ( offset < 0 ) offset = 0;

    result += std::string(offset, ' ');

    result += "<white>";
    result += mut_title;
    result += "</white>\n\n";

    // Innate abilities which don't fit as mutations.
    result += "<lightblue>";
    switch (you.species)
    {
    case SP_MERFOLK:
        result += "You revert to your normal form in water.\n";
        have_any = true;
        break;

    case SP_NAGA:
        result += "You cannot wear boots.\n";
        if (!you.mutation[MUT_FAST])
        {
            result += mutation_name(MUT_FAST, -1, true);
            result += "\n";
        }
        // Breathe poison replaces spit poison.
        if (!you.mutation[MUT_BREATHE_POISON])
            result += "You can spit poison.\n";
        if (you.experience_level > 2)
        {
            std::ostringstream num;
            num << you.experience_level/3;
            result += "Your serpentine skin is tough (AC +" + num.str() + ").\n";
        }
        have_any = true;
        break;

    case SP_GHOUL:
        result += "Your body is rotting away.\n";
        result += "You have sharp claws for hands.\n";
        have_any = true;
        break;

    case SP_TROLL:
        if (!you.mutation[MUT_CLAWS])
        {
            result += mutation_name(MUT_CLAWS, -1, true);
            result += "\n";
        }
        have_any = true;
        break;

    case SP_KENKU:
        if (you.experience_level > 4)
        {
            result += "You can fly";
            if (you.experience_level > 14)
                result += " continuously";
            result += ".\n";
            have_any = true;
        }
        break;

    case SP_MUMMY:
        result += "Your flesh is vulnerable to fire.\n";
        if (you.experience_level > 12)
        {
            result += "You are";
            if (you.experience_level > 25)
                result += " strongly";

            result += " in touch with the powers of death.\n";
            result +=
                "You can restore your body by infusing magical energy.\n";
        }
        have_any = true;
        break;

    case SP_GREY_DRACONIAN:
        if (you.experience_level > 6)
        {
            result += "Your tail is studded with spikes.\n";
            have_any = true;
        }
        break;

    case SP_GREEN_DRACONIAN:
        if (you.experience_level > 6)
        {
            result += "You can breathe poison.\n";
            have_any = true;
        }
        break;

    case SP_RED_DRACONIAN:
        if (you.experience_level > 6)
        {
            result += "You can breathe fire.\n";
            have_any = true;
        }
        break;

    case SP_WHITE_DRACONIAN:
        if (you.experience_level > 6)
        {
            result += "You can breathe cold.\n";
            have_any = true;
        }
        break;

    case SP_BLACK_DRACONIAN:
        if (you.experience_level > 6)
        {
            result += "You can breathe lightning.\n";
            have_any = true;
        }
        break;

    case SP_YELLOW_DRACONIAN:
        if (you.experience_level > 6)
        {
            result += "You can spit acid.\n";
            result += "You are resistant to acid.\n";
            have_any = true;
        }
        break;

    case SP_PURPLE_DRACONIAN:
        if (you.experience_level > 6)
        {
            result += "You can breathe power.\n";
            have_any = true;
        }
        break;

    case SP_MOTTLED_DRACONIAN:
        if (you.experience_level > 6)
        {
            result += "You can breathe sticky flames.\n";
            have_any = true;
        }
        break;

    case SP_PALE_DRACONIAN:
        if (you.experience_level > 6)
        {
            result += "You can breathe steam.\n";
            have_any = true;
        }
        break;

    case SP_KOBOLD:
        result += "You recuperate from illness quickly.\n";
        have_any = true;
        break;

    case SP_VAMPIRE:
        have_any = true;
        if (you.hunger_state == HS_STARVING)
            result += "<green>You do not heal naturally.</green>\n";
        else if (you.hunger_state == HS_ENGORGED)
            result += "<green>Your natural rate of healing is extremely fast.</green>\n";
        else if (you.hunger_state < HS_SATIATED)
            result += "<green>You heal slowly.</green>\n";
        else if (you.hunger_state >= HS_FULL)
            result += "<green>Your natural rate of healing is unusually fast.</green>\n";
        else
            have_any = false;

        if (you.experience_level >= 6)
        {
            result += "You can bottle blood from corpses.\n";
            have_any = true;
        }
        break;

    case SP_DEEP_DWARF:
        result += "You are resistant to damage.\n";
        result += "You can recharge devices by infusing magical energy.\n";
        have_any = true;
        break;

    default:
        break;
    }

    // a bit more stuff
    if (player_genus(GENPC_OGRE) || you.species == SP_TROLL
        || player_genus(GENPC_DRACONIAN) || you.species == SP_SPRIGGAN)
    {
        result += "Your body does not fit into most forms of armour.\n";
        have_any = true;
    }

    if (player_genus(GENPC_DRACONIAN))
    {
        int ac = (you.experience_level < 8) ? 2 :
                 (you.species == SP_GREY_DRACONIAN)
                     ? (you.experience_level - 4) / 2 + 1 :
                       you.experience_level / 4 + 1;
        std::ostringstream num;
        num << ac;
        result += "Your scales are hard (AC +" + num.str() + ").\n";
    }

    result += "</lightblue>";

    if (beogh_water_walk())
    {
        result += "<green>You can walk on water.</green>\n";
        have_any = true;
    }

    if (you.duration[DUR_FIRE_SHIELD])
    {
        result += "<green>You are immune to clouds of flame.</green>\n";
        have_any = true;
    }

    textcolor(LIGHTGREY);

    // First add (non-removable) inborn abilities and demon powers.
    for (int i = 0; i < NUM_MUTATIONS; i++)
    {
        if (you.mutation[i] != 0 && you.demon_pow[i])
        {
            mutation_type mut_type = static_cast<mutation_type>(i);
            result += mutation_name(mut_type, -1, true);
            result += "\n";
            have_any = true;
        }
    }

    // Now add removable mutations.
    for (int i = 0; i < NUM_MUTATIONS; i++)
    {
        if (you.mutation[i] != 0 && !you.demon_pow[i])
        {
            mutation_type mut_type = static_cast<mutation_type>(i);
            result += mutation_name(mut_type, -1, true);
            result += "\n";
            have_any = true;
        }
    }

    if (!have_any)
        result +=  "You are rather mundane.\n";

    if (you.species == SP_VAMPIRE)
    {
        result += "\n\n";
        result += "\n\n";
        result +=
#ifndef USE_TILE
            "Press '<w>!</w>'"
#else
            "<w>Right-click</w>"
#endif
            " to toggle between mutations and properties depending on your\n"
            "hunger status.\n";
    }

    return formatted_string::parse_string(result);
}

static void _display_vampire_attributes()
{
    ASSERT(you.species == SP_VAMPIRE);

    clrscr();
    cgotoxy(1,1);

    std::string result;

    const int lines = 15;
    std::string column[lines][7] =
    {
       {"                     ", "<lightgreen>Alive</lightgreen>      ", "<green>Full</green>    ",
        "Satiated  ", "<yellow>Thirsty</yellow>  ", "<yellow>Near...</yellow>  ",
        "<lightred>Bloodless</lightred>"},
                                //Alive          Full       Satiated      Thirsty   Near...      Bloodless
       {"Metabolism           ", "very fast  ", "fast    ", "fast      ", "normal   ", "slow     ", "none  "},

       {"Regeneration         ", "very fast  ", "fast    ", "normal    ", "slow     ", "slow     ", "none  "},

       {"Stealth boost        ", "none       ", "none    ", "none      ", "minor    ", "major    ", "large "},

       {"Spell hunger         ", "full       ", "full    ", "full      ", "halved   ", "none     ", "none  "},

       {"\n<w>Resistances</w>\n"
        "Poison resistance    ", "           ", "        ", "          ", " +       ", " +       ", " +    "},

       {"Cold resistance      ", "           ", "        ", "          ", " +       ", " ++      ", " ++   "},

       {"Negative resistance  ", "           ", "        ", " +        ", " ++      ", " +++     ", " +++  "},

       {"Rotting resistance   ", "           ", "        ", "          ", " +       ", " +       ", " +    "},

       {"Torment resistance   ", "           ", "        ", "          ", "         ", "         ", " +    "},

       {"\n<w>Other effects</w>\n"
        "Mutation chance      ", "always     ", "often   ", "sometimes ", "never    ", "never    ", "never "},

       {"Non-physical \n"
        "mutation effects     ", "full       ", "capped  ", "capped    ", "none     ", "none     ", "none  "},

       {"Potion effects       ", "full       ", "full    ", "full      ", "halved   ", "halved   ", "halved"},

       {"Bat Form             ", "no         ", "no      ", "yes       ", "yes      ", "yes      ", "yes   "},

       {"Other transformation \n"
        "or going berserk     ", "yes        ", "yes     ", "no        ", "no       ", "no       ", "no    "}
    };

    int current = 0;
    switch (you.hunger_state)
    {
    case HS_ENGORGED:
        current = 1;
        break;
    case HS_VERY_FULL:
    case HS_FULL:
        current = 2;
        break;
    case HS_SATIATED:
        current = 3;
        break;
    case HS_HUNGRY:
    case HS_VERY_HUNGRY:
        current = 4;
        break;
    case HS_NEAR_STARVING:
        current = 5;
        break;
    case HS_STARVING:
        current = 6;
    }

    for (int y = 0; y < lines; y++)  // lines   (properties)
    {
        for (int x = 0; x < 7; x++)  // columns (hunger states)
        {
             if (y > 0 && x == current)
                 result += "<w>";
             result += column[y][x];
             if (y > 0 && x == current)
                 result += "</w>";
        }
        result += "\n";
    }

    result += "\n";
    result +=
#ifndef USE_TILE
        "Press '<w>!</w>'"
#else
        "<w>Right-click</w>"
#endif
        " to toggle between mutations and properties depending on your\n"
        "hunger status.\n";

    const formatted_string vp_props = formatted_string::parse_string(result);
    vp_props.display();

    mouse_control mc(MOUSE_MODE_MORE);
    const int keyin = getchm();
    if (keyin == '!' || keyin == CK_MOUSE_CMD)
        display_mutations();
}

void display_mutations()
{
    clrscr();
    cgotoxy(1,1);

    const formatted_string mutation_fs = describe_mutations();

    if (you.species == SP_VAMPIRE)
    {
        mutation_fs.display();
        mouse_control mc(MOUSE_MODE_MORE);
        const int keyin = getchm();
        if (keyin == '!' || keyin == CK_MOUSE_CMD)
            _display_vampire_attributes();
    }
    else
    {
        Menu mutation_menu(mutation_fs);
        mutation_menu.show();
    }
}

static int _calc_mutation_amusement_value(mutation_type which_mutation)
{
    int amusement = 16 * (11 - get_mutation_def(which_mutation).rarity);

    switch (which_mutation)
    {
    case MUT_STRONG:
    case MUT_CLEVER:
    case MUT_AGILE:
    case MUT_POISON_RESISTANCE:
    case MUT_SHOCK_RESISTANCE:
    case MUT_REGENERATION:
    case MUT_SLOW_METABOLISM:
    case MUT_TELEPORT_CONTROL:
    case MUT_MAGIC_RESISTANCE:
    case MUT_TELEPORT_AT_WILL:
    case MUT_CLARITY:
    case MUT_MUTATION_RESISTANCE:
    case MUT_ROBUST:
    case MUT_HIGH_MAGIC:
        amusement /= 2;  // not funny
        break;

    case MUT_CARNIVOROUS:
    case MUT_HERBIVOROUS:
    case MUT_SLOW_HEALING:
    case MUT_FAST_METABOLISM:
    case MUT_WEAK:
    case MUT_DOPEY:
    case MUT_CLUMSY:
    case MUT_TELEPORT:
    case MUT_FAST:
    case MUT_DEFORMED:
    case MUT_SPIT_POISON:
    case MUT_BREATHE_FLAMES:
    case MUT_BLINK:
    case MUT_HORNS:
    case MUT_BEAK:
    case MUT_SCREAM:
    case MUT_BERSERK:
    case MUT_DETERIORATION:
    case MUT_BLURRY_VISION:
    case MUT_FRAIL:
    case MUT_CLAWS:
    case MUT_FANGS:
    case MUT_HOOVES:
    case MUT_TALONS:
    case MUT_BREATHE_POISON:
    case MUT_STINGER:
    case MUT_BIG_WINGS:
    case MUT_LOW_MAGIC:
        amusement *= 2; // funny!
        break;

    default:
        break;
    }

    return (amusement);
}

static bool _is_deadly(mutation_type mutat, bool delete_mut)
{
    if (delete_mut)
    {
        // First handle non-stat related problems.
        if ( mutat == MUT_HEAT_RESISTANCE && grd(you.pos()) == DNGN_LAVA
             && player_res_fire() == 1 && !you.airborne() )
        {
            // Don't let player instantly fry to a crisp in lava.
            return (true);
        }

        // Swap things around to the same effect, but as if we were
        // gaining a mutation, or return early if deleting the mutation
        // is never a problem.
        switch (mutat)
        {
        case MUT_WEAK:
        case MUT_DOPEY:
        case MUT_CLUMSY:
            return (false);

        case MUT_STRONG_STIFF:
            mutat = MUT_FLEXIBLE_WEAK;
            break;

        case MUT_FLEXIBLE_WEAK:
            mutat = MUT_STRONG_STIFF;
            break;

        case MUT_STRONG:
            mutat = MUT_WEAK;
            break;

        case MUT_CLEVER:
            mutat = MUT_DOPEY;
            break;

        case MUT_AGILE:
            mutat = MUT_CLUMSY;
            break;

        default:
            break;
        }
    }

    stat_type stat = STAT_DEX; // Default for the scales.
    char  amnt     = 1;
    char  mod      = 0;

    switch (mutat)
    {
    case MUT_FLEXIBLE_WEAK:
    case MUT_WEAK:
        stat = STAT_STR;
        // Take might into account so we don't lower base strength below
        // one.
        if (you.duration[DUR_MIGHT])
            mod = -5;
        break;

    case MUT_DOPEY:
        stat = STAT_INT;
        if (you.duration[DUR_BRILLIANCE])
            mod = -5;
        break;

    case MUT_STRONG_STIFF:
    case MUT_CLUMSY:
        stat = STAT_DEX;
        if (you.duration[DUR_AGILITY])
            mod = -5;
        break;

    default:
        return (false);
    }

    return (amnt >= (you.stat(stat) + mod));
}

static bool _accept_mutation(mutation_type mutat, bool ignore_rarity = false,
                             bool non_fatal = false, bool delete_mut = false)
{
    if (!is_valid_mutation(mutat))
        return (false);

    const mutation_def& mdef = get_mutation_def(mutat);

    if (delete_mut)
        return (!non_fatal || !_is_deadly(mutat, delete_mut));

    if (you.mutation[mutat] >= mdef.levels)
        return (false);

    if (non_fatal && _is_deadly(mutat, delete_mut))
        return (false);

    if (ignore_rarity)
        return (true);

    const int rarity = mdef.rarity + you.demon_pow[mutat];

    // Low rarity means unlikely to choose it.
    return (x_chance_in_y(rarity, 10));
}

static mutation_type _get_random_xom_mutation(bool non_fatal = false)
{
    const mutation_type bad_muts[] = {
        MUT_WEAK,          MUT_DOPEY,
        MUT_CLUMSY,        MUT_DEFORMED,      MUT_SCREAM,
        MUT_DETERIORATION, MUT_BLURRY_VISION, MUT_FRAIL
    };

    mutation_type mutat = NUM_MUTATIONS;

    do
    {
        mutat = static_cast<mutation_type>(random2(NUM_MUTATIONS));

        if (one_chance_in(1000))
            return (NUM_MUTATIONS);
        else if (one_chance_in(5))
            mutat = RANDOM_ELEMENT(bad_muts);
    }
    while (!_accept_mutation(mutat, false, non_fatal));

    return (mutat);
}

static mutation_type _get_random_mutation(bool prefer_good,
                                          int preferred_multiplier,
                                          bool non_fatal = false)
{
    int cweight = 0;
    mutation_type chosen = NUM_MUTATIONS;
    for (unsigned i = 0; i < NUM_MUTATIONS; ++i)
    {
        const mutation_type curr = static_cast<mutation_type>(i);
        if (!is_valid_mutation(curr))
            continue;

        const mutation_def& mdef = get_mutation_def(curr);
        if (!mdef.rarity)
            continue;

        if (!_accept_mutation(curr, true, non_fatal))
            continue;

        const bool weighted = mdef.bad != prefer_good;
        int weight = mdef.rarity;
        if (weighted)
            weight = weight * preferred_multiplier / 100;

        cweight += weight;

        if (x_chance_in_y(weight, cweight))
            chosen = curr;
    }

    return (chosen);
}

#ifdef ASSERTS
static bool _is_random(mutation_type which_mutation)
{
    return (which_mutation == RANDOM_MUTATION
            || which_mutation == RANDOM_XOM_MUTATION
            || which_mutation == RANDOM_GOOD_MUTATION
            || which_mutation == RANDOM_BAD_MUTATION);
}
#endif

// Tries to give you the mutation by deleting a conflicting
// one, or clears out conflicting mutations if we should give
// you the mutation anyway.
// Return:
//  1 if we should stop processing (success);
//  0 if we should continue processing;
// -1 if we should stop processing (failure).
static int _handle_conflicting_mutations(mutation_type mutation,
                                         bool override)
{
    if (override)
    {
        // These are mutations which should be cleared away if forced.
        const mutation_type override_conflict[][2] = {
            { MUT_REGENERATION, MUT_SLOW_METABOLISM },
            { MUT_REGENERATION, MUT_SLOW_HEALING    },
            { MUT_ACUTE_VISION, MUT_BLURRY_VISION   }
        };

        // If we have one of the pair, delete all levels of the other,
        // and continue processing.
        for (unsigned i = 0; i < ARRAYSZ(override_conflict); ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                const mutation_type a = override_conflict[i][j];
                const mutation_type b = override_conflict[i][1-j];

                if (mutation == a)
                    while (delete_mutation(b, true, true))
                        ;
            }
        }
    }

    // These are mutations which can't be traded off against each other,
    // so we just fail.
    const mutation_type fail_conflict[][2] = {
        { MUT_REGENERATION, MUT_SLOW_METABOLISM },
        { MUT_FANGS,        MUT_BEAK            },
        { MUT_HOOVES,       MUT_TALONS          }
    };

    for (unsigned i = 0; i < ARRAYSZ(fail_conflict); ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            const mutation_type a = fail_conflict[i][j];
            const mutation_type b = fail_conflict[i][1-j];
            if (mutation == a && you.mutation[b] > 0)
                return (-1);    // Fail.
        }
    }

    // These are mutations which trade off against each other.
    const mutation_type simple_conflict[][2] = {
        { MUT_STRONG,          MUT_WEAK            },
        { MUT_CLEVER,          MUT_DOPEY           },
        { MUT_AGILE,           MUT_CLUMSY          },
        { MUT_STRONG_STIFF,    MUT_FLEXIBLE_WEAK   },
        { MUT_ROBUST,          MUT_FRAIL           },
        { MUT_HIGH_MAGIC,      MUT_LOW_MAGIC       },
        { MUT_CARNIVOROUS,     MUT_HERBIVOROUS     },
        { MUT_SLOW_METABOLISM, MUT_FAST_METABOLISM },
        { MUT_REGENERATION,    MUT_SLOW_HEALING    },
        { MUT_ACUTE_VISION,    MUT_BLURRY_VISION   }
    };

    for (unsigned i = 0; i < ARRAYSZ(simple_conflict); ++i)
        for (int j = 0; j < 2; ++j)
        {
            // If we have one of the pair, delete a level of the other,
            // and that's it.
            const mutation_type a = simple_conflict[i][j];
            const mutation_type b = simple_conflict[i][1-j];
            if (mutation == a && you.mutation[b] > 0)
            {
                delete_mutation(b);
                return (1);     // Nothing more to do.
            }
        }

    return (0);
}

static bool _physiology_mutation_conflict(mutation_type mutat)
{
    // Only Nagas and Draconians can get this one.
    if (mutat == MUT_STINGER
        && you.species != SP_NAGA && !player_genus(GENPC_DRACONIAN))
    {
        return (true);
    }

    if ((mutat == MUT_HOOVES || mutat == MUT_TALONS) && !player_has_feet())
        return (true);

    // Already innate.
    if (mutat == MUT_BREATHE_POISON && you.species != SP_NAGA)
        return (true);

    // Red Draconians can already breathe flames.
    if (mutat == MUT_BREATHE_FLAMES && you.species == SP_RED_DRACONIAN)
        return (true);

    // Green Draconians can already breathe poison, so they don't need
    // to spit it.
    if (mutat == MUT_SPIT_POISON && you.species == SP_GREEN_DRACONIAN)
        return (true);

    // Only Draconians can get wings.
    if (mutat == MUT_BIG_WINGS && !player_genus(GENPC_DRACONIAN))
        return (true);

    // Vampires' healing and thirst rates depend on their blood level.
    if (you.species == SP_VAMPIRE
        && (mutat == MUT_CARNIVOROUS || mutat == MUT_HERBIVOROUS
            || mutat == MUT_REGENERATION || mutat == MUT_SLOW_HEALING
            || mutat == MUT_FAST_METABOLISM || mutat == MUT_SLOW_METABOLISM))
    {
        return (true);
    }

    return (false);
}

static bool _reautomap_callback()
{
    reautomap_level();
    return true;
}

static const char* _stat_mut_desc(mutation_type mut, bool gain)
{
    stat_type stat;
    bool positive = gain;
    switch (mut)
    {
    case MUT_WEAK:
        positive = !positive;
    case MUT_STRONG:
        stat = STAT_STR;
        break;

    case MUT_DOPEY:
        positive = !positive;
    case MUT_CLEVER:
        stat = STAT_INT;
        break;

    case MUT_CLUMSY:
        positive = !positive;
    case MUT_AGILE:
        stat = STAT_DEX;
        break;

    default:
        ASSERT(false);
        break;
    }
    return (stat_desc(stat, positive ? SD_INCREASE : SD_DECREASE));
}

bool mutate(mutation_type which_mutation, bool failMsg,
            bool force_mutation, bool god_gift, bool stat_gain_potion,
            bool demonspawn, bool non_fatal)
{
    ASSERT(!non_fatal || _is_random(which_mutation));

    if (!god_gift)
    {
        const god_type god =
            (crawl_state.is_god_acting()) ? crawl_state.which_god_acting()
                                          : GOD_NO_GOD;

        if (god != GOD_NO_GOD)
            god_gift = true;
    }

    if (demonspawn)
        force_mutation = true;

    mutation_type mutat = which_mutation;

    if (!force_mutation)
    {
        // God gifts override all sources of mutation resistance other
        // than divine protection, and stat gain potions override all
        // sources of mutation resistance other than the mutation
        // resistance mutation.
        if (!god_gift)
        {
            if (wearing_amulet(AMU_RESIST_MUTATION)
                    && !one_chance_in(10) && !stat_gain_potion
                || player_mutation_level(MUT_MUTATION_RESISTANCE) == 3
                || player_mutation_level(MUT_MUTATION_RESISTANCE)
                    && !one_chance_in(3))
            {
                if (failMsg)
                    mpr("You feel odd for a moment.", MSGCH_MUTATION);
                return (false);
            }
        }

        // Zin's protection.
        if (you.religion == GOD_ZIN && x_chance_in_y(you.piety, MAX_PIETY)
            && !stat_gain_potion)
        {
            simple_god_message(" protects your body from mutation!");
            return (false);
        }
    }

    bool rotting = you.is_undead;

    if (you.is_undead == US_SEMI_UNDEAD)
    {
        // The stat gain mutations always come through at Satiated or
        // higher (mostly for convenience), and, for consistency, also
        // their negative counterparts.
        if (which_mutation == MUT_STRONG || which_mutation == MUT_CLEVER
            || which_mutation == MUT_AGILE || which_mutation == MUT_WEAK
            || which_mutation == MUT_DOPEY || which_mutation == MUT_CLUMSY)
        {
            if (you.hunger_state > HS_SATIATED)
                rotting = false;
        }
        else
        {
            // Else, chances depend on hunger state.
            switch (you.hunger_state)
            {
            case HS_SATIATED:  rotting = !one_chance_in(3); break;
            case HS_FULL:      rotting = coinflip();        break;
            case HS_VERY_FULL: rotting = one_chance_in(3);  break;
            case HS_ENGORGED:  rotting = false;             break;
            }
        }
    }

    // Undead bodies don't mutate, they fall apart. -- bwr
    // except for demonspawn (or other permamutations) in lichform -- haranp
    if (rotting && !demonspawn)
    {
        mpr("Your body decomposes!", MSGCH_MUTATION);

        if (coinflip())
            lose_stat(STAT_RANDOM, 1, false, "mutating");
        else
        {
            ouch(3, NON_MONSTER, KILLED_BY_ROTTING);
            rot_hp(roll_dice(1, 3));
        }

        xom_is_stimulated(64);
        return (true);
    }

    if (which_mutation == RANDOM_MUTATION
        || which_mutation == RANDOM_XOM_MUTATION)
    {
        // If already heavily mutated, remove a mutation instead.
        if (x_chance_in_y(how_mutated(false, true), 15))
        {
            // God gifts override mutation loss due to being heavily
            // mutated.
            if (!one_chance_in(3) && !god_gift && !force_mutation)
                return (false);
            else
                return (delete_mutation(RANDOM_MUTATION, failMsg,
                                        force_mutation, false, non_fatal));
        }
    }

    if (which_mutation == RANDOM_MUTATION)
    {
        do
        {
            mutat = static_cast<mutation_type>(random2(NUM_MUTATIONS));
            if (one_chance_in(1000))
                return (false);
        }
        while (!_accept_mutation(mutat, false, non_fatal));
    }
    else if (which_mutation == RANDOM_XOM_MUTATION)
    {
        if ((mutat = _get_random_xom_mutation(non_fatal)) == NUM_MUTATIONS)
            return (false);
    }
    else if (which_mutation == RANDOM_GOOD_MUTATION)
    {
        mutat = _get_random_mutation(true, 500, non_fatal);
        if (mutat == NUM_MUTATIONS)
            return (false);
    }
    else if (which_mutation == RANDOM_BAD_MUTATION)
    {
        mutat = _get_random_mutation(false, 500, non_fatal);
        if (mutat == NUM_MUTATIONS)
            return (false);
    }

    if (!is_valid_mutation(mutat))
        return (false);

    // Saprovorous/gourmand can't be randomly acquired.
    if ((mutat == MUT_SAPROVOROUS || mutat == MUT_GOURMAND) && !force_mutation)
        return (false);

    // These can be forced by demonspawn or god gifts.
    if ((mutat == MUT_SHAGGY_FUR
            || mutat >= MUT_DISTORTION_FIELD && mutat <= MUT_LARGE_BONE_PLATES)
        && _body_covered() >= 3 && !god_gift && !force_mutation)
    {
        return (false);
    }

    if (you.species == SP_NAGA)
    {
        // gdl: Spit poison 'upgrades' to breathe poison.  Why not...
        if (mutat == MUT_SPIT_POISON)
        {
            if (coinflip())
                return (false);

            mutat = MUT_BREATHE_POISON;

            // Breathe poison replaces spit poison (so it takes the slot).
            for (int i = 0; i < 52; ++i)
                if (you.ability_letter_table[i] == ABIL_SPIT_POISON)
                    you.ability_letter_table[i] = ABIL_BREATHE_POISON;
        }
    }

    if (_physiology_mutation_conflict(mutat))
        return (false);

    const mutation_def& mdef = get_mutation_def(mutat);

    if (you.mutation[mutat] >= mdef.levels)
        return (false);

    // God gifts and forced mutations clear away conflicting mutations.
    int rc =_handle_conflicting_mutations(mutat, god_gift || force_mutation);
    if (rc == 1)
        return (true);
    if (rc == -1)
        return (false);

    ASSERT(rc == 0);

    const unsigned int old_talents = your_talents(false).size();

    bool gain_msg = true;

    switch (mutat)
    {
    case MUT_STRONG: case MUT_AGILE:  case MUT_CLEVER:
    case MUT_WEAK:   case MUT_CLUMSY: case MUT_DOPEY:
        mprf(MSGCH_MUTATION, "You feel %s.", _stat_mut_desc(mutat, true));
        gain_msg = false;
        break;

        // FIXME: these cases should be handled better.
    case MUT_HOOVES:
    case MUT_TALONS:
        mpr(mdef.gain[you.mutation[mutat]], MSGCH_MUTATION);
        gain_msg = false;

        // Hooves and talons force boots off at 3,
        // check for level 2 or higher here.
        if (you.mutation[mutat] >= 2 && !you.melded[EQ_BOOTS])
            remove_one_equip(EQ_BOOTS, false, true);
        break;

    case MUT_CLAWS:
        mpr(mdef.gain[you.mutation[mutat]], MSGCH_MUTATION);
        gain_msg = false;

        // Gloves aren't prevented until level 3.  We don't have the
        // mutation yet, so we have to check for level 2 or higher claws
        // here.
        if (you.mutation[mutat] >= 2 && !you.melded[EQ_GLOVES])
            remove_one_equip(EQ_GLOVES, false, true);
        break;

    case MUT_ANTENNAE:
    case MUT_HORNS:
    case MUT_BEAK:
        mpr(mdef.gain[you.mutation[mutat]], MSGCH_MUTATION);
        gain_msg = false;

        // Horns, beaks, and antennae force hard helmets off.
        if (you.equip[EQ_HELMET] != -1
            && is_hard_helmet(you.inv[you.equip[EQ_HELMET]])
            && !you.melded[EQ_HELMET])
        {
            remove_one_equip(EQ_HELMET, false, true);
        }
        break;

    case MUT_ACUTE_VISION:
        // We might have to turn autopickup back on again.
        mpr(mdef.gain[you.mutation[mutat]], MSGCH_MUTATION);
        gain_msg = false;
        autotoggle_autopickup(false);
        break;

    case MUT_NIGHTSTALKER:
        if(player_mutation_level(mutat) > 1)
        {
            you.current_vision -= 2;
            set_los_radius(you.current_vision);
        }
        break;

    default:
        break;
    }

    // For all those scale mutations.
    you.redraw_armour_class = true;

    you.mutation[mutat]++;

    notify_stat_change("losing a mutation");

    if (gain_msg)
        mpr(mdef.gain[you.mutation[mutat]-1], MSGCH_MUTATION);

    // Do post-mutation effects.
    if (mutat == MUT_FRAIL || mutat == MUT_ROBUST)
        calc_hp();
    if (mutat == MUT_LOW_MAGIC || mutat == MUT_HIGH_MAGIC)
        calc_mp();
    if (mutat == MUT_PASSIVE_MAPPING)
        apply_to_all_dungeons(_reautomap_callback);

    // Amusement value will be 16 * (11-rarity) * Xom's-sense-of-humor.
    xom_is_stimulated(_calc_mutation_amusement_value(mutat));

    take_note(Note(NOTE_GET_MUTATION, mutat, you.mutation[mutat]));

    if (Tutorial.tutorial_left && your_talents(false).size() > old_talents)
        learned_something_new(TUT_NEW_ABILITY_MUT);

    return (true);
}

static bool _delete_single_mutation_level(mutation_type mutat)
{
    if (you.mutation[mutat] == 0)
        return (false);

    if (you.demon_pow[mutat] >= you.mutation[mutat])
        return (false);

    const mutation_def& mdef = get_mutation_def(mutat);

    bool lose_msg = true;

    switch (mutat)
    {
    case MUT_STRONG: case MUT_AGILE:  case MUT_CLEVER:
    case MUT_WEAK:   case MUT_CLUMSY: case MUT_DOPEY:
        mprf(MSGCH_MUTATION, "You feel %s.", _stat_mut_desc(mutat, false));
        lose_msg = false;
        break;

    case MUT_BREATHE_POISON:
        // can't be removed yet, but still covered:
        if (you.species == SP_NAGA)
        {
            // natural ability to spit poison retakes the slot
            for (int i = 0; i < 52; ++i)
            {
                if (you.ability_letter_table[i] == ABIL_BREATHE_POISON)
                    you.ability_letter_table[i] = ABIL_SPIT_POISON;
            }
        }
        break;

    default:
        break;
    }

    // For all those scale mutations.
    you.redraw_armour_class = true;

    you.mutation[mutat]--;
    notify_stat_change("losing a mutation");

    if (lose_msg)
        mpr(mdef.lose[you.mutation[mutat]], MSGCH_MUTATION);

    // Do post-mutation effects.
    if (mutat == MUT_FRAIL || mutat == MUT_ROBUST)
        calc_hp();
    if (mutat == MUT_LOW_MAGIC || mutat == MUT_HIGH_MAGIC)
        calc_mp();

    take_note(Note(NOTE_LOSE_MUTATION, mutat, you.mutation[mutat]));

    return (true);
}

bool delete_mutation(mutation_type which_mutation, bool failMsg,
                     bool force_mutation, bool god_gift,
                     bool disallow_mismatch, bool non_fatal)
{
    ASSERT(!non_fatal || _is_random(which_mutation));

    if (!god_gift)
    {
        const god_type god =
            (crawl_state.is_god_acting()) ? crawl_state.which_god_acting()
                                          : GOD_NO_GOD;

        if (god != GOD_NO_GOD)
            god_gift = true;
    }

    mutation_type mutat = which_mutation;

    if (!force_mutation)
    {
        if (!god_gift)
        {
            if (player_mutation_level(MUT_MUTATION_RESISTANCE) > 1
                && (player_mutation_level(MUT_MUTATION_RESISTANCE) == 3
                    || coinflip()))
            {
                if (failMsg)
                    mpr("You feel rather odd for a moment.", MSGCH_MUTATION);
                return (false);
            }
        }
    }

    if (which_mutation == RANDOM_MUTATION
        || which_mutation == RANDOM_XOM_MUTATION
        || which_mutation == RANDOM_GOOD_MUTATION
        || which_mutation == RANDOM_BAD_MUTATION)
    {
        while (true)
        {
            if (one_chance_in(1000))
                return (false);

            mutat = static_cast<mutation_type>(random2(NUM_MUTATIONS));

            if (you.mutation[mutat] == 0
                && mutat != MUT_STRONG
                && mutat != MUT_CLEVER
                && mutat != MUT_AGILE
                && mutat != MUT_WEAK
                && mutat != MUT_DOPEY
                && mutat != MUT_CLUMSY)
            {
                continue;
            }

            if (!_accept_mutation(mutat, true, non_fatal, true))
                continue;

            if (you.demon_pow[mutat] >= you.mutation[mutat])
                continue;

            const mutation_def& mdef = get_mutation_def(mutat);

            if (random2(10) >= mdef.rarity)
                continue;

            const bool mismatch =
                (which_mutation == RANDOM_GOOD_MUTATION && mdef.bad)
                    || (which_mutation == RANDOM_BAD_MUTATION && !mdef.bad);

            if (mismatch && (disallow_mismatch || !one_chance_in(10)))
                continue;

            break;
        }
    }

    return (_delete_single_mutation_level(mutat));
}

bool delete_all_mutations()
{
    for (int i = 0; i < NUM_MUTATIONS; ++i)
    {
        while (_delete_single_mutation_level(static_cast<mutation_type>(i)))
            ;
    }

    return (!how_mutated());
}

static const mutation_type _all_scales[] = {
    MUT_DISTORTION_FIELD,           MUT_ICY_BLUE_SCALES,
    MUT_IRIDESCENT_SCALES,          MUT_LARGE_BONE_PLATES,
    MUT_MOLTEN_SCALES,              MUT_ROUGH_BLACK_SCALES,
    MUT_RUGGED_BROWN_SCALES,        MUT_SLIMY_GREEN_SCALES,
    MUT_THIN_METALLIC_SCALES,       MUT_THIN_SKELETAL_STRUCTURE,
    MUT_YELLOW_SCALES,
};

static int _is_covering(mutation_type mut)
{
    for (unsigned i = 0; i < ARRAYSZ(_all_scales); ++i)
        if (_all_scales[i] == mut)
            return 1;

    return 0;
}

static int _body_covered()
{
    // Check how much of your body is covered by scales, etc.
    int covered = 0;

    if (you.species == SP_NAGA)
        covered++;

    if (player_genus(GENPC_DRACONIAN))
        covered += 3;


    for (unsigned i = 0; i < ARRAYSZ(_all_scales); ++i)
        covered += you.mutation[_all_scales[i]];

    return (covered);
}

// Return a string describing the mutation.
// If colour is true, also add the colour annotation.
std::string mutation_name(mutation_type mut, int level, bool colour)
{
    const bool fully_active = mutation_is_fully_active(mut);
    const bool fully_inactive =
        (!fully_active && _mutation_is_fully_inactive(mut));

    // level == -1 means default action of current level
    if (level == -1)
    {
        if (!fully_inactive)
            level = player_mutation_level(mut);
        else // give description of fully active mutation
            level = you.mutation[mut];
    }

    std::string result;
    bool innate = false;

    if (mut == MUT_CLAWS
        && (you.species == SP_TROLL || you.species == SP_GHOUL))
    {
        innate = true;
        if (you.species == SP_TROLL)
            result = troll_claw_descrip[level];
    }

    if ((mut == MUT_FAST || mut == MUT_BREATHE_POISON)
        && you.species == SP_NAGA)
    {
        innate = true;
        if (mut == MUT_FAST)
            result = naga_speed_descrip[level];
    }

    const mutation_def& mdef = get_mutation_def(mut);

    if (mut == MUT_STRONG || mut == MUT_CLEVER
        || mut == MUT_AGILE || mut == MUT_WEAK
        || mut == MUT_DOPEY || mut == MUT_CLUMSY)
    {
        std::ostringstream ostr;
        ostr << mdef.have[0] << level << ").";
        result = ostr.str();
    }
    else if (mut == MUT_ICEMAIL)
    {
        std::ostringstream ostr;

        int amt = ICEMAIL_MAX;
        amt -= you.duration[DUR_ICEMAIL_DEPLETED] * ICEMAIL_MAX / ICEMAIL_TIME;

        ostr << mdef.have[0] << amt << ").";
        result = ostr.str();
    }
    else if (result.empty() && level > 0)
        result = mdef.have[level - 1];

    if (fully_inactive || player_mutation_level(mut) < you.mutation[mut])
    {
        result = "(" + result;
        result += ")";
    }

    if (colour)
    {
        const char* colourname = "lightgrey"; // the default
        const bool permanent   = (you.demon_pow[mut] > 0);
        if (innate)
            colourname = (level > 0 ? "cyan" : "lightblue");
        else if (permanent)
        {
            const bool demonspawn = (you.species == SP_DEMONSPAWN);
            const bool extra = (you.mutation[mut] > you.demon_pow[mut]);

            if (fully_inactive)
                colourname = "darkgrey";
            else if (!fully_active)
                colourname = demonspawn ? "yellow"   : "blue";
            else if (extra)
                colourname = demonspawn ? "lightred" : "cyan";
            else
                colourname = demonspawn ? "red"      : "lightblue";
        }
        else if (fully_inactive)
            colourname = "darkgrey";

        // Build the result
        std::ostringstream ostr;
        ostr << '<' << colourname << '>' << result
             << "</" << colourname << ">";
        result = ostr.str();
    }

    return (result);
}

struct facet_def
{
    mutation_type muts[3];
    int tiers[3];
};

struct demon_mutation_info
{
    mutation_type mut;
    int tier;
    int facet;

    demon_mutation_info(mutation_type m, int t, int f)
        : mut(m), tier(t), facet(f) { }
};

static int ct_of_tier[] = { 0, 2, 3, 1 };

static const facet_def _demon_facets[] =
{
    // Body Slot facets
    { { MUT_CLAWS, MUT_CLAWS, MUT_CLAWS },
      { 2, 2, 2 } },
    { { MUT_HORNS, MUT_HORNS, MUT_HORNS },
      { 2, 2, 2 } },
    { { MUT_ANTENNAE, MUT_ANTENNAE, MUT_ANTENNAE },
      { 2, 2, 2 } },
    { { MUT_HOOVES, MUT_HOOVES, MUT_HOOVES },
      { 2, 2, 2 } },
    { { MUT_TALONS, MUT_TALONS, MUT_TALONS },
      { 2, 2, 2 } },
    // Regular facets
    { { MUT_THROW_FLAMES, MUT_HEAT_RESISTANCE, MUT_HURL_HELLFIRE },
      { 3, 3, 3 } },
    { { MUT_THROW_FLAMES, MUT_HEAT_RESISTANCE, MUT_CONSERVE_SCROLLS },
      { 3, 3, 3 } },
    { { MUT_FAST, MUT_FAST, MUT_FAST },
      { 3, 3, 3 } },
    { { MUT_ROBUST, MUT_ROBUST, MUT_ROBUST },
      { 3, 3, 3 } },
    { { MUT_POWERED_BY_DEATH, MUT_POWERED_BY_DEATH, MUT_POWERED_BY_DEATH },
      { 3, 3, 3 } },
    { { MUT_NEGATIVE_ENERGY_RESISTANCE, MUT_NEGATIVE_ENERGY_RESISTANCE,
          MUT_NEGATIVE_ENERGY_RESISTANCE },
      { 3, 3, 3 } },
    { { MUT_STOCHASTIC_TORMENT_RESISTANCE, MUT_STOCHASTIC_TORMENT_RESISTANCE,
          MUT_STOCHASTIC_TORMENT_RESISTANCE },
      { 3, 3, 3 } },
    { { MUT_REGENERATION, MUT_REGENERATION, MUT_REGENERATION },
      { 2, 2, 2 } },
    { { MUT_MAGIC_RESISTANCE, MUT_MAGIC_RESISTANCE, MUT_MAGIC_RESISTANCE },
      { 2, 2, 2 } },
    { { MUT_BREATHE_FLAMES, MUT_BREATHE_FLAMES, MUT_BREATHE_FLAMES },
      { 2, 2, 2 } },
    { { MUT_PASSIVE_MAPPING, MUT_PASSIVE_MAPPING, MUT_PASSIVE_MAPPING },
      { 2, 2, 2 } },
    { { MUT_COLD_RESISTANCE, MUT_CONSERVE_POTIONS, MUT_ICEMAIL },
      { 2, 2, 2 } },
    { { MUT_COLD_RESISTANCE, MUT_CONSERVE_POTIONS, MUT_PASSIVE_FREEZE },
      { 2, 2, 2 } },
    { { MUT_DEMONIC_GUARDIAN, MUT_DEMONIC_GUARDIAN, MUT_DEMONIC_GUARDIAN },
      { 2, 2, 2 } },
    { { MUT_NIGHTSTALKER, MUT_NIGHTSTALKER, MUT_NIGHTSTALKER },
      { 2, 2, 2 } },
    { { MUT_SPINY, MUT_SPINY, MUT_SPINY },
      { 2, 2, 2 } },
    { { MUT_TOUGH_SKIN, MUT_TOUGH_SKIN, MUT_TOUGH_SKIN },
      { 1, 1, 1 } },
    { { MUT_SLOW_METABOLISM, MUT_SLOW_METABOLISM, MUT_SLOW_METABOLISM },
      { 1, 1, 1 } },
    { { MUT_SPIT_POISON, MUT_SPIT_POISON, MUT_SPIT_POISON },
      { 1, 1, 1 } },
    { { MUT_HIGH_MAGIC, MUT_HIGH_MAGIC, MUT_HIGH_MAGIC },
      { 1, 1, 1 } },
    // Scale mutations
    { { MUT_DISTORTION_FIELD, MUT_DISTORTION_FIELD, MUT_DISTORTION_FIELD },
      { 1, 1, 1 } },
    { { MUT_ICY_BLUE_SCALES, MUT_ICY_BLUE_SCALES, MUT_ICY_BLUE_SCALES },
      { 1, 1, 1 } },
    { { MUT_IRIDESCENT_SCALES, MUT_IRIDESCENT_SCALES, MUT_IRIDESCENT_SCALES },
      { 1, 1, 1 } },
    { { MUT_LARGE_BONE_PLATES, MUT_LARGE_BONE_PLATES, MUT_LARGE_BONE_PLATES },
      { 1, 1, 1 } },
    { { MUT_MOLTEN_SCALES, MUT_MOLTEN_SCALES, MUT_MOLTEN_SCALES },
      { 1, 1, 1 } },
    { { MUT_ROUGH_BLACK_SCALES, MUT_ROUGH_BLACK_SCALES, MUT_ROUGH_BLACK_SCALES},
      { 1, 1, 1 } },
    { { MUT_RUGGED_BROWN_SCALES, MUT_RUGGED_BROWN_SCALES,
        MUT_RUGGED_BROWN_SCALES },
      { 1, 1, 1 } },
    { { MUT_SLIMY_GREEN_SCALES, MUT_SLIMY_GREEN_SCALES, MUT_SLIMY_GREEN_SCALES},
      { 1, 1, 1 } },
    { { MUT_THIN_METALLIC_SCALES, MUT_THIN_METALLIC_SCALES,
        MUT_THIN_METALLIC_SCALES },
      { 1, 1, 1 } },
    { { MUT_THIN_SKELETAL_STRUCTURE, MUT_THIN_SKELETAL_STRUCTURE,
        MUT_THIN_SKELETAL_STRUCTURE },
      { 1, 1, 1 } },
    { { MUT_YELLOW_SCALES, MUT_YELLOW_SCALES, MUT_YELLOW_SCALES },
      { 1, 1, 1 } }
};

static bool _works_at_tier(const facet_def& facet, int tier)
{
    return facet.tiers[0] == tier
        || facet.tiers[1] == tier
        || facet.tiers[2] == tier;
}

static int _rank_for_tier(const facet_def& facet, int tier)
{
    int k;

    for (k = 0; k < 3 && facet.tiers[k] <= tier; ++k);

    return (k);
}

static std::vector<demon_mutation_info> _select_ds_mutations()
{
try_again:
    std::vector<demon_mutation_info> ret;

    ret.clear();
    int absfacet = 0;
    int scales = 0;
    int slow_dig = 0;
    int regen = 0;
    int slots_lost = 0;
    int breath_weapons = 0;
    int elemental = 0;

    std::set<const facet_def *> facets_used;

    for (int tier = ARRAYSZ(ct_of_tier) - 1; tier >= 0; --tier)
    {
        for (int nfacet = 0; nfacet < ct_of_tier[tier]; ++nfacet)
        {
            const facet_def* next_facet;

            do
            {
                next_facet = &RANDOM_ELEMENT(_demon_facets);
            }
            while (!_works_at_tier(*next_facet, tier)
                   || facets_used.find(next_facet) != facets_used.end());

            facets_used.insert(next_facet);

            for (int i = 0; i < _rank_for_tier(*next_facet, tier); ++i)
            {
                mutation_type m = next_facet->muts[i];

                ret.push_back(demon_mutation_info(m, next_facet->tiers[i],
                                                  absfacet));

                if (_is_covering(m))
                    ++scales;

                if (m == MUT_SLOW_METABOLISM)
                    slow_dig = 1;

                if (m == MUT_REGENERATION)
                    regen = 1;

                if (m == MUT_SPIT_POISON || m == MUT_BREATHE_POISON
                        || m == MUT_BREATHE_FLAMES)
                    breath_weapons++;

                if (m == MUT_COLD_RESISTANCE || m == MUT_BREATHE_FLAMES)
                    elemental++;

                if (m == MUT_CLAWS && i == 2 || m == MUT_HORNS && i == 0 ||
                    m == MUT_BEAK && i == 0 || m == MUT_ANTENNAE && i == 0 ||
                    m == MUT_TALONS && i == 2 || m == MUT_HOOVES && i == 2)
                    ++slots_lost;
            }

            ++absfacet;
        }
    }

    if (scales > 3)
        goto try_again;

    if (slots_lost != 1)
        goto try_again;

    if (slow_dig && regen)
        goto try_again;

    if (breath_weapons > 1)
        goto try_again;

    if (elemental > 1)
        goto try_again;

    return ret;
}

static std::vector<mutation_type>
_order_ds_mutations(std::vector<demon_mutation_info> muts)
{
    std::vector<mutation_type> out;

    while (! muts.empty())
    {
        int first_tier = 99;

        for (unsigned i = 0; i < muts.size(); ++i)
        {
            first_tier = std::min(first_tier, muts[i].tier);
        }

        int ix;

        do
        {
            ix = random2(muts.size());
        }
        // Don't consider mutations from more than two tiers at a time
        while (muts[ix].tier >= first_tier + 2);

        // Don't reorder mutations within a facet
        for (int j = 0; j < ix; ++j)
        {
            if (muts[j].facet == muts[ix].facet)
            {
                ix = j;
                break;
            }
        }

        out.push_back(muts[ix].mut);
        muts.erase(muts.begin() + ix);
    }

    return out;
}

static std::vector<player::demon_trait>
_schedule_ds_mutations(std::vector<mutation_type> muts)
{
    std::list<mutation_type> muts_left(muts.begin(), muts.end());

    std::list<int> slots_left;

    std::vector<player::demon_trait> out;

    for (int level = 2; level <= 27; ++level)
    {
        int ct = coinflip() ? 2 : 1;

        for (int i = 0; i < ct; ++i)
            slots_left.push_back(level);
    }

    while (!muts_left.empty())
    {
        if (x_chance_in_y(muts_left.size(), slots_left.size()))
        {
            player::demon_trait dt;

            dt.level_gained = slots_left.front();
            dt.mutation     = muts_left.front();

#ifdef DEBUG_DIAGNOSTICS
            mprf(MSGCH_DIAGNOSTICS, "Demonspawn will gain %s at level %d",
                    get_mutation_def(dt.mutation).wizname, dt.level_gained);
#endif

            out.push_back(dt);

            muts_left.pop_front();
        }
        slots_left.pop_front();
    }

    return out;
}

void roll_demonspawn_mutations()
{
    you.demonic_traits = _schedule_ds_mutations(
                         _order_ds_mutations(
                         _select_ds_mutations()));
}

bool perma_mutate(mutation_type which_mut, int how_much)
{
    ASSERT(is_valid_mutation(which_mut));

    int levels = 0;

    how_much = std::min(static_cast<short>(how_much),
                        get_mutation_def(which_mut).levels);

    while (how_much-- > 0)
        if (mutate(which_mut, false, true, false, false, true))
            levels++;

    you.demon_pow[which_mut] += levels;

    return (levels > 0);
}

int how_mutated(bool all, bool levels)
{
    int j = 0;

    for (int i = 0; i < NUM_MUTATIONS; ++i)
    {
        if (you.mutation[i])
        {
            if (!all && you.demon_pow[i] >= you.mutation[i])
                continue;

            if (levels)
            {
                // These allow for 14 levels.
                if (i == MUT_STRONG || i == MUT_CLEVER || i == MUT_AGILE
                    || i == MUT_WEAK || i == MUT_DOPEY || i == MUT_CLUMSY)
                {
                    j += (you.mutation[i] / 5 + 1);
                }
                else
                    j += you.mutation[i];
            }
            else
                j++;
        }
    }

    dprf("how_mutated(): all = %u, levels = %u, j = %d", all, levels, j);

    return (j);
}

bool give_bad_mutation(bool failMsg, bool force_mutation, bool non_fatal)
{
    const mutation_type bad_muts[] = {
        MUT_CARNIVOROUS,   MUT_HERBIVOROUS,   MUT_FAST_METABOLISM,
        MUT_WEAK,          MUT_DOPEY,
        MUT_CLUMSY,        MUT_TELEPORT,      MUT_DEFORMED,
        MUT_SCREAM,        MUT_DETERIORATION, MUT_BLURRY_VISION,
        MUT_FRAIL,         MUT_LOW_MAGIC
    };

    mutation_type mutat;

    do
        mutat = RANDOM_ELEMENT(bad_muts);
    while (non_fatal && !_accept_mutation(mutat, true, true));

    const bool result = mutate(mutat, failMsg, force_mutation);
    if (result)
        learned_something_new(TUT_YOU_MUTATED);

    return (result);
}

void check_demonic_guardian()
{
    const int mutlevel = player_mutation_level(MUT_DEMONIC_GUARDIAN);
    if(!you.active_demonic_guardian && you.duration[DUR_DEMONIC_GUARDIAN] == 0
       && !you.disable_demonic_guardian && one_chance_in(mutlevel*100))
    {
        const monster_type mt = static_cast<monster_type>(
                            MONS_WHITE_IMP + (mutlevel-1)*5 + random2(5));
        const int guardian = create_monster(mgen_data(mt, BEH_FRIENDLY, &you,
                                                      0, 0, you.pos(),
                                                      MHITYOU, MG_FORCE_BEH));

        if (guardian == -1)
            return;

        menv[guardian].flags |= MF_NO_REWARD;
        menv[guardian].flags |= MF_DEMONIC_GUARDIAN;

        you.active_demonic_guardian = true;
    }
}

void check_antennae_detect()
{
    // we're already here, so the player has at least 1 level of Antennae
    const int radius = player_mutation_level(MUT_ANTENNAE) == 1 ? 3 : 5;

    for(radius_iterator ri(you.pos(), radius, C_SQUARE);ri; ++ri)
    {
        if (monster_at(*ri))
        {
            show_type st;
            st.cls = SH_INVIS_EXPOSED;
            st.colour = RED;

            set_map_knowledge_obj(*ri, st);
            set_map_knowledge_detected_mons(*ri);
        }
    }
}

int count_pbd_corpses()
{
    int corpse_count = 0;
    for(radius_iterator ri(you.pos(),
        player_mutation_level(MUT_POWERED_BY_DEATH)*3); ri; ++ri)
    {
        for (stack_iterator j(*ri); j; ++j)
        {
            if (j->base_type == OBJ_CORPSES && j->sub_type == CORPSE_BODY
                && j->special > 50)
            {
                ++corpse_count;
            }
        }
    }

    if (corpse_count > 7)
        corpse_count = 7;

    return corpse_count;
}
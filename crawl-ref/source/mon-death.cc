/*
 *  File:       mon-death.cc
 *  Summary:    Monster death functionality (kraken, uniques, and so-on).
 */

#include "AppHdr.h"
#include "mon-death.h"

#include "areas.h"
#include "coordit.h"
#include "database.h"
#include "env.h"
#include "items.h"
#include "message.h"
#include "mgen_data.h"
#include "mon-behv.h"
#include "mon-iter.h"
#include "mon-place.h"
#include "mon-speak.h"
#include "mon-stuff.h"
#include "mon-util.h"
#include "random.h"
#include "religion.h"
#include "state.h"
#include "stuff.h"
#include "transform.h"
#include "view.h"

// Pikel and band.
bool mons_is_pikel (monster* mons)
{
    return (mons->type == MONS_PIKEL
            || (mons->props.exists("original_name")
                && mons->props["original_name"].get_string() == "Pikel"));
}

void pikel_band_neutralise (bool check_tagged)
{
    bool message_made = false;

    for (monster_iterator mi; mi; ++mi)
    {
        if (mi->type == MONS_SLAVE
            && testbits(mi->flags, MF_BAND_MEMBER)
            && mi->props.exists("pikel_band")
            && mi->mname != "freed slave")
        {
            // Don't neutralise band members that are leaving the level with us.
            if (check_tagged && testbits(mi->flags, MF_TAKING_STAIRS))
                continue;

            if (mi->observable() && !message_made)
            {
                if (check_tagged)
                    mprf("With Pikel's spell partly broken, some of the slaves are set free!");
                else
                    mprf("With Pikel's spell broken, the former slaves thank you for their freedom.");

                message_made = true;
            }
            mi->flags |= MF_NAME_REPLACE | MF_NAME_DESCRIPTOR;
            mi->mname = "freed slave";
            mons_pacify(*mi);
        }
    }
}

// Kirke and band
bool mons_is_kirke (monster* mons)
{
    return (mons->type == MONS_KIRKE
            || (mons->props.exists("original_name")
                && mons->props["original_name"].get_string() == "Kirke"));
}

void hogs_to_humans()
{
    // Simplification: if, in a rare event, another hog which was not created
    // as a part of Kirke's band happens to be on the level, the player can't
    // tell them apart anyway.
    // On the other hand, hogs which left the level are too far away to be
    // affected by the magic of Kirke's death.
    int any = 0, human = 0;

    for (monster_iterator mi; mi; ++mi)
    {
        if (mi->type != MONS_HOG)
            continue;

        // Shapeshifters will stop being a hog when they feel like it.
        if (mi->is_shapeshifter())
            continue;

        const bool could_see = you.can_see(*mi);

        monster orig;

        if (mi->props.exists(ORIG_MONSTER_KEY))
            // Copy it, since the instance in props will get deleted
            // as soon a **mi is assigned to.
            orig = mi->props[ORIG_MONSTER_KEY].get_monster();
        else
        {
            orig.type     = MONS_HUMAN;
            orig.attitude = mi->attitude;
            define_monster(&orig);
        }

        // Keep at same spot.
        const coord_def pos = mi->pos();
        // Preserve relative HP.
        const float hp
            = (float) mi->hit_points / (float) mi->max_hit_points;
        // Preserve some flags.
        const unsigned long preserve_flags =
            mi->flags & ~(MF_JUST_SUMMONED | MF_WAS_IN_VIEW);
        // Preserve enchantments.
        mon_enchant_list enchantments = mi->enchantments;

        // Restore original monster.
        **mi = orig;

        mi->move_to_pos(pos);
        mi->enchantments = enchantments;
        mi->hit_points   = std::max(1, (int) (mi->max_hit_points * hp));
        mi->flags        = mi->flags | preserve_flags;

        const bool can_see = you.can_see(*mi);

        // A monster changing factions while in the arena messes up
        // arena book-keeping.
        if (!crawl_state.game_is_arena())
        {
            // * A monster's attitude shouldn't downgrade from friendly
            //   or good-neutral because you helped it.  It'd suck to
            //   lose a permanent ally that way.
            //
            // * A monster has to be smart enough to realize that you
            //   helped it.
            if (mi->attitude == ATT_HOSTILE
                && mons_intel(*mi) >= I_NORMAL)
            {
                mi->attitude = ATT_GOOD_NEUTRAL;
                mi->flags   |= MF_WAS_NEUTRAL;
                mons_att_changed(*mi);
            }
        }

        behaviour_event(*mi, ME_EVAL);

        if (could_see && can_see)
        {
            any++;
            if (mi->type == MONS_HUMAN)
                human++;
        }
        else if (could_see && !can_see)
            mpr("The hog vanishes!");
        else if (!could_see && can_see)
            mprf("%s appears from out of thin air!",
                 mi->name(DESC_CAP_A).c_str());
    }

    if (any == 1)
    {
        if (any == human)
            mpr("No longer under Kirke's spell, the hog turns into a human!");
        else
            mpr("No longer under Kirke's spell, the hog returns to its "
                "original form!");
    }
    else if (any > 1)
    {
        if (any == human)
            mpr("No longer under Kirke's spell, all hogs revert to their "
                "human forms!");
        else
            mpr("No longer under Kirke's spell, all hogs revert to their "
                "original forms!");
    }

    // Revert the player as well.
    if (you.form == TRAN_PIG)
        untransform();
}


// Dowan and Duvessa
bool mons_is_dowan(const monster* mons)
{
    return (mons->type == MONS_DOWAN
            || (mons->props.exists("original_name")
                && mons->props["original_name"].get_string() == "Dowan"));
}

bool mons_is_duvessa(const monster* mons)
{
    return (mons->type == MONS_DUVESSA
            || (mons->props.exists("original_name")
                && mons->props["original_name"].get_string() == "Duvessa"));
}

bool mons_is_elven_twin(const monster* mons)
{
    return (mons_is_dowan(mons) || mons_is_duvessa(mons));
}

void elven_twin_died(monster* twin, bool in_transit, killer_type killer, int killer_index)
{
    // Sometimes, if you pacify one twin near a staircase, they leave
    // in the same turn. Convert, in those instances.
    if (twin->neutral())
    {
        elven_twins_pacify(twin);
        return;
    }

    bool found_duvessa = false;
    bool found_dowan = false;
    monster* mons;

    for (monster_iterator mi; mi; ++mi)
    {
        if (*mi == twin)
            continue;

        // Don't consider already neutralised monsters.
        if ((*mi)->good_neutral())
            continue;

        if (mons_is_duvessa(*mi))
        {
            mons = *mi;
            found_duvessa = true;
            break;
        }
        else if (mons_is_dowan(*mi))
        {
            mons = *mi;
            found_dowan = true;
            break;
        }
    }

    if (!found_duvessa && !found_dowan)
        return;

    // Okay, let them climb stairs now.
    mons->props["can_climb"] = "yes";
    if (!in_transit)
        mons->props["speech_prefix"] = "twin_died";
    else
        mons->props["speech_prefix"] = "twin_banished";

    // If you've stabbed one of them, the other one is likely asleep still.
    if (mons->asleep())
        behaviour_event(mons, ME_DISTURB, MHITNOT, mons->pos());

    // Will generate strings such as 'Duvessa_Duvessa_dies' or, alternately
    // 'Dowan_Dowan_dies', but as neither will match, these can safely be
    // ignored.
    std::string key = "_" + mons->name(DESC_CAP_THE, true) + "_"
                          + twin->name(DESC_CAP_THE) + "_dies_";

    if (mons_near(mons) && !mons->observable())
        key += "invisible_";
    else if (!mons_near(mons))
        key += "distance_";

    bool i_killed = ((killer == KILL_MON || killer == KILL_MON_MISSILE)
                      && mons->mindex() == killer_index);

    if (i_killed)
    {
        key += "bytwin_";
        mons->props["speech_prefix"] = "twin_ikilled";
    }

    std::string death_message = getSpeakString(key);

    // Check if they can speak or not: they may have been polymorphed.
    if (mons_near(mons) && !death_message.empty() && mons->can_speak())
        mons_speaks_msg(mons, death_message, MSGCH_TALK, silenced(you.pos()));
    else if (mons->can_speak())
        mprf("%s", death_message.c_str());

    if (found_duvessa)
    {
        if (mons_near(mons))
            // Provides its own flavour message.
            mons->go_berserk(true);
        else
            // She'll go berserk the next time she sees you
            mons->props["duvessa_berserk"] = bool(true);
    }
    else if (found_dowan)
    {
        if (mons->observable())
        {
            mons->add_ench(ENCH_HASTE);
            simple_monster_message(mons, " seems to find hidden reserves of power!");
        }
        else
            mons->props["dowan_upgrade"] = bool(true);

        mons->spells[0] = SPELL_THROW_ICICLE;
        mons->spells[1] = SPELL_BLINK;
        mons->spells[3] = SPELL_STONE_ARROW;
        mons->spells[4] = SPELL_HASTE;
        // Nothing with 6.
    }
}

// If you pacify one twin, the other also pacifies.
void elven_twins_pacify (monster* twin)
{
    bool found_duvessa = false;
    bool found_dowan = false;
    monster* mons;

    for (monster_iterator mi; mi; ++mi)
    {
        if (*mi == twin)
            continue;

        // Don't consider already neutralised monsters.
        if ((*mi)->neutral())
            continue;

        if (mons_is_duvessa(*mi))
        {
            mons = *mi;
            found_duvessa = true;
            break;
        }
        else if (mons_is_dowan(*mi))
        {
            mons = *mi;
            found_dowan = true;
            break;
        }
    }

    if (!found_duvessa && !found_dowan)
        return;

    // This shouldn't happen, but sometimes it does.
    if (mons->neutral())
        return;

    if (you.religion == GOD_ELYVILON)
        gain_piety(random2(mons->max_hit_points / (2 + you.piety / 20)), 2);

    if (mons_near(mons))
        simple_monster_message(mons, " likewise turns neutral.");

    mons_pacify(mons, ATT_NEUTRAL);
}

// And if you attack a pacified elven twin, the other will unpacify.
void elven_twins_unpacify (monster* twin)
{
    bool found_duvessa = false;
    bool found_dowan = false;
    monster* mons;

    for (monster_iterator mi; mi; ++mi)
    {
        if (*mi == twin)
            continue;

        // Don't consider already neutralised monsters.
        if (!(*mi)->neutral())
            continue;

        if (mons_is_duvessa(*mi))
        {
            mons = *mi;
            found_duvessa = true;
            break;
        }
        else if (mons_is_dowan(*mi))
        {
            mons = *mi;
            found_dowan = true;
            break;
        }
    }

    if (!found_duvessa && !found_dowan)
        return;

    behaviour_event(mons, ME_WHACK, MHITYOU, you.pos(), false);
}

// Spirits

void spirit_fades (monster *spirit)
{

    if (mons_near(spirit))
        simple_monster_message(spirit, " fades away with a wail!", MSGCH_TALK);
    else
        mprf("You hear a distant wailing.", MSGCH_TALK);

    const coord_def c = spirit->pos();

    mgen_data mon = mgen_data(static_cast<monster_type>(random_choose_weighted(
                        10, MONS_SILVER_STAR, 10, MONS_PHOENIX,
                        10, MONS_APIS,        5,  MONS_DAEVA,
                        2,  MONS_HOLY_DRAGON,
                        // No holy dragons
                      0)), SAME_ATTITUDE(spirit),
                      NULL, 0, 0, c,
                      spirit->foe, 0);

    if (spirit->alive())
        monster_die(spirit, KILL_MISC, NON_MONSTER, true);

    int mon_id = create_monster(mon);

    if (mon_id == -1)
        return;

    monster *new_mon = &menv[mon_id];

    if (mons_near(new_mon))
        simple_monster_message(new_mon, " seeks to avenge the fallen spirit!", MSGCH_TALK);
    else
        mprf("A powerful presence appears to avenge a fallen spirit!", MSGCH_TALK);

}

// Shedu
monster* get_shedu_pair (const monster* mons)
{
    monster* pair = monster_by_mid(mons->number);
    if (pair)
        return (pair);

    return (NULL);
}

bool shedu_pair_alive (const monster* mons)
{
    if (get_shedu_pair(mons) == NULL)
        return (false);

    return (true);
}

bool mons_is_shedu(const monster* mons)
{
    return (mons->type == MONS_SHEDU
            || (mons->props.exists("original_name")
                && mons->props["original_name"].get_string() == "shedu"));
}

void shedu_do_resurrection (const monster* mons)
{
    if (!mons_is_shedu(mons))
        return;

    if (mons->number == 0)
        return;

    monster* my_pair = get_shedu_pair(mons);
    if (!my_pair)
        return;

    if (you.can_see(my_pair))
        simple_monster_message(my_pair, " ceases action and prepares to resurrect its fallen mate.");

    my_pair->add_ench(ENCH_PREPARING_RESURRECT);
}

void shedu_do_actual_resurrection (monster* mons)
{
    // Here is where we actually recreate the dead
    // shedu.
    bool found_body = false;
    coord_def place_at;
    bool success = false;
    bool from_inventory = false;

    // Our pair might already be irretrievably dead.
    if (mons->number == 0)
        return;

    for (radius_iterator ri(mons->pos(), LOS_RADIUS, C_ROUND, mons->get_los()); ri; ++ri)
    {
        for (stack_iterator si(*ri); si; ++si)
            if (si->base_type == OBJ_CORPSES && si->sub_type == CORPSE_BODY
                && si->props.exists(MONSTER_MID)
                && static_cast<unsigned int>(si->props[MONSTER_MID].get_int()) == mons->number)
            {
                place_at = *ri;
                destroy_item(si->index());
                found_body = true;
                break;
            }
    }

    if (!found_body)
    {
        for (unsigned slot = 0; slot < ENDOFPACK; ++slot)
        {
            if (!you.inv[slot].defined())
                continue;

            item_def* si = &you.inv[slot];
            if (si->base_type == OBJ_CORPSES && si->sub_type == CORPSE_BODY
                && si->props.exists(MONSTER_MID)
                && static_cast<unsigned int>(si->props[MONSTER_MID].get_int()) == mons->number)
            {
                // it was in the player's inventory
                place_at = coord_def(-1, -1);
                dec_inv_item_quantity(slot, 1, false);
                found_body = true;
                from_inventory = true;
                break;
            }
        }

        if (found_body)
        {
            for (adjacent_iterator ai(mons->pos()); ai; ++ai)
                if ((place_at.origin() || one_chance_in(3))
                    && monster_habitable_grid(mons, grd(*ai)))
                {
                    place_at = *ai;
                }
        }
    }

    if (!found_body)
    {
        mons->number = 0;
        return;
    }

    mgen_data new_shedu;
    new_shedu.cls = MONS_SHEDU;
    new_shedu.behaviour = mons->behaviour;
    if (!place_at.origin())
        new_shedu.pos = place_at;
    new_shedu.foe = mons->foe;
    new_shedu.god = mons->god;

    // try placing at exact location
    bool at_right_place = (!new_shedu.pos.origin());
    int id = place_monster(new_shedu, true);

    // try elsewhere
    if (id == -1)
    {
        at_right_place = false;
        new_shedu.pos.reset();
        id = place_monster(new_shedu, false);
    }

    // give up
    if (id == -1)
    {
        dprf("Couldn't place new shedu!");
        return;
    }

    success = true;

    monster* my_pair = &menv[id];
    my_pair->number = mons->mid;
    mons->number = my_pair->mid;
    my_pair->flags |= MF_BAND_MEMBER;

    if (!at_right_place)
        my_pair->teleport(true);

    if (success && from_inventory)
        simple_monster_message(mons, " resurrects its mate from your pack!");
    else if (success && you.can_see(mons))
        simple_monster_message(mons, " resurrects its mate from the grave!");
}

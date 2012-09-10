/**
 * @file
 * @brief Brand/ench/etc effects that might alter something in an
 *             unexpected way.
**/

#include "AppHdr.h"
#include "coord.h"
#include "effects.h"
#include "env.h"
#include "fineff.h"
#include "libutil.h"
#include "mgen_data.h"
#include "misc.h"
#include "mon-place.h"
#include "ouch.h"
#include "religion.h"
#include "view.h"

void final_effect::schedule()
{
    for (vector<final_effect *>::iterator fi = env.final_effects.begin();
         fi != env.final_effects.end(); ++fi)
    {
        if (mergeable(**fi))
        {
            final_effect *fe = *fi;
            merge(*fe);
            *fi = this;
            delete fe;
            return;
        }
    }
    env.final_effects.push_back(this);
}

bool lightning_fineff::mergeable(const final_effect &fe) const
{
    const lightning_fineff *o = dynamic_cast<const lightning_fineff *>(&fe);
    return o && att == o->att && posn == o->posn;
}

bool mirror_damage_fineff::mergeable(const final_effect &fe) const
{
    const mirror_damage_fineff *o =
        dynamic_cast<const mirror_damage_fineff *>(&fe);
    return o && att == o->att && def == o->def;
}

bool trample_follow_fineff::mergeable(const final_effect &fe) const
{
    const trample_follow_fineff *o =
        dynamic_cast<const trample_follow_fineff *>(&fe);
    return o && att == o->att && posn == o->posn;
}

bool blink_fineff::mergeable(const final_effect &fe) const
{
    const blink_fineff *o = dynamic_cast<const blink_fineff *>(&fe);
    return o && def == o->def;
}

bool distortion_tele_fineff::mergeable(const final_effect &fe) const
{
    const distortion_tele_fineff *o =
        dynamic_cast<const distortion_tele_fineff *>(&fe);
    return o && def == o->def;
}

bool trj_spawn_fineff::mergeable(const final_effect &fe) const
{
    const trj_spawn_fineff *o = dynamic_cast<const trj_spawn_fineff *>(&fe);
    return o && att == o->att && def == o->def && posn == o->posn;
}

bool blood_fineff::mergeable(const final_effect &fe) const
{
    const blood_fineff *o = dynamic_cast<const blood_fineff *>(&fe);
    return o && posn == o->posn && mtype == o->mtype;
}


void mirror_damage_fineff::merge(const final_effect &fe)
{
    const mirror_damage_fineff *mdfe =
        dynamic_cast<const mirror_damage_fineff *>(&fe);
    ASSERT(mdfe && mergeable(*mdfe));
    damage += mdfe->damage;
}

void trj_spawn_fineff::merge(const final_effect &fe)
{
    const trj_spawn_fineff *trjfe =
        dynamic_cast<const trj_spawn_fineff *>(&fe);
    ASSERT(trjfe && mergeable(*trjfe));
    damage += trjfe->damage;
}

void blood_fineff::merge(const final_effect &fe)
{
    const blood_fineff *bfe = dynamic_cast<const blood_fineff *>(&fe);
    ASSERT(bfe && mergeable(*bfe));
    blood += bfe->blood;
}


void lightning_fineff::fire()
{
    if (you.see_cell(posn))
        mpr("Electricity arcs through the water!");
    conduct_electricity(posn, attacker());
}

void mirror_damage_fineff::fire()
{
    actor *attack = attacker();
    if (!attack || attack == defender() || !attack->alive())
        return;
    // defender being dead is ok, if we killed them we still suffer

    if (att == MID_PLAYER)
    {
        mpr("It reflects your damage back at you!");
        ouch(damage, NON_MONSTER, KILLED_BY_REFLECTION);
    }
    else if (def == MID_PLAYER)
    {
        simple_god_message(" mirrors your injury!");
#ifndef USE_TILE_LOCAL
        flash_monster_colour(monster_by_mid(att), RED, 200);
#endif

        attack->hurt(&you, damage);

        if (attack->alive())
            print_wounds(monster_by_mid(att));

        lose_piety(isqrt_ceil(damage));
    }
    else
    {
        simple_monster_message(monster_by_mid(att), " suffers a backlash!");
        attack->hurt(defender(), damage);
    }
}

void trample_follow_fineff::fire()
{
    actor *attack = attacker();
    if (attack
        && attack->pos() != posn
        && adjacent(attack->pos(), posn)
        && attack->is_habitable(posn))
    {
        attack->move_to_pos(posn);
    }
}

void blink_fineff::fire()
{
    actor *defend = defender();
    if (defend && defend->alive() && !defend->no_tele(true, false))
        defend->blink();
}

void distortion_tele_fineff::fire()
{
    actor *defend = defender();
    if (defend && defend->alive() && !defend->no_tele(true, false))
        defend->teleport(true, one_chance_in(5));
}

void trj_spawn_fineff::fire()
{
    const actor *attack = attacker();
    actor *trj = defender();

    int tospawn = div_rand_round(damage, 12);

    if (tospawn <= 0)
        return;

    dprf("Trying to spawn %d jellies.", tospawn);

    unsigned short foe = attack && attack->alive() ? attack->mindex()
                                                       : MHITNOT;

    int spawned = 0;
    for (int i = 0; i < tospawn; ++i)
    {
        const monster_type jelly = royal_jelly_ejectable_monster();
        coord_def jpos = find_newmons_square_contiguous(jelly, posn);
        if (!in_bounds(jpos))
            continue;

        if (monster *mons = mons_place(
                              mgen_data(jelly, BEH_HOSTILE, trj, 0, 0,
                                        jpos, foe, MG_DONT_COME, GOD_JIYVA)))
        {
            // Don't allow milking the royal jelly.
            mons->flags |= MF_NO_REWARD;
            spawned++;
        }
    }

    if (!spawned || !you.see_cell(posn))
        return;

    if (trj)
    {
        const string monnam = trj->name(DESC_THE);
        mprf("%s shudders%s.", monnam.c_str(),
             spawned >= 5 ? " alarmingly" :
             spawned >= 3 ? " violently" :
             spawned > 1 ? " vigorously" : "");

        if (spawned == 1)
            mprf("%s spits out another jelly.", monnam.c_str());
        else
        {
            mprf("%s spits out %s more jellies.",
                 monnam.c_str(),
                 number_in_words(spawned).c_str());
        }
    }
    else if (spawned == 1)
        mpr("One of the Royal Jelly's fragments survives.");
    else
    {
        mprf("The dying Royal Jelly spits out %s more jellies.",
             number_in_words(spawned).c_str());
    }
}

void blood_fineff::fire()
{
    bleed_onto_floor(posn, mtype, blood, true);
}

// Effects that occur after all other effects, even if the monster is dead.
// For example, explosions that would hit other creatures, but we want
// to deal with only one creature at a time, so that's handled last.
void fire_final_effects()
{
    for (unsigned int i = 0; i < env.final_effects.size(); ++i)
        env.final_effects[i]->fire();

    // Delete the allocated final_effects in a separate pass; otherwise,
    // if one fineff schedules another, we would dereference a deleted object
    // when checking mergeability in final_effect::schedule().
    for (unsigned int i = 0; i < env.final_effects.size(); ++i)
        delete env.final_effects[i];
    env.final_effects.clear();
}

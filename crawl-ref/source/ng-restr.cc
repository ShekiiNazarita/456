/**
 * @file
 * @brief Character choice restrictions.
 *
 * The functions in this file are "pure": They don't
 * access any global data.
**/

#include "AppHdr.h"

#include "ng-restr.h"

#include "jobs.h"
#include "newgame.h"
#include "newgame-def.h"
#include "size-type.h"
#include "species.h"

static bool _banned_combination(job_type job, species_type species)
{
    switch (species)
    {
    case SP_FELID:
    case SP_GOLEM:
    case SP_FELID_MUMMY:
        if (job == JOB_GLADIATOR
            || job == JOB_ASSASSIN
            || job == JOB_HUNTER
            || job == JOB_ARCANE_MARKSMAN)
        {
            return true;
        }
        break;
    case SP_HEDGEHOG:
        if (job == JOB_TORPOR_KNIGHT)
            return true;
        break;
    case SP_DEMIGOD:
        if (job == JOB_BERSERKER
            || job == JOB_CHAOS_KNIGHT
            || job == JOB_ABYSSAL_KNIGHT
            || job == JOB_SLIME_PRIEST
            || job == JOB_BLOOD_KNIGHT
            || job == JOB_TORPOR_KNIGHT
            || job == JOB_ANNIHILATOR
            || job == JOB_MONK)
        {
            return true;
        }
        break;
    default:
        break;
    }

    if (job == JOB_TRANSMUTER
        && (species_undead_type(species) == US_UNDEAD
           || species_undead_type(species) == US_HUNGRY_DEAD
           || species == SP_ROBOT))
    {
        return true;	   
    }
    if (job == JOB_ALCHEMIST && (species == SP_MUMMY || species == SP_FELID_MUMMY))
        return true;

    return false;
}

char_choice_restriction species_allowed(job_type job, species_type speci)
{
    if (!is_starting_species(speci) || !is_starting_job(job))
        return CC_BANNED;

    if (_banned_combination(job, speci))
        return CC_BANNED;

    if (job_recommends_species(job, speci))
        return CC_UNRESTRICTED;

    return CC_RESTRICTED;
}

char_choice_restriction job_allowed(species_type speci, job_type job)
{
    if (!is_starting_species(speci) || !is_starting_job(job))
        return CC_BANNED;

    if (_banned_combination(job, speci))
        return CC_BANNED;

    if (species_recommends_job(speci, job))
        return CC_UNRESTRICTED;

    return CC_RESTRICTED;
}

bool is_good_combination(species_type spc, job_type job, bool species_first,
                         bool good)
{
    const char_choice_restriction restrict =
        species_first ? job_allowed(spc, job) : species_allowed(job, spc);

    if (good)
        return restrict == CC_UNRESTRICTED;

    return restrict != CC_BANNED;
}

// Is the given god restricted for the character defined by ng?
// Only uses ng.species and ng.job.
char_choice_restriction weapon_restriction(weapon_type wpn,
                                           const newgame_def &ng)
{
    ASSERT_RANGE(ng.species, 0, NUM_SPECIES);
    ASSERT_RANGE(ng.job, 0, NUM_JOBS);
    ASSERT(ng.species == SP_BASE_DRACONIAN
           || !species_is_draconian(ng.species));

    // Some special cases:

    if ((ng.species == SP_FELID || ng.species == SP_GOLEM || ng.species == SP_FELID_MUMMY)
        && wpn != WPN_UNARMED)
        return CC_BANNED;

    // These recommend short blades because they're good at stabbing,
    // but the fighter's armour hinders that.
    if ((ng.species == SP_NAGA || ng.species == SP_VAMPIRE)
         && ng.job == JOB_FIGHTER && wpn == WPN_RAPIER)
    {
        return CC_RESTRICTED;
    }
	
    if (ng.species == SP_UNIPODE 
        && (wpn == WPN_QUARTERSTAFF || wpn == WPN_SHORTBOW))
    {
       return CC_BANNED;
    }

    if (wpn == WPN_QUARTERSTAFF && ng.job != JOB_GLADIATOR
        && !(ng.job == JOB_FIGHTER && ng.species == SP_FORMICID))
    {
        return CC_BANNED;
    }

    // Javelins are always good, tomahawks not so much.
    if (wpn == WPN_THROWN)
    {
        return species_size(ng.species) >= SIZE_MEDIUM ? CC_UNRESTRICTED
                                                       : CC_RESTRICTED;
    }

    if (species_recommends_weapon(ng.species, wpn))
        return CC_UNRESTRICTED;

    return CC_RESTRICTED;
}

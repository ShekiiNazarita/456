/**
 * @file
 * @brief Monster-affecting enchantment spells.
 *           Other targeted enchantments are handled in spl-zap.cc.
**/

#include "AppHdr.h"

#include "spl-monench.h"

#include "beam.h"
#include "coordit.h"
#include "english.h" // apostrophise
#include "env.h"
#include "fight.h"
#include "losglobal.h"
#include "message.h"
#include "mon-tentacle.h"
#include "spl-util.h"
#include "stringutil.h" // make_stringf
#include "terrain.h"
#include "view.h"

int englaciate(coord_def where, int pow, actor *agent)
{
    actor *victim = actor_at(where);

    if (!victim || victim == agent)
        return 0;

    if (agent->is_monster() && mons_aligned(agent, victim))
        return 0; // don't let monsters hit friendlies

    monster* mons = victim->as_monster();

    // Skip some ineligable monster categories
    if (mons &&
        (mons_is_conjured(mons->type) || mons_is_firewood(*mons)
        || mons_is_tentacle_segment(mons->type)))
    {
        return 0;
    }

    if (victim->res_cold() > 0)
    {
        if (!mons)
            canned_msg(MSG_YOU_UNAFFECTED);
        else
            simple_monster_message(*mons, " is unaffected.");
        return 0;
    }

    int duration = div_rand_round(roll_dice(3, 1 + pow), 6)
                    - div_rand_round(victim->get_hit_dice() - 1, 2);

    if (duration <= 0)
    {
        if (!mons)
            canned_msg(MSG_YOU_RESIST);
        else
            simple_monster_message(*mons, " resists.");
        return 0;
    }

    if ((!mons && you.get_mutation_level(MUT_COLD_BLOODED))
        || (mons && mons_class_flag(mons->type, M_COLD_BLOOD)))
    {
        duration *= 2;
    }

    // Guarantee a minimum duration if not fully resisted.
    duration = max(duration, 2 + random2(4));

    if (!mons)
        return slow_player(duration);

    return do_slow_monster(*mons, agent, duration * BASELINE_DELAY);
}

spret cast_englaciation(int pow, bool fail)
{
    fail_check();
    mpr("You radiate an aura of cold.");
    apply_area_visible([pow] (coord_def where) {
        return englaciate(where, pow, &you);
    }, you.pos());
    return spret::success;
}

/** Corona a monster.
 *
 *  @param mons the monster to get a backlight.
 *  @returns true if it got backlit (even if it was already).
 */
bool backlight_monster(monster* mons)
{
    const mon_enchant bklt = mons->get_ench(ENCH_CORONA);
    const mon_enchant zin_bklt = mons->get_ench(ENCH_SILVER_CORONA);
    const int lvl = bklt.degree + zin_bklt.degree;

    mons->add_ench(mon_enchant(ENCH_CORONA, 1));

    if (lvl == 0)
        simple_monster_message(*mons, " is outlined in light.");
    else if (lvl == 4)
        simple_monster_message(*mons, " glows brighter for a moment.");
    else
        simple_monster_message(*mons, " glows brighter.");

    return true;
}

bool do_slow_monster(monster& mon, const actor* agent, int dur)
{
    if (mon.stasis())
        return true;

    if (mon.add_ench(mon_enchant(ENCH_SLOW, 0, agent, dur)))
    {
        if (!mon.paralysed() && !mon.petrified()
            && simple_monster_message(mon, " seems to slow down."))
        {
            return true;
        }
    }

    return false;
}

bool enfeeble_monster(monster &mon, int pow)
{
    const int res_margin = mon.check_willpower(&you, pow);
    vector<enchant_type> hexes;

    if (mons_has_attacks(mon))
        hexes.push_back(ENCH_WEAK);
    if (mon.antimagic_susceptible())
        hexes.push_back(ENCH_ANTIMAGIC);
    if (res_margin <= 0)
    {
        if (mons_can_be_blinded(mon.type))
            hexes.push_back(ENCH_BLIND);
        hexes.push_back(ENCH_DAZED);
    }

    // Resisted the upgraded effects, and immune to the irresistible effects.
    if (hexes.empty())
    {
        return simple_monster_message(mon,
                   mon.resist_margin_phrase(res_margin).c_str());
    }

    const int max_extra_dur = div_rand_round(pow, 40);
    const int dur = 5 + random2avg(max_extra_dur, 3);

    for (auto hex : hexes)
    {
        if (mon.has_ench(hex))
        {
            mon_enchant ench = mon.get_ench(hex);
            ench.duration = max(dur * BASELINE_DELAY, ench.duration);
            mon.update_ench(ench);
        }
        else
            mon.add_ench(mon_enchant(hex, 0, &you, dur * BASELINE_DELAY));
    }

    if (res_margin > 0)
        simple_monster_message(mon, " partially resists.");

    return simple_monster_message(mon, " is enfeebled!");
}

spret cast_vile_clutch(int pow, bolt &beam, bool fail)
{
    spret result = zapping(ZAP_VILE_CLUTCH, pow, beam, true, nullptr, fail);

    if (result == spret::success)
        you.props[VILE_CLUTCH_POWER_KEY].get_int() = pow;

    return result;
}

bool start_ranged_constriction(actor& caster, actor& target, int duration,
                               constrict_type type)
{
    if (!caster.can_constrict(target, type))
        return false;

    if (target.is_player())
    {
        if (type == CONSTRICT_ROOTS)
        {
            you.increase_duration(DUR_GRASPING_ROOTS, duration);
            mprf(MSGCH_WARN, "The grasping roots grab you!");
        }
        else if (type == CONSTRICT_BVC)
        {
            you.increase_duration(DUR_VILE_CLUTCH, duration);
            mprf(MSGCH_WARN, "Zombie hands grab you from below!");
        }
        caster.start_constricting(you);
    }
    else
    {
        enchant_type etype = (type == CONSTRICT_ROOTS ? ENCH_GRASPING_ROOTS
                                                      : ENCH_VILE_CLUTCH);
        auto ench = mon_enchant(etype, 0, &caster, duration * BASELINE_DELAY);
        target.as_monster()->add_ench(ench);
    }

    return true;
}

dice_def rimeblight_dot_damage(int pow)
{
    return dice_def(2, 3 + div_rand_round(pow, 17));
}

string describe_rimeblight_damage(int pow, bool terse)
{
    dice_def dot_damage = rimeblight_dot_damage(pow);
    dice_def shards_damage = zap_damage(ZAP_RIMEBLIGHT_SHARDS, pow, false, false);

    if (terse)
    {
        return make_stringf("%dd%d/%dd%d", dot_damage.num, dot_damage.size,
                                           shards_damage.num, shards_damage.size);
    }

    return make_stringf("%dd%d (primary target), %dd%d (explosion)",
                        dot_damage.num, dot_damage.size,
                        shards_damage.num, shards_damage.size);
}

bool maybe_spread_rimeblight(monster& victim, int power)
{
    if (victim.holiness() & (MH_NATURAL | MH_DEMONIC | MH_HOLY)
        && !victim.has_ench(ENCH_RIMEBLIGHT)
        && x_chance_in_y(2, 3)
        && you.see_cell_no_trans(victim.pos()))
    {
        apply_rimeblight(victim, power);
        return true;
    }

    return false;
}

bool apply_rimeblight(monster& victim, int power, bool quiet)
{
    if (victim.has_ench(ENCH_RIMEBLIGHT)
        || !(victim.holiness() & (MH_NATURAL | MH_DEMONIC | MH_HOLY)))
    {
        return false;
    }

    int duration = (random_range(6, 10) + div_rand_round(power, 30))
                    * BASELINE_DELAY;
    victim.add_ench(mon_enchant(ENCH_RIMEBLIGHT, 0, &you, duration));
    victim.props[RIMEBLIGHT_POWER_KEY] = power;
    victim.props[RIMEBLIGHT_TICKS_KEY] = random_range(0, 2);

    if (!quiet)
        simple_monster_message(victim, " is afflicted with rimeblight.");

    return true;
}

void do_rimeblight_explosion(coord_def pos, int power, int size)
{
    bolt shards;
    zappy(ZAP_RIMEBLIGHT_SHARDS, power, false, shards);
    shards.ex_size = size;
    shards.source_id     = MID_PLAYER;
    shards.thrower       = KILL_YOU_MISSILE;
    shards.origin_spell  = SPELL_RIMEBLIGHT;
    shards.target        = pos;
    shards.source        = pos;
    shards.hit_verb      = "hits";
    shards.aimed_at_spot = true;
    shards.explode();
}

void tick_rimeblight(monster& victim)
{
    const int pow = victim.props[RIMEBLIGHT_POWER_KEY].get_int();
    int ticks = victim.props[RIMEBLIGHT_TICKS_KEY].get_int();

    // Determine chance to explode with ice (rises over time)
    // Never happens below 3, always happens at 4, random chance beyond that
    if (ticks == 4 || ticks > 4 && x_chance_in_y(ticks, ticks + 16)
        && you.see_cell_no_trans(victim.pos()))
    {
        mprf("Shards of ice erupt from %s body!", apostrophise(victim.name(DESC_THE)).c_str());
        do_rimeblight_explosion(victim.pos(), pow, 1);
    }

    // Injury bond or some other effects may have killed us by now
    if (!victim.alive())
        return;

    // Apply direct AC-ignoring cold damage
    int dmg = rimeblight_dot_damage(pow).roll();
    dmg = resist_adjust_damage(&victim, BEAM_COLD, dmg);
    victim.hurt(&you, dmg, BEAM_COLD, KILLED_BY_FREEZING);

    // Increment how long rimeblight has been active
    if (victim.alive())
        victim.props[RIMEBLIGHT_TICKS_KEY] = (++ticks);
}

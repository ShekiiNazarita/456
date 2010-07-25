/*
 *  File:       mon-abil.h
 *  Summary:    Monster abilities.
 *  Written by: Linley Henzell
 */

#ifndef MONABIL_H
#define MONABIL_H

class monsters;
struct bolt;

bool mon_special_ability(monsters *monster, bolt & beem);
void mon_nearby_ability(monsters *monster);

bool ugly_thing_mutate(monsters *ugly, bool proximity = false);

void ballisto_on_move(monsters * monster, const coord_def & pos);
void activate_ballistomycetes(monsters * monster, const coord_def & origin,
                              bool player_kill);

void move_kraken_tentacles(monsters * kraken);
void move_demon_tentacle(monsters * tentacle);

#endif

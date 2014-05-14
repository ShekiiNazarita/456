/**
 * @file
 * @brief Monster death functionality.
**/

#ifndef MONDEATH_H
#define MONDEATH_H

#define FRESHEST_CORPSE 210
#define ROTTING_CORPSE   99

struct bolt;

int monster_die(monster* mons, const actor *killer, bool silent = false,
                bool wizard = false, bool fake = false);

int monster_die(monster* mons, killer_type killer,
                int killer_index, bool silent = false, bool wizard = false,
                bool fake = false);

int mounted_kill(monster* daddy, monster_type mc, killer_type killer,
                int killer_index);

monster_type fill_out_corpse(const monster* mons,
                             monster_type mtype,
                             item_def& corpse,
                             bool force_corpse = false);

bool explode_corpse(item_def& corpse, const coord_def& where);

int place_monster_corpse(const monster* mons, bool silent, bool force = false);
void monster_cleanup(monster* mons);
void setup_spore_explosion(bolt & beam, const monster& origin);
void record_monster_defeat(monster* mons, killer_type killer);
int destroy_tentacles(monster* head);
void unawaken_vines(const monster* mons, bool quiet);
void fire_monster_death_event(monster* mons, killer_type killer, int i, bool polymorph);

int exp_rate(int killer);

void mons_check_pool(monster* mons, const coord_def &oldpos,
                     killer_type killer = KILL_NONE, int killnum = -1);

string summoned_poof_msg(const monster* mons, bool plural = false);
string summoned_poof_msg(const int midx, const item_def &item);
string summoned_poof_msg(const monster* mons, const item_def &item);

bool mons_is_pikel(monster* mons);
void pikel_band_neutralise();

bool mons_is_duvessa(const monster* mons);
bool mons_is_dowan(const monster* mons);
bool mons_is_elven_twin(const monster* mons);
void elven_twin_died(monster* twin, bool in_transit, killer_type killer, int killer_index);
void elven_twins_pacify(monster* twin);
void elven_twins_unpacify(monster* twin);

bool mons_is_kirke(monster* mons);
void hogs_to_humans();

bool mons_is_natasha(const monster* mons);
bool mons_felid_can_revive(const monster* mons);
void mons_felid_revive(monster* mons);
#endif

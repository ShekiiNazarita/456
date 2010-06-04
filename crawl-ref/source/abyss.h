/*
 *  File:       abyss.h
 *  Summary:    Misc abyss specific functions.
 *  Written by: Linley Henzell
 */


#ifndef ABYSS_H
#define ABYSS_H

const int ABYSS_AREA_SHIFT_RADIUS = 10;

void generate_abyss();
void abyss_area_shift();
void abyss_teleport(bool new_area);
void save_abyss_uniques();
bool is_level_incorruptible();
bool lugonu_corrupt_level(int power);
void run_corruption_effects(int duration);

#endif

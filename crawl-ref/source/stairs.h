/*
 * File:     stairs.h
 * Summary:  Moving between levels.
 */

#ifndef STAIRS_H
#define STAIRS_H

bool check_annotation_exclusion_warning();
void down_stairs(int old_level,
                 dungeon_feature_type force_stair = DNGN_UNSEEN,
                 entry_cause_type entry_cause = EC_UNKNOWN);
void up_stairs(dungeon_feature_type force_stair = DNGN_UNSEEN,
               entry_cause_type entry_cause = EC_UNKNOWN);
void new_level();

#endif


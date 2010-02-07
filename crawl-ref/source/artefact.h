/*
 *  File:       artefact.h
 *  Summary:    Random and unrandom artefact functions.
 *  Written by: Linley Henzell
 */


#ifndef RANDART_H
#define RANDART_H

#include "externs.h"

struct bolt;

// NOTE: NO_UNRANDARTS is automatically set by util/art-data.pl
#define NO_UNRANDARTS 82

#define ART_PROPERTIES ARTP_NUM_PROPERTIES

// Reserving the upper bits for later expansion/versioning.
#define RANDART_SEED_MASK  0x00ffffff

#define KNOWN_PROPS_KEY     "artefact_known_props"
#define ARTEFACT_PROPS_KEY  "artefact_props"
#define ARTEFACT_NAME_KEY   "artefact_name"
#define ARTEFACT_APPEAR_KEY "artefact_appearance"

enum unrand_special_type
{
    UNRANDSPEC_EITHER,
    UNRANDSPEC_NORMAL,
    UNRANDSPEC_SPECIAL
};

// NOTE: This enumeration is automatically generated from art-data.txt
// via util/art-data.pl
enum unrand_type
{
    UNRAND_START = 180,
    UNRAND_DUMMY1 = UNRAND_START,
    UNRAND_SINGING_SWORD,    // Singing Sword
    UNRAND_TROG,             // Wrath of Trog
    UNRAND_VARIABILITY,      // Mace of Variability
    UNRAND_PRUNE,            // Glaive of Prune
    UNRAND_POWER,            // Sword of Power
    UNRAND_OLGREB,           // Staff of Olgreb
    UNRAND_WUCAD_MU,         // Staff of Wucad Mu
    UNRAND_VAMPIRES_TOOTH,   // Vampire's Tooth
    UNRAND_CURSES,           // Scythe of Curses
    UNRAND_TORMENT,          // Sceptre of Torment
    UNRAND_ZONGULDROK,       // Sword of Zonguldrok
    UNRAND_CEREBOV,          // Sword of Cerebov
    UNRAND_DISPATER,         // Staff of Dispater
    UNRAND_ASMODEUS,         // Sceptre of Asmodeus
    UNRAND_BLOODBANE,        // long sword "Bloodbane"
    UNRAND_FLAMING_DEATH,    // scimitar of Flaming Death
    UNRAND_BRILLIANCE,       // mace of Brilliance
    UNRAND_LEECH,            // demon blade "Leech"
    UNRAND_CHILLY_DEATH,     // dagger of Chilly Death
    UNRAND_MORG,             // dagger "Morg"
    UNRAND_FINISHER,         // scythe "Finisher"
    UNRAND_PUNK,             // sling "Punk"
    UNRAND_KRISHNA,          // bow of Krishna "Sharnga"
    UNRAND_SKULLCRUSHER,     // giant club "Skullcrusher"
    UNRAND_GUARD,            // glaive of the Guard
    UNRAND_JIHAD,            // sword of Jihad
    UNRAND_HELLFIRE,         // crossbow "Hellfire"
    UNRAND_DOOM_KNIGHT,      // sword of the Doom Knight
    UNRAND_EOS,              // "Eos"
    UNRAND_BOTONO,           // spear of the Botono
    UNRAND_OCTOPUS_KING,     // trident of the Octopus King
    UNRAND_ARGA,             // mithril axe "Arga"
    UNRAND_ELEMENTAL_STAFF,  // Elemental Staff
    UNRAND_SNIPER,           // hand crossbow "Sniper"
    UNRAND_PIERCER,          // longbow "Piercer"
    UNRAND_BLOWGUN_ASSASSIN, // blowgun of the Assassin
    UNRAND_WYRMBANE,         // Wyrmbane
    UNRAND_SPRIGGANS_KNIFE,  // Spriggan's Knife
    UNRAND_PLUTONIUM_SWORD,  // plutonium sword
    UNRAND_UNDEADHUNTER,     // great mace "Undeadhunter"
    UNRAND_SERPENT_SCOURGE,  // whip "Serpent-Scourge"
    UNRAND_ACCURACY,         // knife of Accuracy
    UNRAND_CRYSTAL_SPEAR,    // Lehudib's crystal spear
    UNRAND_CAPTAINS_CUTLASS, // captain's cutlass
    UNRAND_STORM_BOW,        // storm bow
    UNRAND_IGNORANCE,        // shield of Ignorance
    UNRAND_AUGMENTATION,     // robe of Augmentation
    UNRAND_THIEF,            // cloak of the Thief
    UNRAND_BULLSEYE,         // shield "Bullseye"
    UNRAND_DYROVEPREVA,      // crown of Dyrovepreva
    UNRAND_BEAR_SPIRIT,      // hat of the Bear Spirit
    UNRAND_MISFORTUNE,       // robe of Misfortune
    UNRAND_FLASH,            // cloak of Flash
    UNRAND_BOOTS_ASSASSIN,   // boots of the Assassin
    UNRAND_LEAR,             // Lear's chain mail
    UNRAND_ZHOR,             // skin of Zhor
    UNRAND_SALAMANDER,       // salamander hide armour
    UNRAND_WAR,              // gauntlets of War
    UNRAND_RESISTANCE,       // shield of Resistance
    UNRAND_FOLLY,            // robe of Folly
    UNRAND_MAXWELL,          // Maxwell's patent armour
    UNRAND_DRAGONMASK,       // mask of the Dragon
    UNRAND_NIGHT,            // robe of Night
    UNRAND_DRAGON_KING,      // armour of the Dragon King
    UNRAND_ALCHEMIST,        // hat of the Alchemist
    UNRAND_FENCERS_GLOVES,   // Fencer's gloves
    UNRAND_STARLIGHT,        // cloak of Starlight
    UNRAND_RATSKIN_CLOAK,    // ratskin cloak
    UNRAND_GONG,             // shield of the gong
    UNRAND_AIR,              // amulet of the Air
    UNRAND_SHADOWS,          // ring of Shadows
    UNRAND_CEKUGOB,          // amulet of Cekugob
    UNRAND_FOUR_WINDS,       // amulet of the Four Winds
    UNRAND_BLOODLUST,        // necklace of Bloodlust
    UNRAND_SHAOLIN,          // ring of Shaolin
    UNRAND_ROBUSTNESS,       // ring of Robustness
    UNRAND_MAGE,             // ring of the Mage
    UNRAND_SHIELDING,        // brooch of Shielding
    UNRAND_RCLOUDS,          // robe of Clouds
    UNRAND_PONDERING,        // hat of Pondering
    UNRAND_DUMMY2,           // DUMMY UNRANDART 2
    UNRAND_LAST = UNRAND_DUMMY2
};

enum unrand_flag_type
{
    UNRAND_FLAG_NONE             = 0x00,
    UNRAND_FLAG_SPECIAL          = 0x01,
    UNRAND_FLAG_HOLY             = 0x02,
    UNRAND_FLAG_UNHOLY           = 0x04,
    UNRAND_FLAG_EVIL             = 0x08,
    UNRAND_FLAG_UNCLEAN          = 0x10,
    UNRAND_FLAG_CHAOTIC          = 0x20,
    UNRAND_FLAG_CORPSE_VIOLATING = 0x40
};

enum setup_missile_type
{
    SM_CONTINUE,
    SM_FINISHED,
    SM_CANCEL
};

// The following unrandart bits were taken from $pellbinder's mon-util
// code (see mon-util.h & mon-util.cc) and modified (LRH).
struct unrandart_entry
{
    const char *name;        // true name of unrandart (max 31 chars)
    const char *unid_name;   // un-id'd name of unrandart (max 31 chars)

    object_class_type base_type;
    unsigned char     sub_type;
    short             plus;
    short             plus2;
    unsigned char     colour;       // colour of ura

    short         value;
    unsigned char flags;

    short prpty[ART_PROPERTIES];

    // special description added to 'v' command output (max 31 chars)
    const char *desc;
    // special description added to 'v' command output (max 31 chars)
    const char *desc_id;
    // special description added to 'v' command output (max 31 chars)
    const char *desc_end;

    void (*equip_func)(item_def* item, bool* show_msgs, bool unmeld);
    void (*unequip_func)(const item_def* item, bool* show_msgs);
    void (*world_reacts_func)(item_def* item);
    // An item can't be a melee weapon and launcher at the same time, so have
    // the functions relevant to those item types share a union.
    union
    {
        void (*melee_effects)(item_def* item, actor* attacker,
                              actor* defender, bool mondied);
        setup_missile_type (*launch)(item_def* item, bolt* beam,
                                     std::string* ammo_name, bool* returning);
    } fight_func;
    bool (*evoke_func)(item_def *item, int* pract, bool* did_work,
                       bool* unevokable);
};

bool is_known_artefact( const item_def &item );
bool is_artefact( const item_def &item );
bool is_random_artefact( const item_def &item );
bool is_unrandom_artefact( const item_def &item );
bool is_special_unrandom_artefact( const item_def &item );

unique_item_status_type get_unique_item_status(const item_def& item);
unique_item_status_type get_unique_item_status(int unrand_index);
void set_unique_item_status(const item_def& item,
                            unique_item_status_type status );
void set_unique_item_status(int unrand_index,
                            unique_item_status_type status );

std::string get_artefact_name( const item_def &item );

void set_artefact_name( item_def &item, const std::string &name );
void set_artefact_appearance( item_def &item, const std::string &appear );

std::string artefact_name( const item_def &item, bool appearance = false );

const char *unrandart_descrip( int which_descrip, const item_def &item );

int find_okay_unrandart(unsigned char aclass, unsigned char atype = OBJ_RANDOM,
                        unrand_special_type specialness = UNRANDSPEC_EITHER,
                        bool in_abyss = false);

typedef FixedVector< int, ART_PROPERTIES >  artefact_properties_t;
typedef FixedVector< bool, ART_PROPERTIES > artefact_known_props_t;

void artefact_desc_properties( const item_def        &item,
                              artefact_properties_t &proprt,
                              artefact_known_props_t &known,
                              bool force_fake_props = false);

void artefact_wpn_properties( const item_def       &item,
                             artefact_properties_t &proprt,
                             artefact_known_props_t &known );

void artefact_wpn_properties( const item_def &item,
                             artefact_properties_t &proprt );

int artefact_wpn_property( const item_def &item, artefact_prop_type prop,
                          bool &known );

int artefact_wpn_property( const item_def &item, artefact_prop_type prop );

int artefact_known_wpn_property( const item_def &item,
                                 artefact_prop_type prop );

void artefact_wpn_learn_prop( item_def &item, artefact_prop_type prop );
bool artefact_wpn_known_prop( const item_def &item, artefact_prop_type prop );

bool make_item_randart( item_def &item, bool force_mundane = false );
bool make_item_unrandart( item_def &item, int unrand_index );

bool randart_is_bad( const item_def &item );
bool randart_is_bad( const item_def &item, artefact_properties_t &proprt );

int find_unrandart_index(const item_def& artefact);

unrandart_entry* get_unrand_entry(int unrand_index);

unrand_special_type get_unrand_specialness(int unrand_index);
unrand_special_type get_unrand_specialness(const item_def &item);

void artefact_set_properties( item_def              &item,
                              artefact_properties_t &proprt );
void artefact_set_property( item_def           &item,
                            artefact_prop_type  prop,
                            int                 val );

int get_unrandart_num( const char *name );
#endif

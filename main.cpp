#include <kamek.h>

extern "C" void post_lotto_trophy_write(void);
extern "C" void post_lotto_sound_check(void);
extern "C" void character_is_locked(void);
extern "C" void display_unlocked_character(void);
extern "C" void Evnt_load_base(void);
extern "C" void Evnt_load_hasone(void);
extern "C" void add_permanent_trophies(void);
extern "C" void add_cur_trophy(void);
extern "C" void load_1p_game(void);
extern "C" void load_stadium_game(void);
extern "C" void GetModeByte();
extern "C" void play_sound();
extern "C" void event_not_selected();
extern "C" void play_sel_event();
extern "C" void get_random_character();
extern "C" void select_character();
extern "C" void CheckUnlockedCharacters();
extern "C" void check_trophy_message();
extern "C" void CheckUnlockedStages();
extern "C" void add_coins();
extern "C" void unlock_trophy_class();
extern "C" void load_base_pool();
extern "C" void unlock_trophy_class_real();
extern "C" void load_1p_pool();
extern "C" void count_trophies();
extern "C" void CheckTrophyOwned();

int UnlockedCharactersMain = 0x00000000;
int EventsTable = 0xFFFF0000;
unsigned char UnlockedTrophyClasses = 0x00;
unsigned short CurTrophyValue = 0x0000;

char CharLockedEvents[21] = { //Which events have characters locking them
	0x00, 0x01, 0x04, 0x0C, 0x0F, 0x10,
	0x11, 0x12, 0x18, 0x1A, 0x1C, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x26, 0x27,
	0x28, 0x29, 0x2A
};

char EventCharacters[21] = { //Which characters are on X event
	0x01, 0x06, 0x0B, 0x05, 0x0D, 0x0E,
	0x10, 0x01, 0x03, 0x0E, 0x10, 0x09,
	0x07, 0x11, 0x02, 0x0B, 0x14, 0x17,
	0x02, 0x0A, 0x16,
};


//////////////////////////////////
//Trophy gallery code
kmWrite32(0x803049D8, 0x68000000); // Prevent trophies from marking off the NEW text.
kmWrite32(0x803105A4, 0x68000000); //
kmWrite32(0x80310754, 0x68000000); //
kmWrite32(0x8030B2F0, 0x68000000); //

kmWrite32(0x803134C8, 0x48000024); // Trophies don't display new text

/////////////////////////////////
//Lottery Trophies
kmBranchDefAsm(0x803057CC, NULL) {  // When a new Trophy is collected, set the Obtained bit instead of incrementing the number of them
	lhz r0, 0 (r3)
	ori r0, r0, 0x8000
	sth r0, 0 (r3)
	b post_lotto_trophy_write
}

asm void CheckTrophyOwned(void) 
{
    nofralloc
	lis r4, 0x8045
	ori r4, r4, 0xC394
	mulli r3, r3, 2
	add r3, r3, r4
	lhz r3, 0(r3)
	andi. r3, r3, 0x8000
	and r3, r4, r3
    blr
}

asm void CheckTrophyOwnedAndStore(void) 
{
    nofralloc
	mr r5, r3
	lis r4, 0x8045
	ori r4, r4, 0xC394
	mulli r3, r3, 2
	add r3, r3, r4
	lhz r3, 0(r3)
	lis r4, CurTrophyValue@ha
	addi r4, r4, CurTrophyValue@l
	sth r3, 0(r4)
	mr r3, r5
    b CheckTrophyOwned
}

asm void CheckLatestTrophy(void) 
{
    nofralloc
	lis r4, CurTrophyValue@ha
	addi r4, r4, CurTrophyValue@l
	lhz r3, 0(r4)
	andi. r3, r3, 0x8000
	blr
}

kmBranchDefAsm(0x801BFEB4, NULL) {  // Message conditions check Trophy Obtained instead of Trophy Count
	nofralloc
	lis r3, 0x8045
	ori r3, r3, 0xC394
	mulli r0, r27, 2
	add r3, r0, r3
	lbz r3, 0(r3)
	andi. r3, r3, 0x80
	b check_trophy_message
}

kmBranchDefAsm(0x8030490C, NULL) {  // For the New Trophy Percent calc, check the Obtained bit instead of the count
	nofralloc
	lis r31, 0x8047
	ori r31, r31, 0x9D30
	lbz r31, 0(r31)
	cmpwi r31, 0x0C
	bne NotLottery
	andi. r3, r0, 0x8000
	b count_trophies
NotLottery:
	rlwinm r3, r0, 0, 24, 31
	b count_trophies
}

kmWrite32(0x80305680, 0x70008000); // Lottery trophies still set the New flag if you have the trophy. Doesn't set new flag if the new flag is set

kmWrite32(0x803052A8, 0x70008000); // Lottery trophies search for the Obtained bit instead of the count... This runs between classic matches. I should make sure it's working...

kmWrite32(0x803056B8, 0x68000000); // Prevent trophies from setting the Obtained bit; this part needs to happen later so other checks can run.

kmWrite32(0x80312470, 0x60000000); // Prevent lottery trophies from incrementing the counter

kmWrite32(0x80305720, 0x60000000); // Prevent 1P clears from incrementing the trophy counter

kmWrite32(0x803123C4, 0x60000000); //This is overwriting trophies with 0

kmCall(0x80316450, CheckTrophyOwnedAndStore); // Got new trophy message

kmCall(0x803161E0, CheckLatestTrophy); // Got new trophy sound
kmWrite32(0x803161E4, 0x2c030000); // This wont work because the trophy is already set as Owned at this point
//kmWrite32(0x803161E8, 0x2c030000); //IDK what to do here. gonna sleep on it

///////////////////////////////////////
//Character Unlocks

//I should make this a CheckIfUnlocked sub?
kmBranchDefAsm(0x80260C18, NULL) {  // When hovering over a character, check if the Character Unlock Bit is set to control it as unlocked.
	nofralloc
	lis r24, UnlockedCharactersMain@ha
	addi r24, r24, UnlockedCharactersMain@l
	lwz r0, 0(r24)
	li r24, 1
	slw r24, r24, r20 // Load the bit value of the char into memory
	and r24, r24, r0
	cmpwi r24, 0
	beq CharNotUnlocked
	b display_unlocked_character
CharNotUnlocked:
	b character_is_locked
}

kmBranchDefAsm(0x8025FB78, NULL) {  // Check if random character is unlocked
	nofralloc
	lis r31, UnlockedCharactersMain@ha
	addi r31, r31, UnlockedCharactersMain@l
	lwz r29, 0(r31)
	li r31, 1
	slw r31, r31, r3
	and r29, r29, r31
	cmpwi r29, 0
	beq GetRandChar
	addi r31, r3, 0
	b select_character
GetRandChar:
	b get_random_character
}

////////////////////////////////////////
// Event Mode
kmBranchDefAsm(0x8024CEC4, NULL) {  // Check the Progressive Event Packs
	nofralloc
	lis r3, EventsTable@ha
	addi  r3, r3, EventsTable@l
	lwz   r31, 0(r3)
	andi. r31, r31, 0xFFFF
	cmpwi r31, 0
	beq Evnt_None
	b Evnt_load_hasone
Evnt_None:
	b Evnt_load_base
}

kmWrite32(0x8024CED4, 0x2c1f0001); // Event pack 2
kmWrite32(0x8024CEE4, 0x2c1f0002); // Event pack 3
kmWrite32(0x8024CEF4, 0x2c1f0003); // Event pack 4
kmWrite32(0x8024CF04, 0x2c1f0004); // Event pack 5
//Event Pack 6:
kmWrite32(0x8024CF40, 0x2c1f0005); //Pack check
kmWrite32(0x8024CF44, 0x41810024);
kmWrite32(0x8024CF48, 0x480000b0);
//Event Pack 7:
kmWrite32(0x8024CF70, 0x2c1f0007); //Pack check
kmWrite32(0x8024CF74, 0x41820074); //
kmWrite32(0x8024CF78, 0x48000078); //

kmWrite32(0x8024CF14, 0x2c1f0008); // Event pack 8
kmWrite32(0x8024CF24, 0x60000000); // Second check for Evnt 51

kmWrite32(0x8024D958, 0x60000000); //Remove the selected sound

kmBranchDefAsm(0x8024D964, NULL) {  // Check if we have the event's character unlocked
	nofralloc
	add r3, r3, r0
	mr r24, r3
	lis r3, CharLockedEvents@ha
	li r5, 0
	addi r3, r3, CharLockedEvents@l
CheckNextEvnt:
	lbz r6, 0(r3)
	cmpw r6, r24
	beq EventHasCharacter
	addi r3, r3, 1
	addi r5, r5, 1
	cmpwi r5, 23
	blt CheckNextEvnt
LoadEvent:
	li r3, 1
	bl play_sound
	mr r3, r24
	b play_sel_event
EventHasCharacter:
	lis r3, EventCharacters@ha
	addi r3, r3, EventCharacters@l
	add r3, r3, r5
	lbz r4, 0(r3)
	li r3, 1
	slw r4, r3, r4
	lis r5, UnlockedCharactersMain@ha
	addi r5, r5, UnlockedCharactersMain@l
	lwz r5, 0(r5)
	and r4, r4, r5
	cmpwi r4, 0
	bne LoadEvent
	li r3, 3
	bl play_sound
	b event_not_selected
}


//////////////////////////////////////////
//Legendary Pokemon spawns
kmWrite32(0x8027A570, 0x60000000); //NOP out the Mew unlocked check
kmWrite32(0x8027A540, 0x38600064); //Change Mew to 1/100 instead of 1/251

kmWrite32(0x8027A528, 0x60000000); //NOP out the Mew unlocked check
kmWrite32(0x8027A4F4, 0x38600064); //Change Celebi to 1/100 instead of 1/251
//////////////////////////////////////////
//Trophy Collection
kmBranchDefAsm(0x8017E2F0, NULL) {  // Unlock picked up trophies immediately
	lis r5, 0x8045
	ori r5, r5, 0xC394
	mulli r3, r3, 2
	add r5, r5, r3
	lbz r3, 0(r5)
	ori r3, r3, 0x80
	stb r3, 0(r5)
	b add_cur_trophy
	}

///////////////////////////////////////////
//Coins
kmBranchDefAsm(0x801624F0, NULL) {  // Unlock picked up trophies immediately
	nofralloc
	mulli r28, r28, 10
	add r28, r29, r28
	b add_coins
	}


/////////////////////////////////
//1P Mode Locks
kmBranchDefAsm(0x8022CCB8, NULL) {  // Lock Main Modes
	nofralloc
	bl GetModeByte
	lhz r0, 0x0002 (r29)
	li r5, 2
	slw r5, r5, r0
	and r5, r4, r5
	cmpwi r5, 0
	beq ModeLocked
	b load_1p_game
	ModeLocked:
	li r0, 3
	li r3, 3
	bl play_sound
	b load_1p_game
	}

kmBranchDefAsm(0x8022C820, NULL) {  // Lock Stadium Modes
	nofralloc
	bl GetModeByte
	andi. r4, r4, 0xF0 //Filter main modes
	lhz r0, 0x0002 (r30)
	li r5, 0x10
	slw r5, r5, r0
	and r5, r4, r5
	cmpwi r5, 0
	beq ModeLocked
	b load_stadium_game
	ModeLocked:
	li r0, 3
	li r3, 3
	bl play_sound
	b load_stadium_game
	}

asm void GetModeByte(void)
{
    nofralloc
    lis   r4, UnlockedCharactersMain@ha
    addi  r4, r4, UnlockedCharactersMain@l
    lbz   r4, 0(r4)
    andi. r4, r4, 0x00FE    // clear Roy bit
    blr
}
//////////////////////////////////////
//Secret Characters
kmWrite32(0x801649EC, 0x80010024); //Prevent them from unlocking themselves

kmWrite32(0x80172DA0, 0x38600000); //Reduce Mewtwo to 1 Hour of vs
kmWrite32(0x80172DA8, 0x38030E10);

kmWrite32(0x801730D4, 0x38600400); // Roy
kmCall(0x801730D8, CheckUnlockedCharacters);

kmWrite32(0x801730F8, 0x38600020); // Dr Mario
kmCall(0x801730FC, CheckUnlockedCharacters);

kmWrite32(0x80173170, 0x38600080); // Young Link
kmCall(0x80173174, CheckUnlockedCharacters);

kmWrite32(0x80173194, 0x38600010); // Game & Watch
kmCall(0x80173198, CheckUnlockedCharacters);

kmWrite32(0x801731EC, 0x38600001); // Jigglypuff
kmCall(0x801731F0, CheckUnlockedCharacters);

kmWrite32(0x801731BC, 0x38600004); // Luigi
kmCall(0x801731C0, CheckUnlockedCharacters);

kmWrite32(0x8017329C, 0x38600008); // Marth
kmCall(0x801732A0, CheckUnlockedCharacters);

kmWrite32(0x80172ECC, 0x38600008); // Marth Vs?
kmCall(0x80172ED0, CheckUnlockedCharacters);

kmWrite32(0x801732EC, 0x38600040); // Ganondorf
kmCall(0x801732F0, CheckUnlockedCharacters);

kmWrite32(0x80173318, 0x38600200); // Pichu
kmCall(0x8017331C, CheckUnlockedCharacters);

kmWrite32(0x80172D90, 0x38600002); // Mewtwo
kmCall(0x80172D94, CheckUnlockedCharacters);

kmWrite32(0x80173464, 0x38600002); // Falco
kmCall(0x80173470, CheckUnlockedCharacters);

kmWrite32(0x801A60B0, 0x38600000); // Prevent characters from checking the VS quota

asm void CheckUnlockedCharacters(void)
{
    nofralloc
	lis r4, 0x8045
	ori r4, r4, 0xC20E
	lhz r4, 0(r4)
	andi. r4, r4, 0x7FFF
	and r3, r4, r3
    blr
}
/////////////////////////////////////
//Really high unlock requirements

kmWrite32(0x80173E5C, 0x2800000a); // Replace 1,000 KO's for Landmaster with 10 KO's

kmWrite32(0x80173658, 0x28000032); //Replace 5,000 KO's for Score Display with 50
///////////////////////////////////////
//Secret Stages

kmWrite32(0x801645DC, 0x60000000); //Prevent stages from unlocking themself

kmWrite32(0x80172F1C, 0x38600008); // Depths
kmCall(0x80172F20, CheckUnlockedStages);

kmWrite32(0x80172F3C, 0x38600010); // Fourside
kmCall(0x80172F40, CheckUnlockedStages);

kmWrite32(0x80172F5C, 0x38600040); // Blue THESE ARE ALL WORNG!!!!!!!!!!!
kmCall(0x80172F60, CheckUnlockedStages);

kmWrite32(0x80172F7C, 0x38600020); // Floats
kmCall(0x80172F80, CheckUnlockedStages);

kmWrite32(0x801BEECC, 0x38600100); // Flat
kmCall(0x801BEED0, CheckUnlockedStages);

kmWrite32(0x801734E4, 0x38601000); // Yoshi
kmCall(0x801734E8, CheckUnlockedStages);

kmWrite32(0x8017349C, 0x38602000); // DK
kmCall(0x801734A8, CheckUnlockedStages);

kmWrite32(0x80173434, 0x38600800); // Dream
kmCall(0x80173438, CheckUnlockedStages);

kmWrite32(0x801733A4, 0x38600400); // Final
kmCall(0x801733A8, CheckUnlockedStages);

kmWrite32(0x803058C4, 0x38600080); // Mushroom
kmCall(0x803058C8, CheckUnlockedStages);

kmWrite32(0x8017CCF4, 0x38600002); // Battlefield
kmCall(0x8017CCF8, CheckUnlockedStages);

asm void CheckUnlockedStages(void)
{
    nofralloc
	lis r4, 0x8045
	ori r4, r4, 0xC20D
	lhz r4, 0(r4)
	andi. r4, r4, 0x3FF8
	and r3, r4, r3
    blr
}
/////////////////////////
//Trophy checkers
kmWrite32(0x803058BC, 0x2c00012c); // Prevent the Birdo trophy from unlocking the stage

kmWrite32(0x80173F24, 0x48000058); //Prevent character trophies from unlocking with VS counts
kmWrite32(0x80173F84, 0x48000058);
kmWrite32(0x80173FE4, 0x48000058);

kmWrite32(0x80174090, 0x280003E8); //Lower the count for tom nook


//////////////////////////
//All-Star Vanilla Lock
kmWrite32(0x8015EDDC, 0x38600001);


////////////////////////////
//Trophy classes
kmWrite32(0x80304FA0, 0x60000000); //Prevent rank up from increasing the pool

//THESE SHOULD BE SETTINGS-DEPENDANT!!!!!!!
//kmWrite32(0x80305898, 0x4800001c); //Disable the Normal unlock for the 250 trophy pool
//kmWrite32(0x801741CC, 0x60000000); //Disable the VS Trophy Class
//kmWrite32(0x801741E8, 0x60000000); //Disable the All Characters Trophy Class
//kmWrite32(0x801741B0, 0x60000000); //Disable the Adventure/Classic Class
//////////

kmWrite32(0x801741B8, 0x28030005); //Change the 200 Class requirement to 5

kmBranchDefAsm(0x80317E10, NULL) {  // Update Lottery pool
	nofralloc
	li r6, 4
CheckClass:
	lis r5, UnlockedTrophyClasses@ha
	addi r5, r5, UnlockedTrophyClasses@l
	lbz r5, 0(r5)
	li r4, 1
	slw r3, r4, r6
	and r3, r5, r3
	cmpwi r3, 0
	beq ClassLocked
	mr r3, r6
	li r4, 0
	li r5, 0
	li r7, 0
	bl unlock_trophy_class
	bl unlock_trophy_class_real
ClassLocked:
	cmpwi r6, 7
	beq DoneUnlocks
	addi r6, r6, 1
	b CheckClass
DoneUnlocks:
	li r3, 3
	li r6, 0
	b load_base_pool
	}

kmBranchDefAsm(0x801C5A40, NULL) {  // Update 1P Pool
	nofralloc
	lis r3, UnlockedTrophyClasses@ha
	addi r3, r3, UnlockedTrophyClasses@l
	lbz r3, 0(r3)
	andi. r3, r3, 0x20
	cmpwi r3, 0
	beq PoolDone
	li r3, 5
	li r4, 0
	li r5, 0
	li r7, 0
	bl unlock_trophy_class
	bl unlock_trophy_class_real
PoolDone:
	li r3, 0
	b load_1p_pool
	}
///////////
kmWrite32(0x80173EB8, 0x38002710); //Reduce Meter requirement
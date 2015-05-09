/*! \page License
   KQ is Copyright (C) 2002 by Josh Bolduc

   This file is part of KQ... a freeware RPG.

   KQ is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   KQ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with KQ; see the file COPYING.  If not, write to
   the Free Software Foundation,
       675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "kq.h"
#include "combat.h"
#include "draw.h"
#include "effects.h"
#include "enemyc.h"
#include "eskill.h"
#include "heroc.h"
#include "itemmenu.h"
#include "magic.h"
#include "masmenu.h"
#include "menu.h"
#include "res.h"
#include "setup.h"

#include <cstdio>
#include <cstring>

/*! \file
 * \brief Enemy skills
 */


/*! \brief Process each enemy skill
 *
 * Just a function to process each enemy skill by index number.
 *
 * \param   who Index of attacker
 */
void combat_skill(int who)
{
    if (who < 0 || who >= NUM_FIGHTERS) return;

    int attack_special_skill = fighter[who].ai[fighter[who].csmem] - 100;
    int attack_recipient = fighter[who].ctmem;
    int a;
    int b;

    tempa = status_adjust(who);
    battle_render(0, 0, 0);
    blit2screen(0, 0);
    if (attack_special_skill == 1)
    {
        ctext = "Venomous Bite";
        dct = 1;
        tempa.welem = R_POISON + 1;
        fight(who, attack_recipient, 1);
        dct = 0;
        fighter[who].atrack[fighter[who].csmem] = 2;
    }
    if (attack_special_skill == 2)
    {
        ctext = "Double Slash";
        dct = 1;
        tempa.stats[A_ATT] = tempa.stats[A_ATT] * 15 / 10;
        fight(who, attack_recipient, 1);
        dct = 0;
        fighter[who].atrack[fighter[who].csmem] = 2;
    }
    if (attack_special_skill == 3)
    {
        ctext = "Chill Touch";
        dct = 1;
        draw_spellsprite(attack_recipient, 0, 10, 1);
        special_damage_oneall_enemies(who, 60, R_ICE, attack_recipient, 0);
        dct = 0;
        fighter[who].atrack[fighter[who].csmem] = 2;
    }
    if (attack_special_skill == 4)
    {
        ctext = "Flash Flood";
        dct = 1;
        draw_hugesprite(0, 80, 108, 21, 1);
        /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
        special_damage_oneall_enemies(who, 40, R_ICE, SEL_ALL_ENEMIES, 1);
        dct = 0;
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
    if (attack_special_skill == 5)
    {
        unsigned int dead_fighter_count = 0;
        for (int fighter_index = 0; fighter_index < numchrs; fighter_index++)
            if (fighter[fighter_index].sts[S_DEAD] == 0)
            {
                dead_fighter_count++;
            }
        if (dead_fighter_count > 1)
        {
            fighter[who].ctmem = 1000;
        }
        ctext = "Sweep";
        dct = 1;
        tempa.stats[A_ATT] = tempa.stats[A_ATT] * 75 / 100;
        multi_fight(who);
        dct = 0;
        fighter[who].atrack[fighter[who].csmem] = 2;
    }
    if (attack_special_skill == 6)
    {
        ctext = "ParaClaw";
        dct = 1;
        tempa.welem = R_PARALYZE + 1;
        fight(who, attack_recipient, 1);
        dct = 0;
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
    if (attack_special_skill == 7)
    {
        ctext = "Dragon Bite";
        dct = 1;
        tempa.stats[A_ATT] = tempa.stats[A_ATT] * 15 / 10;
        tempa.stats[A_HIT] = tempa.stats[A_HIT] * 9 / 10;
        tempa.welem = 0;
        fight(who, attack_recipient, 1);
        dct = 0;
        fighter[who].atrack[fighter[who].csmem] = 2;
    }
    if (attack_special_skill == 8)
    {
        b = 0;
        ctext = "Stone Gas";
        draw_spellsprite(0, 1, 46, 1);
        for (a = 0; a < numchrs; a++)
        {
            if (fighter[a].sts[S_DEAD] == 0)
            {
                if (res_throw(a, R_PETRIFY) == 0 && non_dmg_save(a, 75) == 0)
                {
                    fighter[a].sts[S_STONE] = rand() % 3 + 2;
                    ta[a] = NODISPLAY;
                }
                else
                {
                    ta[a] = MISS;
                    b++;
                }
            }
        }
        if (b > 0)
        {
            display_amount(0, FNORMAL, 1);
        }
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
    if (attack_special_skill == 9)
    {
        b = 0;
        ctext = "Zemmel Rod";
        if (rand() % 4 < 2)
        {
            draw_spellsprite(0, 1, 11, 1);
            /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
            special_damage_oneall_enemies(who, 25, R_THUNDER, SEL_ALL_ENEMIES, 1);
            fighter[who].atrack[fighter[who].csmem] = 2;
            return;
        }
        draw_spellsprite(0, 1, 40, 0);
        for (a = 0; a < numchrs; a++)
        {
            if (res_throw(a, R_TIME) == 0)
            {
                if (non_dmg_save(a, 75) == 0 && fighter[a].sts[S_STONE] == 0)
                {
                    if (fighter[a].sts[S_TIME] == 2)
                    {
                        fighter[a].sts[S_TIME] = 0;
                    }
                    else
                    {
                        if (fighter[a].sts[S_TIME] == 0)
                        {
                            fighter[a].sts[S_TIME] = 1;
                            ta[a] = NODISPLAY;
                        }
                        else
                        {
                            ta[a] = MISS;
                            b++;
                        }
                    }
                }
                else
                {
                    ta[a] = MISS;
                    b++;
                }
            }
            else
            {
                ta[a] = MISS;
                b++;
            }
        }
        if (b > 0)
        {
            display_amount(0, FNORMAL, 1);
        }
        fighter[who].atrack[fighter[who].csmem] = 2;
    }
    if (attack_special_skill == 10)
    {
        ctext = "Poison Gas";
        draw_spellsprite(0, 1, 47, 1);
        /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
        special_damage_oneall_enemies(who, 40, R_POISON, SEL_ALL_ENEMIES, 1);
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
    if (attack_special_skill == 11)
    {
        b = 0;
        ctext = "Tangle Root";
        draw_spellsprite(0, 1, 24, 0);
        for (a = 0; a < numchrs; a++)
        {
            if (res_throw(a, S_STOP) == 0 && non_dmg_save(a, 65) == 0 && fighter[a].sts[S_STONE] == 0)
            {
                fighter[a].sts[S_STOP] = 2 + rand() % 2;
                ta[a] = NODISPLAY;
            }
            else
            {
                ta[a] = MISS;
                b++;
            }
        }
        if (b > 0)
        {
            display_amount(0, FNORMAL, 1);
        }
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
    if (attack_special_skill == 12)
    {
        ctext = "Petrifying Bite";
        dct = 1;
        tempa.stats[A_ATT] = tempa.stats[A_ATT];
        tempa.stats[A_HIT] = tempa.stats[A_HIT] * 8 / 10;
        tempa.welem = 13;
        fight(who, attack_recipient, 1);
        dct = 0;
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
    if (attack_special_skill == 13)
    {
        ctext = "Maul of the Titans";
        draw_hugesprite(0, 80, 110, 29, 1);
        /*  dudaskank suggest replacing 999 with SEL_ALL_ENEMIES  */
        special_damage_oneall_enemies(who, 60, R_EARTH, SEL_ALL_ENEMIES, 1);
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
    if (attack_special_skill == 14)
    {
        ctext = "Stunning Strike";
        dct = 1;
        tempa.stats[A_ATT] = tempa.stats[A_ATT] * 8 / 10;
        fight(who, attack_recipient, 1);
        dct = 0;
        if (non_dmg_save(attack_recipient, 80) == 0 && ta[attack_recipient] != MISS)
        {
            fighter[attack_recipient].sts[S_STOP] = 2;
        }
        fighter[who].atrack[fighter[who].csmem] = 4;
    }
    if (attack_special_skill == 15)
    {
        ctext = "Howl";
        draw_spellsprite(0, 1, 14, 0);
        b = 0;
        for (a = 0; a < numchrs; a++)
        {
            if (fighter[who].sts[S_MUTE] == 0)
            {
                if (res_throw(a, S_CHARM) == 0 && non_dmg_save(a, 65) == 0 && fighter[a].sts[S_STONE] == 0)
                {
                    fighter[a].sts[S_CHARM] = 2 + rand() % 2;
                    ta[a] = NODISPLAY;
                }
                else
                {
                    ta[a] = MISS;
                    b++;
                }
            }
            else
            {
                ta[a] = MISS;
                b++;
            }
        }
        if (b > 0)
        {
            display_amount(0, FNORMAL, 1);
        }
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
    if (attack_special_skill == 16)
    {
        ctext = "Rasp";
        draw_spellsprite(0, 1, 48, 0);
        for (a = 0; a < numchrs; a++)
        {
            b = fighter[a].hp / 3;
            ta[a] = 0 - b;
        }
        display_amount(0, FNORMAL, 1);
        for (a = 0; a < numchrs; a++)
        {
            adjust_hp(a, ta[a]);
        }
        for (a = 0; a < numchrs; a++)
        {
            b = fighter[a].mp / 3;
            ta[a] = 0 - b;
        }
        display_amount(0, FRED, 1);
        for (a = 0; a < numchrs; a++)
        {
            adjust_mp(a, ta[a]);
        }
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
    if (attack_special_skill == 17)
    {
        ctext = "Shadow Blast";
        draw_spellsprite(0, 1, 49, 1);
        special_damage_oneall_enemies(who, 75, R_BLACK, SEL_ALL_ENEMIES, 1);
        fighter[who].atrack[fighter[who].csmem] = 3;
    }
}

/* Local Variables:     */
/* mode: c              */
/* comment-column: 0    */
/* indent-tabs-mode nil */
/* tab-width: 4         */
/* End:                 */
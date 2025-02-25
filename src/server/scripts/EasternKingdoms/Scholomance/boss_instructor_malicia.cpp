/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Boss_instructormalicia
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "ScriptMgr.h"
#include "scholomance.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_CALLOFGRAVES          = 17831,
    SPELL_CORRUPTION            = 11672,
    SPELL_FLASHHEAL             = 10917,
    SPELL_RENEW                 = 10929,
    SPELL_HEALINGTOUCH          = 9889
};

enum Events
{
    EVENT_CALLOFGRAVES          = 1,
    EVENT_CORRUPTION            = 2,
    EVENT_FLASHHEAL             = 3,
    EVENT_RENEW                 = 4,
    EVENT_HEALINGTOUCH          = 5
};

class boss_instructor_malicia : public CreatureScript
{
    public: boss_instructor_malicia() : CreatureScript("boss_instructor_malicia") { }

        struct boss_instructormaliciaAI : public BossAI
        {
            boss_instructormaliciaAI(Creature* creature) : BossAI(creature, DATA_INSTRUCTORMALICIA)
            {
                Initialize();
            }

            void Initialize()
            {
                FlashCounter = 0;
                TouchCounter = 0;
            }

            uint32 FlashCounter;
            uint32 TouchCounter;

            void Reset() override
            {
                _Reset();
                Initialize();
            }

            void JustEngagedWith(Unit* who) override
            {
                BossAI::JustEngagedWith(who);
                events.ScheduleEvent(EVENT_CALLOFGRAVES, 4s);
                events.ScheduleEvent(EVENT_CORRUPTION, 8s);
                events.ScheduleEvent(EVENT_RENEW, 32s);
                events.ScheduleEvent(EVENT_FLASHHEAL, 38s);
                events.ScheduleEvent(EVENT_HEALINGTOUCH, 45s);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CALLOFGRAVES:
                            DoCastVictim(SPELL_CALLOFGRAVES, true);
                            events.ScheduleEvent(EVENT_CALLOFGRAVES, 65s);
                            break;
                        case EVENT_CORRUPTION:
                            DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_CORRUPTION, true);
                            events.ScheduleEvent(EVENT_CORRUPTION, 24s);
                            break;
                        case EVENT_RENEW:
                            DoCast(me, SPELL_RENEW);
                            events.ScheduleEvent(EVENT_RENEW, 10s);
                            break;
                        case EVENT_FLASHHEAL:
                            //5 Flashheals will be cast
                            DoCast(me, SPELL_FLASHHEAL);
                            if (FlashCounter < 2)
                            {
                                events.ScheduleEvent(EVENT_FLASHHEAL, 5s);
                                ++FlashCounter;
                            }
                            else
                            {
                                FlashCounter=0;
                                events.ScheduleEvent(EVENT_FLASHHEAL, 30s);
                            }
                            break;
                        case EVENT_HEALINGTOUCH:
                            //3 Healing Touch will be cast
                            DoCast(me, SPELL_HEALINGTOUCH);
                            if (TouchCounter < 2)
                            {
                                events.ScheduleEvent(EVENT_HEALINGTOUCH, 5500ms);
                                ++TouchCounter;
                            }
                            else
                            {
                                TouchCounter=0;
                                events.ScheduleEvent(EVENT_HEALINGTOUCH, 30s);
                            }
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetScholomanceAI<boss_instructormaliciaAI>(creature);
        }
};

void AddSC_boss_instructormalicia()
{
    new boss_instructor_malicia();
}

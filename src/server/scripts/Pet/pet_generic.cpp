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

/*
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "npc_pet_gen_".
 */

 /* ContentData
 npc_pet_gen_pandaren_monk          100%    Pandaren Monk drinks and bows with you
 npc_pet_gen_mojo                   100%    Mojo follows you when you kiss it
 EndContentData */

#include "ScriptMgr.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "PassiveAI.h"
#include "PetDefines.h"
#include "Player.h"
#include "ScriptedCreature.h"

enum PandarenMonkMisc
{
    SPELL_PANDAREN_MONK = 69800,
    EVENT_FOCUS = 1,
    EVENT_EMOTE = 2,
    EVENT_FOLLOW = 3,
    EVENT_DRINK = 4
};

class npc_pet_gen_pandaren_monk : public CreatureScript
{
public:
    npc_pet_gen_pandaren_monk() : CreatureScript("npc_pet_gen_pandaren_monk") {}

    struct npc_pet_gen_pandaren_monkAI : public NullCreatureAI
    {
        npc_pet_gen_pandaren_monkAI(Creature* creature) : NullCreatureAI(creature) { }

        void Reset() override
        {
            _events.Reset();
            _events.ScheduleEvent(EVENT_FOCUS, 1s);
        }

        void EnterEvadeMode(EvadeReason why) override
        {
            if (!_EnterEvadeMode(why))
                return;

            Reset();
        }

        void ReceiveEmote(Player* /*player*/, uint32 emote) override
        {
            me->InterruptSpell(CURRENT_CHANNELED_SPELL);
            me->StopMoving();

            switch (emote)
            {
            case TEXT_EMOTE_BOW:
                _events.ScheduleEvent(EVENT_FOCUS, 1s);
                break;
            case TEXT_EMOTE_DRINK:
                _events.ScheduleEvent(EVENT_DRINK, 1s);
                break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            if (Unit* owner = me->GetCharmerOrOwner())
                if (!me->IsWithinDist(owner, 30.f))
                    me->InterruptSpell(CURRENT_CHANNELED_SPELL);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FOCUS:
                    if (Unit* owner = me->GetCharmerOrOwner())
                        me->SetFacingToObject(owner);
                    _events.ScheduleEvent(EVENT_EMOTE, 1s);
                    break;
                case EVENT_EMOTE:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    _events.ScheduleEvent(EVENT_FOLLOW, 1s);
                    break;
                case EVENT_FOLLOW:
                    if (Unit* owner = me->GetCharmerOrOwner())
                        me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                    break;
                case EVENT_DRINK:
                    me->CastSpell(me, SPELL_PANDAREN_MONK, false);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pet_gen_pandaren_monkAI(creature);
    }
};

enum Mojo
{
    SAY_MOJO                = 0,

    SPELL_FEELING_FROGGY    = 43906,
    SPELL_SEDUCTION_VISUAL  = 43919
};

class npc_pet_gen_mojo : public CreatureScript
{
    public:
        npc_pet_gen_mojo() : CreatureScript("npc_pet_gen_mojo") { }

        struct npc_pet_gen_mojoAI : public ScriptedAI
        {
            npc_pet_gen_mojoAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                _victimGUID.Clear();

                if (Unit* owner = me->GetOwner())
                    me->GetMotionMaster()->MoveFollow(owner, 0.0f, 0.0f);
            }

            void JustEngagedWith(Unit* /*who*/) override { }
            void UpdateAI(uint32 /*diff*/) override { }

            void ReceiveEmote(Player* player, uint32 emote) override
            {
                me->HandleEmoteCommand(emote);
                Unit* owner = me->GetOwner();
                if (emote != TEXT_EMOTE_KISS || !owner || owner->GetTypeId() != TYPEID_PLAYER ||
                    owner->ToPlayer()->GetTeam() != player->GetTeam())
                {
                    return;
                }

                Talk(SAY_MOJO, player);

                if (!_victimGUID.IsEmpty())
                    if (Player* victim = ObjectAccessor::GetPlayer(*me, _victimGUID))
                        victim->RemoveAura(SPELL_FEELING_FROGGY);

                _victimGUID = player->GetGUID();

                DoCast(player, SPELL_FEELING_FROGGY, true);
                DoCast(me, SPELL_SEDUCTION_VISUAL, true);
                me->GetMotionMaster()->MoveFollow(player, 0.0f, 0.0f);
            }

        private:
            ObjectGuid _victimGUID;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_pet_gen_mojoAI(creature);
        }
};

enum SoulTrader
{
    SAY_SOUL_TRADER_INTRO           = 0,

    SPELL_ETHEREAL_ONSUMMON         = 50052,
    SPELL_ETHEREAL_PET_REMOVE_AURA  = 50055
};

struct npc_pet_gen_soul_trader : public ScriptedAI
{
    npc_pet_gen_soul_trader(Creature* creature) : ScriptedAI(creature) { }

    void LeavingWorld() override
    {
        if (Unit* owner = me->GetOwner())
            DoCast(owner, SPELL_ETHEREAL_PET_REMOVE_AURA);
    }

    void JustAppeared() override
    {
        Talk(SAY_SOUL_TRADER_INTRO);
        if (Unit* owner = me->GetOwner())
            DoCast(owner, SPELL_ETHEREAL_ONSUMMON);

        CreatureAI::JustAppeared();
    }
};

void AddSC_generic_pet_scripts()
{
    new npc_pet_gen_pandaren_monk();
    new npc_pet_gen_mojo();
    RegisterCreatureAI(npc_pet_gen_soul_trader);
}

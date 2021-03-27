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
Blasted_Lands
Quest support: 3628.
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "Player.h"
#include "Group.h"

enum DeathlyUsher
{
    SPELL_TELEPORT_SINGLE               = 12885,
    SPELL_TELEPORT_SINGLE_IN_GROUP      = 13142,
    SPELL_TELEPORT_GROUP                = 27686
};

/*#####
# spell_razelikh_teleport_group
#####*/

class spell_razelikh_teleport_group : public SpellScriptLoader
{
    public: spell_razelikh_teleport_group() : SpellScriptLoader("spell_razelikh_teleport_group") { }

        class spell_razelikh_teleport_group_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_razelikh_teleport_group_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                return ValidateSpellInfo({ SPELL_TELEPORT_SINGLE, SPELL_TELEPORT_SINGLE_IN_GROUP });
            }

            void HandleScriptEffect(SpellEffIndex /* effIndex */)
            {
                if (Player* player = GetHitPlayer())
                {
                    if (Group* group = player->GetGroup())
                    {
                        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                            if (Player* member = itr->GetSource())
                                if (member->IsWithinDistInMap(player, 20.0f) && !member->isDead())
                                    member->CastSpell(member, SPELL_TELEPORT_SINGLE_IN_GROUP, true);
                    }
                    else
                        player->CastSpell(player, SPELL_TELEPORT_SINGLE, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_razelikh_teleport_group_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_razelikh_teleport_group_SpellScript();
        }
};

enum BlastedLandsMisc
{
    BLASTER_LANDS_ZONE_ID   = 4,

    MAP_BLASTED_LANDS_PHASE = 1190,
    MAP_EASTERN_KINGDOMS    = 0,

    SPELL_TIME_TRAVELLING   = 176111
};

class DarkPortal_Phasing: public PlayerScript
{
public:
    DarkPortal_Phasing() : PlayerScript("DarkPortal_Phasing") { }

    void OnUpdateZone(Player* player, uint32 newZoneID, uint32 oldZoneID, uint32 /*newAreaID*/) override
    {
        if (player->IsInFlight())
            return;

        if (player->GetMapId() == MAP_EASTERN_KINGDOMS || player->GetMapId() == MAP_BLASTED_LANDS_PHASE)
        {
            if (newZoneID != oldZoneID && (newZoneID == BLASTER_LANDS_ZONE_ID || oldZoneID == BLASTER_LANDS_ZONE_ID))
            {
                if (player->getLevel() >= 90 && newZoneID == BLASTER_LANDS_ZONE_ID && player->GetMapId() == MAP_EASTERN_KINGDOMS && !player->HasAura(SPELL_TIME_TRAVELLING))
                {
                    player->SeamlessTeleportToMap(MAP_BLASTED_LANDS_PHASE);
                }
                if (newZoneID != BLASTER_LANDS_ZONE_ID && player->GetMapId() == MAP_BLASTED_LANDS_PHASE)
                {
                    player->SeamlessTeleportToMap(MAP_EASTERN_KINGDOMS);
                }
            }
        }
    }
};

void AddSC_blasted_lands()
{
    RegisterPlayerScript(DarkPortal_Phasing);
    new spell_razelikh_teleport_group();
}

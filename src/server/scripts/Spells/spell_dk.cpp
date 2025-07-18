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
 * Scripts for spells with SPELLFAMILY_DEATHKNIGHT and SPELLFAMILY_GENERIC spells used by deathknight players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dk_".
 */

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "PetAI.h"
#include "DynamicObject.h"
#include "ScriptMgr.h"
#include "Containers.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include <numeric>

enum DeathKnightSpells
{
    SPELL_DK_ANTI_MAGIC_BARRIER                 = 205727,
    SPELL_DK_ARMY_FLESH_BEAST_TRANSFORM         = 127533,
    SPELL_DK_ARMY_GEIST_TRANSFORM               = 127534,
    SPELL_DK_ARMY_NORTHREND_SKELETON_TRANSFORM  = 127528,
    SPELL_DK_ARMY_SKELETON_TRANSFORM            = 127527,
    SPELL_DK_ARMY_SPIKED_GHOUL_TRANSFORM        = 127525,
    SPELL_DK_ARMY_SUPER_ZOMBIE_TRANSFORM        = 127526,
    SPELL_DK_BLINDING_SLEET_SLOW                = 317898,
    SPELL_DK_BLOOD                              = 137008,
    SPELL_DK_BLOODDRINKER_DEBUFF                = 458687,
    SPELL_DK_BLOOD_PLAGUE                       = 55078,
    SPELL_DK_BLOOD_SHIELD_ABSORB                = 77535,
    SPELL_DK_BLOOD_SHIELD_MASTERY               = 77513,
    SPELL_DK_BONE_SHIELD                        = 195181,
    SPELL_DK_BREATH_OF_SINDRAGOSA               = 152279,
    SPELL_DK_BRITTLE_DEBUFF                     = 374557,
    SPELL_DK_CLEAVING_STRIKES                   = 316916,
    SPELL_DK_CORPSE_EXPLOSION_TRIGGERED         = 43999,
    SPELL_DK_DARK_SIMULACRUM_BUFF               = 77616,
    SPELL_DK_DARK_SIMULACRUM_SPELLPOWER_BUFF    = 94984,
    SPELL_DK_DEATH_AND_DECAY_DAMAGE             = 52212,
    SPELL_DK_DEATH_AND_DECAY_INCREASE_TARGETS   = 188290,
    SPELL_DK_DEATH_COIL_DAMAGE                  = 47632,
    SPELL_DK_DEATH_GRIP_DUMMY                   = 243912,
    SPELL_DK_DEATH_GRIP_JUMP                    = 49575,
    SPELL_DK_DEATH_GRIP_TAUNT                   = 51399,
    SPELL_DK_DEATH_STRIKE_ENABLER               = 89832, // Server Side
    SPELL_DK_DEATH_STRIKE_HEAL                  = 45470,
    SPELL_DK_DEATH_STRIKE_OFFHAND               = 66188,
    SPELL_DK_FESTERING_WOUND                    = 194310,
    SPELL_DK_FROST                              = 137006,
    SPELL_DK_FROST_FEVER                        = 55095,
    SPELL_DK_FROST_SCYTHE                       = 207230,
    SPELL_DK_FROST_SHIELD                       = 207203,
    SPELL_DK_GLYPH_OF_FOUL_MENAGERIE            = 58642,
    SPELL_DK_GLYPH_OF_THE_GEIST                 = 58640,
    SPELL_DK_GLYPH_OF_THE_SKELETON              = 146652,
    SPELL_DK_GOREFIENDS_GRASP                   = 108199,
    SPELL_DK_HEARTBREAKER_ENERGIZE              = 210738,
    SPELL_DK_HEARTBREAKER_TALENT                = 221536,
    SPELL_DK_ICE_PRISON_ROOT                    = 454787,
    SPELL_DK_ICE_PRISON_TALENT                  = 454786,
    SPELL_DK_KILLING_MACHINE_PROC               = 51124,
    SPELL_DK_MARK_OF_BLOOD_HEAL                 = 206945,
    SPELL_DK_NECROSIS_EFFECT                    = 216974,
    SPELL_DK_OBLITERATION                       = 281238,
    SPELL_DK_OBLITERATION_RUNE_ENERGIZE         = 281327,
    SPELL_DK_PILLAR_OF_FROST                    = 51271,
    SPELL_DK_RAISE_DEAD_SUMMON                  = 52150,
    SPELL_DK_REAPER_OF_SOULS_PROC               = 469172,
    SPELL_DK_RECENTLY_USED_DEATH_STRIKE         = 180612,
    SPELL_DK_RUNIC_CORRUPTION                   = 51460,
    SPELL_DK_RUNIC_POWER_ENERGIZE               = 49088,
    SPELL_DK_RUNIC_RETURN                       = 61258,
    SPELL_DK_SANGUINE_GROUND_TALENT             = 391458,
    SPELL_DK_SANGUINE_GROUND                    = 391459,
    SPELL_DK_SLUDGE_BELCHER                     = 207313,
    SPELL_DK_SLUDGE_BELCHER_SUMMON              = 212027,
    SPELL_DK_SMOTHERING_OFFENSE                 = 435005,
    SPELL_DK_SOUL_REAPER                        = 343294,
    SPELL_DK_SOUL_REAPER_DAMAGE                 = 343295,
    SPELL_DK_SUBDUING_GRASP_DEBUFF              = 454824,
    SPELL_DK_SUBDUING_GRASP_TALENT              = 454822,
    SPELL_DK_UNHOLY                             = 137007,
    SPELL_DK_UNHOLY_GROUND_HASTE                = 374271,
    SPELL_DK_UNHOLY_GROUND_TALENT               = 374265,
    SPELL_DK_UNHOLY_VIGOR                       = 196263,
    SPELL_DH_VORACIOUS_LEECH                    = 274009,
    SPELL_DH_VORACIOUS_TALENT                   = 273953,
    SPELL_DK_GLYPH_OF_ABSORB_MAGIC              = 159415,
    SPELL_DK_NECROSIS                           = 207346,
    SPELL_DK_IMPROVED_BLOOD_PRESENCE            = 50371,
    SPELL_DK_VIRULENT_PLAGUE                    = 191587,
    SPELL_DK_UNHOLY_FRENZY                      = 207289,
    SPELL_DK_UNHOLY_FRENZY_BUFF                 = 207290,
    SPELL_DK_PESTILENT_PUSTULES                 = 194917,
    SPELL_DK_OUTBREAK_PERIODIC                  = 196782,
    SPELL_DK_CASTIGATOR                         = 207305,
    SPELL_DK_FESTERING_WOUND_DAMAGE             = 194311,
    SPELL_DK_SUMMON_DEFILE                      = 169018, //npc 82521
    SPELL_DK_DEFILE_DUMMY                       = 156004,
    SPELL_DK_DEFILE_DAMAGE                      = 156000,
    SPELL_DK_DEFILE_MASTERY                     = 218100,
    SPELL_DK_DARK_INFUSION_STACKS               = 91342,
    SPELL_DK_EPIDEMIC_DAMAGE_SINGLE             = 212739,
    SPELL_DK_EPIDEMIC_DAMAGE_AOE                = 215969,
    SPELL_DK_ALL_WILL_SERVE                     = 194916,
    SPELL_DK_ALL_WILL_SERVE_SUMMON              = 196910,
    SPELL_DK_ARMY_OF_THE_DAMNED                 = 276837,
    SPELL_DK_ARMY_OF_THE_DEAD                   = 42650,
    SPELL_DK_APOCALYPSE                         = 275699,
};

enum Misc
{
    NPC_DK_DANCING_RUNE_WEAPON                  = 27893
};

// 70656 - Advantage (T10 4P Melee Bonus)
class spell_dk_advantage_t10_4p : public AuraScript
{
    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (Unit* caster = eventInfo.GetActor())
        {
            Player* player = caster->ToPlayer();
            if (!player  || caster->GetClass() != CLASS_DEATH_KNIGHT)
                return false;

            for (uint8 i = 0; i < player->GetMaxPower(POWER_RUNES); ++i)
                if (player->GetRuneCooldown(i) == 0)
                    return false;

            return true;
        }

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_advantage_t10_4p::CheckProc);
    }
};

// 48707 - Anti-Magic Shell
class spell_dk_anti_magic_shell : public AuraScript
{
public:
    spell_dk_anti_magic_shell()
    {
        absorbPct = 0;
        maxHealth = 0;
        absorbedAmount = 0;
    }

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_DK_RUNIC_POWER_ENERGIZE })
            && ValidateSpellEffect({ { spellInfo->Id, EFFECT_1 }, { SPELL_DK_ANTI_MAGIC_BARRIER, EFFECT_2 } });
    }

    bool Load() override
    {
        absorbPct = GetEffectInfo(EFFECT_1).CalcValue(GetCaster());
        maxHealth = GetCaster()->GetMaxHealth();
        absorbedAmount = 0;
        return true;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = CalculatePct(maxHealth, absorbPct);

        if (AuraEffect* const antiMagicBarrier = GetCaster()->GetAuraEffect(SPELL_DK_ANTI_MAGIC_BARRIER, EFFECT_2))
            AddPct(amount, antiMagicBarrier->GetAmount());

        if (Player const* player = GetUnitOwner()->ToPlayer())
            AddPct(amount, player->GetRatingBonusValue(CR_VERSATILITY_DAMAGE_DONE) + player->GetTotalAuraModifier(SPELL_AURA_MOD_VERSATILITY));
    }

    void Trigger(AuraEffect* aurEff, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
    {
        absorbedAmount += absorbAmount;

        CastSpellExtraArgs args(aurEff);
        args.AddSpellMod(SPELLVALUE_BASE_POINT0, CalculatePct(absorbAmount, 2 * absorbAmount * 100 / maxHealth));
        GetTarget()->CastSpell(GetTarget(), SPELL_DK_RUNIC_POWER_ENERGIZE, args);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell::Trigger, EFFECT_0);
    }

private:
    int32 absorbPct;
    int32 maxHealth;
    uint32 absorbedAmount;
};

// 195182 - Marrowrend
// 195292 - Death's Caress
class spell_dk_apply_bone_shield : public SpellScript
{
public:
    explicit spell_dk_apply_bone_shield(SpellEffIndex effIndex) : _effIndex(effIndex) { }

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_DK_BONE_SHIELD })
            && ValidateSpellEffect({ { spellInfo->Id, _effIndex } })
            && spellInfo->GetEffect(_effIndex).CalcBaseValue(nullptr, nullptr, 0, 0) <= int32(sSpellMgr->AssertSpellInfo(SPELL_DK_BONE_SHIELD, DIFFICULTY_NONE)->StackAmount);
    }

    void HandleHitTarget(SpellEffIndex /*effIndex*/) const
    {
        Unit* caster = GetCaster();
        for (int32 i = 0; i < GetEffectValue(); ++i)
            caster->CastSpell(caster, SPELL_DK_BONE_SHIELD, CastSpellExtraArgs()
                .SetTriggerFlags(TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR)
                .SetTriggeringSpell(GetSpell()));
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_dk_apply_bone_shield::HandleHitTarget, _effIndex, SPELL_EFFECT_DUMMY);
    }

private:
    SpellEffIndex _effIndex;
};

static uint32 const ArmyTransforms[]
{
    SPELL_DK_ARMY_FLESH_BEAST_TRANSFORM,
    SPELL_DK_ARMY_GEIST_TRANSFORM,
    SPELL_DK_ARMY_NORTHREND_SKELETON_TRANSFORM,
    SPELL_DK_ARMY_SKELETON_TRANSFORM,
    SPELL_DK_ARMY_SPIKED_GHOUL_TRANSFORM,
    SPELL_DK_ARMY_SUPER_ZOMBIE_TRANSFORM
};

// 127517 - Army Transform
/// 6.x, does this belong here or in spell_generic? where do we cast this? sniffs say this is only cast when caster has glyph of foul menagerie.
class spell_dk_army_transform : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_GLYPH_OF_FOUL_MENAGERIE });
    }

    bool Load() override
    {
        return GetCaster()->IsGuardian();
    }

    SpellCastResult CheckCast()
    {
        if (Unit* owner = GetCaster()->GetOwner())
            if (owner->HasAura(SPELL_DK_GLYPH_OF_FOUL_MENAGERIE))
                return SPELL_CAST_OK;

        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), Trinity::Containers::SelectRandomContainerElement(ArmyTransforms), true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_army_transform::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_dk_army_transform::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 207167 - Blinding Sleet
class spell_dk_blinding_sleet : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_BLINDING_SLEET_SLOW });
    }

    void HandleOnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
            GetTarget()->CastSpell(GetTarget(), SPELL_DK_BLINDING_SLEET_SLOW, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_dk_blinding_sleet::HandleOnRemove, EFFECT_0, SPELL_AURA_MOD_CONFUSE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 206931 - Blooddrinker
class spell_dk_blooddrinker : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_BLOODDRINKER_DEBUFF });
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/) const
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), SPELL_DK_BLOODDRINKER_DEBUFF, CastSpellExtraArgsInit{
                .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR,
                .TriggeringAura = aurEff
            });
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_dk_blooddrinker::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_LEECH, AURA_EFFECT_HANDLE_REAL);
    }
};

// 50842 - Blood Boil
class spell_dk_blood_boil : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_BLOOD_PLAGUE });
    }

    void HandleEffect()
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_BLOOD_PLAGUE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_blood_boil::HandleEffect);
    }
};

// 374504 - Brittle
class spell_dk_brittle : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_BRITTLE_DEBUFF });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo const& eventInfo) const
    {
        GetTarget()->CastSpell(eventInfo.GetActionTarget(), SPELL_DK_BRITTLE_DEBUFF, CastSpellExtraArgsInit{
            .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR
        });
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dk_brittle::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 49028 - Dancing Rune Weapon
/// 7.1.5
class spell_dk_dancing_rune_weapon : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        if (!sObjectMgr->GetCreatureTemplate(NPC_DK_DANCING_RUNE_WEAPON))
            return false;
        return true;
    }

    // This is a port of the old switch hack in Unit.cpp, it's not correct
    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = GetCaster();
        if (!caster)
            return;

        Unit* drw = nullptr;
        for (Unit* controlled : caster->m_Controlled)
        {
            if (controlled->GetEntry() == NPC_DK_DANCING_RUNE_WEAPON)
            {
                drw = controlled;
                break;
            }
        }

        if (!drw || !drw->GetVictim())
            return;

        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        int32 amount = static_cast<int32>(damageInfo->GetDamage()) / 2;
        SpellNonMeleeDamage log(drw, drw->GetVictim(), spellInfo, { static_cast<uint32>(spellInfo->GetSpellXSpellVisualId(drw)), 0 }, spellInfo->GetSchoolMask());
        log.damage = amount;
        Unit::DealDamage(drw, drw->GetVictim(), amount, nullptr, SPELL_DIRECT_DAMAGE, spellInfo->GetSchoolMask(), spellInfo, true);
        drw->SendSpellNonMeleeDamageLog(&log);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dk_dancing_rune_weapon::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 77606 - Dark Simulacrum
class spell_dk_dark_simulacrum : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_DARK_SIMULACRUM_BUFF, SPELL_DK_DARK_SIMULACRUM_SPELLPOWER_BUFF });
    }

    bool CheckProc(AuraEffect const* /*aurEff*/, ProcEventInfo const& eventInfo) const
    {
        Spell const* procSpell = eventInfo.GetProcSpell();
        if (!procSpell)
            return false;

        if (!GetTarget()->IsPlayer())
            return procSpell->GetSpellInfo()->HasAttribute(SPELL_ATTR9_ALLOW_DARK_SIMULACRUM);

        if (!procSpell->HasPowerTypeCost(POWER_MANA))
            return false;

        // filter out spells not castable by mind controlled players (teleports, summons, item creations (healthstones))
        if (procSpell->GetSpellInfo()->HasAttribute(SPELL_ATTR1_NO_AUTOCAST_AI))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo const& eventInfo) const
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->CastSpell(caster, SPELL_DK_DARK_SIMULACRUM_BUFF, CastSpellExtraArgs()
            .SetTriggerFlags(TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR)
            .SetTriggeringSpell(eventInfo.GetProcSpell())
            .AddSpellMod(SPELLVALUE_BASE_POINT0, eventInfo.GetSpellInfo()->Id));

        caster->CastSpell(caster, SPELL_DK_DARK_SIMULACRUM_SPELLPOWER_BUFF, CastSpellExtraArgs()
            .SetTriggerFlags(TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR)
            .SetTriggeringSpell(eventInfo.GetProcSpell())
            .AddSpellMod(SPELLVALUE_BASE_POINT0, GetTarget()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_MAGIC))
            .AddSpellMod(SPELLVALUE_BASE_POINT1, GetTarget()->SpellBaseHealingBonusDone(SPELL_SCHOOL_MASK_MAGIC)));
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_dk_dark_simulacrum::CheckProc, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectProc += AuraEffectProcFn(spell_dk_dark_simulacrum::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 77616 - Dark Simulacrum
class spell_dk_dark_simulacrum_buff : public AuraScript
{
    bool CheckProc(AuraEffect const* aurEff, ProcEventInfo const& eventInfo) const
    {
        return uint32(aurEff->GetAmount()) == eventInfo.GetSpellInfo()->Id;
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_dk_dark_simulacrum_buff::CheckProc, EFFECT_0, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_TRIGGERED);
    }
};

// 43265 - Death and Decay (Aura)
class spell_dk_death_and_decay : public AuraScript
{
    void HandleDummyTick(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), SPELL_DK_DEATH_AND_DECAY_DAMAGE, aurEff);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_death_and_decay::HandleDummyTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 188290 - Death and Decay (triggered by 316916 - Cleaving Strikes)
class spell_dk_death_and_decay_increase_targets : public AuraScript
{
    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_UNHOLY_GROUND_HASTE });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/) const
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DK_UNHOLY_GROUND_HASTE);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_dk_death_and_decay_increase_targets::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 47541 - Death Coil
class spell_dk_death_coil : public SpellScript
{
    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_DEATH_COIL_DAMAGE, SPELL_DK_UNHOLY, SPELL_DK_UNHOLY_VIGOR });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        caster->CastSpell(GetHitUnit(), SPELL_DK_DEATH_COIL_DAMAGE, true);
        if (AuraEffect const* unholyAura = caster->GetAuraEffect(SPELL_DK_UNHOLY, EFFECT_6)) // can be any effect, just here to send SPELL_FAILED_DONT_REPORT on failure
            caster->CastSpell(caster, SPELL_DK_UNHOLY_VIGOR, unholyAura);

        if (GetCaster()->HasAura(SPELL_DK_ARMY_OF_THE_DAMNED))
        {
            if (GetCaster()->GetSpellHistory()->HasCooldown(SPELL_DK_ARMY_OF_THE_DEAD))
                GetCaster()->GetSpellHistory()->ModifyCooldown(SPELL_DK_ARMY_OF_THE_DEAD, -5000ms);

            if (GetCaster()->GetSpellHistory()->HasCooldown(SPELL_DK_APOCALYPSE))
                GetCaster()->GetSpellHistory()->ModifyCooldown(SPELL_DK_APOCALYPSE, -1000ms);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_death_coil::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 52751 - Death Gate
class spell_dk_death_gate : public SpellScript
{
    SpellCastResult CheckClass()
    {
        if (GetCaster()->GetClass() != CLASS_DEATH_KNIGHT)
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_DEATH_KNIGHT);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        return SPELL_CAST_OK;
    }

    void HandleScript(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        Unit* target = GetHitUnit();
        if (!target)
            return;

        if (Player* player = target->ToPlayer())
        {
            if (player->IsManualRecallPositionValid())
            {
                player->ManualRecall();
                player->ClearManualRecallPosition();
                return;
            }
            else
            {
                player->SaveManualRecallPosition();
            }

            int QUEST_SCARLET_ARMIES_APPROACH = 12757; // only time death gate teles to classic ebon hold.
            int QUEST_LIGHT_OF_DAWN = 12801; // lights hope chappel fight. after this death gate teles to final phase of classic ebon hold.
            int QUEST_THE_BATTLE_FOR_EBON_HOLD = 13166;
            /* If player is over level 45 tele to legion ebon hold */
            if (player->GetLevel() >= 45 || (player->GetQuestStatus(QUEST_THE_BATTLE_FOR_EBON_HOLD) == QUEST_STATUS_REWARDED))
            {
                player->TeleportTo(1220, -1503.367f, 1052.059f, 260.396f, 3.75f); // legion ebon hold
            }

            /* If on quest 12757 "Scarlet enemies approach" */
            else if ((player->GetQuestStatus(QUEST_LIGHT_OF_DAWN) == QUEST_STATUS_NONE) && (player->GetQuestStatus(QUEST_SCARLET_ARMIES_APPROACH) == QUEST_STATUS_NONE) && (!player->IsAlliedRace())
                || (player->HasQuest(QUEST_SCARLET_ARMIES_APPROACH) || (player->IsAlliedRace())))
            {
                player->TeleportTo(609, 2368.0444f, -5656.1748f, 382.2804f, 3.74f); // classic ebon hold
            }

            /* If quest 12801 "Light of Dawn" is completed*/
            else if ((player->GetQuestStatus(QUEST_LIGHT_OF_DAWN) == QUEST_STATUS_REWARDED) && (player->GetQuestStatus(QUEST_SCARLET_ARMIES_APPROACH) == QUEST_STATUS_REWARDED))
            {
                player->TeleportTo(0, 2368.0444f, -5656.1748f, 382.2804f, 3.735f); // final phase of classic ebon hold
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_death_gate::CheckClass);
        OnEffectHitTarget += SpellEffectFn(spell_dk_death_gate::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 49576 - Death Grip Initial
class spell_dk_death_grip_initial : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_DK_DEATH_GRIP_DUMMY,
            SPELL_DK_DEATH_GRIP_JUMP,
            SPELL_DK_BLOOD,
            SPELL_DK_DEATH_GRIP_TAUNT
        });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        // Death Grip should not be castable while jumping/falling
        if (caster->HasUnitState(UNIT_STATE_JUMPING) || caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
            return SPELL_FAILED_MOVING;

        return SPELL_CAST_OK;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_DEATH_GRIP_DUMMY, true);
        GetHitUnit()->CastSpell(GetCaster(), SPELL_DK_DEATH_GRIP_JUMP, true);
        if (GetCaster()->HasAura(SPELL_DK_BLOOD))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_DEATH_GRIP_TAUNT, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_death_grip_initial::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_dk_death_grip_initial::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 48743 - Death Pact
class spell_dk_death_pact : public AuraScript
{
    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellEffect({ { spellInfo->Id, EFFECT_2 } });
    }

    void HandleCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = GetCaster())
            amount = int32(caster->CountPctFromMaxHealth(GetEffectInfo(EFFECT_2).CalcValue(caster)));
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_death_pact::HandleCalcAmount, EFFECT_1, SPELL_AURA_SCHOOL_HEAL_ABSORB);
    }
};

// 49998 - Death Strike
class spell_dk_death_strike : public SpellScript
{
    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo(
        {
            SPELL_DK_DEATH_STRIKE_ENABLER,
            SPELL_DK_DEATH_STRIKE_HEAL,
            SPELL_DK_BLOOD_SHIELD_MASTERY,
            SPELL_DK_BLOOD_SHIELD_ABSORB,
            SPELL_DK_FROST,
            SPELL_DK_DEATH_STRIKE_OFFHAND
        })
            && ValidateSpellEffect({ { spellInfo->Id, EFFECT_2 } });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (AuraEffect* enabler = caster->GetAuraEffect(SPELL_DK_DEATH_STRIKE_ENABLER, EFFECT_0, GetCaster()->GetGUID()))
        {
            // Heals you for 25% of all damage taken in the last 5 sec,
            int32 heal = CalculatePct(enabler->CalculateAmount(GetCaster()), GetEffectInfo(EFFECT_1).CalcValue(GetCaster()));
            // minimum 7.0% of maximum health.
            int32 pctOfMaxHealth = CalculatePct(GetEffectInfo(EFFECT_2).CalcValue(GetCaster()), caster->GetMaxHealth());
            heal = std::max(heal, pctOfMaxHealth);

            caster->CastSpell(caster, SPELL_DK_DEATH_STRIKE_HEAL, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_BASE_POINT0, heal));

            if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_DK_BLOOD_SHIELD_MASTERY, EFFECT_0))
                caster->CastSpell(caster, SPELL_DK_BLOOD_SHIELD_ABSORB, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_BASE_POINT0, CalculatePct(heal, aurEff->GetAmount())));

            if (caster->HasAura(SPELL_DK_FROST))
                caster->CastSpell(GetHitUnit(), SPELL_DK_DEATH_STRIKE_OFFHAND, true);
        }
    }

    void TriggerRecentlyUsedDeathStrike()
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_DK_RECENTLY_USED_DEATH_STRIKE, true);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_dk_death_strike::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
        AfterCast += SpellCastFn(spell_dk_death_strike::TriggerRecentlyUsedDeathStrike);
    }
};

// 89832 - Death Strike Enabler - SPELL_DK_DEATH_STRIKE_ENABLER
class spell_dk_death_strike_enabler : public AuraScript
{
    // Amount of seconds we calculate damage over
    constexpr static uint8 LAST_SECONDS = 5;

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetDamageInfo() != nullptr;
    }

    void Update(AuraEffect* /*aurEff*/)
    {
        // Move backwards all datas by one from [23][0][0][0][0] -> [0][23][0][0][0]
        std::move_backward(_damagePerSecond.begin(), std::next(_damagePerSecond.begin(), LAST_SECONDS - 1), _damagePerSecond.end());
        _damagePerSecond[0] = 0;
    }

    void HandleCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = true;
        amount = int32(std::accumulate(_damagePerSecond.begin(), _damagePerSecond.end(), 0u));
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        _damagePerSecond[0] += eventInfo.GetDamageInfo()->GetDamage();
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_death_strike_enabler::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dk_death_strike_enabler::HandleProc, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_death_strike_enabler::HandleCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_dk_death_strike_enabler::Update, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }

private:
    std::array<uint32, LAST_SECONDS> _damagePerSecond = { };
};

// 85948 - Festering Strike
class spell_dk_festering_strike : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_FESTERING_WOUND });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_FESTERING_WOUND, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_AURA_STACK, GetEffectValue()));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_festering_strike::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// 195621 - Frost Fever
class spell_dk_frost_fever_proc : public AuraScript
{
    bool CheckProc(AuraEffect const* aurEff, ProcEventInfo const& /*eventInfo*/) const
    {
        return roll_chance_i(aurEff->GetAmount());
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_dk_frost_fever_proc::CheckProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 47496 - Explode, Ghoul spell for Corpse Explosion
class spell_dk_ghoul_explode : public SpellScript
{
    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_DK_CORPSE_EXPLOSION_TRIGGERED }) && ValidateSpellEffect({ { spellInfo->Id, EFFECT_2 } });
    }

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        SetHitDamage(GetCaster()->CountPctFromMaxHealth(GetEffectInfo(EFFECT_2).CalcValue(GetCaster())));
    }

    void Suicide(SpellEffIndex /*effIndex*/)
    {
        if (Unit* unitTarget = GetHitUnit())
        {
            // Corpse Explosion (Suicide)
            unitTarget->CastSpell(unitTarget, SPELL_DK_CORPSE_EXPLOSION_TRIGGERED, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_ghoul_explode::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_dk_ghoul_explode::Suicide, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 69961 - Glyph of Scourge Strike
class spell_dk_glyph_of_scourge_strike_script : public SpellScript
{
    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        Unit::AuraEffectList const& mPeriodic = target->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE);
        for (Unit::AuraEffectList::const_iterator i = mPeriodic.begin(); i != mPeriodic.end(); ++i)
        {
            AuraEffect const* aurEff = *i;
            SpellInfo const* spellInfo = aurEff->GetSpellInfo();
            // search our Blood Plague and Frost Fever on target
            if (spellInfo->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && spellInfo->SpellFamilyFlags[2] & 0x2 &&
                aurEff->GetCasterGUID() == caster->GetGUID())
            {
                uint32 countMin = aurEff->GetBase()->GetMaxDuration();
                uint32 countMax = spellInfo->GetMaxDuration();

                // this Glyph
                countMax += 9000;

                if (countMin < countMax)
                {
                    aurEff->GetBase()->SetDuration(aurEff->GetBase()->GetDuration() + 3000);
                    aurEff->GetBase()->SetMaxDuration(countMin + 3000);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_glyph_of_scourge_strike_script::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Called by 206930 - Heart Strike
class spell_dk_heartbreaker : public SpellScript
{
    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_HEARTBREAKER_TALENT, SPELL_DK_HEARTBREAKER_ENERGIZE });
    }

    bool Load() override
    {
        return GetCaster()->HasAura(SPELL_DK_HEARTBREAKER_TALENT);
    }

    void HandleEnergize(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_DK_HEARTBREAKER_ENERGIZE, CastSpellExtraArgs()
            .SetTriggeringSpell(GetSpell())
            .SetTriggerFlags(TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_heartbreaker::HandleEnergize, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 49184 - Howling Blast
class spell_dk_howling_blast : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_FROST_FEVER });
    }

    void HandleFrostFever(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_FROST_FEVER);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_howling_blast::HandleFrostFever, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Called by 45524 - Chains of Ice
// 454786 - Ice Prison
class spell_dk_ice_prison : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_ICE_PRISON_TALENT, SPELL_DK_ICE_PRISON_ROOT });
    }

    bool Load() override
    {
        return GetCaster()->HasAura(SPELL_DK_ICE_PRISON_TALENT);
    }

    void HandleOnHit() const
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_ICE_PRISON_ROOT, CastSpellExtraArgsInit{
            .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR,
            .TriggeringSpell = GetSpell()
        });
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_dk_ice_prison::HandleOnHit);
    }
};

// 194878 - Icy Talons
class spell_dk_icy_talons : public AuraScript
{
    bool CheckProc(AuraEffect const* /*aurEff*/, ProcEventInfo const& eventInfo) const
    {
        if (Spell const* procSpell = eventInfo.GetProcSpell())
            return procSpell->GetPowerTypeCostAmount(POWER_RUNIC_POWER) > 0;

        return false;
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_dk_icy_talons::CheckProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE);
    }
};

// 194879 - Icy Talons
class spell_dk_icy_talons_buff : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_SMOTHERING_OFFENSE });
    }

    void HandleSmotheringOffense(WorldObject*& target) const
    {
        if (!GetCaster()->HasAura(SPELL_DK_SMOTHERING_OFFENSE))
            target = nullptr;
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_dk_icy_talons_buff::HandleSmotheringOffense, EFFECT_1, TARGET_UNIT_CASTER);
    }
};

// 374277 - Improved Death Strike
class spell_dk_improved_death_strike : public AuraScript
{
    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_DK_BLOOD })
            && ValidateSpellEffect({ { spellInfo->Id, EFFECT_4 } });
    }

    void CalcHealIncrease(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/) const
    {
        if (GetUnitOwner()->HasAura(SPELL_DK_BLOOD))
            amount = GetEffectInfo(EFFECT_3).CalcValue(GetCaster());
    }

    void CalcPowerCostReduction(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/) const
    {
        if (GetUnitOwner()->HasAura(SPELL_DK_BLOOD))
            amount = GetEffectInfo(EFFECT_4).CalcValue(GetCaster());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_improved_death_strike::CalcHealIncrease, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_improved_death_strike::CalcHealIncrease, EFFECT_1, SPELL_AURA_ADD_PCT_MODIFIER);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_improved_death_strike::CalcPowerCostReduction, EFFECT_2, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// 206940 - Mark of Blood
class spell_dk_mark_of_blood : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_MARK_OF_BLOOD_HEAL });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (Unit* caster = GetCaster())
            caster->CastSpell(eventInfo.GetProcTarget(), SPELL_DK_MARK_OF_BLOOD_HEAL, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dk_mark_of_blood::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 207346 - Necrosis
class spell_dk_necrosis : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_NECROSIS_EFFECT });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_DK_NECROSIS_EFFECT, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dk_necrosis::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 207256 - Obliteration
class spell_dk_obliteration : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_OBLITERATION, SPELL_DK_OBLITERATION_RUNE_ENERGIZE, SPELL_DK_KILLING_MACHINE_PROC })
            && ValidateSpellEffect({ { SPELL_DK_OBLITERATION, EFFECT_1 } });
    }

    void HandleProc(AuraEffect* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_DK_KILLING_MACHINE_PROC, aurEff);

        if (AuraEffect const* oblitaration = target->GetAuraEffect(SPELL_DK_OBLITERATION, EFFECT_1))
            if (roll_chance_i(oblitaration->GetAmount()))
                target->CastSpell(target, SPELL_DK_OBLITERATION_RUNE_ENERGIZE, aurEff);
    }

    void Register() override
    {
        AfterEffectProc += AuraEffectProcFn(spell_dk_obliteration::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 207200 - Permafrost
class spell_dk_permafrost : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_FROST_SHIELD });
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        CastSpellExtraArgs args(aurEff);
        args.AddSpellMod(SPELLVALUE_BASE_POINT0, CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount()));
        GetTarget()->CastSpell(GetTarget(), SPELL_DK_FROST_SHIELD, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dk_permafrost::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 121916 - Glyph of the Geist (Unholy)
/// 6.x, does this belong here or in spell_generic? apply this in creature_template_addon? sniffs say this is always cast on raise dead.
class spell_dk_pet_geist_transform : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_GLYPH_OF_THE_GEIST });
    }

    bool Load() override
    {
        return GetCaster()->IsPet();
    }

    SpellCastResult CheckCast()
    {
        if (Unit* owner = GetCaster()->GetOwner())
            if (owner->HasAura(SPELL_DK_GLYPH_OF_THE_GEIST))
                return SPELL_CAST_OK;

        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_pet_geist_transform::CheckCast);
    }
};

// 147157 Glyph of the Skeleton (Unholy)
/// 6.x, does this belong here or in spell_generic? apply this in creature_template_addon? sniffs say this is always cast on raise dead.
class spell_dk_pet_skeleton_transform : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_GLYPH_OF_THE_SKELETON });
    }

    SpellCastResult CheckCast()
    {
        if (Unit* owner = GetCaster()->GetOwner())
            if (owner->HasAura(SPELL_DK_GLYPH_OF_THE_SKELETON))
                return SPELL_CAST_OK;

        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dk_pet_skeleton_transform::CheckCast);
    }
};

// 61257 - Runic Power Back on Snare/Root
/// 7.1.5
class spell_dk_pvp_4p_bonus : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_RUNIC_RETURN });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return false;

        return (spellInfo->GetAllEffectsMechanicMask() & ((1 << MECHANIC_ROOT) | (1 << MECHANIC_SNARE))) != 0;
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActionTarget()->CastSpell(nullptr, SPELL_DK_RUNIC_RETURN, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dk_pvp_4p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dk_pvp_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 46584 - Raise Dead
class spell_dk_raise_dead : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_RAISE_DEAD_SUMMON });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        uint32 spellId = SPELL_DK_RAISE_DEAD_SUMMON;
        GetCaster()->CastSpell(nullptr, spellId, true);

        // The skeleton does not disappear after you release the pet
        if (GetCaster()->HasAura(SPELL_DK_ALL_WILL_SERVE))
            GetCaster()->CastSpell(GetCaster(), SPELL_DK_ALL_WILL_SERVE_SUMMON, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_raise_dead::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 440002 - Reaper of Souls (attached to 343294 - Soul Reaper)
class spell_dk_reaper_of_souls : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_REAPER_OF_SOULS_PROC });
    }

    bool IsAffectedByReaperOfSouls() const
    {
        if (Aura* reaperOfSouls = GetCaster()->GetAura(SPELL_DK_REAPER_OF_SOULS_PROC))
            return GetSpell()->m_appliedMods.contains(reaperOfSouls);
        return false;
    }

    void HandleDefault(WorldObject*& target) const
    {
        if (IsAffectedByReaperOfSouls())
            target = nullptr;
    }

    void HandleReaperOfSouls(SpellEffIndex effIndex)
    {
        if (!IsAffectedByReaperOfSouls())
            PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_dk_reaper_of_souls::HandleDefault, EFFECT_1, TARGET_UNIT_TARGET_ENEMY);
        OnEffectLaunch += SpellEffectFn(spell_dk_reaper_of_souls::HandleReaperOfSouls, EFFECT_3, SPELL_EFFECT_TRIGGER_SPELL);
    }
};

// 59057 - Rime
class spell_dk_rime : public AuraScript
{
    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellEffect({ { spellInfo->Id, EFFECT_1 } }) && ValidateSpellInfo({ SPELL_DK_FROST_SCYTHE });
    }

    bool CheckProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        float chance = static_cast<float>(GetSpellInfo()->GetEffect(EFFECT_1).CalcValue(GetTarget()));
        if (eventInfo.GetSpellInfo()->Id == SPELL_DK_FROST_SCYTHE)
            chance /= 2.f;

        return roll_chance_f(chance);
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_dk_rime::CheckProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 343294 - Soul Reaper
// 469180 - Soul Reaper
class spell_dk_soul_reaper : public AuraScript
{
public:
    explicit spell_dk_soul_reaper(SpellEffIndex auraEffectIndex, Optional<SpellEffIndex> healthLimitEffectIndex)
        : _auraEffectIndex(auraEffectIndex), _healthLimitEffectIndex(healthLimitEffectIndex) { }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_SOUL_REAPER, SPELL_DK_SOUL_REAPER_DAMAGE, SPELL_DK_RUNIC_CORRUPTION });
    }

    void HandleOnTick(AuraEffect const* aurEff) const
    {
        Unit* target = GetTarget();
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (!_healthLimitEffectIndex || target->GetHealthPct() < float(GetEffectInfo(*_healthLimitEffectIndex).CalcValue(caster)))
            caster->CastSpell(target, SPELL_DK_SOUL_REAPER_DAMAGE, CastSpellExtraArgsInit{
                .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR,
                .TriggeringAura = aurEff
            });
    }

    void RemoveEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/) const
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEATH)
            return;

        Player* caster = Object::ToPlayer(GetCaster());
        if (!caster)
            return;

        if (caster->isHonorOrXPTarget(GetTarget()))
            caster->CastSpell(caster, SPELL_DK_RUNIC_CORRUPTION, CastSpellExtraArgsInit{
                .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR,
                .TriggeringAura = aurEff
            });
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_soul_reaper::HandleOnTick, _auraEffectIndex, SPELL_AURA_PERIODIC_DUMMY);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dk_soul_reaper::RemoveEffect, _auraEffectIndex, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }

private:
    SpellEffIndex _auraEffectIndex;
    Optional<SpellEffIndex> _healthLimitEffectIndex;
};

// Called by 383312 Abomination Limb and 49576 - Death Grip
// 454822 - Subduing Grasp
class spell_dk_subduing_grasp : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_SUBDUING_GRASP_TALENT, SPELL_DK_SUBDUING_GRASP_DEBUFF });
    }

    bool Load() override
    {
        return GetCaster()->HasAura(SPELL_DK_SUBDUING_GRASP_TALENT);
    }

    void HandleSubduingGrasp(SpellEffIndex /*effIndex*/) const
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_DK_SUBDUING_GRASP_DEBUFF, CastSpellExtraArgsInit{
            .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR,
            .TriggeringSpell = GetSpell()
        });
    }

    void Register() override
    {
        if (m_scriptSpellId == SPELL_DK_GOREFIENDS_GRASP)
            OnEffectHitTarget += SpellEffectFn(spell_dk_subduing_grasp::HandleSubduingGrasp, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        else
            OnEffectHitTarget += SpellEffectFn(spell_dk_subduing_grasp::HandleSubduingGrasp, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 242057 - Rune Empowered
class spell_dk_t20_2p_rune_empowered : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_PILLAR_OF_FROST, SPELL_DK_BREATH_OF_SINDRAGOSA });
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& procInfo)
    {
        Spell const* procSpell = procInfo.GetProcSpell();
        if (!procSpell)
            return;

        Aura* pillarOfFrost = GetTarget()->GetAura(SPELL_DK_PILLAR_OF_FROST);
        if (!pillarOfFrost)
            return;

        _runicPowerSpent += procSpell->GetPowerTypeCostAmount(POWER_RUNIC_POWER).value_or(0);
        // Breath of Sindragosa special case
        SpellInfo const* breathOfSindragosa = sSpellMgr->AssertSpellInfo(SPELL_DK_BREATH_OF_SINDRAGOSA, DIFFICULTY_NONE);
        if (procSpell->IsTriggeredByAura(breathOfSindragosa))
        {
            auto powerItr = std::find_if(breathOfSindragosa->PowerCosts.begin(), breathOfSindragosa->PowerCosts.end(),
                [](SpellPowerEntry const* power) { return power->PowerType == POWER_RUNIC_POWER && power->PowerPctPerSecond > 0.0f; });
            if (powerItr != breathOfSindragosa->PowerCosts.end())
                _runicPowerSpent += CalculatePct(GetTarget()->GetMaxPower(POWER_RUNIC_POWER), (*powerItr)->PowerPctPerSecond);
        }

        if (_runicPowerSpent >= 600)
        {
            pillarOfFrost->SetDuration(pillarOfFrost->GetDuration() + 1000);
            _runicPowerSpent -= 600;
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dk_t20_2p_rune_empowered::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

    int32 _runicPowerSpent = 0;
};

// 55233 - Vampiric Blood
class spell_dk_vampiric_blood : public AuraScript
{
    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = GetUnitOwner()->CountPctFromMaxHealth(amount);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_vampiric_blood::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_INCREASE_HEALTH_2);
    }
};

// 273953 - Voracious (attached to 49998 - Death Strike)
class spell_dk_voracious : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DH_VORACIOUS_TALENT, SPELL_DH_VORACIOUS_LEECH });
    }

    bool Load() override
    {
        return GetCaster()->HasAura(SPELL_DH_VORACIOUS_TALENT);
    }

    void HandleHit(SpellEffIndex /*effIndex*/) const
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, SPELL_DH_VORACIOUS_LEECH, CastSpellExtraArgsInit{
            .TriggerFlags = TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_DONT_REPORT_CAST_ERROR,
            .TriggeringSpell = GetSpell()
        });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_voracious::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 43265 - Death and Decay
struct at_dk_death_and_decay : AreaTriggerAI
{
    using AreaTriggerAI::AreaTriggerAI;

    void OnUnitEnter(Unit* unit) override
    {
        if (unit->GetGUID() != at->GetCasterGuid())
            return;

        if (unit->HasAura(SPELL_DK_CLEAVING_STRIKES))
            unit->CastSpell(unit, SPELL_DK_DEATH_AND_DECAY_INCREASE_TARGETS, TRIGGERED_DONT_REPORT_CAST_ERROR);

        if (unit->HasAura(SPELL_DK_UNHOLY_GROUND_TALENT))
            unit->CastSpell(unit, SPELL_DK_UNHOLY_GROUND_HASTE);

        if (unit->HasAura(SPELL_DK_SANGUINE_GROUND_TALENT))
            unit->CastSpell(unit, SPELL_DK_SANGUINE_GROUND);
    }

    void OnUnitExit(Unit* unit) override
    {
        if (unit->GetGUID() != at->GetCasterGuid())
            return;

        if (!unit->HasAura(SPELL_DK_CLEAVING_STRIKES))
            unit->RemoveAurasDueToSpell(SPELL_DK_UNHOLY_GROUND_HASTE);

        if (Aura* deathAndDecay = unit->GetAura(SPELL_DK_DEATH_AND_DECAY_INCREASE_TARGETS))
        {
            if (AuraEffect* const cleavingStrikes = unit->GetAuraEffect(SPELL_DK_CLEAVING_STRIKES, EFFECT_3))
                deathAndDecay->SetDuration(cleavingStrikes->GetAmount());
        }

        unit->RemoveAurasDueToSpell(SPELL_DK_SANGUINE_GROUND);
    }
};

// 48792 - Icebound Fortitude
/// 6.x
class spell_dk_icebound_fortitude : public SpellScriptLoader
{
public:
    spell_dk_icebound_fortitude() : SpellScriptLoader("spell_dk_icebound_fortitude") { }

    class spell_dk_icebound_fortitude_AuraScript : public AuraScript
    {

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (GetUnitOwner()->HasAura(SPELL_DK_IMPROVED_BLOOD_PRESENCE))
                amount += 30; /// todo, figure out how tooltip is updated
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_icebound_fortitude_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dk_icebound_fortitude_AuraScript();
    }
};

// Outbreak - 77575
class spell_dk_outbreak : public SpellScript
{

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (!target->HasAura(SPELL_DK_OUTBREAK_PERIODIC, GetCaster()->GetGUID()))
                GetCaster()->CastSpell(target, SPELL_DK_OUTBREAK_PERIODIC, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_outbreak::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Outbreak - 196782
class aura_dk_outbreak_periodic : public AuraScript
{

    void HandleDummyTick(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Unit*> friendlyUnits;
            GetTarget()->GetFriendlyUnitListInRange(friendlyUnits, 10.f);

            for (Unit* unit : friendlyUnits)
                if (!unit->HasUnitFlag(UNIT_FLAG_IMMUNE_TO_PC) && unit->IsInCombatWith(caster))
                    caster->CastSpell(unit, SPELL_DK_VIRULENT_PLAGUE, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_dk_outbreak_periodic::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 115994 - Unholy Blight, triggered by 115989
class spell_dk_unholy_blight : public AuraScript
{

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_unholy_blight::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 55090 - Scourge Strike
class spell_dk_scourge_strike : public SpellScript
{

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetHitUnit())
        {
            if (Aura* festeringWoundAura = target->GetAura(SPELL_DK_FESTERING_WOUND, GetCaster()->GetGUID()))
            {
                if (caster->HasAura(SPELL_DK_UNHOLY_FRENZY))
                    caster->CastSpell(caster, SPELL_DK_UNHOLY_FRENZY_BUFF, true);

                if (Aura* pestilentPustulesAura = caster->GetAura(SPELL_DK_PESTILENT_PUSTULES))
                    if (festeringWoundAura->GetStackAmount() >= pestilentPustulesAura->HasEffect(EFFECT_0))
                        caster->ModifyPower(POWER_RUNES, 1);

                uint8 festeringWoundBurst = 1;
                if (Aura* castiragorAura = caster->GetAura(SPELL_DK_CASTIGATOR))
                    festeringWoundBurst += castiragorAura->HasEffect(EFFECT_1);

                festeringWoundBurst = std::min(festeringWoundBurst, festeringWoundAura->GetStackAmount());

                for (uint8 i = 0; i < festeringWoundBurst; ++i)
                {
                    caster->CastSpell(target, SPELL_DK_FESTERING_WOUND_DAMAGE, true);
                    festeringWoundAura->ModStackAmount(-1);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_scourge_strike::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 152280 - Defile
class aura_dk_defile : public AuraScript
{

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
        {
            for (AreaTrigger* at : caster->GetAreaTriggers(GetId()))
            {
                if (at->GetInsideUnits().size())
                    caster->CastSpell(caster, SPELL_DK_DEFILE_MASTERY, true);
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_dk_defile::HandlePeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Spell 152280
// At 6212
struct at_dk_defile : AreaTriggerAI
{
    at_dk_defile(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnCreate(Spell const* /*creatingSpell*/) override
    {
        at->GetCaster()->CastSpell(at->GetPosition(), SPELL_DK_SUMMON_DEFILE, true);
    }

    void OnUnitEnter(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
            caster->CastSpell(unit, SPELL_DK_DEFILE_DUMMY, true);
    }

    void OnUnitExit(Unit* unit) override
    {
        unit->RemoveAurasDueToSpell(SPELL_DK_DEFILE_DUMMY);
    }
};

//156004
class spell_dk_defile_aura : public AuraScript
{

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        Unit* caster = GetCaster();
        if (!target || !caster)
            return;

        scheduler.Schedule(1s, [caster, target](TaskContext context)
        {
            if (!target || !caster)
                return;

            caster->CastSpell(target, SPELL_DK_DEFILE_DAMAGE, true);
            if (target->HasAura(156004) && caster)
                context.Repeat(1s);
            else
                context.CancelAll();
        });
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dk_defile_aura::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
private:
    TaskScheduler scheduler;
};

//82521
struct npc_dk_defile : public ScriptedAI
{
    npc_dk_defile(Creature* creature) : ScriptedAI(creature) {
        SetCombatMovement(false);
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UnitFlags(UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_NON_ATTACKABLE));
        me->AddUnitState(UNIT_STATE_ROOT);
    }

    void Reset() override
    {
        me->DespawnOrUnsummon(11000ms);
    }
};

// Dark transformation - transform pet spell - 63560
class spell_dk_dark_transformation_form : public SpellScriptLoader
{
public:
    spell_dk_dark_transformation_form() : SpellScriptLoader("spell_dk_dark_transformation_form") { }

    class spell_dk_dark_transformation_form_SpellScript : public SpellScript
    {

        void HandleOnHit()
        {
            if (Player* _player = GetCaster()->ToPlayer())
            {
                if (Unit* pet = GetHitUnit())
                {
                    if (pet->HasAura(SPELL_DK_DARK_INFUSION_STACKS))
                    {
                        _player->RemoveAura(SPELL_DK_DARK_INFUSION_STACKS);
                        pet->RemoveAura(SPELL_DK_DARK_INFUSION_STACKS);
                    }
                }
            }
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_dk_dark_transformation_form_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dk_dark_transformation_form_SpellScript();
    }
};

// 207317 - Epidemic
class spell_dk_epidemic : public SpellScript
{

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            if (Aura* aura = target->GetAura(SPELL_DK_VIRULENT_PLAGUE, GetCaster()->GetGUID()))
            {
                target->RemoveAura(aura);
                GetCaster()->CastSpell(target, SPELL_DK_EPIDEMIC_DAMAGE_SINGLE, true);
                GetCaster()->CastSpell(target, SPELL_DK_EPIDEMIC_DAMAGE_AOE, true);
            }
        }

        PreventHitDamage();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_epidemic::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 215969 - Epidemic AOE
class spell_dk_epidemic_aoe : public SpellScript
{

    void HandleOnHitMain(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            explicitTarget = target->GetGUID();
    }

    void HandleOnHitAOE(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (target->GetGUID() == explicitTarget)
                PreventHitDamage();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_epidemic_aoe::HandleOnHitMain, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_dk_epidemic_aoe::HandleOnHitAOE, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
private:
    ObjectGuid explicitTarget;
};

// 195182 - Marrowrend
class spell_dk_marrowrend : public SpellScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DK_BONE_SHIELD });
    }

    void HandleHitTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (Aura* aura = caster->GetAura(SPELL_DK_BONE_SHIELD))
            aura->ModStackAmount(GetEffectValue());
        else
            caster->CastSpell(caster, SPELL_DK_BONE_SHIELD, CastSpellExtraArgs()
                .AddSpellMod(SPELLVALUE_AURA_STACK, GetEffectValue()));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dk_marrowrend::HandleHitTarget, EFFECT_2, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_deathknight_spell_scripts()
{
    RegisterSpellScript(spell_dk_advantage_t10_4p);
    RegisterSpellScript(spell_dk_anti_magic_shell);
    RegisterSpellScriptWithArgs(spell_dk_apply_bone_shield, "spell_dk_marrowrend_apply_bone_shield", EFFECT_2);
    RegisterSpellScriptWithArgs(spell_dk_apply_bone_shield, "spell_dk_deaths_caress_apply_bone_shield", EFFECT_2);
    RegisterSpellScript(spell_dk_army_transform);
    RegisterSpellScript(spell_dk_blinding_sleet);
    RegisterSpellScript(spell_dk_blooddrinker);
    RegisterSpellScript(spell_dk_blood_boil);
    RegisterSpellScript(spell_dk_brittle);
    RegisterSpellScript(spell_dk_dancing_rune_weapon);
    RegisterSpellScript(spell_dk_dark_simulacrum);
    RegisterSpellScript(spell_dk_dark_simulacrum_buff);
    RegisterSpellScript(spell_dk_death_and_decay);
    RegisterSpellScript(spell_dk_death_and_decay_increase_targets);
    RegisterSpellScript(spell_dk_death_coil);
    RegisterSpellScript(spell_dk_death_gate);
    RegisterSpellScript(spell_dk_death_grip_initial);
    RegisterSpellScript(spell_dk_death_pact);
    RegisterSpellScript(spell_dk_death_strike);
    RegisterSpellScript(spell_dk_death_strike_enabler);
    RegisterSpellScript(spell_dk_festering_strike);
    RegisterSpellScript(spell_dk_frost_fever_proc);
    RegisterSpellScript(spell_dk_ghoul_explode);
    RegisterSpellScript(spell_dk_glyph_of_scourge_strike_script);
    RegisterSpellScript(spell_dk_heartbreaker);
    RegisterSpellScript(spell_dk_howling_blast);
    RegisterSpellScript(spell_dk_ice_prison);
    RegisterSpellScript(spell_dk_icy_talons);
    RegisterSpellScript(spell_dk_icy_talons_buff);
    RegisterSpellScript(spell_dk_improved_death_strike);
    RegisterSpellScript(spell_dk_mark_of_blood);
    RegisterSpellScript(spell_dk_necrosis);
    RegisterSpellScript(spell_dk_obliteration);
    RegisterSpellScript(spell_dk_permafrost);
    RegisterSpellScript(spell_dk_pet_geist_transform);
    RegisterSpellScript(spell_dk_pet_skeleton_transform);
    RegisterSpellScript(spell_dk_pvp_4p_bonus);
    RegisterSpellScript(spell_dk_raise_dead);
    RegisterSpellScript(spell_dk_reaper_of_souls);
    RegisterSpellScript(spell_dk_rime);
    RegisterSpellScriptWithArgs(spell_dk_soul_reaper, "spell_dk_soul_reaper", EFFECT_1, EFFECT_2);
    RegisterSpellScriptWithArgs(spell_dk_soul_reaper, "spell_dk_soul_reaper_reaper_of_souls", EFFECT_0, Optional<SpellEffIndex>());
    RegisterSpellScript(spell_dk_subduing_grasp);
    RegisterSpellScript(spell_dk_t20_2p_rune_empowered);
    RegisterSpellScript(spell_dk_vampiric_blood);
    RegisterSpellScript(spell_dk_voracious);
    RegisterSpellScript(spell_dk_marrowrend);

    RegisterAreaTriggerAI(at_dk_death_and_decay);

    //new
    new spell_dk_icebound_fortitude();
    RegisterSpellScript(aura_dk_outbreak_periodic);
    RegisterSpellScript(spell_dk_unholy_blight);
    RegisterSpellScript(spell_dk_scourge_strike);
    RegisterSpellScript(aura_dk_defile);
    RegisterAreaTriggerAI(at_dk_defile);
    RegisterCreatureAI(npc_dk_defile);
    RegisterSpellScript(spell_dk_defile_aura);
    RegisterSpellScript(spell_dk_epidemic);
    RegisterSpellScript(spell_dk_epidemic_aoe);
}

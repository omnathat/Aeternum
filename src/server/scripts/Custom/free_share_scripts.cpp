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

#include "Chat.h"
#include "ChatCommandTags.h"
#include "ChatCommand.h"
#include "DB2Stores.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "MiscPackets.h"
#include "MovementGenerator.h"
#include "ScriptMgr.h"
#include "WorldSession.h"
#include "RBAC.h"
#include "MotionMaster.h"
#include "Map.h"
#include "World.h"
#include "WorldPacket.h"
#include "GameTime.h"
#include "RoleplayDatabase.h"

class StaticTimeManager
{
private:
    static inline uint32 m_staticHour = 12;
    static inline uint32 m_staticMinute = 0;
    static inline uint32 m_staticYear = 1900;
    static inline uint32 m_staticMonth = 1;
    static inline uint32 m_staticMonthDay = 1;
    static inline bool m_timeFreezed = false;


public:

    static void SaveStaticTimeToDB()
    {
        using namespace std::string_view_literals;

        RoleplayDatabasePreparedStatement* stmt = RoleplayDatabase.GetPreparedStatement(Roleplay_DEL_SERVER_SETTINGS);
        RoleplayDatabase.Execute(stmt);

        stmt = RoleplayDatabase.GetPreparedStatement(Roleplay_REP_SERVER_SETTINGS);
        stmt->setString(0, "static_hour"sv);
        stmt->setString(1, std::to_string(m_staticHour));
        RoleplayDatabase.Execute(stmt);

        stmt = RoleplayDatabase.GetPreparedStatement(Roleplay_REP_SERVER_SETTINGS);
        stmt->setString(0, "static_minute"sv);
        stmt->setString(1, std::to_string(m_staticMinute));
        RoleplayDatabase.Execute(stmt);

        stmt = RoleplayDatabase.GetPreparedStatement(Roleplay_REP_SERVER_SETTINGS);
        stmt->setString(0, "time_freezed"sv);
        stmt->setString(1, std::to_string(m_timeFreezed ? 1 : 0));
        RoleplayDatabase.Execute(stmt);
    }

    static void LoadStaticTimeFromDB()
    {
        RoleplayDatabasePreparedStatement* stmt = RoleplayDatabase.GetPreparedStatement(Roleplay_SEL_SERVER_SETTINGS);
        PreparedQueryResult result = RoleplayDatabase.Query(stmt);

        if (!result)
        {
            ResetToServerTime();
            return;
        }

        bool hourSet = false, minuteSet = false, freezeSet = false;

        do
        {
            Field* fields = result->Fetch();
            std::string settingName = fields[0].GetString();
            std::string settingValue = fields[1].GetString();

            if (settingName == "static_hour")
            {
                m_staticHour = std::stoi(settingValue);
                hourSet = true;
            }
            else if (settingName == "static_minute")
            {
                m_staticMinute = std::stoi(settingValue);
                minuteSet = true;
            }
            else if (settingName == "time_freezed")
            {
                m_timeFreezed = (settingValue == "1");
                freezeSet = true;
            }

        } while (result->NextRow());

        if (!hourSet || !minuteSet || !freezeSet)
        {
            ResetToServerTime();
        }

        SendTimeSync();
    }

    static bool IsTimeFreezed()
    {
        return m_timeFreezed;
    }

    static void SetStaticTime(uint32 hour, uint32 minute, bool freeze = false)
    {
        m_staticHour = hour;
        m_staticMinute = minute;
        m_timeFreezed = freeze;

        SaveStaticTimeToDB();
        SendTimeSync();
    }

    static void ResetToServerTime()
    {
        time_t now = time(nullptr);
        struct tm* localTime = localtime(&now);

        m_staticHour = localTime->tm_hour;
        m_staticMinute = localTime->tm_min;
        m_timeFreezed = false;

        RoleplayDatabasePreparedStatement* stmt = RoleplayDatabase.GetPreparedStatement(Roleplay_DEL_SERVER_SETTINGS);
        RoleplayDatabase.Execute(stmt);

        SendTimeSync();
    }

    static void SendTimeSync()
    {
        WowTime custom;
        WorldPackets::Misc::LoginSetTimeSpeed timePacket;

        time_t nowYM = time(nullptr);
        struct tm* localTimeYM = localtime(&nowYM);

        m_staticYear = (localTimeYM->tm_year + 1900) % 100;
        m_staticMonth = localTimeYM->tm_mon;
        m_staticMonthDay = localTimeYM->tm_mday;

        custom.SetHour(m_staticHour);
        custom.SetMinute(m_staticMinute);
        custom.SetYear(m_staticYear);
        custom.SetMonth(m_staticMonth);
        custom.SetMonthDay(m_staticMonthDay);
        timePacket.GameTime = custom;
        timePacket.ServerTime = custom;
        static float const TimeSpeed = 0.01666667f;
        timePacket.NewSpeed = TimeSpeed;

        sWorld->SendGlobalMessage(timePacket.Write());
    }
};

class free_share_scripts : public CommandScript
{
public:
    free_share_scripts() : CommandScript("free_share_scripts") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> commandTable =
        {
            { "barbershop",      rbac::RBAC_PERM_COMMAND_BARBER,        false,      &HandleBarberCommand,       ""},
            { "castgroup",       rbac::RBAC_PERM_COMMAND_CAST_GROUP,    false,      &HandleCastGroupCommand,    ""},
            { "castgroupscene",  rbac::RBAC_PERM_COMMAND_CAST_SCENE,    false,      &HandleCastSceneCommand,    ""},
            { "npcmoveto",       rbac::RBAC_PERM_COMMAND_NPC_MOVE,      false,      &HandleNpcMoveTo,           ""},
            { "npcguidsay",      rbac::RBAC_PERM_COMMAND_NPC_SAY,       false,      &HandleNpcGuidSay,          ""},
            { "npcguidyell",     rbac::RBAC_PERM_COMMAND_NPC_YELL,      false,      &HandleNpcGuidYell,         ""},
            { "settime",         rbac::RBAC_PERM_COMMAND_NPC_YELL,      false,      &HandleSetTimeCommand,      ""},
        };

        return commandTable;
    }

    // custom command .barber
    static bool HandleBarberCommand(ChatHandler* handler)
    {
        WorldPackets::Misc::EnableBarberShop packet;
        handler->GetSession()->GetPlayer()->SendDirectMessage(WorldPackets::Misc::EnableBarberShop().Write());

        return true;
    }

    // custom command .castgroup
    static bool HandleCastGroupCommand(ChatHandler* handler, SpellInfo const* spellInfo)
    {
        uint32 spellId = spellInfo->Id;

        if (!spellId)
            return false;

        if (!handler->GetSession()->GetPlayer()->GetGroup())
            return false;

        for (GroupReference* itr = handler->GetSession()->GetPlayer()->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* plr = itr->GetSource();
            if (!plr || !plr->GetSession())
                continue;

            plr->CastSpell(plr, spellId, false);
        }

        return true;
    }

    // custom command .castscene
    static bool HandleCastSceneCommand(ChatHandler* handler, char const* args)
    {

        if (!*args)
            return false;

        char const* scenePackageIdStr = strtok((char*)args, " ");
        char const* flagsStr = strtok(NULL, "");

        if (!scenePackageIdStr)
            return false;

        uint32 scenePackageId = atoi(scenePackageIdStr);
        uint32 flags = flagsStr ? atoi(flagsStr) : 0;

        if (!handler->GetSession()->GetPlayer()->GetGroup())
            return false;

        for (GroupReference* itr = handler->GetSession()->GetPlayer()->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* plr = itr->GetSource();
            if (!plr || !plr->GetSession())
                continue;

            if (!sSceneScriptPackageStore.HasRecord(scenePackageId))
                return false;

            plr->GetSceneMgr().PlaySceneByPackageId(scenePackageId, SceneFlag(flags));
        }

        return true;
    }

    // custom command .npcmoveto
    static bool HandleNpcMoveTo(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* guid = strtok((char*)args, " ");

        if (!guid)
            return false;

        char* x = strtok(nullptr, " ");
        char* y = strtok(nullptr, " ");
        char* z = strtok(nullptr, " ");

        if (!x || !y || !z)
            return false;

        float x2, y2, z2;
        x2 = (float)atof(x);
        y2 = (float)atof(y);
        z2 = (float)atof(z);

        Creature* creature = nullptr;

        char* cId = handler->extractKeyFromLink((char*)guid, "Hcreature");

        ObjectGuid::LowType lowguid = Trinity::StringTo<ObjectGuid::LowType>(cId).value_or(UI64LIT(0));

        creature = handler->GetCreatureFromPlayerMapByDbGuid(lowguid);

        if (!creature)
        {
            return false;
        }

        uint32 MapIdPlayer = player->GetMapId();

        uint32 MapIdCreature = creature->GetMapId();

        if (MapIdPlayer != MapIdCreature)
        {
            return false;
        }
        else
        {
            creature->GetMotionMaster()->MovePoint(0, x2, y2, z2);
        }

        return true;
    }

    // custom command .npcguidsay
    static bool HandleNpcGuidSay(ChatHandler* handler, char const* args)
    {

        char* guid = strtok((char*)args, " ");

        if (!guid)
            return false;

        char* say = strtok(nullptr, "");

        if (!say)
            return false;

        Creature* creature = nullptr;

        char* cId = handler->extractKeyFromLink((char*)guid, "Hcreature");

        ObjectGuid::LowType lowguid = Trinity::StringTo<ObjectGuid::LowType>(cId).value_or(UI64LIT(0));

        creature = handler->GetCreatureFromPlayerMapByDbGuid(lowguid);

        if (!creature)
        {
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        uint32 MapIdPlayer = player->GetMapId();
        uint32 MapIdCreature = creature->GetMapId();

        if (MapIdPlayer != MapIdCreature)
        {
            return false;
        }
        else
        {
            creature->Say(say, LANG_UNIVERSAL);

            char lastchar = args[strlen(args) - 1];
            switch (lastchar)
            {
            case '?':   creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);      break;
            case '!':   creature->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);   break;
            default:    creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);          break;
            }
        }

        return true;
    }

    // custom command .npcguidyell
    static bool HandleNpcGuidYell(ChatHandler* handler, char const* args)
    {
        char* guid = strtok((char*)args, " ");

        if (!guid)
            return false;

        char* yell = strtok(nullptr, "");

        if (!yell)
            return false;

        Creature* creature = nullptr;

        char* cId = handler->extractKeyFromLink((char*)guid, "Hcreature");

        ObjectGuid::LowType lowguid = Trinity::StringTo<ObjectGuid::LowType>(cId).value_or(UI64LIT(0));

        creature = handler->GetCreatureFromPlayerMapByDbGuid(lowguid);

        if (!creature)
        {
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        uint32 MapIdPlayer = player->GetMapId();
        uint32 MapIdCreature = creature->GetMapId();

        if (MapIdPlayer != MapIdCreature)
        {
            return false;
        }
        else
        {
            creature->Yell(yell, LANG_UNIVERSAL);

            creature->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);
        }

        return true;
    }

    // custom command .settime
    static bool HandleSetTimeCommand(ChatHandler* handler, Optional<uint32> hour, Optional<uint32> minute)
    {
        if (hour && *hour == 999)
        {
            StaticTimeManager::ResetToServerTime();
            handler->PSendSysMessage("Time reset");
            return true;
        }

        uint32 setHour = hour && *hour >= 0 ? *hour : 15;
        uint32 setMinute = minute && *minute >= 0 ? *minute : 30;

        if (setHour > 23 || setMinute > 59)
        {
            handler->SendSysMessage("Incorrect time. Use hours 0-23, minutes 0-59.");
            return false;
        }

        StaticTimeManager::SetStaticTime(setHour, setMinute, true);

        handler->PSendSysMessage("Server time is set to %02u:%02u (frozen)", setHour, setMinute);
        return true;
    }
};

class PlayerScript_TimeSync : public PlayerScript
{
public:
    PlayerScript_TimeSync() : PlayerScript("PlayerScript_TimeSync") {}

    void OnLogin(Player* /*player*/, bool /*firstLogin*/) override
    {
        StaticTimeManager::LoadStaticTimeFromDB();
    }
};

class WorldScript_TimeSync : public WorldScript
{
public:
    WorldScript_TimeSync() : WorldScript("WorldScript_TimeSync") {}

    void OnUpdate(uint32 diff) override
    {
        static uint32 timeCheckTimer = 0;
        timeCheckTimer += diff;

        if (timeCheckTimer >= 30 * IN_MILLISECONDS)
        {
            if (StaticTimeManager::IsTimeFreezed())
            {
                StaticTimeManager::SendTimeSync();
            }

            timeCheckTimer = 0;
        }
    }
};

void AddSC_free_share_scripts()
{
    new free_share_scripts();
    new PlayerScript_TimeSync();
    new WorldScript_TimeSync();
}


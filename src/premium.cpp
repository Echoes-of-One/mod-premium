#ifndef ResetTalents
#define ResetTalents ResetTalents_NOT_AVAILABLE_IN_THIS_CORE
#endif

#include "Config.h"
#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "Chat.h"
#include "PlayerSettings.h"
#include "InstanceSaveMgr.h"

// Individual Progression (phases/expansions)
#include "IndividualProgression.h"

// Echo Coins storage is account-based (acore_auth.account.donationpoints)
#include "DatabaseEnv.h"
#include "LoginDatabase.h"

enum Vendors
{
    NPC_VENDOR_A    = 54,
    NPC_VENDOR_H    = 3163,
    NPC_AUCTION_H   = 9856,
    NPC_AUCTION_A   = 8670
};

enum Trainers
{
    // Alliance
    DRUID_A     = 5504,
    HUNTER_A    = 5515,
    MAGE_A      = 5497,
    PALADIN_A   = 928,
    PRIEST_A    = 376,
    ROGUE_A     = 918,
    SHAMAN_A    = 20407,
    WARLOCK_A   = 461,
    WARRIOR_A   = 5479,

    // Horde
    DRUID_H     = 3033,
    HUNTER_H    = 3406,
    MAGE_H      = 5883,
    PALADIN_H   = 23128,
    PRIEST_H    = 3045,
    ROGUE_H     = 3401,
    SHAMAN_H    = 3344,
    WARLOCK_H   = 3324,
    WARRIOR_H   = 3354,

    DEATHKNIGHT_AH  = 28472
};

enum Mounts
{
    HUMAN_MOUNT     = 470,
    ORC_MOUNT       = 6653,
    GNOME_MOUNT     = 17454,
    NIGHTELF_MOUNT  = 8394,
    DWARF_MOUNT     = 6899,
    UNEAD_MOUNT     = 17463,
    TAUREN_MOUNT    = 64657,
    TROLL_MOUNT     = 8395,
    BLOODELF_MOUNT  = 35022,
    DRAENEI_MOUNT   = 34406
};

enum PremiumGossip
{
    PREMIUM_MENU_TEXT = 90003,
};

enum PremiumPage : uint32
{
    PAGE_MAIN = 1,
    PAGE_APPEARANCE,
    PAGE_SERVICES,
    PAGE_TRAVEL,
    PAGE_CHARACTER,
    PAGE_MOUNT,
    PAGE_PROGRESSION
};

enum PremiumAction : uint32
{
    ACTION_OPEN_APPEARANCE = GOSSIP_ACTION_INFO_DEF + 10,
    ACTION_OPEN_SERVICES = GOSSIP_ACTION_INFO_DEF + 11,
    ACTION_OPEN_TRAVEL = GOSSIP_ACTION_INFO_DEF + 12,
    ACTION_OPEN_CHARACTER = GOSSIP_ACTION_INFO_DEF + 13,
    ACTION_OPEN_MOUNT = GOSSIP_ACTION_INFO_DEF + 14,

    ACTION_OPEN_PROGRESSION = GOSSIP_ACTION_INFO_DEF + 15,

    ACTION_BACK_TO_MAIN = GOSSIP_ACTION_INFO_DEF + 20,
    ACTION_CLOSE = GOSSIP_ACTION_INFO_DEF + 21,

    // Appearance
    ACTION_MORPH = GOSSIP_ACTION_INFO_DEF + 30,
    ACTION_DEMORPH = GOSSIP_ACTION_INFO_DEF + 31,
    ACTION_MOUNT = GOSSIP_ACTION_INFO_DEF + 32,

    // Character
    ACTION_RESET_ALL_INSTANCES = GOSSIP_ACTION_INFO_DEF + 33,
    ACTION_BUY_EC_1 = GOSSIP_ACTION_INFO_DEF + 34,

    // Individual Progression
    ACTION_IP_SKIP_PHASE = GOSSIP_ACTION_INFO_DEF + 35,
    ACTION_IP_SKIP_EXPANSION = GOSSIP_ACTION_INFO_DEF + 36,

    // Services
    ACTION_VENDOR = GOSSIP_ACTION_INFO_DEF + 40,
    ACTION_MAILBOX = GOSSIP_ACTION_INFO_DEF + 41,
    ACTION_BANK = GOSSIP_ACTION_INFO_DEF + 42,
    ACTION_AUCTION = GOSSIP_ACTION_INFO_DEF + 43,
    ACTION_TRAINER = GOSSIP_ACTION_INFO_DEF + 44,

    // Travel (teleports)
    ACTION_TP_STORMWIND = GOSSIP_ACTION_INFO_DEF + 60,
    ACTION_TP_IRONFORGE = GOSSIP_ACTION_INFO_DEF + 61,
    ACTION_TP_DARNASSUS = GOSSIP_ACTION_INFO_DEF + 62,
    ACTION_TP_EXODAR = GOSSIP_ACTION_INFO_DEF + 63,

    ACTION_TP_ORGRIMMAR = GOSSIP_ACTION_INFO_DEF + 70,
    ACTION_TP_UNDERCITY = GOSSIP_ACTION_INFO_DEF + 71,
    ACTION_TP_THUNDERBLUFF = GOSSIP_ACTION_INFO_DEF + 72,
    ACTION_TP_SILVERMOON = GOSSIP_ACTION_INFO_DEF + 73,
};

struct TpLocation
{
    uint32 map;
    float x;
    float y;
    float z;
    float o;
};

namespace
{
    static std::string const PremiumPSSource = "mod-premium";

    enum PremiumSettingIndexes : uint8
    {
        SETTING_ACTIVITY_POINTS = 0,
        SETTING_ACTIVITY_SECONDS = 1,
        SETTING_ACTIVITY_MILLISECONDS = 2,
        MAX_PREMIUM_SETTINGS = 3
    };

    uint32 GetAP(Player* player);
    void SetAP(Player* player, uint32 ap);
    uint32 GetAccumSeconds(Player* player);
    void SetAccumSeconds(Player* player, uint32 secs);
    uint32 GetAccumMilliseconds(Player* player);
    void SetAccumMilliseconds(Player* player, uint32 ms);

    uint64 GetExchangeRateGoldPerEC()
    {
        return sConfigMgr->GetOption<uint32>("Premium.EC.GoldPerEC", 5000000); // 500g in copper
    }

    std::string FormatMoneyCopper(uint64 copper)
    {
        uint64 gold = copper / GOLD;
        uint64 silver = (copper % GOLD) / SILVER;
        uint64 copperRest = copper % SILVER;
        return std::to_string(gold) + "g " + std::to_string(silver) + "s " + std::to_string(copperRest) + "c";
    }

    bool TryBuyECWithGold(Player* player, uint32 ecAmount)
    {
        if (!player || ecAmount == 0)
            return false;

        if (!sConfigMgr->GetOption<bool>("Premium.EC.Exchange.Enabled", true))
            return false;

        uint64 cost = GetExchangeRateGoldPerEC() * uint64(ecAmount);
        if (cost == 0)
            return false;

        if (player->GetMoney() < cost)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Not enough gold. Need {} for {} EC.", FormatMoneyCopper(cost), ecAmount);
            return false;
        }

        player->ModifyMoney(-int64(cost));
        uint32 ec = GetAP(player);
        SetAP(player, ec + ecAmount);
        ChatHandler(player->GetSession()).PSendSysMessage("You purchased {} EC for {}. Total: {} EC.", ecAmount, FormatMoneyCopper(cost), ec + ecAmount);
        return true;
    }

    uint32 GetAP(Player* player)
    {
        if (!player || !player->GetSession())
            return 0;

        if (!sConfigMgr->GetOption<bool>("Premium.EC.UseAuthDonationPoints", true))
            return player->GetPlayerSetting(PremiumPSSource, SETTING_ACTIVITY_POINTS).value;

        QueryResult res = LoginDatabase.Query("SELECT donationpoints FROM account WHERE id = {}", player->GetSession()->GetAccountId());
        if (!res)
            return 0;

        Field* f = res->Fetch();
        return f[0].Get<uint32>();
    }

    void SetAP(Player* player, uint32 ap)
    {
        if (!player || !player->GetSession())
            return;

        if (!sConfigMgr->GetOption<bool>("Premium.EC.UseAuthDonationPoints", true))
        {
            player->UpdatePlayerSetting(PremiumPSSource, SETTING_ACTIVITY_POINTS, ap);
            return;
        }

        LoginDatabase.Execute("UPDATE account SET donationpoints = {} WHERE id = {}", ap, player->GetSession()->GetAccountId());
    }

    uint32 GetAccumSeconds(Player* player)
    {
        return player->GetPlayerSetting(PremiumPSSource, SETTING_ACTIVITY_SECONDS).value;
    }

    void SetAccumSeconds(Player* player, uint32 secs)
    {
        player->UpdatePlayerSetting(PremiumPSSource, SETTING_ACTIVITY_SECONDS, secs);
    }

    uint32 GetAccumMilliseconds(Player* player)
    {
        return player->GetPlayerSetting(PremiumPSSource, SETTING_ACTIVITY_MILLISECONDS).value;
    }

    void SetAccumMilliseconds(Player* player, uint32 ms)
    {
        player->UpdatePlayerSetting(PremiumPSSource, SETTING_ACTIVITY_MILLISECONDS, ms);
    }

    bool IsPremium(Player* player)
    {
        if (!player)
            return false;

        if (!sConfigMgr->GetOption<bool>("PremiumAccount", true))
            return false;

        QueryResult result = CharacterDatabase.Query(
            "SELECT `AccountId` FROM `premium` WHERE `active`=1 AND `AccountId`={}",
            player->GetSession()->GetAccountId());

        return result != nullptr;
    }

    uint32 GetActionCostAP(uint32 action)
    {
        // default costs (can be overridden by config via per-feature toggles below)
        switch (action)
        {
            case ACTION_MORPH: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Morph", 5);
            case ACTION_DEMORPH: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Demorph", 0);
            case ACTION_MOUNT: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Mount", 2);
            case ACTION_RESET_ALL_INSTANCES: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.ResetAllInstances", 10);

            case ACTION_IP_SKIP_PHASE: return 5;
            case ACTION_IP_SKIP_EXPANSION: return 15;

            case ACTION_VENDOR: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Vendor", 3);
            case ACTION_AUCTION: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Auction", 3);
            case ACTION_TRAINER: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Trainer", 3);
            case ACTION_MAILBOX: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Mailbox", 1);
            case ACTION_BANK: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Bank", 1);
            case ACTION_TP_STORMWIND:
            case ACTION_TP_IRONFORGE:
            case ACTION_TP_DARNASSUS:
            case ACTION_TP_EXODAR:
            case ACTION_TP_ORGRIMMAR:
            case ACTION_TP_UNDERCITY:
            case ACTION_TP_THUNDERBLUFF:
            case ACTION_TP_SILVERMOON:
                return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Teleport", 2);
            default:
                return 0;
        }
    }

    static char const* GetProgressionName(uint8 state)
    {
        switch (state)
        {
            case PROGRESSION_START: return "T0-1: Molten Core & Onyxia";
            case PROGRESSION_MOLTEN_CORE: return "T2: Blackwing Lair";
            case PROGRESSION_ONYXIA: return "T2: Blackwing Lair";
            case PROGRESSION_BLACKWING_LAIR: return "T3: Pre-AQ";
            case PROGRESSION_PRE_AQ: return "T4: AQ War";
            case PROGRESSION_AQ_WAR: return "T5: Ahn'Qiraj";
            case PROGRESSION_AQ: return "T6: Naxxramas";
            case PROGRESSION_NAXX40: return "T7: Pre-TBC";
            case PROGRESSION_PRE_TBC: return "T8: Karazhan, Gruul & Magtheridon";
            case PROGRESSION_TBC_TIER_1: return "T9: SSC & Tempest Keep";
            case PROGRESSION_TBC_TIER_2: return "T10: Hyjal & Black Temple";
            case PROGRESSION_TBC_TIER_3: return "T11: Zul'Aman";
            case PROGRESSION_TBC_TIER_4: return "T12: Sunwell Plateau";
            case PROGRESSION_TBC_TIER_5: return "T13: Naxx, EoE & OS";
            case PROGRESSION_WOTLK_TIER_1: return "T14: Ulduar";
            case PROGRESSION_WOTLK_TIER_2: return "T15: Trial of the Crusader";
            case PROGRESSION_WOTLK_TIER_3: return "T16: Icecrown Citadel";
            case PROGRESSION_WOTLK_TIER_4: return "T17: Ruby Sanctum";
            case PROGRESSION_WOTLK_TIER_5: return "T17: Ruby Sanctum";
            default: return "Unknown";
        }
    }

    static uint8 GetIPStateSafe(Player* player)
    {
        if (!player)
            return 0;

        return player->GetPlayerSetting("mod-individual-progression", SETTING_PROGRESSION_STATE).value;
    }

    static std::string GetSkipPhaseLabel(Player* player)
    {
        uint8 current = GetIPStateSafe(player);
        uint8 next = current < PROGRESSION_WOTLK_TIER_5 ? uint8(current + 1) : current;

        if (current >= PROGRESSION_WOTLK_TIER_5)
            return "Skip phase (already at maximum)";

        return "Skip " + std::string(GetProgressionName(current)) + ", unlock " + std::string(GetProgressionName(next));
    }

    static std::string GetSkipExpansionLabel(Player* player)
    {
        uint8 current = GetIPStateSafe(player);

        if (current < PROGRESSION_PRE_TBC)
            return "Skip Vanilla content, unlock TBC";

        if (current < PROGRESSION_TBC_TIER_5)
            return "Skip TBC content, unlock WotLK";

        return "";
    }

    bool TrySpendAPForAction(Player* player, uint32 action)
    {
        if (!player)
            return false;

        // Progression skips always cost EC, even for Premium accounts.
        if (IsPremium(player) && action != ACTION_IP_SKIP_PHASE && action != ACTION_IP_SKIP_EXPANSION)
            return true;

        if (!sConfigMgr->GetOption<bool>("Premium.AP.Enabled", true))
            return false;

        uint32 cost = GetActionCostAP(action);
        if (cost == 0)
            return true;

        uint32 ap = GetAP(player);
        if (ap < cost)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Not enough Echo Coins (EC). You have {} EC, need {} EC.", ap, cost);
            return false;
        }

        SetAP(player, ap - cost);
        ChatHandler(player->GetSession()).PSendSysMessage("Spent {} EC. Remaining: {} EC.", cost, ap - cost);
        return true;
    }

    bool TrySpendAPForActionAny(Player* player, uint32 action)
    {
        if (!player)
            return false;

        if (!sConfigMgr->GetOption<bool>("Premium.AP.Enabled", true))
            return false;

        uint32 cost = GetActionCostAP(action);
        if (cost == 0)
            return true;

        uint32 ap = GetAP(player);
        if (ap < cost)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Not enough Echo Coins (EC). You have {} EC, need {} EC.", ap, cost);
            return false;
        }

        SetAP(player, ap - cost);
        ChatHandler(player->GetSession()).PSendSysMessage("Spent {} EC. Remaining: {} EC.", cost, ap - cost);
        return true;
    }

    std::string LabelWithCost(Player* player, std::string const& label, uint32 action)
    {
        if (IsPremium(player))
            return label + " (Premium)";

        if (!sConfigMgr->GetOption<bool>("Premium.AP.Enabled", true))
            return label;

        uint32 cost = GetActionCostAP(action);
        if (cost == 0)
            return label;

        return label + " [" + std::to_string(cost) + " EC]";
    }

    std::string LabelWithCostAny(Player* player, std::string const& label, uint32 action)
    {
        if (!sConfigMgr->GetOption<bool>("Premium.AP.Enabled", true))
            return label;

        uint32 cost = GetActionCostAP(action);
        if (cost == 0)
            return label;

        return label + " [" + std::to_string(cost) + " EC]";
    }

    uint8 GetIPState(Player* player)
    {
        if (!player)
            return 0;

        return player->GetPlayerSetting("mod-individual-progression", SETTING_PROGRESSION_STATE).value;
    }

    bool TrySkipIPPhase(Player* player)
    {
        if (!player || !player->IsInWorld())
            return false;

        if (!sIndividualProgression->enabled)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Individual Progression is disabled.");
            return false;
        }

        if (sIndividualProgression->isExcludedFromProgression(player))
        {
            ChatHandler(player->GetSession()).SendSysMessage("This account is excluded from Individual Progression.");
            return false;
        }

        uint8 currentState = GetIPState(player);
        if (currentState >= PROGRESSION_WOTLK_TIER_5)
        {
            ChatHandler(player->GetSession()).SendSysMessage("You are already at the maximum progression phase.");
            return false;
        }

        uint8 nextState = currentState + 1;
        if (sIndividualProgression->progressionLimit && nextState > sIndividualProgression->progressionLimit)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Cannot skip: progression limit reached.");
            return false;
        }

        uint32 currentArea = player->GetAreaId();
        sIndividualProgression->ForceUpdateProgressionState(player, static_cast<ProgressionState>(nextState));
        sIndividualProgression->UpdateProgressionQuests(player);
        sIndividualProgression->checkIPPhasing(player, currentArea);
        sIndividualProgression->CheckAdjustments(player);

        ChatHandler(player->GetSession()).PSendSysMessage("Progression advanced by 1 phase. New phase: {}.", nextState);
        return true;
    }

    bool TrySkipIPExpansion(Player* player)
    {
        if (!player || !player->IsInWorld())
            return false;

        if (!sIndividualProgression->enabled)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Individual Progression is disabled.");
            return false;
        }

        if (sIndividualProgression->isExcludedFromProgression(player))
        {
            ChatHandler(player->GetSession()).SendSysMessage("This account is excluded from Individual Progression.");
            return false;
        }

        uint8 currentState = GetIPState(player);
        uint8 targetState = currentState;

        // Vanilla -> start of TBC progression bucket
        if (currentState < PROGRESSION_PRE_TBC)
            targetState = PROGRESSION_PRE_TBC;
        // TBC -> start of WotLK progression bucket
        else if (currentState < PROGRESSION_TBC_TIER_5)
            targetState = PROGRESSION_TBC_TIER_5;
        else
        {
            ChatHandler(player->GetSession()).SendSysMessage("You are already in the last expansion bucket.");
            return false;
        }

        if (targetState == currentState)
        {
            ChatHandler(player->GetSession()).SendSysMessage("No expansion skip available from your current phase.");
            return false;
        }

        if (sIndividualProgression->progressionLimit && targetState > sIndividualProgression->progressionLimit)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Cannot skip: progression limit reached.");
            return false;
        }

        uint32 currentArea = player->GetAreaId();
        sIndividualProgression->ForceUpdateProgressionState(player, static_cast<ProgressionState>(targetState));
        sIndividualProgression->UpdateProgressionQuests(player);
        sIndividualProgression->checkIPPhasing(player, currentArea);
        sIndividualProgression->CheckAdjustments(player);

        ChatHandler(player->GetSession()).PSendSysMessage("Progression advanced to expansion milestone. New phase: {}.", targetState);
        return true;
    }
}

static bool PremiumCanUse(Player* player)
{
    if (!player)
        return false;

    if (player->IsInCombat())
        return false;

    if (player->IsInFlight())
        return false;

    return true;
}

static bool TeleportPlayer(Player* player, TpLocation const& loc)
{
    if (!player)
        return false;

    if (!PremiumCanUse(player))
        return false;

    CloseGossipMenuFor(player);
    return player->TeleportTo(loc.map, loc.x, loc.y, loc.z, loc.o);
}

static void AddNavBack(Player* player)
{
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, ACTION_BACK_TO_MAIN);
}

static void AddNavClose(Player* player)
{
    AddGossipItemFor(player, GOSSIP_ICON_TALK, "Close", GOSSIP_SENDER_MAIN, ACTION_CLOSE);
}

static void ShowMenu(Player* player, ObjectGuid ownerGuid, uint32 page)
{
    ClearGossipMenuFor(player);

    // Header (always visible)
    AddGossipItemFor(player, GOSSIP_ICON_CHAT,
        "Echo Coins (EC): " + std::to_string(GetAP(player)) + (IsPremium(player) ? " | Premium" : ""),
        GOSSIP_SENDER_MAIN, ACTION_BACK_TO_MAIN);

    switch (page)
    {
        default:
        case PAGE_MAIN:
        {
            if (sConfigMgr->GetOption<bool>("Morph", true))
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Appearance", GOSSIP_SENDER_MAIN, ACTION_OPEN_APPEARANCE);

            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Services", GOSSIP_SENDER_MAIN, ACTION_OPEN_SERVICES);
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Travel", GOSSIP_SENDER_MAIN, ACTION_OPEN_TRAVEL);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Character", GOSSIP_SENDER_MAIN, ACTION_OPEN_CHARACTER);

            if (sIndividualProgression->enabled && !sIndividualProgression->isExcludedFromProgression(player))
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Progression", GOSSIP_SENDER_MAIN, ACTION_OPEN_PROGRESSION);

            if (sConfigMgr->GetOption<bool>("Mount", true))
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Mount", GOSSIP_SENDER_MAIN, ACTION_OPEN_MOUNT);

            AddNavClose(player);
            break;
        }

        case PAGE_APPEARANCE:
        {
            if (sConfigMgr->GetOption<bool>("Morph", true))
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCost(player, "Random morph", ACTION_MORPH), GOSSIP_SENDER_MAIN, ACTION_MORPH);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCost(player, "Demorph", ACTION_DEMORPH), GOSSIP_SENDER_MAIN, ACTION_DEMORPH);
            }

            AddNavBack(player);
            break;
        }

        case PAGE_SERVICES:
        {
            if (sConfigMgr->GetOption<bool>("Trainers", true))
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, LabelWithCost(player, "Class trainer", ACTION_TRAINER), GOSSIP_SENDER_MAIN, ACTION_TRAINER);

            if (sConfigMgr->GetOption<bool>("Vendor", true))
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, LabelWithCost(player, "Vendor", ACTION_VENDOR), GOSSIP_SENDER_MAIN, ACTION_VENDOR);

            if (sConfigMgr->GetOption<bool>("MailBox", true))
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCost(player, "Mailbox", ACTION_MAILBOX), GOSSIP_SENDER_MAIN, ACTION_MAILBOX);

            if (sConfigMgr->GetOption<bool>("Bank", true))
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCost(player, "Bank", ACTION_BANK), GOSSIP_SENDER_MAIN, ACTION_BANK);

            if (sConfigMgr->GetOption<bool>("Auction", true))
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCost(player, "Auction House", ACTION_AUCTION), GOSSIP_SENDER_MAIN, ACTION_AUCTION);

            AddNavBack(player);
            break;
        }

        case PAGE_CHARACTER:
        {
            if (sConfigMgr->GetOption<bool>("Premium.EC.Exchange.Enabled", true))
            {
                uint64 rate = GetExchangeRateGoldPerEC();
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG,
                    "Buy 1 EC (" + FormatMoneyCopper(rate) + ")",
                    GOSSIP_SENDER_MAIN, ACTION_BUY_EC_1);
            }

            if (sConfigMgr->GetOption<bool>("ResetInstances", true))
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCostAny(player, "Reset all dungeons/raids", ACTION_RESET_ALL_INSTANCES), GOSSIP_SENDER_MAIN, ACTION_RESET_ALL_INSTANCES);

            AddNavBack(player);
            break;
        }

        case PAGE_PROGRESSION:
        {
            if (!sIndividualProgression->enabled || sIndividualProgression->isExcludedFromProgression(player))
            {
                AddNavBack(player);
                break;
            }

            AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                LabelWithCostAny(player, GetSkipPhaseLabel(player), ACTION_IP_SKIP_PHASE),
                GOSSIP_SENDER_MAIN, ACTION_IP_SKIP_PHASE);

            {
                std::string expLabel = GetSkipExpansionLabel(player);
                if (!expLabel.empty())
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                        LabelWithCostAny(player, expLabel, ACTION_IP_SKIP_EXPANSION),
                        GOSSIP_SENDER_MAIN, ACTION_IP_SKIP_EXPANSION);
                }
            }

            AddNavBack(player);
            break;
        }

        case PAGE_TRAVEL:
        {
            if (player->GetTeamId() == TEAM_ALLIANCE)
            {
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "Stormwind", ACTION_TP_STORMWIND), GOSSIP_SENDER_MAIN, ACTION_TP_STORMWIND);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "Ironforge", ACTION_TP_IRONFORGE), GOSSIP_SENDER_MAIN, ACTION_TP_IRONFORGE);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "Darnassus", ACTION_TP_DARNASSUS), GOSSIP_SENDER_MAIN, ACTION_TP_DARNASSUS);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "The Exodar", ACTION_TP_EXODAR), GOSSIP_SENDER_MAIN, ACTION_TP_EXODAR);
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "Orgrimmar", ACTION_TP_ORGRIMMAR), GOSSIP_SENDER_MAIN, ACTION_TP_ORGRIMMAR);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "Undercity", ACTION_TP_UNDERCITY), GOSSIP_SENDER_MAIN, ACTION_TP_UNDERCITY);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "Thunder Bluff", ACTION_TP_THUNDERBLUFF), GOSSIP_SENDER_MAIN, ACTION_TP_THUNDERBLUFF);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "Silvermoon", ACTION_TP_SILVERMOON), GOSSIP_SENDER_MAIN, ACTION_TP_SILVERMOON);
            }

            AddNavBack(player);
            break;
        }

        case PAGE_MOUNT:
        {
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, LabelWithCost(player, "Summon racial mount", ACTION_MOUNT), GOSSIP_SENDER_MAIN, ACTION_MOUNT);
            AddNavBack(player);
            break;
        }
    }

    SendGossipMenuFor(player, PREMIUM_MENU_TEXT, ownerGuid);
}

class premium_account : public ItemScript
{
public:
    premium_account() : ItemScript("premium_account") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (!player || !item)
            return false;

        // Everyone can open the menu
        if (!PremiumCanUse(player))
            return false;

        // Keep proximity restrictions from original
        float rangeCheck = 10.0f;
        if (player->FindNearestCreature(NPC_AUCTION_A, rangeCheck) ||
            player->FindNearestCreature(NPC_AUCTION_H, rangeCheck) ||
            player->FindNearestCreature(NPC_VENDOR_A, rangeCheck) ||
            player->FindNearestCreature(NPC_VENDOR_H, rangeCheck) ||
            player->FindNearestCreature(ROGUE_A, rangeCheck) ||
            player->FindNearestCreature(WARRIOR_A, rangeCheck) ||
            player->FindNearestCreature(HUNTER_A, rangeCheck) ||
            player->FindNearestCreature(PRIEST_A, rangeCheck) ||
            player->FindNearestCreature(PALADIN_A, rangeCheck) ||
            player->FindNearestCreature(DRUID_A, rangeCheck) ||
            player->FindNearestCreature(SHAMAN_A, rangeCheck) ||
            player->FindNearestCreature(MAGE_A, rangeCheck) ||
            player->FindNearestCreature(WARLOCK_A, rangeCheck) ||
            player->FindNearestCreature(HUNTER_H, rangeCheck) ||
            player->FindNearestCreature(WARRIOR_H, rangeCheck) ||
            player->FindNearestCreature(SHAMAN_H, rangeCheck) ||
            player->FindNearestCreature(PALADIN_H, rangeCheck) ||
            player->FindNearestCreature(ROGUE_H, rangeCheck) ||
            player->FindNearestCreature(WARLOCK_H, rangeCheck) ||
            player->FindNearestCreature(MAGE_H, rangeCheck) ||
            player->FindNearestCreature(PRIEST_H, rangeCheck) ||
            player->FindNearestCreature(DRUID_H, rangeCheck) ||
            player->FindNearestCreature(DEATHKNIGHT_AH, rangeCheck))
            return false;

        ShowMenu(player, item->GetGUID(), PAGE_MAIN);
        return true;
    }

    void OnGossipSelect(Player* player, Item* item, uint32 /*sender*/, uint32 action) override
    {
        if (!player || !item)
            return;

        if (!PremiumCanUse(player))
        {
            CloseGossipMenuFor(player);
            return;
        }

        // Gold -> EC exchange
        if (action == ACTION_BUY_EC_1)
        {
            CloseGossipMenuFor(player);
            TryBuyECWithGold(player, 1);
            return;
        }

        // Navigation actions are always free
        switch (action)
        {
            case ACTION_OPEN_APPEARANCE:
                ShowMenu(player, item->GetGUID(), PAGE_APPEARANCE);
                return;
            case ACTION_OPEN_SERVICES:
                ShowMenu(player, item->GetGUID(), PAGE_SERVICES);
                return;
            case ACTION_OPEN_TRAVEL:
                ShowMenu(player, item->GetGUID(), PAGE_TRAVEL);
                return;
            case ACTION_OPEN_CHARACTER:
                ShowMenu(player, item->GetGUID(), PAGE_CHARACTER);
                return;
            case ACTION_OPEN_MOUNT:
                ShowMenu(player, item->GetGUID(), PAGE_MOUNT);
                return;
            case ACTION_OPEN_PROGRESSION:
                ShowMenu(player, item->GetGUID(), PAGE_PROGRESSION);
                return;
            case ACTION_BACK_TO_MAIN:
                ShowMenu(player, item->GetGUID(), PAGE_MAIN);
                return;
            case ACTION_CLOSE:
                CloseGossipMenuFor(player);
                return;
        }

        // Enforce per-option enable/disable via config (menu also hides them, but double-check here)
        if ((action == ACTION_MORPH || action == ACTION_DEMORPH) && !sConfigMgr->GetOption<bool>("Morph", true))
            return;
        if (action == ACTION_MOUNT && !sConfigMgr->GetOption<bool>("Mount", true))
            return;
        if (action == ACTION_TRAINER && !sConfigMgr->GetOption<bool>("Trainers", true))
            return;
        if (action == ACTION_VENDOR && !sConfigMgr->GetOption<bool>("Vendor", true))
            return;
        if (action == ACTION_MAILBOX && !sConfigMgr->GetOption<bool>("MailBox", true))
            return;
        if (action == ACTION_BANK && !sConfigMgr->GetOption<bool>("Bank", true))
            return;
        if (action == ACTION_AUCTION && !sConfigMgr->GetOption<bool>("Auction", true))
            return;
        if (action == ACTION_RESET_ALL_INSTANCES && !sConfigMgr->GetOption<bool>("ResetInstances", true))
            return;

        // Charge AP (if non-premium)
        if (action == ACTION_RESET_ALL_INSTANCES)
        {
            if (!TrySpendAPForActionAny(player, action))
            {
                ShowMenu(player, item->GetGUID(), PAGE_MAIN);
                return;
            }
        }
        else if (!TrySpendAPForAction(player, action))
        {
            ShowMenu(player, item->GetGUID(), PAGE_MAIN);
            return;
        }

        switch (action)
        {
            case ACTION_IP_SKIP_PHASE:
            {
                CloseGossipMenuFor(player);
                TrySkipIPPhase(player);
                return;
            }
            case ACTION_IP_SKIP_EXPANSION:
            {
                CloseGossipMenuFor(player);
                TrySkipIPExpansion(player);
                return;
            }

            case ACTION_MORPH:
                CloseGossipMenuFor(player);
                ApplyRandomMorph(player);
                return;
            case ACTION_DEMORPH:
                player->DeMorph();
                CloseGossipMenuFor(player);
                return;

            case ACTION_MOUNT:
            {
                CloseGossipMenuFor(player);
                switch (player->getRace())
                {
                    case RACE_HUMAN:         player->CastSpell(player, HUMAN_MOUNT); break;
                    case RACE_ORC:           player->CastSpell(player, ORC_MOUNT); break;
                    case RACE_GNOME:         player->CastSpell(player, GNOME_MOUNT); break;
                    case RACE_NIGHTELF:      player->CastSpell(player, NIGHTELF_MOUNT); break;
                    case RACE_DWARF:         player->CastSpell(player, DWARF_MOUNT); break;
                    case RACE_DRAENEI:       player->CastSpell(player, DRAENEI_MOUNT); break;
                    case RACE_UNDEAD_PLAYER: player->CastSpell(player, UNEAD_MOUNT); break;
                    case RACE_TAUREN:        player->CastSpell(player, TAUREN_MOUNT); break;
                    case RACE_TROLL:         player->CastSpell(player, TROLL_MOUNT); break;
                    case RACE_BLOODELF:      player->CastSpell(player, BLOODELF_MOUNT); break;
                    default: break;
                }
                return;
            }

            case ACTION_BANK:
                player->GetSession()->SendShowBank(player->GetGUID());
                return;
            case ACTION_MAILBOX:
                player->GetSession()->SendShowMailBox(player->GetGUID());
                return;

            case ACTION_VENDOR:
            {
                uint32 vendorId = player->GetTeamId() == TEAM_ALLIANCE ? NPC_VENDOR_A : NPC_VENDOR_H;
                std::string salute = player->GetTeamId() == TEAM_ALLIANCE ? "Greetings" : "Zug zug";
                SummonTempNPC(player, vendorId, salute.c_str());
                CloseGossipMenuFor(player);
                return;
            }

            case ACTION_AUCTION:
            {
                uint32 auctionId = player->GetTeamId() == TEAM_ALLIANCE ? NPC_AUCTION_A : NPC_AUCTION_H;
                SummonTempNPC(player, auctionId);
                CloseGossipMenuFor(player);
                return;
            }

            case ACTION_TRAINER:
            {
                uint32 trainerId = 0;
                switch (player->getClass())
                {
                    case CLASS_ROGUE:        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? ROGUE_A : ROGUE_H; break;
                    case CLASS_WARRIOR:      trainerId = player->GetTeamId() == TEAM_ALLIANCE ? WARRIOR_A : WARRIOR_H; break;
                    case CLASS_PRIEST:       trainerId = player->GetTeamId() == TEAM_ALLIANCE ? PRIEST_A : PRIEST_H; break;
                    case CLASS_MAGE:         trainerId = player->GetTeamId() == TEAM_ALLIANCE ? MAGE_A : MAGE_H; break;
                    case CLASS_PALADIN:      trainerId = player->GetTeamId() == TEAM_ALLIANCE ? PALADIN_A : PALADIN_H; break;
                    case CLASS_HUNTER:       trainerId = player->GetTeamId() == TEAM_ALLIANCE ? HUNTER_A : HUNTER_H; break;
                    case CLASS_DRUID:        trainerId = player->GetTeamId() == TEAM_ALLIANCE ? DRUID_A : DRUID_H; break;
                    case CLASS_SHAMAN:       trainerId = player->GetTeamId() == TEAM_ALLIANCE ? SHAMAN_A : SHAMAN_H; break;
                    case CLASS_WARLOCK:      trainerId = player->GetTeamId() == TEAM_ALLIANCE ? WARLOCK_A : WARLOCK_H; break;
                    case CLASS_DEATH_KNIGHT: trainerId = DEATHKNIGHT_AH; break;
                    default: break;
                }

                SummonTempNPC(player, trainerId);
                CloseGossipMenuFor(player);
                return;
            }

            case ACTION_TP_STORMWIND:
                TeleportPlayer(player, TpLocation{0, -8842.09f, 626.358f, 94.0867f, 0.0f});
                return;
            case ACTION_TP_IRONFORGE:
                TeleportPlayer(player, TpLocation{0, -4981.25f, -881.542f, 501.66f, 0.0f});
                return;
            case ACTION_TP_DARNASSUS:
                TeleportPlayer(player, TpLocation{1, 9951.52f, 2280.32f, 1341.39f, 0.0f});
                return;
            case ACTION_TP_EXODAR:
                TeleportPlayer(player, TpLocation{530, -3864.92f, -11643.7f, -137.644f, 0.0f});
                return;

            case ACTION_TP_ORGRIMMAR:
                TeleportPlayer(player, TpLocation{1, 1502.71f, -4415.42f, 21.56f, 0.0f});
                return;
            case ACTION_TP_UNDERCITY:
                TeleportPlayer(player, TpLocation{0, 1831.26f, 238.53f, 60.5f, 0.0f});
                return;
            case ACTION_TP_THUNDERBLUFF:
                TeleportPlayer(player, TpLocation{1, -1277.37f, 124.804f, 131.287f, 0.0f});
                return;
            case ACTION_TP_SILVERMOON:
                TeleportPlayer(player, TpLocation{530, 9473.03f, -7279.67f, 14.2285f, 0.0f});
                return;

            case ACTION_RESET_ALL_INSTANCES:
            {
                CloseGossipMenuFor(player);

                // Dungeons (normal instances)
                Player::ResetInstances(player->GetGUID(), INSTANCE_RESET_ALL, false);
                // Raids (current raid difficulty)
                Player::ResetInstances(player->GetGUID(), INSTANCE_RESET_CHANGE_DIFFICULTY, true);

                ChatHandler(player->GetSession()).SendSysMessage("All dungeons/raids have been reset (where allowed).");
                return;
            }

            default:
                // Unknown action -> return to main menu
                ShowMenu(player, item->GetGUID(), PAGE_MAIN);
                return;
        }
    }

    void ApplyRandomMorph(Player* player)
    {
        uint32 random = (urand(1, 26)); // Change this line when adding more morphs
        {
            switch (random)
            {
                case 1: player->SetDisplayId(10134); break;     // Troll Female                 'Orb of Deception'
                case 2: player->SetDisplayId(10135); break;     // Troll Male                   'Orb of Deception'
                case 3: player->SetDisplayId(10136); break;     // Tauren Male                  'Orb of Deception'
                case 4: player->SetDisplayId(10137); break;     // Human Male                   'Orb of Deception'
                case 5: player->SetDisplayId(10138); break;     // Human Female                 'Orb of Deception'
                case 6: player->SetDisplayId(10139); break;     // Orc Male                     'Orb of Deception'
                case 7: player->SetDisplayId(10140); break;     // Orc Female                   'Orb of Deception'
                case 8: player->SetDisplayId(10141); break;     // Dwarf Male                   'Orb of Deception'
                case 9: player->SetDisplayId(10142); break;     // Dwarf Female                 'Orb of Deception'
                case 10: player->SetDisplayId(10143); break;    // NightElf Male                'Orb of Deception'
                case 11: player->SetDisplayId(10144); break;    // NightElf Female              'Orb of Deception'
                case 12: player->SetDisplayId(10145); break;    // Undead Female                'Orb of Deception'
                case 13: player->SetDisplayId(10146); break;    // Undead Male                  'Orb of Deception'
                case 14: player->SetDisplayId(10147); break;    // Tauren Female                'Orb of Deception'
                case 15: player->SetDisplayId(10148); break;    // Gnome Male                   'Orb of Deception'
                case 16: player->SetDisplayId(10149); break;    // Gnome Female                 'Orb of Deception'
                case 17: player->SetDisplayId(4527); break;     // Thrall                       'Orgrimmar Boss'
                case 18: player->SetDisplayId(11657); break;    // Lady Sylvanas                'Undercity Boss'
                case 19: player->SetDisplayId(4307); break;     // Cairne Bloodhoof             'Thunderbluff Boss'
                case 20: player->SetDisplayId(17122); break;    // Lor'themar Theron            'Silvermoon City Boss'
                case 21: player->SetDisplayId(3597); break;     // King Magni Bronzebeard       'Ironforge Boss'
                case 22: player->SetDisplayId(5566); break;     // Highlord Bolvar Fordragon    'Stormwind Boss'
                case 23: player->SetDisplayId(7006); break;     // High Tinker Mekkatorque      'Gnomer Boss'
                case 24: player->SetDisplayId(7274); break;     // Tyrande Whisperwind          'Darnassus Boss'
                case 25: player->SetDisplayId(21976); break;    // Arthus Small                 'Arthus'
                case 26: player->SetDisplayId(24641); break;    // Arthus Ghost                 'Arthus Ghost'

                default:
                    break;
            }
        }
    }

    void SummonTempNPC(Player* player, uint32 entry, const char* salute = "")
    {
        if (!player || entry == 0)
            return;

        int npcDuration = sConfigMgr->GetOption<int32>("Premium.NpcDuration", 60) * IN_MILLISECONDS;
        if (npcDuration <= 0) // Safeguard
            npcDuration = 60;

        // Summon a personal service NPC for this player.
        Creature* npc = player->SummonCreature(entry, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0,
            TEMPSUMMON_TIMED_DESPAWN, npcDuration);
        if (!npc)
            return;
        npc->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        npc->SetReactState(REACT_PASSIVE);
        npc->SetFaction(player->GetFaction());

        // Attach the summon to the player so it behaves like a personal helper.
        npc->SetOwnerGUID(player->GetGUID());
        npc->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, player->GetFollowAngle());

        if (salute && !(salute[0] == '\0'))
            npc->Whisper(salute, LANG_UNIVERSAL, player, false);
    }
};

class premium_ap_tracker : public PlayerScript
{
public:
    premium_ap_tracker() : PlayerScript("premium_ap_tracker", { PLAYERHOOK_ON_UPDATE }) { }

    void OnPlayerUpdate(Player* player, uint32 diff) override
    {
        if (!player || !player->IsInWorld())
            return;

        if (!sConfigMgr->GetOption<bool>("Premium.AP.Enabled", true))
            return;

        // award only while not AFK and not in combat (configurable)
        if (sConfigMgr->GetOption<bool>("Premium.AP.RequireNotAFK", true) && player->isAFK())
            return;

        if (sConfigMgr->GetOption<bool>("Premium.AP.RequireNotInCombat", false) && player->IsInCombat())
            return;

        uint32 storedSecs = GetAccumSeconds(player);
        uint32 storedMs = GetAccumMilliseconds(player);
        uint64 totalMs = uint64(storedSecs) * IN_MILLISECONDS + storedMs + diff;

        uint32 secondsPerPoint = sConfigMgr->GetOption<uint32>("Premium.AP.SecondsPerPoint", 3600);
        uint32 pointsPerAward = sConfigMgr->GetOption<uint32>("Premium.AP.PointsPerAward", 1);

        if (secondsPerPoint == 0)
            secondsPerPoint = 3600;

        uint64 secondsTotal = totalMs / IN_MILLISECONDS;
        if (secondsTotal >= secondsPerPoint)
        {
            uint32 awards = uint32(secondsTotal / secondsPerPoint);
            uint64 secondsRemainder = secondsTotal % secondsPerPoint;
            uint64 msRemainder = totalMs % IN_MILLISECONDS;
            totalMs = secondsRemainder * IN_MILLISECONDS + msRemainder;

            uint32 add = awards * pointsPerAward;
            uint32 ap = GetAP(player);
            SetAP(player, ap + add);

            if (sConfigMgr->GetOption<bool>("Premium.AP.Notify", true))
                ChatHandler(player->GetSession()).PSendSysMessage("You earned {} Echo Coins (EC). Total: {} EC.", add, ap + add);
        }

        SetAccumSeconds(player, uint32(totalMs / IN_MILLISECONDS));
        SetAccumMilliseconds(player, uint32(totalMs % IN_MILLISECONDS));
    }
};

void AddPremiumAccount()
{
    new premium_account();
    new premium_ap_tracker();
}

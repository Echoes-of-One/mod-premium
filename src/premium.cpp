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
    PAGE_PROGRESSION,
    PAGE_PROFESSIONS,
    PAGE_BUFFS,
    PAGE_RATES
};

enum PremiumAction : uint32
{
    ACTION_OPEN_APPEARANCE = GOSSIP_ACTION_INFO_DEF + 10,
    ACTION_OPEN_SERVICES = GOSSIP_ACTION_INFO_DEF + 11,
    ACTION_OPEN_TRAVEL = GOSSIP_ACTION_INFO_DEF + 12,
    ACTION_OPEN_CHARACTER = GOSSIP_ACTION_INFO_DEF + 13,
    ACTION_OPEN_MOUNT = GOSSIP_ACTION_INFO_DEF + 14,

    ACTION_OPEN_PROGRESSION = GOSSIP_ACTION_INFO_DEF + 15,
    ACTION_OPEN_PROFESSIONS = GOSSIP_ACTION_INFO_DEF + 16,
    ACTION_OPEN_BUFFS = GOSSIP_ACTION_INFO_DEF + 17,

    //Rates
    ACTION_OPEN_RATES = GOSSIP_ACTION_INFO_DEF + 23,
    ACTION_SET_XP_RATE_0 = GOSSIP_ACTION_INFO_DEF + 240,
    ACTION_SET_XP_RATE_1,
    ACTION_SET_XP_RATE_5,
    ACTION_SET_XP_RATE_10,
    ACTION_SET_XP_RATE_25,
    ACTION_SET_XP_RATE_50,
    ACTION_SET_XP_RATE_100,

    // Buffs
    ACTION_BUFF_ME_MELEE = GOSSIP_ACTION_INFO_DEF + 18,
    ACTION_BUFF_ME_CASTER = GOSSIP_ACTION_INFO_DEF + 19,
    ACTION_BUFF_ME_TANK = GOSSIP_ACTION_INFO_DEF + 22,

    ACTION_BACK_TO_MAIN = GOSSIP_ACTION_INFO_DEF + 20,
    ACTION_CLOSE = GOSSIP_ACTION_INFO_DEF + 21,

    // Appearance
    ACTION_MORPH = GOSSIP_ACTION_INFO_DEF + 30,
    ACTION_DEMORPH = GOSSIP_ACTION_INFO_DEF + 31,
    ACTION_MOUNT = GOSSIP_ACTION_INFO_DEF + 32,

    // Character
    ACTION_RESET_ALL_INSTANCES = GOSSIP_ACTION_INFO_DEF + 33,
    ACTION_BUY_EC_1 = GOSSIP_ACTION_INFO_DEF + 34,
    ACTION_FREE_REPAIR = GOSSIP_ACTION_INFO_DEF + 35,

    ACTION_CHARACTER_CUSTOMIZE = GOSSIP_ACTION_INFO_DEF + 100,
    ACTION_CHARACTER_CHANGE_RACE = GOSSIP_ACTION_INFO_DEF + 101,
    ACTION_CHARACTER_CHANGE_FACTION = GOSSIP_ACTION_INFO_DEF + 102,

    // Individual Progression
    ACTION_IP_SKIP_PHASE = GOSSIP_ACTION_INFO_DEF + 36,
    ACTION_IP_SKIP_EXPANSION = GOSSIP_ACTION_INFO_DEF + 37,

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

    ACTION_TP_SHATTRATH = GOSSIP_ACTION_INFO_DEF + 74,
    ACTION_TP_DALARAN = GOSSIP_ACTION_INFO_DEF + 75,

    // Professions (buy max skill)
    ACTION_PROFESSION_BUY_ALCHEMY = GOSSIP_ACTION_INFO_DEF + 200,
    ACTION_PROFESSION_BUY_BLACKSMITHING,
    ACTION_PROFESSION_BUY_ENCHANTING,
    ACTION_PROFESSION_BUY_ENGINEERING,
    ACTION_PROFESSION_BUY_HERBALISM,
    ACTION_PROFESSION_BUY_INSCRIPTION,
    ACTION_PROFESSION_BUY_JEWELCRAFTING,
    ACTION_PROFESSION_BUY_LEATHERWORKING,
    ACTION_PROFESSION_BUY_MINING,
    ACTION_PROFESSION_BUY_SKINNING,
    ACTION_PROFESSION_BUY_TAILORING,

    // Professions (confirmation)
    ACTION_PROFESSION_CONFIRM = GOSSIP_ACTION_INFO_DEF + 230,
    ACTION_PROFESSION_CANCEL,
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

        // Persisted personal progression rates (per-character)
        SETTING_XP_RATE_PERMILLE = 3,          // 0..1000 (1000 = 100.0%)
        SETTING_PROF_SKILL_RATE_PERMILLE = 4,  // 0..1000

        MAX_PREMIUM_SETTINGS = 5
    };

    // ... keep PremiumPSSource + PremiumSettingIndexes as-is ...

    // Stored value meaning:
    // 0  = x0.0
    // 1  = x0.1
    // 5  = x0.5
    // 10 = x1.0
    // 20 = x2.0
    // 60 = x6.0 (matches your server default rates)
    static uint32 GetXpRateMultiplierTenths(Player* player)
    {
        if (!player)
            return 10; // x1.0 default

        uint32 v = player->GetPlayerSetting(PremiumPSSource, SETTING_XP_RATE_PERMILLE).value;

        // clamp to something sane; adjust max if you want higher than x10.0
        if (v > 100)
            v = 100; // x10.0

        return v;
    }

    static void SetXpRateMultiplierTenths(Player* player, uint32 tenths)
    {
        if (!player)
            return;

        if (tenths > 100)
            tenths = 100;

        player->UpdatePlayerSetting(PremiumPSSource, SETTING_XP_RATE_PERMILLE, tenths);
    }

    static std::string FormatMultiplierLabel(uint32 tenths)
    {
        // x0, x1, x2 ... and x0.5, x0.1 etc
        uint32 whole = tenths / 10;
        uint32 frac = tenths % 10;

        if (frac == 0)
            return "x" + std::to_string(whole);

        return "x" + std::to_string(whole) + "." + std::to_string(frac);
    }
}

namespace
{
    // Re-open anonymous namespace so the existing helper functions below are in the same scope.
    // This restores compilation without deleting any of your existing code.

    // Compatibility wrapper for older call sites that still use permille.
    // Converts stored tenths multiplier to permille percent (x1.0 => 1000).
    static uint32 GetXpRatePermille(Player* player)
    {
        uint32 tenths = GetXpRateMultiplierTenths(player);
        if (tenths > 100)
            tenths = 100;
        return tenths * 100; // tenths -> permille
    }

    static uint32 GetProfSkillRatePermille(Player* player)
    {
        // Reuse the same stored tenths multiplier for now (follows XP system)
        // tenths -> permille (x1.0 => 1000)
        uint32 tenths = GetXpRateMultiplierTenths(player);
        if (tenths > 100)
            tenths = 100;
        return tenths * 100;
    }

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

            case ACTION_CHARACTER_CUSTOMIZE: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Customize", 5);
            case ACTION_CHARACTER_CHANGE_RACE: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.ChangeRace", 10);
            case ACTION_CHARACTER_CHANGE_FACTION: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.ChangeFaction", 15);

            case ACTION_FREE_REPAIR: return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Repair", 2);

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
        // Character services also always cost EC.
        if (IsPremium(player)
            && action != ACTION_IP_SKIP_PHASE && action != ACTION_IP_SKIP_EXPANSION
            && action != ACTION_CHARACTER_CUSTOMIZE && action != ACTION_CHARACTER_CHANGE_RACE && action != ACTION_CHARACTER_CHANGE_FACTION)
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

    static uint32 GetProfessionCostEC()
    {
        return 20;
    }

    static bool IsPlayerInTbcBucket(Player* player)
    {
        return GetIPStateSafe(player) >= PROGRESSION_PRE_TBC;
    }

    static bool IsPlayerInWotlkBucket(Player* player)
    {
        return GetIPStateSafe(player) >= PROGRESSION_TBC_TIER_5;
    }

    static uint16 GetMaxProfessionSkillForPlayer(Player* player)
    {
        // Vanilla: 300, TBC: 375, WotLK: 450
        if (IsPlayerInWotlkBucket(player))
            return 450;
        if (IsPlayerInTbcBucket(player))
            return 375;
        return 300;
    }

    static uint32 GetProfessionBaseSpell(SkillType skill)
    {
        // Base spells that make a profession usable (add the ability button, etc.)
        switch (skill)
        {
            case SKILL_ALCHEMY: return 2259;
            case SKILL_BLACKSMITHING: return 2018;
            case SKILL_ENCHANTING: return 7411;
            case SKILL_ENGINEERING: return 4036;
            case SKILL_TAILORING: return 3908;
            case SKILL_LEATHERWORKING: return 2108;
            case SKILL_JEWELCRAFTING: return 25229;
            case SKILL_INSCRIPTION: return 45357;
            case SKILL_MINING: return 2575;
            case SKILL_HERBALISM: return 2366;
            case SKILL_SKINNING: return 8613;
            default: return 0;
        }
    }

    static bool IsPrimaryProfession(SkillType skill)
    {
        switch (skill)
        {
            case SKILL_ALCHEMY:
            case SKILL_BLACKSMITHING:
            case SKILL_ENCHANTING:
            case SKILL_ENGINEERING:
            case SKILL_LEATHERWORKING:
            case SKILL_TAILORING:
            case SKILL_JEWELCRAFTING:
            case SKILL_INSCRIPTION:
                return true;
            default:
                return false;
        }
    }

    static uint32 GetMaxPrimaryProfessions()
    {
        // Core config (worldserver.conf): MaxPrimaryTradeSkill
        return sConfigMgr->GetOption<uint32>("MaxPrimaryTradeSkill", 3);
    }

    static uint32 CountPrimaryProfessions(Player* player)
    {
        if (!player)
            return 0;

        uint32 count = 0;
        if (player->HasSkill(SKILL_ALCHEMY)) ++count;
        if (player->HasSkill(SKILL_BLACKSMITHING)) ++count;
        if (player->HasSkill(SKILL_ENCHANTING)) ++count;
        if (player->HasSkill(SKILL_ENGINEERING)) ++count;
        if (player->HasSkill(SKILL_LEATHERWORKING)) ++count;
        if (player->HasSkill(SKILL_TAILORING)) ++count;
        if (player->HasSkill(SKILL_JEWELCRAFTING)) ++count;
        if (player->HasSkill(SKILL_INSCRIPTION)) ++count;
        return count;
    }

    static void EnsureProfessionBase(Player* player, SkillType skill)
    {
        if (!player)
            return;

        if (uint32 spellId = GetProfessionBaseSpell(skill))
            if (!player->HasSpell(spellId))
                player->learnSpell(spellId);
    }

    static bool LearnProfessionAtMax(Player* player, SkillType skill)
    {
        if (!player)
            return false;

        uint16 maxSkill = GetMaxProfessionSkillForPlayer(player);
        if (maxSkill < 1)
            return false;

        EnsureProfessionBase(player, skill);

        player->SetSkill(skill, 1, maxSkill, maxSkill);

        ChatHandler(player->GetSession()).PSendSysMessage("Learned {} at skill {}.", uint32(skill), maxSkill);
        return true;
    }

    static bool HandleBuyProfession(Player* player, SkillType skill)
    {
        if (!player)
            return false;

        if (IsPrimaryProfession(skill))
        {
            uint32 maxPrimary = GetMaxPrimaryProfessions();
            uint32 currentPrimary = CountPrimaryProfessions(player);

            if (!player->HasSkill(skill) && currentPrimary >= maxPrimary)
            {
                ChatHandler(player->GetSession()).PSendSysMessage(
                    "You already have the maximum number of primary professions ({}).", maxPrimary);
                return false;
            }
        }

        if (!sConfigMgr->GetOption<bool>("Premium.AP.Enabled", true))
            return false;

        uint32 cost = GetProfessionCostEC();
        uint32 ap = GetAP(player);
        if (ap < cost)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Not enough Echo Coins (EC). You have {} EC, need {} EC.", ap, cost);
            return false;
        }

        SetAP(player, ap - cost);
        ChatHandler(player->GetSession()).PSendSysMessage("Spent {} EC. Remaining: {} EC.", cost, ap - cost);

        return LearnProfessionAtMax(player, skill);
    }

    static SkillType ProfessionSkillFromAction(uint32 action)
    {
        switch (action)
        {
            case ACTION_PROFESSION_BUY_ALCHEMY: return SKILL_ALCHEMY;
            case ACTION_PROFESSION_BUY_BLACKSMITHING: return SKILL_BLACKSMITHING;
            case ACTION_PROFESSION_BUY_ENCHANTING: return SKILL_ENCHANTING;
            case ACTION_PROFESSION_BUY_ENGINEERING: return SKILL_ENGINEERING;
            case ACTION_PROFESSION_BUY_HERBALISM: return SKILL_HERBALISM;
            case ACTION_PROFESSION_BUY_INSCRIPTION: return SKILL_INSCRIPTION;
            case ACTION_PROFESSION_BUY_JEWELCRAFTING: return SKILL_JEWELCRAFTING;
            case ACTION_PROFESSION_BUY_LEATHERWORKING: return SKILL_LEATHERWORKING;
            case ACTION_PROFESSION_BUY_MINING: return SKILL_MINING;
            case ACTION_PROFESSION_BUY_SKINNING: return SKILL_SKINNING;
            case ACTION_PROFESSION_BUY_TAILORING: return SKILL_TAILORING;
            default: return SKILL_NONE;
        }
    }

    static void ShowProfessionConfirmMenu(Player* player, ObjectGuid ownerGuid, SkillType skill)
    {
        ClearGossipMenuFor(player);

        uint32 cost = GetProfessionCostEC();
        uint32 maxPrimary = GetMaxPrimaryProfessions();

        AddGossipItemFor(player, GOSSIP_ICON_CHAT,
            "Confirm: spend " + std::to_string(cost) + " EC (no refund)",
            GOSSIP_SENDER_MAIN, ACTION_BACK_TO_MAIN);

        if (IsPrimaryProfession(skill))
            AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                "Primary professions limit: " + std::to_string(maxPrimary),
                GOSSIP_SENDER_MAIN, ACTION_BACK_TO_MAIN);

        // Encode the chosen skill in sender so we know what to purchase on confirm.
        uint32 confirmSender = uint32(skill);

        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Confirm purchase", confirmSender, ACTION_PROFESSION_CONFIRM);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Cancel", GOSSIP_SENDER_MAIN, ACTION_PROFESSION_CANCEL);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, ACTION_OPEN_PROFESSIONS);

        SendGossipMenuFor(player, PREMIUM_MENU_TEXT, ownerGuid);
    }

    static std::string ProfessionLabel(std::string const& base, Player* player)
    {
        uint16 maxSkill = GetMaxProfessionSkillForPlayer(player);
        return base + " (" + std::to_string(maxSkill) + ")";
    }

    enum BuffSpells : uint32
    {
        // WotLK raid buffs (common)
        SPELL_ARCANE_INTELLECT = 42995,
        SPELL_POWER_WORD_FORTITUDE = 48161,
        SPELL_MARK_OF_THE_WILD = 48469,
        // Use single-target Blessing of Kings to avoid group/raid application.
        SPELL_BLESSING_OF_KINGS = 20217,

        // Warrior shouts
        SPELL_COMMANDING_SHOUT = 47440,

        // Paladin blessings (WotLK higher ranks)
        SPELL_BLESSING_OF_MIGHT = 48934,
        SPELL_BLESSING_OF_WISDOM = 48938,
        SPELL_BLESSING_OF_SANCTUARY = 25899,

        // Consumption
        SPELL_FISH_FEAST = 57399,
        SPELL_FLASK_ENDLESS_RAGE = 53760,
        SPELL_FLASK_FROST_WYRM = 53755,
        SPELL_FLASK_STONEBLOOD = 53758,

        // Sayge's Dark Fortune of Damage (base spell is random 1-10%).
        // Servers can override this to a specific 10% aura spell if they use a custom spell.
        SPELL_SAYGES_DARK_FORTUNE_OF_DAMAGE_BASE = 23768,

        // Darkmoon Faire (Sayge) buffs
        SPELL_SAYGES_DARK_FORTUNE_OF_AGILITY = 23736,
        SPELL_SAYGES_DARK_FORTUNE_OF_STRENGTH = 23735,
        SPELL_SAYGES_DARK_FORTUNE_OF_INTELLIGENCE = 23766,
        SPELL_SAYGES_DARK_FORTUNE_OF_SPIRIT = 23738,
        SPELL_SAYGES_DARK_FORTUNE_OF_STAMINA = 23737,
        SPELL_SAYGES_DARK_FORTUNE_OF_RESISTANCE = 23769,
        SPELL_SAYGES_DARK_FORTUNE_OF_ARMOR = 23767
    };

    static uint32 GetBuffCostEC(Player* player)
    {
        if (player && IsPremium(player))
            return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Buffs.Premium", 1);

        return sConfigMgr->GetOption<uint32>("Premium.AP.Cost.Buffs", 2);
    }

    static void TryBuff(Player* player, uint32 spellId)
    {
        if (!player || spellId == 0)
            return;

        // Always cast explicitly on self.
        // Use triggered cast to bypass GCD so we can apply multiple buffs in one click.
        player->CastSpell(player, spellId, TriggerCastFlags(TRIGGERED_FULL_MASK));
    }

    static void ApplyCommonBuffs(Player* player)
    {
        // Only one paladin blessing can be active at a time; apply Kings.
        TryBuff(player, SPELL_BLESSING_OF_KINGS);

        // Warrior shout
        TryBuff(player, SPELL_COMMANDING_SHOUT);

        // Other common buffs
        TryBuff(player, SPELL_MARK_OF_THE_WILD);
        TryBuff(player, SPELL_POWER_WORD_FORTITUDE);
        TryBuff(player, SPELL_ARCANE_INTELLECT);

        // Consume buffs
        TryBuff(player, SPELL_FISH_FEAST);


        // Sayge's Dark Fortune of Damage
        // The base spell (23768) is random 1-10% on some cores. To force 10% deterministically,
        // configure Premium.Buffs.SaygeDamageSpellId10 with the spell ID of your 10% variant.
        uint32 saygeSpell = sConfigMgr->GetOption<uint32>("Premium.Buffs.SaygeDamageSpellId10", SPELL_SAYGES_DARK_FORTUNE_OF_DAMAGE_BASE);
        TryBuff(player, saygeSpell);
    }

    static bool TrySpendECForBuff(Player* player)
    {
        if (!player)
            return false;

        if (!sConfigMgr->GetOption<bool>("Premium.AP.Enabled", true))
            return false;

        uint32 cost = GetBuffCostEC(player);
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

    static void BuffMe(Player* player, bool caster)
    {
        if (!player)
            return;

        if (!TrySpendECForBuff(player))
            return;

        ApplyCommonBuffs(player);

        if (caster)
        {
            TryBuff(player, SPELL_SAYGES_DARK_FORTUNE_OF_INTELLIGENCE);
            TryBuff(player, SPELL_SAYGES_DARK_FORTUNE_OF_SPIRIT);
        }
        else
        {
            TryBuff(player, SPELL_SAYGES_DARK_FORTUNE_OF_STRENGTH);
            TryBuff(player, SPELL_SAYGES_DARK_FORTUNE_OF_AGILITY);
        }

        TryBuff(player, caster ? SPELL_FLASK_FROST_WYRM : SPELL_FLASK_ENDLESS_RAGE);

        ChatHandler(player->GetSession()).SendSysMessage("Buffs applied.");
    }

    static void BuffMeTank(Player* player)
    {
        if (!player)
            return;

        if (!TrySpendECForBuff(player))
            return;

        ApplyCommonBuffs(player);

        TryBuff(player, SPELL_SAYGES_DARK_FORTUNE_OF_STAMINA);
        TryBuff(player, SPELL_SAYGES_DARK_FORTUNE_OF_RESISTANCE);
        TryBuff(player, SPELL_SAYGES_DARK_FORTUNE_OF_ARMOR);

        TryBuff(player, SPELL_FLASK_STONEBLOOD);

        ChatHandler(player->GetSession()).SendSysMessage("Buffs applied.");
    }

    static bool TryClearAllInstanceBinds(Player* player)
    {
        if (!player)
            return false;

        // Safety: avoid clearing binds while inside an instance.
        if (Map* map = player->FindMap())
        {
            if (map->IsDungeon())
            {
                ChatHandler(player->GetSession()).SendSysMessage("You cannot clear instance binds while inside an instance/raid.");
                return false;
            }
        }

        ObjectGuid guid = player->GetGUID();

        // If enabled, clear permanent (raid) binds too.
        bool clearPermanent = sConfigMgr->GetOption<bool>("Premium.ResetInstances.ClearPermanent", true);

        uint32 unboundCount = 0;
        uint32 skippedPermanent = 0;

        for (uint8 d = 0; d < MAX_DIFFICULTY; ++d)
        {
            Difficulty diff = Difficulty(d);
            BoundInstancesMap const& binds = sInstanceSaveMgr->PlayerGetBoundInstances(guid, diff);

            // Copy map ids first; unbinding mutates the underlying storage.
            std::vector<uint32> mapIds;
            mapIds.reserve(binds.size());

            for (auto const& kv : binds)
            {
                uint32 mapId = kv.first;
                InstancePlayerBind const& bind = kv.second;

                if (bind.perm && !clearPermanent)
                {
                    ++skippedPermanent;
                    continue;
                }

                mapIds.push_back(mapId);
            }

            for (uint32 mapId : mapIds)
            {
                sInstanceSaveMgr->PlayerUnbindInstance(guid, mapId, diff, true, player);
                ++unboundCount;
            }
        }

        // Refresh client lockout UI.
        player->SendRaidInfo();

        if (skippedPermanent)
            ChatHandler(player->GetSession()).PSendSysMessage("Cleared {} instance binds. Skipped {} permanent lockouts.", unboundCount, skippedPermanent);
        else
            ChatHandler(player->GetSession()).PSendSysMessage("Cleared {} instance binds.", unboundCount);

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

            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Buffs", GOSSIP_SENDER_MAIN, ACTION_OPEN_BUFFS);

            if (sIndividualProgression->enabled && !sIndividualProgression->isExcludedFromProgression(player))
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Progression", GOSSIP_SENDER_MAIN, ACTION_OPEN_PROGRESSION);

            if (sConfigMgr->GetOption<bool>("Mount", true))
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Mount", GOSSIP_SENDER_MAIN, ACTION_OPEN_MOUNT);

            if (sConfigMgr->GetOption<bool>("Premium.Professions.Enabled", true))
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Professions", GOSSIP_SENDER_MAIN, ACTION_OPEN_PROFESSIONS);

            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Rates (XP / Professions)", GOSSIP_SENDER_MAIN, ACTION_OPEN_RATES);

            AddNavClose(player);
            break;
        }

        case PAGE_BUFFS:
        {
            uint32 buffCost = GetBuffCostEC(player);

            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Buffs", GOSSIP_SENDER_MAIN, ACTION_BACK_TO_MAIN);

            AddGossipItemFor(player, GOSSIP_ICON_TRAINER,
                (IsPremium(player) ? ("Buff Me (Melee) [" + std::to_string(buffCost) + " EC]") : ("Buff Me (Melee) [" + std::to_string(buffCost) + " EC]")),
                GOSSIP_SENDER_MAIN, ACTION_BUFF_ME_MELEE);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER,
                (IsPremium(player) ? ("Buff Me (Caster) [" + std::to_string(buffCost) + " EC]") : ("Buff Me (Caster) [" + std::to_string(buffCost) + " EC]")),
                GOSSIP_SENDER_MAIN, ACTION_BUFF_ME_CASTER);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER,
                (IsPremium(player) ? ("Buff Me (Tank) [" + std::to_string(buffCost) + " EC]") : ("Buff Me (Tank) [" + std::to_string(buffCost) + " EC]")),
                GOSSIP_SENDER_MAIN, ACTION_BUFF_ME_TANK);

            AddNavBack(player);
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

            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, LabelWithCost(player, "Repair", ACTION_FREE_REPAIR), GOSSIP_SENDER_MAIN, ACTION_FREE_REPAIR);

            if (sConfigMgr->GetOption<bool>("ResetInstances", true))
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCostAny(player, "Reset all dungeons/raids", ACTION_RESET_ALL_INSTANCES), GOSSIP_SENDER_MAIN, ACTION_RESET_ALL_INSTANCES);

            if (sConfigMgr->GetOption<bool>("Premium.CharacterServices.Enabled", true))
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCostAny(player, "Recustomize", ACTION_CHARACTER_CUSTOMIZE), GOSSIP_SENDER_MAIN, ACTION_CHARACTER_CUSTOMIZE);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCostAny(player, "Change race", ACTION_CHARACTER_CHANGE_RACE), GOSSIP_SENDER_MAIN, ACTION_CHARACTER_CHANGE_RACE);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, LabelWithCostAny(player, "Change faction", ACTION_CHARACTER_CHANGE_FACTION), GOSSIP_SENDER_MAIN, ACTION_CHARACTER_CHANGE_FACTION);
            }

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

            if (IsPlayerInTbcBucket(player))
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "Shattrath", ACTION_TP_SHATTRATH), GOSSIP_SENDER_MAIN, ACTION_TP_SHATTRATH);

            if (IsPlayerInWotlkBucket(player))
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, LabelWithCost(player, "Dalaran", ACTION_TP_DALARAN), GOSSIP_SENDER_MAIN, ACTION_TP_DALARAN);

            AddNavBack(player);
            break;
        }

        case PAGE_MOUNT:
        {
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, LabelWithCost(player, "Summon racial mount", ACTION_MOUNT), GOSSIP_SENDER_MAIN, ACTION_MOUNT);
            AddNavBack(player);
            break;
        }

        case PAGE_RATES:
        {
            uint32 xpMulTenths = GetXpRateMultiplierTenths(player);

            AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                "Rates Changer: " + FormatMultiplierLabel(xpMulTenths),
                GOSSIP_SENDER_MAIN, ACTION_BACK_TO_MAIN);

            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Set Rates to x0 (no XP)", GOSSIP_SENDER_MAIN, ACTION_SET_XP_RATE_0);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Set Rates to x0.1", GOSSIP_SENDER_MAIN, ACTION_SET_XP_RATE_1);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Set Rates to x0.5", GOSSIP_SENDER_MAIN, ACTION_SET_XP_RATE_5);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Set Rates to x1", GOSSIP_SENDER_MAIN, ACTION_SET_XP_RATE_10);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Set Rates to x2.5", GOSSIP_SENDER_MAIN, ACTION_SET_XP_RATE_25);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Set Rates to x5", GOSSIP_SENDER_MAIN, ACTION_SET_XP_RATE_50);

            // With your server set to 6x globally, x6 here means "keep server pace"
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Set Rates to x6 (server default)", GOSSIP_SENDER_MAIN, ACTION_SET_XP_RATE_100);

            AddNavBack(player);
            break;
        }

        case PAGE_PROFESSIONS:
        {
            uint16 maxSkill = GetMaxProfessionSkillForPlayer(player);

            AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                "Buy / Max Profession (no recipes) [" + std::to_string(GetProfessionCostEC()) + " EC]",
                GOSSIP_SENDER_MAIN, ACTION_BACK_TO_MAIN);

            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Alchemy", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_ALCHEMY);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Blacksmithing", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_BLACKSMITHING);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Enchanting", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_ENCHANTING);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Engineering", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_ENGINEERING);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Herbalism", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_HERBALISM);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Leatherworking", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_LEATHERWORKING);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Mining", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_MINING);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Skinning", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_SKINNING);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Tailoring", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_TAILORING);

            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Jewelcrafting", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_JEWELCRAFTING);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, ProfessionLabel("Inscription", player), GOSSIP_SENDER_MAIN, ACTION_PROFESSION_BUY_INSCRIPTION);

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

    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
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
        case ACTION_SET_XP_RATE_0:   SetXpRateMultiplierTenths(player, 0);  ShowMenu(player, item->GetGUID(), PAGE_RATES); return;  // x0.0
        case ACTION_SET_XP_RATE_1:   SetXpRateMultiplierTenths(player, 1);  ShowMenu(player, item->GetGUID(), PAGE_RATES); return;  // x0.1
        case ACTION_SET_XP_RATE_5:   SetXpRateMultiplierTenths(player, 5);  ShowMenu(player, item->GetGUID(), PAGE_RATES); return;  // x0.5
        case ACTION_SET_XP_RATE_10:  SetXpRateMultiplierTenths(player, 10); ShowMenu(player, item->GetGUID(), PAGE_RATES); return;  // x1.0
        case ACTION_SET_XP_RATE_25:  SetXpRateMultiplierTenths(player, 25); ShowMenu(player, item->GetGUID(), PAGE_RATES); return;  // x2.5
        case ACTION_SET_XP_RATE_50:  SetXpRateMultiplierTenths(player, 50); ShowMenu(player, item->GetGUID(), PAGE_RATES); return;  // x5.0
        case ACTION_SET_XP_RATE_100: SetXpRateMultiplierTenths(player, 60); ShowMenu(player, item->GetGUID(), PAGE_RATES); return;  // x6.0

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
        case ACTION_OPEN_PROFESSIONS:
            ShowMenu(player, item->GetGUID(), PAGE_PROFESSIONS);
            return;
        case ACTION_OPEN_BUFFS:
            ShowMenu(player, item->GetGUID(), PAGE_BUFFS);
            return;
        case ACTION_BACK_TO_MAIN:
            ShowMenu(player, item->GetGUID(), PAGE_MAIN);
            return;
        case ACTION_CLOSE:
            CloseGossipMenuFor(player);
            return;
        case ACTION_OPEN_RATES:
            ShowMenu(player, item->GetGUID(), PAGE_RATES);
            return;
        };

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

        // Profession confirmation handling
        if (action == ACTION_PROFESSION_CONFIRM)
        {
            CloseGossipMenuFor(player);
            HandleBuyProfession(player, SkillType(sender));
            return;
        }

        if (action == ACTION_PROFESSION_CANCEL)
        {
            ShowMenu(player, item->GetGUID(), PAGE_PROFESSIONS);
            return;
        }

        // Professions: first show a confirmation page before spending EC.
        if (SkillType skill = ProfessionSkillFromAction(action); skill != SKILL_NONE)
        {
            ShowProfessionConfirmMenu(player, item->GetGUID(), skill);
            return;
        }

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
            case ACTION_BUFF_ME_MELEE:
                CloseGossipMenuFor(player);
                BuffMe(player, false);
                return;
            case ACTION_BUFF_ME_CASTER:
                CloseGossipMenuFor(player);
                BuffMe(player, true);
                return;
            case ACTION_BUFF_ME_TANK:
                CloseGossipMenuFor(player);
                BuffMeTank(player);
                return;
            case ACTION_FREE_REPAIR:
                CloseGossipMenuFor(player);
                player->DurabilityRepairAll(false, 0.0f, false);
                ChatHandler(player->GetSession()).SendSysMessage("All items repaired.");
                return;

            case ACTION_CHARACTER_CUSTOMIZE:
                CloseGossipMenuFor(player);
                player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
                ChatHandler(player->GetSession()).SendSysMessage("Recustomize purchased. Log out to the character selection screen to apply it.");
                return;

            case ACTION_CHARACTER_CHANGE_RACE:
                CloseGossipMenuFor(player);
                player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
                ChatHandler(player->GetSession()).SendSysMessage("Race change purchased. Log out to the character selection screen to apply it.");
                return;

            case ACTION_CHARACTER_CHANGE_FACTION:
                CloseGossipMenuFor(player);
                player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
                ChatHandler(player->GetSession()).SendSysMessage("Faction change purchased. Log out to the character selection screen to apply it.");
                return;

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
            {
                CloseGossipMenuFor(player);

                // Mail list only works when the client opens a valid mailbox GUID (GO or NPC with mailbox flag).
                // Using the player's GUID will be rejected by CanOpenMailBox for normal players.
                uint32 mailboxEntry = sConfigMgr->GetOption<uint32>("Premium.MailboxNpcEntry", 0);

                if (mailboxEntry == 0)
                {
                    // If not configured, fall back to summoning the faction vendor and force mailbox flag.
                    // (This relies on being able to talk to the summoned creature, which we allow by setting the flag.)
                    mailboxEntry = player->GetTeamId() == TEAM_ALLIANCE ? NPC_VENDOR_A : NPC_VENDOR_H;
                }

                int npcDuration = sConfigMgr->GetOption<int32>("Premium.NpcDuration", 60) * IN_MILLISECONDS;
                if (npcDuration <= 0)
                    npcDuration = 60 * IN_MILLISECONDS;

                Creature* npc = player->SummonCreature(mailboxEntry,
                    player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0,
                    TEMPSUMMON_TIMED_DESPAWN, npcDuration);

                if (!npc)
                {
                    ChatHandler(player->GetSession()).SendSysMessage("Could not summon mailbox.");
                    return;
                }

                npc->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_MAILBOX);
                npc->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                npc->SetReactState(REACT_PASSIVE);
                npc->SetFaction(player->GetFaction());
                npc->SetOwnerGUID(player->GetGUID());
                npc->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, player->GetFollowAngle());

                player->GetSession()->SendShowMailBox(npc->GetGUID());
                return;
            }

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

            case ACTION_TP_SHATTRATH:
                TeleportPlayer(player, TpLocation{530, -1835.0618f, 5323.3115f, -12.428f, 5.0f});
                return;
            case ACTION_TP_DALARAN:
                TeleportPlayer(player, TpLocation{571, 5809.55f, 503.98f, 657.57f, 2.0f});
                return;

            case ACTION_RESET_ALL_INSTANCES:
            {
                CloseGossipMenuFor(player);

                TryClearAllInstanceBinds(player);
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

class premium_personal_rates : public PlayerScript
{
public:
    premium_personal_rates() : PlayerScript("premium_personal_rates", { PLAYERHOOK_ON_GIVE_EXP, PLAYERHOOK_ON_UPDATE_GATHERING_SKILL, PLAYERHOOK_ON_UPDATE_CRAFTING_SKILL }) {}

    void OnPlayerGiveXP(Player* player, uint32& amount, Unit* /*victim*/, uint8 /*xpSource*/) override
    {
        if (!player || !amount)
            return;

        uint32 permille = GetXpRatePermille(player);

        if (permille == 0)
        {
            amount = 0;
            return;
        }

        if (permille >= 1000)
            return;

        uint64 scaled = (uint64(amount) * permille) / 1000;
        if (scaled == 0)
            scaled = 1;

        amount = uint32(scaled);
    }

    void OnPlayerUpdateGatheringSkill(Player* player, uint32 /*skill_id*/, uint32 /*current*/, uint32 /*gray*/, uint32 /*green*/, uint32 /*yellow*/, uint32& gain) override
    {
        if (!player || !gain)
            return;

        uint32 permille = GetProfSkillRatePermille(player);

        if (permille == 0)
        {
            gain = 0;
            return;
        }

        if (permille >= 1000)
            return;

        uint64 scaled = (uint64(gain) * permille) / 1000;
        if (scaled == 0)
            scaled = 1;

        gain = uint32(scaled);
    }

    void OnPlayerUpdateCraftingSkill(Player* player, SkillLineAbilityEntry const* /*skill*/, uint32 /*current_level*/, uint32& gain) override
    {
        if (!player || !gain)
            return;

        uint32 permille = GetProfSkillRatePermille(player);

        if (permille == 0)
        {
            gain = 0;
            return;
        }

        if (permille >= 1000)
            return;

        uint64 scaled = (uint64(gain) * permille) / 1000;
        if (scaled == 0)
            scaled = 1;

        gain = uint32(scaled);
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
    new premium_personal_rates();
}

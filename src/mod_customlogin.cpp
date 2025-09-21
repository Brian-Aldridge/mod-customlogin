#include "Chat.h"
#include "Config.h"
#include "GuildMgr.h"
#include "Log.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldSession.h"
#include <cstdio>
#include "SpellMgr.h"
#include "ObjectMgr.h"
#include <set>
#include <vector>
#include <sstream>

#define AC_SCRIPT_NAME "mod_customlogin"

class CustomLogin : public PlayerScript
{

public:
  CustomLogin() : PlayerScript("CustomLogin")
  {
    if (sConfigMgr->GetOption<bool>("CustomLogin.Debug", false))
    {
      LOG_INFO("module",
               "[mod_customlogin] CustomLogin PlayerScript constructed");
    }
    // Optionally run startup-time validation of configured spells/items to help admins
    // (can be disabled via CustomLogin.ValidateOnStartup = 0)
    if (sConfigMgr->GetOption<bool>("CustomLogin.ValidateOnStartup", true))
      ValidateConfig();
  }

  // Validate configured spell and item IDs at module load and emit a single
  // consolidated report to the log. This avoids per-player noisy warnings and
  // helps admins fix config typos early.
  void ValidateConfig()
  {
    bool debug = sConfigMgr->GetOption<bool>("CustomLogin.Debug", false);
    std::set<uint32> missingSpells;
    std::set<uint32> missingItems;

    // Helper to check a spell id
    auto checkSpell = [&](uint32 id)
    {
      if (id == 0)
        return;
      if (!sSpellMgr->GetSpellInfo(id))
        missingSpells.insert(id);
    };

    // Helper to check an item id via ItemTemplate lookup
    auto checkItem = [&](uint32 id)
    {
      if (id == 0)
        return;
      if (!sObjectMgr->GetItemTemplate(id))
        missingItems.insert(id);
    };

    // Collect spells from Skills (global + class-specific)
    if (sConfigMgr->GetOption<bool>("CustomLogin.Skills", false))
    {
      // global list
      std::string global = sConfigMgr->GetOption<std::string>("CustomLogin.Skills.List", "");
      if (!global.empty())
      {
        std::istringstream iss(global);
        std::string token;
        while (std::getline(iss, token, ','))
        {
          token.erase(0, token.find_first_not_of(" \t\n\r"));
          token.erase(token.find_last_not_of(" \t\n\r") + 1);
          if (token.empty())
            continue;
          try
          {
            checkSpell(static_cast<uint32>(std::stoul(token)));
          }
          catch (...)
          {
          }
        }
      }
      // class lists
      std::vector<std::string> classes = {"Warrior", "Paladin", "Hunter", "Rogue", "Priest", "DeathKnight", "Shaman", "Mage", "Warlock", "Druid"};
      for (auto &c : classes)
      {
        std::string key = std::string("CustomLogin.Skills.") + c;
        std::string list = sConfigMgr->GetOption<std::string>(key, "");
        if (list.empty())
          continue;
        std::istringstream iss(list);
        std::string token;
        while (std::getline(iss, token, ','))
        {
          token.erase(0, token.find_first_not_of(" \t\n\r"));
          token.erase(token.find_last_not_of(" \t\n\r") + 1);
          if (token.empty())
            continue;
          try
          {
            checkSpell(static_cast<uint32>(std::stoul(token)));
          }
          catch (...)
          {
          }
        }
      }
    }

    // Professions
    if (sConfigMgr->GetOption<bool>("CustomLogin.StartingProfessions.Enable", false))
    {
      std::string profs = sConfigMgr->GetOption<std::string>("CustomLogin.StartingProfessions.List", "");
      if (!profs.empty())
      {
        std::istringstream iss(profs);
        std::string token;
        while (std::getline(iss, token, ','))
        {
          token.erase(0, token.find_first_not_of(" \t\n\r"));
          token.erase(token.find_last_not_of(" \t\n\r") + 1);
          if (token.empty())
            continue;
          try
          {
            checkSpell(static_cast<uint32>(std::stoul(token)));
          }
          catch (...)
          {
          }
        }
      }
    }

    // Starting mount/spell
    if (sConfigMgr->GetOption<bool>("CustomLogin.StartingMount.Enable", false))
    {
      checkSpell(sConfigMgr->GetOption<uint32>("CustomLogin.StartingMount.Spell", 0));
      checkSpell(sConfigMgr->GetOption<uint32>("CustomLogin.StartingMount.Skill", 0));
    }

    // SpecialAbility spells
    if (sConfigMgr->GetOption<bool>("CustomLogin.SpecialAbility", true))
    {
      checkSpell(sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Spell1", 0));
      checkSpell(sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Spell2", 0));
      checkSpell(sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Mount", 0));
    }

    // Starting bags (defaults and class-specific bag ids are items)
    std::string defaultBags = sConfigMgr->GetOption<std::string>("CustomLogin.Bags.Default", "");
    if (!defaultBags.empty())
    {
      std::istringstream iss(defaultBags);
      std::string token;
      while (std::getline(iss, token, ','))
      {
        token.erase(0, token.find_first_not_of(" \t\n\r"));
        token.erase(token.find_last_not_of(" \t\n\r") + 1);
        if (token.empty())
          continue;
        try
        {
          checkItem(static_cast<uint32>(std::stoul(token)));
        }
        catch (...)
        {
        }
      }
    }
    // class bag ids
    checkItem(sConfigMgr->GetOption<uint32>("CustomLogin.Bags.Hunter", 0));
    checkItem(sConfigMgr->GetOption<uint32>("CustomLogin.Bags.Warlock", 0));
    checkItem(sConfigMgr->GetOption<uint32>("CustomLogin.Bags.Rogue", 0));

    // Class item grants
    std::vector<std::string> classes = {"Warrior", "Paladin", "Hunter", "Rogue", "Priest", "DeathKnight", "Shaman", "Mage", "Warlock", "Druid"};
    std::vector<std::string> slots = {"Shoulders", "Chest", "Trinket", "Weapon1", "Weapon2", "Weapon3"};
    for (auto &c : classes)
    {
      for (auto &s : slots)
      {
        std::string key = std::string("CustomLogin.") + c + "." + s;
        uint32 id = sConfigMgr->GetOption<uint32>(key, 0);
        checkItem(id);
      }
    }

    // Consolidated report
    if (!missingSpells.empty() || !missingItems.empty())
    {
      LOG_WARN("module", "[mod_customlogin] Configuration validation found missing entries:");
      if (!missingSpells.empty())
      {
        std::ostringstream ss;
        ss << " Missing spells:";
        for (auto id : missingSpells)
          ss << " " << id;
        LOG_WARN("module", ss.str().c_str());
      }
      if (!missingItems.empty())
      {
        std::ostringstream ss;
        ss << " Missing items:";
        for (auto id : missingItems)
          ss << " " << id;
        LOG_WARN("module", ss.str().c_str());
      }
    }
    else if (debug)
    {
      LOG_INFO("module", "[mod_customlogin] Configuration validation passed (no missing spells/items found)");
    }
  }

  void OnPlayerLogin(Player *player)
  {
    bool debug = sConfigMgr->GetOption<bool>("CustomLogin.Debug", false);
    if (debug)
      LOG_INFO("module", "[mod_customlogin] OnPlayerLogin called for {}",
               player->GetName().c_str());

    bool enable = sConfigMgr->GetOption<bool>("CustomLogin.Enable", true);
    if (!enable)
    {
      if (debug)
        LOG_INFO("module", "[mod_customlogin] Module disabled; skipping login logic for {}", player->GetName().c_str());
      return;
    }

    // Use total played time to detect first login
    if (player->GetTotalPlayedTime() == 0)
    {
      if (debug)
        LOG_INFO("module", "[mod_customlogin] First login detected for {}",
                 player->GetName().c_str());
      GiveFirstLoginRewards(player, debug);
    }

    bool announce = sConfigMgr->GetOption<bool>("CustomLogin.Announce", true);
    bool playerAnnounce =
        sConfigMgr->GetOption<bool>("CustomLogin.PlayerAnnounce", true);

    if (announce)
    {
      std::string announceMsg = sConfigMgr->GetOption<std::string>(
          "CustomLogin.AnnounceMessage",
          "This server is running the |cff4CFF00CustomLogin |rmodule.");
      ChatHandler(player->GetSession()).SendSysMessage(announceMsg.c_str());
    }
    if (playerAnnounce)
    {
      std::ostringstream ss;
      if (player->GetTeamId() == TEAM_ALLIANCE)
      {
        ss << "|cffFFFFFF[|cff2897FF Alliance |cffFFFFFF]:|cff4CFF00 "
           << player->GetName() << "|cffFFFFFF has come online.";
      }
      else
      {
        ss << "|cffFFFFFF[|cffFF0000 Horde |cffFFFFFF]:|cff4CFF00 "
           << player->GetName() << "|cffFFFFFF has come online.";
      }
      ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
    }
  }

  void OnPlayerLogout(Player *player)
  {
    bool debug = sConfigMgr->GetOption<bool>("CustomLogin.Debug", false);
    if (debug)
      LOG_INFO("module", "[mod_customlogin] OnPlayerLogout called for {}",
               player->GetName().c_str());
    bool enable = sConfigMgr->GetOption<bool>("CustomLogin.Enable", true);
    bool playerAnnounce =
        sConfigMgr->GetOption<bool>("CustomLogin.PlayerAnnounce", true);

    if (enable && playerAnnounce)
    {
      std::ostringstream ss;
      if (player->GetTeamId() == TEAM_ALLIANCE)
      {
        ss << "|cffFFFFFF[|cff2897FF Alliance |cffFFFFFF]|cff4CFF00 "
           << player->GetName() << "|cffFFFFFF has left the game.";
      }
      else
      {
        ss << "|cffFFFFFF[|cffFF0000 Horde |cffFFFFFF]|cff4CFF00 "
           << player->GetName() << "|cffFFFFFF has left the game.";
      }
      ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
    }
  }

private:
  // GiveFirstLoginRewards contract (first-login only):
  // - Teach configured professions (optional)
  // - Teach riding skill and mount (optional)
  // - Equip up to four default bags into bag slots 19–22, in order (optional)
  //   • Then, if a class-specific bag is set (>0), equip it into the first empty of 19–22
  //   • Invalid/missing bag IDs are skipped
  // - Grant starting gold (optional)
  // - Add class-configured BoA items (if enabled)
  // Edge cases: missing items/IDs are skipped; we don’t overwrite occupied bag slots.
  void GiveFirstLoginRewards(Player *player, bool debug)
  {
    // Teach starting professions if enabled
    if (sConfigMgr->GetOption<bool>("CustomLogin.StartingProfessions.Enable", false))
    {
      std::string profList = sConfigMgr->GetOption<std::string>("CustomLogin.StartingProfessions.List", "");
      if (!profList.empty())
      {
        std::istringstream iss(profList);
        std::string profIdStr;
        while (std::getline(iss, profIdStr, ','))
        {
          uint32 profId = std::stoi(profIdStr);
          if (profId > 0)
          {
            SpellInfo const *spellInfo = sSpellMgr->GetSpellInfo(profId);
            if (!spellInfo)
            {
              if (debug)
                LOG_WARN("module", "[CustomLogin] Profession spell {} not found in Spell store; skipping", profId);
            }
            else
            {
              player->learnSpell(profId, false);
              if (debug)
                LOG_INFO("module", "[CustomLogin] Granted profession spell {} to {}", profId, player->GetName().c_str());
            }
          }
        }
      }
    }
    // Grant configured weapon / extra skills if enabled
    if (sConfigMgr->GetOption<bool>("CustomLogin.Skills", false))
    {
      // First check for a class-specific skills list
      std::string classSkillsKey;
      switch (player->getClass())
      {
      case CLASS_WARRIOR:
        classSkillsKey = "CustomLogin.Skills.Warrior";
        break;
      case CLASS_PALADIN:
        classSkillsKey = "CustomLogin.Skills.Paladin";
        break;
      case CLASS_HUNTER:
        classSkillsKey = "CustomLogin.Skills.Hunter";
        break;
      case CLASS_ROGUE:
        classSkillsKey = "CustomLogin.Skills.Rogue";
        break;
      case CLASS_PRIEST:
        classSkillsKey = "CustomLogin.Skills.Priest";
        break;
      case CLASS_DEATH_KNIGHT:
        classSkillsKey = "CustomLogin.Skills.DeathKnight";
        break;
      case CLASS_SHAMAN:
        classSkillsKey = "CustomLogin.Skills.Shaman";
        break;
      case CLASS_MAGE:
        classSkillsKey = "CustomLogin.Skills.Mage";
        break;
      case CLASS_WARLOCK:
        classSkillsKey = "CustomLogin.Skills.Warlock";
        break;
      case CLASS_DRUID:
        classSkillsKey = "CustomLogin.Skills.Druid";
        break;
      default:
        classSkillsKey.clear();
        break;
      }

      std::string skillsList;
      if (!classSkillsKey.empty())
        skillsList = sConfigMgr->GetOption<std::string>(classSkillsKey, "");
      if (skillsList.empty())
        skillsList = sConfigMgr->GetOption<std::string>("CustomLogin.Skills.List", "");

      if (!skillsList.empty())
      {
        std::istringstream iss(skillsList);
        std::string skillStr;
        while (std::getline(iss, skillStr, ','))
        {
          // trim
          skillStr.erase(0, skillStr.find_first_not_of(" \t\n\r"));
          skillStr.erase(skillStr.find_last_not_of(" \t\n\r") + 1);
          if (skillStr.empty())
            continue;
          try
          {
            uint32 skillId = static_cast<uint32>(std::stoul(skillStr));
            if (skillId > 0)
            {
              SpellInfo const *spellInfo = sSpellMgr->GetSpellInfo(skillId);
              if (!spellInfo)
              {
                if (debug)
                  LOG_WARN("module", "[CustomLogin] Skill/spell {} not found in Spell store; skipping", skillId);
              }
              else
              {
                player->learnSpell(skillId, false);
                if (debug)
                  LOG_INFO("module", "[CustomLogin] Granted skill/spell {} to {}", skillId, player->GetName().c_str());
              }
            }
          }
          catch (...)
          {
            if (debug)
              LOG_WARN("module", "[CustomLogin] Invalid skill token '{}' in config", skillStr);
          }
        }
      }
      else if (debug)
      {
        LOG_INFO("module", "[CustomLogin] Skills enabled but no skills configured for {} (global or class-specific)", player->GetName().c_str());
      }
    }
    // Give starting mount and riding skill if enabled
    if (sConfigMgr->GetOption<bool>("CustomLogin.StartingMount.Enable", false))
    {
      uint32 mountSpell = sConfigMgr->GetOption<uint32>("CustomLogin.StartingMount.Spell", 0);
      uint32 ridingSkill = sConfigMgr->GetOption<uint32>("CustomLogin.StartingMount.Skill", 0);
      if (ridingSkill)
      {
        SpellInfo const *spellInfo = sSpellMgr->GetSpellInfo(ridingSkill);
        if (!spellInfo)
        {
          if (debug)
            LOG_WARN("module", "[CustomLogin] Riding skill {} not found in Spell store; skipping", ridingSkill);
        }
        else
        {
          player->learnSpell(ridingSkill, false);
          if (debug)
            LOG_INFO("module", "[CustomLogin] Granted riding skill {} to {}", ridingSkill, player->GetName().c_str());
        }
      }
      if (mountSpell)
      {
        SpellInfo const *spellInfo = sSpellMgr->GetSpellInfo(mountSpell);
        if (!spellInfo)
        {
          if (debug)
            LOG_WARN("module", "[CustomLogin] Mount spell {} not found in Spell store; skipping", mountSpell);
        }
        else
        {
          player->learnSpell(mountSpell, false);
          if (debug)
            LOG_INFO("module", "[CustomLogin] Granted mount spell {} to {}", mountSpell, player->GetName().c_str());
        }
      }
    }
    // Give starting bags if configured
    std::string defaultBags = sConfigMgr->GetOption<std::string>("CustomLogin.Bags.Default", "");
    if (!defaultBags.empty())
    {
      std::istringstream iss(defaultBags);
      std::string bagIdStr;
      int slot = 19;
      while (std::getline(iss, bagIdStr, ',') && slot <= 22)
      {
        // Trim whitespace
        bagIdStr.erase(0, bagIdStr.find_first_not_of(" \t\n\r"));
        bagIdStr.erase(bagIdStr.find_last_not_of(" \t\n\r") + 1);
        uint32 bagId = 0;
        bool parsed = true;
        try
        {
          bagId = static_cast<uint32>(std::stoul(bagIdStr));
        }
        catch (...)
        {
          parsed = false;
          if (debug)
            LOG_WARN("module", "[CustomLogin] Skipping invalid bag ID token '{}' in defaults", bagIdStr);
        }
        // If parsing succeeded but the ID resolved to zero, log an informational message
        if (parsed && bagId == 0 && debug)
        {
          LOG_INFO("module", "[CustomLogin] Skipping default bag in slot {} due to zero/invalid ID token '{}'", slot, bagIdStr);
        }
        // Only equip if the actual bag slot (19–22) is empty in the base inventory container
        if (bagId > 0 && !player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        {
          Item *bag = Item::CreateItem(bagId, 1, player);
          if (bag)
          {
            player->EquipItem(slot, bag, true);
            if (debug)
              LOG_INFO("module", "[CustomLogin] Equipped default bag {} to {} in slot {}", bagId, player->GetName().c_str(), slot);
          }
          else if (debug)
          {
            LOG_WARN("module", "[CustomLogin] Failed to create default bag {} for {} (missing in DB?)", bagId, player->GetName().c_str());
          }
        }
        else if (bagId > 0 && debug)
        {
          LOG_INFO("module", "[CustomLogin] Bag slot {} already occupied; not equipping default bag {}", slot, bagId);
        }
        ++slot;
      }
      if (debug)
        LOG_INFO("module", "[CustomLogin] Granted default starting bags: {}", defaultBags);
    }

    // Give class-specific bag if configured
    uint32 classBagId = 0;
    switch (player->getClass())
    {
    case CLASS_HUNTER:
      classBagId = sConfigMgr->GetOption<uint32>("CustomLogin.Bags.Hunter", 0);
      break;
    case CLASS_WARLOCK:
      classBagId = sConfigMgr->GetOption<uint32>("CustomLogin.Bags.Warlock", 0);
      break;
    case CLASS_ROGUE:
      classBagId = sConfigMgr->GetOption<uint32>("CustomLogin.Bags.Rogue", 0);
      break;
    // Add more class-specific cases as needed
    default:
      break;
    }
    if (classBagId > 0)
    {
      // Place in the first empty bag slot (19-22)
      bool classBagEquipped = false;
      for (int slot = 19; slot <= 22; ++slot)
      {
        // Only equip into an actually empty bag slot
        if (!player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        {
          Item *bag = Item::CreateItem(classBagId, 1, player);
          if (bag)
          {
            player->EquipItem(slot, bag, true);
            if (debug)
              LOG_INFO("module", "[CustomLogin] Granted class-specific bag {} to {} in slot {}", classBagId, player->GetName().c_str(), slot);
            classBagEquipped = true;
          }
          else if (debug)
          {
            LOG_WARN("module", "[CustomLogin] Failed to create class-specific bag {} for {} (missing in DB?)", classBagId, player->GetName().c_str());
          }
          break;
        }
      }
      if (!classBagEquipped && debug)
        LOG_INFO("module", "[CustomLogin] No empty bag slot (19–22) available for class-specific bag {} for {}", classBagId, player->GetName().c_str());
    }
    // Give starting gold if enabled
    if (sConfigMgr->GetOption<bool>("CustomLogin.StartingGold.Enable", false))
    {
      uint32 gold = sConfigMgr->GetOption<uint32>("CustomLogin.StartingGold.Gold", 0);
      uint32 silver = sConfigMgr->GetOption<uint32>("CustomLogin.StartingGold.Silver", 0);
      uint32 copper = sConfigMgr->GetOption<uint32>("CustomLogin.StartingGold.Copper", 0);
      uint32 totalCopper = (gold * 10000) + (silver * 100) + copper;
      if (totalCopper > 0)
      {
        player->ModifyMoney(totalCopper);
        if (debug)
          LOG_INFO("module", "[CustomLogin] Granted starting money: {} gold, {} silver, {} copper to {}", gold, silver, copper, player->GetName().c_str());
      }
    }
    if (debug)
      LOG_INFO("module", "[CustomLogin] Giving first login rewards to {}", player->GetName().c_str());

    bool boa = sConfigMgr->GetOption<bool>("CustomLogin.BoA", true);
    if (!boa)
    {
      if (debug)
        LOG_INFO("module", "[CustomLogin] BoA item grants disabled for {}", player->GetName().c_str());
      return;
    }

    // Define item variables
    uint32 shoulders = 0, chest = 0, trinket = 0, weapon1 = 0, weapon2 = 0,
           weapon3 = 0;

    // Read items from the config file based on the player's class
    switch (player->getClass())
    {
    case CLASS_WARRIOR:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.Warrior.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.Warrior.Chest", 0);
      trinket = sConfigMgr->GetOption<uint32>("CustomLogin.Warrior.Trinket", 0);
      weapon1 = sConfigMgr->GetOption<uint32>("CustomLogin.Warrior.Weapon1", 0);
      weapon2 = sConfigMgr->GetOption<uint32>("CustomLogin.Warrior.Weapon2", 0);
      weapon3 = sConfigMgr->GetOption<uint32>("CustomLogin.Warrior.Weapon3", 0);
      break;
    case CLASS_PALADIN:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.Paladin.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.Paladin.Chest", 0);
      trinket = sConfigMgr->GetOption<uint32>("CustomLogin.Paladin.Trinket", 0);
      weapon1 = sConfigMgr->GetOption<uint32>("CustomLogin.Paladin.Weapon1", 0);
      weapon2 = sConfigMgr->GetOption<uint32>("CustomLogin.Paladin.Weapon2", 0);
      break;
    case CLASS_HUNTER:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.Hunter.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.Hunter.Chest", 0);
      trinket = sConfigMgr->GetOption<uint32>("CustomLogin.Hunter.Trinket", 0);
      weapon1 = sConfigMgr->GetOption<uint32>("CustomLogin.Hunter.Weapon1", 0);
      weapon2 = sConfigMgr->GetOption<uint32>("CustomLogin.Hunter.Weapon2", 0);
      weapon3 = sConfigMgr->GetOption<uint32>("CustomLogin.Hunter.Weapon3", 0);
      break;
    case CLASS_ROGUE:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.Rogue.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.Rogue.Chest", 0);
      trinket = sConfigMgr->GetOption<uint32>("CustomLogin.Rogue.Trinket", 0);
      weapon1 = sConfigMgr->GetOption<uint32>("CustomLogin.Rogue.Weapon1", 0);
      weapon2 = sConfigMgr->GetOption<uint32>("CustomLogin.Rogue.Weapon2", 0);
      break;
    case CLASS_PRIEST:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.Priest.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.Priest.Chest", 0);
      trinket = sConfigMgr->GetOption<uint32>("CustomLogin.Priest.Trinket", 0);
      weapon1 = sConfigMgr->GetOption<uint32>("CustomLogin.Priest.Weapon1", 0);
      break;
    case CLASS_DEATH_KNIGHT:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.DeathKnight.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.DeathKnight.Chest", 0);
      trinket =
          sConfigMgr->GetOption<uint32>("CustomLogin.DeathKnight.Trinket", 0);
      weapon1 =
          sConfigMgr->GetOption<uint32>("CustomLogin.DeathKnight.Weapon1", 0);
      weapon2 =
          sConfigMgr->GetOption<uint32>("CustomLogin.DeathKnight.Weapon2", 0);
      break;
    case CLASS_SHAMAN:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.Shaman.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.Shaman.Chest", 0);
      trinket = sConfigMgr->GetOption<uint32>("CustomLogin.Shaman.Trinket", 0);
      weapon1 = sConfigMgr->GetOption<uint32>("CustomLogin.Shaman.Weapon1", 0);
      weapon2 = sConfigMgr->GetOption<uint32>("CustomLogin.Shaman.Weapon2", 0);
      break;
    case CLASS_MAGE:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.Mage.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.Mage.Chest", 0);
      trinket = sConfigMgr->GetOption<uint32>("CustomLogin.Mage.Trinket", 0);
      weapon1 = sConfigMgr->GetOption<uint32>("CustomLogin.Mage.Weapon1", 0);
      break;
    case CLASS_WARLOCK:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.Warlock.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.Warlock.Chest", 0);
      trinket = sConfigMgr->GetOption<uint32>("CustomLogin.Warlock.Trinket", 0);
      weapon1 = sConfigMgr->GetOption<uint32>("CustomLogin.Warlock.Weapon1", 0);
      break;
    case CLASS_DRUID:
      shoulders =
          sConfigMgr->GetOption<uint32>("CustomLogin.Druid.Shoulders", 0);
      chest = sConfigMgr->GetOption<uint32>("CustomLogin.Druid.Chest", 0);
      trinket = sConfigMgr->GetOption<uint32>("CustomLogin.Druid.Trinket", 0);
      weapon1 = sConfigMgr->GetOption<uint32>("CustomLogin.Druid.Weapon1", 0);
      weapon2 = sConfigMgr->GetOption<uint32>("CustomLogin.Druid.Weapon2", 0);
      break;
    default:
      break;
    }

    // Add items to the player's inventory if set
    if (shoulders)
    {
      player->AddItem(shoulders, 1);
      if (debug)
        LOG_INFO("module", "[CustomLogin] Added item (shoulders) {} to {}", shoulders, player->GetName().c_str());
    }
    if (chest)
    {
      player->AddItem(chest, 1);
      if (debug)
        LOG_INFO("module", "[CustomLogin] Added item (chest) {} to {}", chest, player->GetName().c_str());
    }
    if (trinket)
    {
      player->AddItem(trinket, 1);
      if (debug)
        LOG_INFO("module", "[CustomLogin] Added item (trinket) {} to {}", trinket, player->GetName().c_str());
    }
    if (weapon1)
    {
      player->AddItem(weapon1, 1);
      if (debug)
        LOG_INFO("module", "[CustomLogin] Added item (weapon1) {} to {}", weapon1, player->GetName().c_str());
    }
    if (weapon2)
    {
      player->AddItem(weapon2, 1);
      if (debug)
        LOG_INFO("module", "[CustomLogin] Added item (weapon2) {} to {}", weapon2, player->GetName().c_str());
    }
    if (weapon3)
    {
      player->AddItem(weapon3, 1);
      if (debug)
        LOG_INFO("module", "[CustomLogin] Added item (weapon3) {} to {}", weapon3, player->GetName().c_str());
    }

    // Inform the player
    std::ostringstream ss;
    ss << "|cffFF0000[CustomLogin]:|cffFF8000 You have received heirloom "
          "items.";
    ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());

    if (sConfigMgr->GetOption<bool>("CustomLogin.SpecialAbility", true))
    {
      uint32 spell1 =
          sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Spell1", 0);
      uint32 spell2 =
          sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Spell2", 0);
      uint32 title =
          sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Title", 0);
      uint32 mount =
          sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Mount", 0);

      if (spell1)
      {
        SpellInfo const *s1 = sSpellMgr->GetSpellInfo(spell1);
        if (!s1)
        {
          if (debug)
            LOG_WARN("module", "[CustomLogin] SpecialAbility.Spell1 {} not found in Spell store; skipping", spell1);
        }
        else
        {
          player->learnSpell(spell1, false);
        }
      }
      if (spell2)
      {
        SpellInfo const *s2 = sSpellMgr->GetSpellInfo(spell2);
        if (!s2)
        {
          if (debug)
            LOG_WARN("module", "[CustomLogin] SpecialAbility.Spell2 {} not found in Spell store; skipping", spell2);
        }
        else
        {
          player->learnSpell(spell2, false);
        }
      }
      if (title)
      {
        if (CharTitlesEntry const *titleEntry =
                sCharTitlesStore.LookupEntry(title))
        {
          player->SetTitle(titleEntry);
        }
      }
      if (mount)
      {
        SpellInfo const *sm = sSpellMgr->GetSpellInfo(mount);
        if (!sm)
        {
          if (debug)
            LOG_WARN("module", "[CustomLogin] SpecialAbility.Mount {} not found in Spell store; skipping", mount);
        }
        else
        {
          player->learnSpell(mount, false);
        }
      }
    }

    if (sConfigMgr->GetOption<bool>("CustomLogin.Reputation", true))
    {
      std::map<uint32, std::string> reputations = {
          {72, "CustomLogin.Reputation.Stormwind"},
          {76, "CustomLogin.Reputation.Orgrimmar"},
          {69, "CustomLogin.Reputation.Darnassus"},
          {81, "CustomLogin.Reputation.ThunderBluff"},
          {54, "CustomLogin.Reputation.Gnomeregan"},
          {47, "CustomLogin.Reputation.Ironforge"},
          {530, "CustomLogin.Reputation.DarkspearTrolls"},
          {68, "CustomLogin.Reputation.Undercity"},
          {930, "CustomLogin.Reputation.Exodar"},
          {911, "CustomLogin.Reputation.Silvermoon"},
          {529, "CustomLogin.Reputation.ArgentDawn"},
          {609, "CustomLogin.Reputation.CenarionCircle"},
          {576, "CustomLogin.Reputation.TimbermawHold"},
          {270, "CustomLogin.Reputation.ZandalarTribe"},
          {87, "CustomLogin.Reputation.BloodsailBuccaneers"},
          {21, "CustomLogin.Reputation.SteamwheedleCartel"},
          {935, "CustomLogin.Reputation.ShaTar"},
          {1011, "CustomLogin.Reputation.LowerCity"},
          {942, "CustomLogin.Reputation.CenarionExpedition"},
          {932, "CustomLogin.Reputation.TheAldor"},
          {934, "CustomLogin.Reputation.TheScryers"},
          {933, "CustomLogin.Reputation.TheConsortium"},
          {941, "CustomLogin.Reputation.TheMaghar"},
          {978, "CustomLogin.Reputation.Kurenai"},
          {970, "CustomLogin.Reputation.Sporeggar"},
          {1015, "CustomLogin.Reputation.Netherwing"},
          {1106, "CustomLogin.Reputation.ArgentCrusade"},
          {1104, "CustomLogin.Reputation.FrenzyheartTribe"},
          {1105, "CustomLogin.Reputation.TheOracles"},
          {1090, "CustomLogin.Reputation.KirinTor"},
          {1091, "CustomLogin.Reputation.TheWyrmrestAccord"},
          {1052, "CustomLogin.Reputation.HordeExpedition"},
          {1037, "CustomLogin.Reputation.AllianceVanguard"},
          {1119, "CustomLogin.Reputation.TheSonsOfHodir"},
          {1156, "CustomLogin.Reputation.TheAshenVerdict"}};

      for (const auto &[factionId, configKey] : reputations)
      {
        uint32 reputationValue = sConfigMgr->GetOption<uint32>(configKey, 0);
        if (reputationValue > 0)
        {
          player->SetReputation(factionId, reputationValue);
          if (debug)
            LOG_INFO("module",
                     "[CustomLogin] Setting reputation for faction {} to {}",
                     factionId, reputationValue);
        }
      }
    }
  }
};

void AddSC_mod_customlogin() { new CustomLogin(); };

#include "Chat.h"
#include "Config.h"
#include "GuildMgr.h"
#include "Log.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldSession.h"
#include <cstdio>

#define AC_SCRIPT_NAME "mod_customlogin"

class CustomLogin : public PlayerScript {

public:
  CustomLogin() : PlayerScript("CustomLogin") {
    if (sConfigMgr->GetOption<bool>("CustomLogin.Debug", false)) {
      LOG_INFO("module",
               "[mod_customlogin] CustomLogin PlayerScript constructed");
    }
  }

  void OnPlayerLogin(Player *player) {
    bool debug = sConfigMgr->GetOption<bool>("CustomLogin.Debug", false);
    if (debug)
      LOG_INFO("module", "[mod_customlogin] OnPlayerLogin called for {}",
               player->GetName().c_str());

    bool enable = sConfigMgr->GetOption<bool>("CustomLogin.Enable", true);
    if (!enable)
      return;

    // Use total played time to detect first login
    if (player->GetTotalPlayedTime() == 0) {
      if (debug)
        LOG_INFO("module", "[mod_customlogin] First login detected for {}",
                 player->GetName().c_str());
      GiveFirstLoginRewards(player, debug);
    }

    bool announce = sConfigMgr->GetOption<bool>("CustomLogin.Announce", true);
    bool playerAnnounce =
        sConfigMgr->GetOption<bool>("CustomLogin.PlayerAnnounce", true);

    if (announce) {
      std::string announceMsg = sConfigMgr->GetOption<std::string>(
          "CustomLogin.AnnounceMessage",
          "This server is running the |cff4CFF00CustomLogin |rmodule.");
      ChatHandler(player->GetSession()).SendSysMessage(announceMsg.c_str());
    }
    if (playerAnnounce) {
      std::ostringstream ss;
      if (player->GetTeamId() == TEAM_ALLIANCE) {
        ss << "|cffFFFFFF[|cff2897FF Alliance |cffFFFFFF]:|cff4CFF00 "
           << player->GetName() << "|cffFFFFFF has come online.";
      } else {
        ss << "|cffFFFFFF[|cffFF0000 Horde |cffFFFFFF]:|cff4CFF00 "
           << player->GetName() << "|cffFFFFFF has come online.";
      }
      ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
    }
  }

  void OnPlayerLogout(Player *player) {
    bool debug = sConfigMgr->GetOption<bool>("CustomLogin.Debug", false);
    if (debug)
      LOG_INFO("module", "[mod_customlogin] OnPlayerLogout called for {}",
               player->GetName().c_str());
    bool enable = sConfigMgr->GetOption<bool>("CustomLogin.Enable", true);
    bool playerAnnounce =
        sConfigMgr->GetOption<bool>("CustomLogin.PlayerAnnounce", true);

    if (enable && playerAnnounce) {
      std::ostringstream ss;
      if (player->GetTeamId() == TEAM_ALLIANCE) {
        ss << "|cffFFFFFF[|cff2897FF Alliance |cffFFFFFF]|cff4CFF00 "
           << player->GetName() << "|cffFFFFFF has left the game.";
      } else {
        ss << "|cffFFFFFF[|cffFF0000 Horde |cffFFFFFF]|cff4CFF00 "
           << player->GetName() << "|cffFFFFFF has left the game.";
      }
      ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
    }
  }

private:
  void GiveFirstLoginRewards(Player *player, bool debug) {
  // Teach starting professions if enabled
  if (sConfigMgr->GetOption<bool>("CustomLogin.StartingProfessions.Enable", false)) {
    std::string profList = sConfigMgr->GetOption<std::string>("CustomLogin.StartingProfessions.List", "");
    if (!profList.empty()) {
      std::istringstream iss(profList);
      std::string profIdStr;
      while (std::getline(iss, profIdStr, ',')) {
        uint32 profId = std::stoi(profIdStr);
        if (profId > 0) {
          player->learnSpell(profId, false);
          if (debug)
            LOG_INFO("module", "[CustomLogin] Granted profession spell {} to {}", profId, player->GetName().c_str());
        }
      }
    }
  }
  // Give starting mount and riding skill if enabled
  if (sConfigMgr->GetOption<bool>("CustomLogin.StartingMount.Enable", false)) {
    uint32 mountSpell = sConfigMgr->GetOption<uint32>("CustomLogin.StartingMount.Spell", 0);
    uint32 ridingSkill = sConfigMgr->GetOption<uint32>("CustomLogin.StartingMount.Skill", 0);
    if (ridingSkill) {
      player->learnSpell(ridingSkill, false);
      if (debug)
        LOG_INFO("module", "[CustomLogin] Granted riding skill {} to {}", ridingSkill, player->GetName().c_str());
    }
    if (mountSpell) {
      player->learnSpell(mountSpell, false);
      if (debug)
        LOG_INFO("module", "[CustomLogin] Granted mount spell {} to {}", mountSpell, player->GetName().c_str());
    }
  }
  // Give starting bags if configured
  std::string defaultBags = sConfigMgr->GetOption<std::string>("CustomLogin.Bags.Default", "");
  if (!defaultBags.empty()) {
    std::istringstream iss(defaultBags);
    std::string bagIdStr;
    int bagSlot = 0;
    while (std::getline(iss, bagIdStr, ',') && bagSlot < 4) {
      uint32 bagId = std::stoi(bagIdStr);
      if (bagId > 0)
        player->SetBagSlot(bagSlot, bagId);
      bagSlot++;
    }
    if (debug)
      LOG_INFO("module", "[CustomLogin] Granted default starting bags: {}", defaultBags);
  }

  // Give class-specific bag if configured
  uint32 classBagId = 0;
  switch (player->getClass()) {
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
  if (classBagId > 0) {
    // Place in the first empty bag slot
    for (int bagSlot = 0; bagSlot < 4; ++bagSlot) {
      if (!player->GetBagByIndex(bagSlot)) {
        player->SetBagSlot(bagSlot, classBagId);
        if (debug)
          LOG_INFO("module", "[CustomLogin] Granted class-specific bag {} to {} in slot {}", classBagId, player->GetName().c_str(), bagSlot);
        break;
      }
    }
  }
  // Give starting gold if enabled
  if (sConfigMgr->GetOption<bool>("CustomLogin.StartingGold.Enable", false)) {
    uint32 gold = sConfigMgr->GetOption<uint32>("CustomLogin.StartingGold.Gold", 0);
    uint32 silver = sConfigMgr->GetOption<uint32>("CustomLogin.StartingGold.Silver", 0);
    uint32 copper = sConfigMgr->GetOption<uint32>("CustomLogin.StartingGold.Copper", 0);
    uint32 totalCopper = (gold * 10000) + (silver * 100) + copper;
    if (totalCopper > 0) {
      player->ModifyMoney(totalCopper);
      if (debug)
        LOG_INFO("module", "[CustomLogin] Granted starting money: {} gold, {} silver, {} copper to {}", gold, silver, copper, player->GetName().c_str());
    }
  }
    if (debug)
      printf("[CustomLogin] Giving first login rewards to %s\n",
             player->GetName().c_str());

    bool boa = sConfigMgr->GetOption<bool>("CustomLogin.BoA", true);
    if (!boa)
      return;

  // Define item variables
  uint32 shoulders = 0, chest = 0, trinket = 0, weapon1 = 0, weapon2 = 0,
       weapon3 = 0;

  // Read items from the config file based on the player's class
    switch (player->getClass()) {
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
      player->AddItem(shoulders, 1);
    if (chest)
      player->AddItem(chest, 1);
    if (trinket)
      player->AddItem(trinket, 1);
    if (weapon1)
      player->AddItem(weapon1, 1);
    if (weapon2)
      player->AddItem(weapon2, 1);
    if (weapon3)
      player->AddItem(weapon3, 1);

    // Inform the player
    std::ostringstream ss;
    ss << "|cffFF0000[CustomLogin]:|cffFF8000 You have received heirloom "
          "items.";
    ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());

    if (sConfigMgr->GetOption<bool>("CustomLogin.SpecialAbility", true)) {
      uint32 spell1 =
          sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Spell1", 0);
      uint32 spell2 =
          sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Spell2", 0);
      uint32 title =
          sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Title", 0);
      uint32 mount =
          sConfigMgr->GetOption<uint32>("CustomLogin.SpecialAbility.Mount", 0);

      if (spell1)
        player->learnSpell(spell1, false);
      if (spell2)
        player->learnSpell(spell2, false);
      if (title) {
        if (CharTitlesEntry const *titleEntry =
                sCharTitlesStore.LookupEntry(title)) {
          player->SetTitle(titleEntry);
        }
      }
      if (mount)
        player->learnSpell(mount, false);
    }

    if (sConfigMgr->GetOption<bool>("CustomLogin.Reputation", true)) {
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

      for (const auto &[factionId, configKey] : reputations) {
        uint32 reputationValue = sConfigMgr->GetOption<uint32>(configKey, 0);
        if (reputationValue > 0) {
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

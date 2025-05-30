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
    LOG_INFO("module",
             "[mod_customlogin] CustomLogin PlayerScript constructed");
  }

  void OnPlayerLogin(Player *player) {
    LOG_INFO("module", "[mod_customlogin] OnPlayerLogin called for {}",
             player->GetName().c_str());

    bool enable = sConfigMgr->GetOption<bool>("CustomLogin.Enable", true);
    if (!enable)
      return;

    const uint32 markerItem = 50255; // Dread Pirate Ring (used as marker)
    if (!player->HasItemCount(markerItem, 1, false)) {
      LOG_INFO("module", "[mod_customlogin] Giving first login rewards to {}",
               player->GetName().c_str());
      GiveFirstLoginRewards(player);
    }

    bool announce = sConfigMgr->GetOption<bool>("CustomLogin.Announce", true);
    bool playerAnnounce =
        sConfigMgr->GetOption<bool>("CustomLogin.PlayerAnnounce", true);

    if (announce) {
      ChatHandler(player->GetSession())
          .SendSysMessage(
              "This server is running the |cff4CFF00CustomLogin |rmodule.");
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
  void GiveFirstLoginRewards(Player *player) {
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

    // Add items to the player's inventory
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
        player->LearnSpell(spell1, false);
      if (spell2)
        player->LearnSpell(spell2, false);
      if (title)
        player->SetTitle(title);
      if (mount)
        player->LearnSpell(mount, false);
    }

    if (sConfigMgr->GetOption<bool>("CustomLogin.Reputation", true)) {
      player->SetReputation(72, 42000); // Stormwind
      player->SetReputation(76, 42000); // Orgrimmar
      // Add other factions as needed
    }
  }
};

void AddSC_mod_customlogin() { new CustomLogin(); };

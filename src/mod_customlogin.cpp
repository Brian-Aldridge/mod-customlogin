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
    LOG_INFO("module", "[mod_customlogin] OnPlayerLogin called for %s",
             player->GetName().c_str());

    bool enable = sConfigMgr->GetOption<bool>("CustomLogin.Enable", true);
    if (!enable)
      return;

    const uint32 markerItem = 50255; // Dread Pirate Ring (used as marker)
    if (!player->HasItemCount(markerItem, 1, false)) {
      LOG_INFO("module", "[mod_customlogin] Giving first login rewards to %s",
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
    LOG_INFO("module", "[mod_customlogin] OnPlayerLogout called for %s",
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

    bool enable = sConfigMgr->GetOption<bool>("CustomLogin.Enable", true);
    bool boa = sConfigMgr->GetOption<bool>("CustomLogin.BoA", true);
    bool skills = sConfigMgr->GetOption<bool>("CustomLogin.Skills", true);
    bool special =
        sConfigMgr->GetOption<bool>("CustomLogin.SpecialAbility", true);
    bool rep = sConfigMgr->GetOption<bool>("CustomLogin.Reputation", true);

    printf("[CustomLogin] Config: Enable=%d, BoA=%d, Skills=%d, "
           "SpecialAbility=%d, Reputation=%d\n",
           enable, boa, skills, special, rep);

    if (enable) {
      if (boa) {
        printf("[CustomLogin] Granting BoA items to %s\n",
               player->GetName().c_str());
        // Define Equipment
        uint32 shoulders = 0, chest = 0, trinket = 0, weapon = 0, weapon2 = 0,
               weapon3 = 0, shoulders2 = 0, chest2 = 0, trinket2 = 0;
        // const uint32 bag = 23162;		// Foror's Crate of Endless
        // Resist Gear Storage (36 Slot)
        const uint32 ring = 50255; // Dread Pirate Ring (5% XP Boost)

        // Outfit the character with bags and heirlooms that match their class
        // NOTE: Some classes have more than one heirloom option per slot
        switch (player->getClass()) {

        case CLASS_WARRIOR:
          shoulders = 42949;
          chest = 48685;
          trinket = 42991;
          weapon = 42943;
          weapon2 = 44092;
          weapon3 = 44093;
          break;

        case CLASS_PALADIN:
          shoulders = 42949;
          chest = 48685;
          trinket = 42991;
          weapon = 42945;
          weapon2 = 44092;
          break;

        case CLASS_HUNTER:
          shoulders = 42950;
          chest = 48677;
          trinket = 42991;
          weapon = 42943;
          weapon2 = 42946;
          weapon3 = 44093;
          break;

        case CLASS_ROGUE:
          shoulders = 42952;
          chest = 48689;
          trinket = 42991;
          weapon = 42944;
          weapon2 = 42944;
          break;

        case CLASS_PRIEST:
          shoulders = 42985;
          chest = 48691;
          trinket = 42992;
          weapon = 42947;
          break;

        case CLASS_DEATH_KNIGHT:
          shoulders = 42949;
          chest = 48685;
          trinket = 42991;
          weapon = 42945;
          weapon2 = 44092;
          weapon3 = 42943;
          break;

        case CLASS_SHAMAN:
          shoulders = 42951;
          chest = 48683;
          trinket = 42992;
          weapon = 42948;
          shoulders2 = 42951;
          chest2 = 48683;
          weapon2 = 42947;
          break;

        case CLASS_MAGE:
          shoulders = 42985;
          chest = 48691;
          trinket = 42992;
          weapon = 42947;
          break;

        case CLASS_WARLOCK:
          shoulders = 42985;
          chest = 48691;
          trinket = 42992;
          weapon = 42947;
          break;

        case CLASS_DRUID:
          shoulders = 42984;
          chest = 48687;
          trinket = 42992;
          weapon = 42948;
          shoulders2 = 42952;
          chest2 = 48689;
          trinket2 = 42991;
          weapon2 = 48718;
          break;

        default:
          break;
        }

        // Hand out the heirlooms. I prefer only the ring and trinkets for new
        // characters.
        switch (player->getClass()) {

        case CLASS_DEATH_KNIGHT:
          player->AddItem(trinket, 2);
          player->AddItem(ring, 1);
          // player->AddItem(shoulders, 1);
          // player->AddItem(chest, 1);
          // player->AddItem(weapon, 1);
          // player->AddItem(weapon2, 1);
          // player->AddItem(weapon3, 1);
          // player->AddItem(bag, 4);
          break;

        case CLASS_PALADIN:
          player->AddItem(trinket, 2);
          player->AddItem(ring, 1);
          // player->AddItem(shoulders, 1);
          // player->AddItem(chest, 1);
          // player->AddItem(weapon, 1);
          // player->AddItem(weapon2, 1);
          // player->AddItem(bag, 4);
          break;

        case CLASS_WARRIOR:
          player->AddItem(trinket, 2);
          player->AddItem(ring, 1);
          // player->AddItem(shoulders, 1);
          // player->AddItem(chest, 1);
          // player->AddItem(weapon, 1);
          // player->AddItem(weapon2, 1);
          // player->AddItem(weapon3, 1);
          // player->AddItem(bag, 4);
          break;

        case CLASS_HUNTER:
          player->AddItem(trinket, 2);
          player->AddItem(ring, 1);
          // player->AddItem(shoulders, 1);
          // player->AddItem(chest, 1);
          // player->AddItem(weapon, 1);
          // player->AddItem(weapon2, 1);
          // player->AddItem(weapon3, 1);
          // player->AddItem(bag, 4);
          break;

        case CLASS_ROGUE:
          player->AddItem(trinket, 2);
          player->AddItem(ring, 1);
          // player->AddItem(shoulders, 1);
          // player->AddItem(chest, 1);
          // player->AddItem(weapon, 1);
          // player->AddItem(weapon2, 1);
          // player->AddItem(bag, 4);
          break;

        case CLASS_DRUID:
          player->AddItem(trinket, 2);
          player->AddItem(trinket2, 2);
          player->AddItem(ring, 1);
          // player->AddItem(shoulders, 1);
          // player->AddItem(chest, 1);
          // player->AddItem(weapon, 1);
          // player->AddItem(shoulders2, 1);
          // player->AddItem(chest2, 1);
          // player->AddItem(weapon2, 1);
          // player->AddItem(bag, 4);
          break;

        case CLASS_SHAMAN:
          player->AddItem(trinket, 2);
          player->AddItem(ring, 1);
          // player->AddItem(shoulders, 1);
          // player->AddItem(chest, 1);
          // player->AddItem(weapon, 1);
          // player->AddItem(shoulders2, 1);
          // player->AddItem(chest2, 1);
          // player->AddItem(weapon2, 1);
          // player->AddItem(bag, 4);
          break;

        default:
          player->AddItem(trinket, 2);
          player->AddItem(ring, 1);
          // player->AddItem(shoulders, 1);
          // player->AddItem(chest, 1);
          // player->AddItem(weapon, 1);
          // player->AddItem(bag, 4);
          break;
        }

        // Inform the player they have new items
        std::ostringstream ss;
        ss << "|cffFF0000[CustomLogin]:|cffFF8000 The outfitter has placed "
              "Heirloom gear in your backpack.";
        ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
      }

      // If enabled, learn additional skills
      if (sConfigMgr->GetOption<bool>("CustomLogin.Skills", true)) {
        switch (player->getClass()) {

          /*
              // Skill Reference

              player->learnSpell(204);	// Defense
              player->learnSpell(264);	// Bows
              player->learnSpell(5011);	// Crossbow
              player->learnSpell(674);	// Dual Wield
              player->learnSpell(15590);	// Fists
              player->learnSpell(266);	// Guns
              player->learnSpell(196);	// Axes
              player->learnSpell(198);	// Maces
              player->learnSpell(201);	// Swords
              player->learnSpell(750);	// Plate Mail
              player->learnSpell(200);	// PoleArms
              player->learnSpell(9116);	// Shields
              player->learnSpell(197);	// 2H Axe
              player->learnSpell(199);	// 2H Mace
              player->learnSpell(202);	// 2H Sword
              player->learnSpell(227);	// Staves
              player->learnSpell(2567);	// Thrown
          */

        case CLASS_PALADIN:
          player->learnSpell(196); // Axes
          player->learnSpell(750); // Plate Mail
          player->learnSpell(200); // PoleArms
          player->learnSpell(197); // 2H Axe
          player->learnSpell(199); // 2H Mace
          break;

        case CLASS_SHAMAN:
          player->learnSpell(15590); // Fists
          player->learnSpell(8737);  // Mail
          player->learnSpell(196);   // Axes
          player->learnSpell(197);   // 2H Axe
          player->learnSpell(199);   // 2H Mace
          break;

        case CLASS_WARRIOR:
          player->learnSpell(264);   // Bows
          player->learnSpell(5011);  // Crossbow
          player->learnSpell(674);   // Dual Wield
          player->learnSpell(15590); // Fists
          player->learnSpell(266);   // Guns
          player->learnSpell(750);   // Plate Mail
          player->learnSpell(200);   // PoleArms
          player->learnSpell(199);   // 2H Mace
          player->learnSpell(227);   // Staves
          break;

        case CLASS_HUNTER:
          player->learnSpell(674);   // Dual Wield
          player->learnSpell(15590); // Fists
          player->learnSpell(266);   // Guns
          player->learnSpell(8737);  // Mail
          player->learnSpell(200);   // PoleArms
          player->learnSpell(227);   // Staves
          player->learnSpell(202);   // 2H Sword
          break;

        case CLASS_ROGUE:
          player->learnSpell(264);   // Bows
          player->learnSpell(5011);  // Crossbow
          player->learnSpell(15590); // Fists
          player->learnSpell(266);   // Guns
          player->learnSpell(196);   // Axes
          player->learnSpell(198);   // Maces
          player->learnSpell(201);   // Swords
          break;

        case CLASS_DRUID:
          player->learnSpell(1180);  // Daggers
          player->learnSpell(15590); // Fists
          player->learnSpell(198);   // Maces
          player->learnSpell(200);   // PoleArms
          player->learnSpell(227);   // Staves
          player->learnSpell(199);   // 2H Mace
          break;

        case CLASS_MAGE:
          player->learnSpell(201); // Swords
          break;

        case CLASS_WARLOCK:
          player->learnSpell(201); // Swords
          break;

        case CLASS_PRIEST:
          player->learnSpell(1180); // Daggers
          break;

        case CLASS_DEATH_KNIGHT:
          player->learnSpell(198); // Maces
          player->learnSpell(199); // 2H Mace
          break;

        default:
          break;
        }

        // Inform the player they have new skills
        std::ostringstream ss;
        ss << "|cffFF0000[CustomLogin]:|cffFF8000 You have been granted "
              "additional weapon skills.";
        ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
      }

      // If enabled.. learn special skills abilities
      if (sConfigMgr->GetOption<bool>("CustomLogin.SpecialAbility", true)) {
        // Learn Specialized Skills
        player->learnSpell(1784);  // Stealth
        player->learnSpell(921);   // Pick Pocket
        player->learnSpell(1804);  // Lockpicking
        player->learnSpell(11305); // Sprint (3)
        player->learnSpell(5384);  // Feign Death
        // player->learnSpell(475);	// Remove Curse

        // Add a few teleportation runes
        player->AddItem(17031, 5); // Rune of Teleportation

        // Learn Teleports
        switch (player->GetTeamId()) {

        case TEAM_ALLIANCE:

          // Alliance Teleports
          player->learnSpell(3565);  // Darnassus
          player->learnSpell(32271); // Exodar
          player->learnSpell(3562);  // Ironforge
          player->learnSpell(33690); // Shattrath
          player->learnSpell(3561);  // Stormwind
          break;

        case TEAM_HORDE:

          // Horde Teleports
          player->learnSpell(3567);  // Orgrimmar
          player->learnSpell(35715); // Shattrath
          player->learnSpell(32272); // Silvermoon
          player->learnSpell(3566);  // Thunder Bluff
          player->learnSpell(3563);  // Undercity
          break;

        default:
          break;
        }

        // Inform the player they have new skills
        std::ostringstream ss;
        ss << "|cffFF0000[CustomLogin]:|cffFF8000 Your spellbook has been "
              "scribed with special abilities.";
        ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
      }

      // If enabled.. set exalted factions (AzerothCore config for rep not
      // working as of 2017-08-25)
      if (sConfigMgr->GetOption<bool>("CustomLogin.Reputation", true)) {
        switch (player->GetTeamId()) {

          // Alliance Capital Cities
        case TEAM_ALLIANCE:
          player->SetReputation(47, 999999);  // IronForge
          player->SetReputation(72, 999999);  // Stormwind
          player->SetReputation(69, 999999);  // Darnassus
          player->SetReputation(389, 999999); // Gnomeregan
          player->SetReputation(930, 999999); // Exodar
          break;

          // Horde Capital Cities
        case TEAM_HORDE:
          player->SetReputation(68, 999999);  // Undercity
          player->SetReputation(76, 999999);  // Orgrimmar
          player->SetReputation(81, 999999);  // Thunder Bluff
          player->SetReputation(530, 999999); // DarkSpear
          player->SetReputation(911, 999999); // Silvermoon
          break;

        default:
          break;
        }

        // Inform the player they have exalted reputations
        std::ostringstream ss;
        ss << "|cffFF0000[CustomLogin]:|cffFF8000 Your are now Exalted with "
              "your faction's capital cities "
           << player->GetName() << ".";
        ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
      }
    }
  }
};

void AddSC_mod_customlogin() { new CustomLogin(); };

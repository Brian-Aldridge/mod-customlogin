/*

# Custom Login Modifications #

#### A module for AzerothCore by
[StygianTheBest](https://github.com/StygianTheBest/AzerothCore-Content/tree/master/Modules)
------------------------------------------------------------------------------------------------------------------


### Description ###
------------------------------------------------------------------------------------------------------------------
This module performs several actions on new players. It has the option to give
new players BOA starting gear, additional weapon skills, and special abilities
such as custom spells. It can also set the reputation of the player to exalted
with all capital cities for their faction granting them the Ambassador title.
This is typically done in the core's config file, but it's bugged (as of
2017.08.23) in AzerothCore. It can also announce when players login or logoff
the server.


### Features ###
------------------------------------------------------------------------------------------------------------------
- Player ([ Faction ] - Name - Logon/Logoff message) notification can be
announced to the world
- New characters can receive items, bags, and class-specific heirlooms
- New characters can receive additional weapon skills
- New characters can receive special abilities
- New characters can receive exalted rep with capital cities (Title: Ambassador)
on first login


### Data ###
------------------------------------------------------------------------------------------------------------------
- Type: Player/Server
- Script: CustomLogin
- Config: Yes
    - Enable Module
    - Enable Module Announce
    - Enable Announce Player Login/Logoff
    - Enable Starting Gear for new players
    - Enable Additional Weapon Skills for new players
    - Enable Special Abilities for new players
    - Enable Reputation Boost for new players
- SQL: No


### Version ###
------------------------------------------------------------------------------------------------------------------
- v2017.07.26 - Release
- v2017.07.29 - Clean up code, Add rep gain, Add config options


### Credits ###
------------------------------------------------------------------------------------------------------------------
- [Blizzard Entertainment](http://blizzard.com)
- [TrinityCore](https://github.com/TrinityCore/TrinityCore/blob/3.3.5/THANKS)
- [SunwellCore](http://www.azerothcore.org/pages/sunwell.pl/)
-
[AzerothCore](https://github.com/AzerothCore/azerothcore-wotlk/graphs/contributors)
- [AzerothCore Discord](https://discord.gg/gkt4y2x)
- [EMUDevs](https://youtube.com/user/EmuDevs)
- [AC-Web](http://ac-web.org/)
- [ModCraft.io](http://modcraft.io/)
- [OwnedCore](http://ownedcore.com/)
- [OregonCore](https://wiki.oregon-core.net/)
- [Wowhead.com](http://wowhead.com)
- [AoWoW](https://wotlk.evowow.com/)


### License ###
------------------------------------------------------------------------------------------------------------------
- This code and content is released under the [GNU AGPL
v3](https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3).

*/

#include "Chat.h"
#include "Config.h"
#include "GuildMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldSession.h"
#include <cstdio>
typedef std::unordered_map<uint32, WorldSession *> SessionMap;
extern SessionMap m_sessions;

class CustomLogin : public PlayerScript {

public:
  CustomLogin() : PlayerScript("CustomLogin") {}

  void OnFirstLogin(Player *player) {
    printf("[CustomLogin] OnFirstLogin called for %s\n",
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

  void OnLogin(Player *player) {
    printf("[CustomLogin] OnLogin called for %s\n", player->GetName().c_str());
    bool enable = sConfigMgr->GetOption<bool>("CustomLogin.Enable", true);
    bool announce = sConfigMgr->GetOption<bool>("CustomLogin.Announce", true);
    bool playerAnnounce =
        sConfigMgr->GetOption<bool>("CustomLogin.PlayerAnnounce", true);
    printf("[CustomLogin] Config: Enable=%d, Announce=%d, PlayerAnnounce=%d\n",
           enable, announce, playerAnnounce);

    if (enable) {
      if (announce) {
        printf("[CustomLogin] Announcing module to %s\n",
               player->GetName().c_str());
        ChatHandler(player->GetSession())
            .SendSysMessage(
                "This server is running the |cff4CFF00CustomLogin |rmodule.");
      }
      if (playerAnnounce) {
        printf("[CustomLogin] Announcing player login: %s\n",
               player->GetName().c_str());
        std::ostringstream ss;
        if (player->GetTeamId() == TEAM_ALLIANCE) {
          ss << "|cffFFFFFF[|cff2897FF Alliance |cffFFFFFF]:|cff4CFF00 "
             << player->GetName() << "|cffFFFFFF has come online.";
        } else {
          ss << "|cffFFFFFF[|cffFF0000 Horde |cffFFFFFF]:|cff4CFF00 "
             << player->GetName() << "|cffFFFFFF has come online.";
        }
        for (auto const &sessionPair : m_sessions) {
          if (Player *plr = sessionPair.second->GetPlayer()) {
            ChatHandler(plr->GetSession()).SendSysMessage(ss.str().c_str());
          }
        }
      }
    }
  }

  void OnLogout(Player *player) {
    printf("[CustomLogin] OnLogout called for %s\n", player->GetName().c_str());
    bool enable = sConfigMgr->GetOption<bool>("CustomLogin.Enable", true);
    bool playerAnnounce =
        sConfigMgr->GetOption<bool>("CustomLogin.PlayerAnnounce", true);
    printf("[CustomLogin] Config: Enable=%d, PlayerAnnounce=%d\n", enable,
           playerAnnounce);

    if (enable) {
      if (playerAnnounce) {
        printf("[CustomLogin] Announcing player logout: %s\n",
               player->GetName().c_str());
        std::ostringstream ss;
        if (player->GetTeamId() == TEAM_ALLIANCE) {
          ss << "|cffFFFFFF[|cff2897FF Alliance |cffFFFFFF]|cff4CFF00 "
             << player->GetName() << "|cffFFFFFF has left the game.";
        } else {
          ss << "|cffFFFFFF[|cffFF0000 Horde |cffFFFFFF]|cff4CFF00 "
             << player->GetName() << "|cffFFFFFF has left the game.";
        }
        for (auto const &sessionPair : m_sessions) {
          if (Player *plr = sessionPair.second->GetPlayer()) {
            ChatHandler(plr->GetSession()).SendSysMessage(ss.str().c_str());
          }
        }
      }
    }
  }
};

void AddCustomLoginScripts() { new CustomLogin(); }
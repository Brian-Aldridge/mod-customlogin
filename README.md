<!-- @format -->

# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## mod-customlogin

- Latest build status with AzerothCore: [![core-build](https://github.com/Brian-Aldridge/mod-customlogin/actions/workflows/core-build.yml/badge.svg?branch=master)](https://github.com/Brian-Aldridge/mod-customlogin/actions/workflows/core-build.yml)

---

### **Description**

`mod-customlogin` is a highly configurable module for AzerothCore that provides custom login rewards, announcements, and features for new characters. It allows server administrators to customize items, abilities, reputations, and more through a configuration file without modifying the source code.

This module is compatible with Playerbot and other forks that do not expose public session accessors.

---

### **Features**

#### **1. Login/Logout Announcements**

- Sends a custom message to the logging-in or logging-out player (not globally).
- Configurable via `CustomLogin.PlayerAnnounce`.

#### **2. First Login Rewards**

- Grants Bind-on-Account (BoA) starting gear (heirlooms, trinkets, and weapons) to new characters.
- Items are class-specific and fully configurable via the `.conf` file.

#### **3. Additional Weapon Skills**

- Grants additional weapon skills to new characters based on their class.
- Configurable via `CustomLogin.Skills`.

#### **4. Special Abilities**

- Grants special abilities, spells, titles, and mounts to new characters.
- Fully configurable via the `.conf` file.

#### **5. Reputation Configuration**

- Sets custom reputation levels for new characters with various factions.
- Supports all major factions, including Alliance, Horde, Neutral, Burning Crusade, and Wrath of the Lich King factions.
- Reputation levels are configurable (e.g., Neutral, Friendly, Honored, Revered, Exalted).

#### **6. Fully Configurable**

- All features are controlled via the `mod_customlogin.conf` file.
- No need to modify the source code to adjust items, abilities, or reputations.

---

### **Configuration**

All options are controlled via the `mod_customlogin.conf` file located in the `configs` directory. Below is a summary of the available options:

#### **General Settings**

- `CustomLogin.Enable` — Enable or disable the module.
- `CustomLogin.Announce` — Announce the module to the player on login.
- `CustomLogin.PlayerAnnounce` — Send a login/logout message to the player.

#### **First Login Rewards**

- `CustomLogin.BoA` — Give new characters BoA items.
- `CustomLogin.Skills` — Give new characters additional weapon skills.

#### **Starting Mount**

- `CustomLogin.StartingMount.Enable` — Enable or disable granting a starting mount and riding skill.
- `CustomLogin.StartingMount.Spell` — Spell ID for the mount to grant (e.g., 458 = Brown Horse).
- `CustomLogin.StartingMount.Skill` — Spell ID for the riding skill to grant (e.g., 33388 = Apprentice Riding).

#### **Starting Professions**

Automatically teach one or more professions (e.g., First Aid, Cooking, Mining, Herbalism) to new characters on their first login.

- `CustomLogin.StartingProfessions.Enable` — Enable or disable granting starting professions.
- `CustomLogin.StartingProfessions.List` — Comma-separated list of profession spell IDs to teach (e.g., 129,185,186 for First Aid, Cooking, Mining).

**Example profession spell IDs:**

| Profession     | Spell ID |
| -------------- | -------- |
| First Aid      | 129      |
| Cooking        | 185      |
| Mining         | 186      |
| Tailoring      | 197      |
| Alchemy        | 171      |
| Herbalism      | 182      |
| Engineering    | 202      |
| Enchanting     | 333      |
| Fishing        | 356      |
| Skinning       | 393      |
| Jewelcrafting  | 755      |
| Blacksmithing  | 164      |
| Leatherworking | 165      |

Set the spell IDs you want in the config file. Example:

```
CustomLogin.StartingProfessions.Enable = 1
CustomLogin.StartingProfessions.List = 129,185,186
```

#### **Starting Professions**

- `CustomLogin.StartingProfessions.Enable` — Enable or disable granting starting professions.
- `CustomLogin.StartingProfessions.List` — Comma-separated list of profession spell IDs to teach (e.g., 129,185,186 for First Aid, Cooking, Mining).

#### **Special Abilities**

- `CustomLogin.SpecialAbility` — Enable or disable special abilities.
- `CustomLogin.SpecialAbility.Spell1` — Spell ID for the first custom spell.
- `CustomLogin.SpecialAbility.Spell2` — Spell ID for the second custom spell.
- `CustomLogin.SpecialAbility.Title` — Title ID for the custom title.
- `CustomLogin.SpecialAbility.Mount` — Spell ID for the custom mount.

#### **Reputation Configuration**

- `CustomLogin.Reputation` — Enable or disable reputation configuration.
- Reputation levels are defined for each faction. Example:
  - `CustomLogin.Reputation.Stormwind = 42000` (Exalted)
  - `CustomLogin.Reputation.Orgrimmar = 42000` (Exalted)
  - `CustomLogin.Reputation.ArgentDawn = 9000` (Honored)

#### **Item Configuration**

- Items are defined per class. Example:
  - `CustomLogin.Warrior.Shoulders = 42949`
  - `CustomLogin.Paladin.Chest = 48685`
  - `CustomLogin.Hunter.Weapon1 = 42943`

---

### **Installation**

1. Clone or copy this module into your `modules` directory:
   ```bash
   git clone https://github.com/Brian-Aldridge/mod-customlogin.git modules/mod-customlogin
   ```
2. Add the config file to your `configs` directory.
3. Rebuild your AzerothCore server.
4. Restart your worldserver.

---

**For questions or support, visit the AzerothCore Discord or GitHub discussions.**

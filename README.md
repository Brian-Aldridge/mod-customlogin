<!-- @format -->

# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## mod-customlogin

- Latest build status with AzerothCore: [![core-build](https://github.com/Brian-Aldridge/mod-customlogin/actions/workflows/core-build.yml/badge.svg?branch=master)](https://github.com/Brian-Aldridge/mod-customlogin/actions/workflows/core-build.yml)

---

### **Description**

`mod-customlogin` is a highly configurable module for AzerothCore that provides custom login rewards, announcements, and features for new characters. It lets server administrators customize items, abilities, reputations, bags, mounts, professions, and more via configuration—no source edits needed.

Compatibility: built and tested for AzerothCore WotLK (3.3.5). All default examples use WotLK-valid IDs. Playerbot and forks without public session accessors are supported.

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

Configuration:

Set `CustomLogin.Skills = 1` to enable. Then either set a global list of spell IDs:

```ini
CustomLogin.Skills.List = 674,1764
```

Or class-specific overrides:

```ini
CustomLogin.Skills.Warrior = 674,1764
CustomLogin.Skills.Hunter = 674,533
```

Spell IDs should be the spell/ability IDs to teach the player (the same IDs used in `CustomLogin.SpecialAbility.Spell1`), not internal skill type IDs.

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

#### **Starting Gold**

Automatically grant starting currency to new characters on first login.

- `CustomLogin.StartingGold.Enable` — Enable/disable starting money.
- `CustomLogin.StartingGold.Gold` — Amount of gold (>= 0).
- `CustomLogin.StartingGold.Silver` — Amount of silver (>= 0).
- `CustomLogin.StartingGold.Copper` — Amount of copper (>= 0).

Example:

```ini
CustomLogin.StartingGold.Enable = 1
CustomLogin.StartingGold.Gold = 2
CustomLogin.StartingGold.Silver = 50
CustomLogin.StartingGold.Copper = 75
```

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

```ini
CustomLogin.StartingProfessions.Enable = 1
CustomLogin.StartingProfessions.List = 129,185,186
```

#### **Starting Bags**

Equip up to four starting bags and optionally one class-specific bag (consumes one of the four slots if enabled).

- `CustomLogin.Bags.Default` — Comma-separated list of up to 4 bag item IDs. These fill bag slots 19–22 in order.
- Class-specific overrides (set to `0` to disable):
  - `CustomLogin.Bags.Hunter` — Quiver/Ammo Pouch ID (e.g., 8217, 8218, 7371, 7372)
  - `CustomLogin.Bags.Warlock` — Shard bag ID (e.g., 21342, 21341, 21340, 22243)
  - `CustomLogin.Bags.Rogue` — No class bag in WotLK; keep 0

Behavior notes:

- Defaults equip first; any class bag (if non-zero) is placed into the first empty slot among 19–22 and will consume one of the four.
- Fewer than 4 defaults are fine; remaining slots stay empty unless a class bag is enabled.
- Invalid/missing IDs are skipped (slot remains empty) and can be taken by a class bag if enabled.
- Use WotLK 3.3.5 item IDs (e.g., 41599 Frostweave 20-slot, 21841 Netherweave 16-slot). Cataclysm+ bags don’t exist in stock WotLK DBs.

Examples:

```ini
# Four Netherweave bags
CustomLogin.Bags.Default = 21841,21841,21841,21841

# Hunters: give a Quickdraw Quiver (14-slot) in the first empty bag slot
CustomLogin.Bags.Hunter = 8217
```

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

> Tip: the config file (`conf/mod_customlogin.conf.dist`) includes a quick-reference table of common WotLK bag IDs and class bags, and a reminder to verify against your DB.

---

### **Installation**

1. Clone or copy this module into your `modules` directory:

```bash
git clone https://github.com/Brian-Aldridge/mod-customlogin.git modules/mod-customlogin
```

1. Copy `conf/mod_customlogin.conf.dist` into your AzerothCore `configs` folder as `mod_customlogin.conf`, then edit to taste.
1. Rebuild AzerothCore and restart `worldserver`.

Validation tips:

- To verify bag IDs exist in your DB, consult `data/sql/base/db_world/item_template.sql` or query your DB: `SELECT entry, name FROM item_template WHERE entry IN (...);`

---

**For questions or support, visit the AzerothCore Discord or GitHub discussions.**

# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore
## mod-customlogin

- Latest build status with AzerothCore: [![Build Status](https://github.com/azerothcore/mod-customlogin/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-customlogin)

### Description

This module provides custom login rewards and announcements for new characters on your AzerothCore server.  
It is compatible with Playerbot and other forks that do not expose public session accessors.

**Key features:**
- Announces to the logging-in or logging-out player (not globally) when they enter or leave the world.
- Optionally gives new characters Bind-on-Account (BoA) starting gear (heirlooms and trinkets).
- Optionally grants additional weapon skills based on class.
- Optionally grants special abilities and custom spells.
- Optionally sets the reputation of the player to exalted with all capital cities for their faction, granting the Ambassador title (useful if the core config for rep is bugged).
- All features are configurable via `mod_customlogin.conf`.

### Features
------------------------------------------------------------------------------------------------------------------
- **Login/Logout Announcements:**  
  The player receives a custom message when they log in or out.
- **First Login Rewards:**  
  New characters can receive class-appropriate heirlooms, trinkets, and bags.
- **Additional Weapon Skills:**  
  New characters can receive extra weapon skills based on their class.
- **Special Abilities:**  
  New characters can receive special spells or abilities.
- **Exalted Reputation:**  
  New characters can start with exalted reputation for their faction's capital cities (Ambassador title).

### Configuration

All options are controlled via `mod_customlogin.conf`:

- `CustomLogin.Enable` — Enable or disable the module.
- `CustomLogin.Announce` — Announce the module to the player on login.
- `CustomLogin.PlayerAnnounce` — Send a login/logout message to the player.
- `CustomLogin.BoA` — Give new characters BoA items.
- `CustomLogin.Skills` — Give new characters additional weapon skills.
- `CustomLogin.SpecialAbility` — Give new characters special abilities.
- `CustomLogin.Reputation` — Set new characters to exalted with their faction's capital cities.

### Notes

- **This module only sends announcements to the logging-in/logging-out player, not globally to all players.**  
  This ensures compatibility with all AzerothCore forks, including those without public session accessors.
- Place your configuration file in the `configs` directory and ensure it is named `mod_customlogin.conf`.

### Installation

1. Clone or copy this module into your `modules` directory.
2. Add the config file to your `configs` directory.
3. Rebuild your AzerothCore server.
4. Restart your worldserver.

---

**For questions or support, visit the AzerothCore Discord or GitHub discussions.**

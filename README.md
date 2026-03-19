# GhostX – External CS2 Cheat

**Fully external, read-only Counter-Strike 2 cheat** focused on safety, customization, and visual clarity.

**GhostX** is an **external** overlay-based cheat for CS2 that **never writes** to game memory — it only reads process memory. This design choice significantly reduces detection vectors compared to internal cheats.

> ⚠️ **Educational / research use only.**  
> Using cheats in matchmaking or VAC-secured is against Valve conditions. The author is **not responsible** for any bans, losses, or consequences from misuse.

## Features

### Aimbot
- **Customizable per weapon** — each weapon has completely independent settings
- Adjustable **FOV**, **Smoothing**, and **Switch Delay** for natural, human-like aiming behavior
- **Hitbox selection**: Head, Chest, Stomach, Legs
- **Triggerbot** with:
  - Hitchance
  - Delay
  - Autostop
  - Predictive shot timing
- **Visible Only** filter to control when the aimbot / triggerbot fires

### Visuals (ESP)
- **Box ESP** — multiple styles:
  - Cornered
  - Filled
  - Outlined
  - Separate **visible** and **occluded** color support
- **Skeleton** rendering with adjustable thickness + visible-only option
- **Health Bar**:
  - Gradient support
  - Outline
  - Numeric value display
- **Name & Weapon** display:
  - Icon mode or text mode
- **Snaplines** — dynamic lines to track enemies anywhere on screen
- **Items ESP**:
  - Maximum distance control
  - Icon + name + ammo information

### Misc
- **Grenade Prediction**:
  - Trajectory line with bounce points
  - Gradient line support
  - Adjustable thickness
  - Customizable **detonate** and **bounce** circle sizes for precise reads
- **Custom Colors** system — full per-type color personalization for every visual element

### Why GhostX is considered safer than most cheats
- **100% external** — no memory writes, only memory reads
- Every feature is **tested against live VAC** before public release
- Risky options (e.g. extremely low smoothing values) are clearly **flagged with warnings**
- Continuously updated attachment/detection bypass logic to maintain long-term undetectability when hooking to the CS2 process

## Controls

| Key       | Action                  |
|-----------|-------------------------|
| **Insert**| Open / close menu      |
| **Settings logo**  | (in menu) – click to unload / safely detach the cheat |

## Screenshots

https://github.com/SKEETZEN0X/GhostX-CS2-External/blob/76987cf7b84c063412be1b16855f8a46f57d6acd/ghostx/project/resources/aimbot.png
https://github.com/SKEETZEN0X/GhostX-CS2-External/blob/76987cf7b84c063412be1b16855f8a46f57d6acd/ghostx/project/resources/config.png
https://github.com/SKEETZEN0X/GhostX-CS2-External/blob/76987cf7b84c063412be1b16855f8a46f57d6acd/ghostx/project/resources/misc.png
https://github.com/SKEETZEN0X/GhostX-CS2-External/blob/76987cf7b84c063412be1b16855f8a46f57d6acd/ghostx/project/resources/visuals.png

<!-- Example:
![Menu](screenshots/menu.png)
![Aimbot in action](screenshots/aimbot.jpg)
![ESP + grenade prediction](screenshots/esp-nade.png)
-->

## Disclaimer & Legal Notice

**This software is provided for educational and research purposes only.**

- It is **against Valve's Terms of Service** to use any form of cheat/hack in Counter-Strike 2.
- Using this cheat **will** result in VAC ban and/or game ban.
- The developer assumes **no responsibility** for any bans, account losses, hardware bans, monetary losses, or any other consequences.
- Do **not** use on your main / valuable account.

## Contributing

Contributions are welcome for:
- Bug fixes
- Offset / signature updates
- Quality-of-life improvements
- Better documentation

**No** feature requests that increase detection risk will be accepted.

## License

This project is released under the **MIT License** — but remember the legal disclaimer above still applies.

---

Made with ❤️. Made so everyone can add their features or modify de program
Stay safe. Stay external.

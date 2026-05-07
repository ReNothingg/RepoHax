# RepoHax

<img width="1338" height="691" alt="Screenshot 2026-04-19 021713 (1)" src="https://github.com/user-attachments/assets/aa2fec40-b32f-4616-b4df-26789feb9843" />

### About
**RepoHax** is an advanced internal C++ cheat for the game **REPO**, developed with minimal reliance on third-party libraries. The project features custom graphics library and SDK for interacting with the Unity engine directly from C++ (both will be released as standalone open-source projects in the future).

> [!IMPORTANT]
> This project was created for educational purposes as part of a university project. It aims to provide developers with deeper insights into game hacking / mod development aspects.

---

### Official Sources
> [!CAUTION]
> To ensure your security, only download the mod from these official sources:
> 1. [This GitHub Repository] https://github.com/Sacracia/RepoHax
> 2. [Nexus Mods] https://www.nexusmods.com/repo/mods/214
> 3. [Playground] https://users.playground.ru/3241576/posts

---

### Features

**Health**
- God Mode
- Heal to Maximum

**Movement**
- Infinite Stamina
- Modify Walk / Crouch / Sprint Speed
- Infinite Jumps
- No tumble (fall only by pressing hotkey)

**Grabbing**
- High Strength (Host / Single-player only)
- Unlimited Grab Range
- No Overload

**Upgrades**
- Apply purchased upgrades (or shop upgrades) to self

**Medkits**
- Apply purchased medkit to self

**Enemies**
- Spawn (Host / Single-player only)
- Blind (Host / Single-player only, enemies can still hear)
- No Hold Time Limit (Host / Single-player only)
- Display through walls (ESP)

**Players**
- Display through walls (ESP)
- Chams (Highlight through walls)
- Tumble selected player
- Kill / Revive selected player (Host / Single-player only)
- Toggle Voice Chat: Alive / Dead

**Vision**
- Enhanced Vision
- Third Person
- Field of View (FOV)
- Infinite Dead Head Charge

**Flashlight**
- Light Intensity
- Lighting Angle
- Works while crawling

**Valuables**
- Display through walls (ESP)
- Chams (Highlight through walls)
- No Damage (Host / Single-player only)
- Set all prices to Zero / Maximum

**Collection Points**
- Display through walls (ESP)
- Activate next point

**Weapons**
- Maximum Battery while holding
- Laser Sight

**Items**
- Spawn (Host / Single-player only)

**Level**
- Modify completed levels count (Host / Single-player only)
- Skip to next level (Host / Single-player only)

**Map Filter**
- Remove maps from rotation (Host / Single-player only)

**Truck**
- Display through walls (ESP)
- Teleport to truck

---

### Configuration
The menu hotkey and interface language can be adjusted in the **Settings** tab.

---

### Installation

For convenience and to avoid using external injectors, this mod utilizes **DLL Proxying**:

1. Download the `version.dll` file.
2. Place it in the game's root directory (the folder containing the `.exe` file).
3. Launch the game. The mod will load automatically and the menu will appear.

*If you prefer using a **DLL Injector**, simply rename dll file to something else and proceed with your preferred injector.*

---

## ❗ Important Note
Using external mod loaders (such as **BepInEx** or **MelonLoader**) may cause conflicts. RepoHax uses its own hooking system, which may be overwritten by other tools. It is recommended to use this on a clean game installation.

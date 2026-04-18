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
> 2. [Nexus Mods] **TBD**
> 3. [Playground] https://users.playground.ru/3241576/posts

---

### Features

**Health**
- God Mode
- Max Heal

**Movement**
- Infinite Stamina
- Walk / Crawl / Sprint Speed Modifier
- Infinite Jump
- No Fall (fall via hotkey only)

**Grabbing & Strength**
- High Strength (Host / Solo only)
- Infinite Grab Range
- No Overload

**Upgrades & Medkits**
- Apply Shop Upgrades
- Remote Medkit Usage

**Enemies**
- Enemy Spawner (Host / Solo only)
- Blind Enemies (Host / Solo only)
- No Hold Limit (Host / Solo only)
- Enemy ESP

**Players**
- Player ESP & Chams
- Trip Selected Player
- Kill / Revive Player (Host / Solo only)
- Voice Chat Filter (Live / Dead)

**Visuals**
- Enhanced Vision
- Third Person Mode
- FOV Changer
- Infinite Dead Head Charge

**Flashlight**
- Light Intensity & Angle
- Flashlight while Crawling

**Valuables**
- Scrap ESP & Chams
- Invincible Items (Host / Solo only)
- Price Override (Zero / Max)

**Collection Points**
- Point ESP
- Activate Next Point

**Equipment**
- Infinite Battery
- Laser Sight
- Item Spawner (Host / Solo only)

**Level & World**
- Level Manager (Host / Solo only)
- Map Filter (Host / Solo only)
- Van ESP & Teleport

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

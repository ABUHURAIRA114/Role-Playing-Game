# ⚔️ Role-Playing Game

> A 2.5D role-playing game built in C++ using [Raylib](https://www.raylib.com/), developed as a CS112 semester project.

![preview](preview.jpg)

---

## 🗺️ Overview

This is a third-person 2.5D RPG set in a fantasy world. The player explores a city populated with civilians, hostile enemies, merchants, and a final boss — each with unique behaviour and dialogue. 

Click [here](https://abuhuraira.itch.io/the-kalled-city-of-waloon) to download the game.

---

## ✨ Features

- **Dual-mode architecture** — switch between `GAME` mode and a free-camera `EDITOR` mode at runtime (`Ctrl + E + Alt`)
- **Character system** — full inheritance hierarchy: `Character → NPC / Player`, with `PossessedNPC`, `Civilian`, `Merchant`, and `Boss` subclasses
- **Dialogue trees** — branching dialogue with the Merchant (buy/sell) and the Boss (fight / join / leave, each leading to a different ending)
- **Combat** — player attacks, enemy AI with aggro/chase/attack/return logic, knockback, and hurt states
- **Inventory & potions** — two inventory slots, seven potion types (health, stamina, strength), usable mid-combat
- **Merchant trading** — buy and sell items; prices scale with the player's Charisma stat
- **Stat selection screen** — distribute 10 points across Strength, Armour, and Charisma before the game starts
- **Save / load system** — player state, enemy positions and health, and scene layout are all serialised to the `saves/` folder
- **Billboard sprite rendering** — characters are depth-sorted and rendered as 2D sprites in 3D space with directional animations
- **Background music** — looping soundtrack with `Ctrl+P` to skip tracks
- **Level editor** — place `Box` objects and spawn points (enemy / civilian / merchant / boss) with mouse selection, duplication, and transform controls

---

## 🏗️ Project Structure

```
Role-Playing-Game/
├── src/
│   ├── main.cpp            # Entry point, game loop
│   ├── HeaderAndProto.h    # All includes, enums, forward declarations
│   ├── ClassDef.h          # Full class definitions
│   ├── FuncDef.h           # Function implementations
│   └── CustomExceptions.h  # out_of_space, empty_collection, failed_execution
├── assets/
│   ├── models/             # 3D .obj models
│   ├── textures/           # Model textures
│   ├── sprites/            # Character animation sprite sheets
│   └── audio/              # Background music tracks
├── lib/                    # Raylib static library & headers
├── saves/                  # Auto-generated save files
└── Makefile
```

---

## 🎮 Controls

### In-Game
| Key | Action |
|-----|--------|
| `W A S D` / Arrow keys | Move |
| Mouse | Look around |
| `Space` | Jump |
| `Left Shift` | Sprint |
| `Left Ctrl` / `LMB` | Attack |
| `E` | Interact with NPC / close dialogue |
| `1` `2` | Use inventory slot 1 / 2 |
| `Escape` | Pause menu |
| `Ctrl + P` | Skip music track |

### Editor Mode (`Ctrl + Alt + E` in-game to enter, `Escape` to exit)
| Input | Action |
|-------|--------|
| `RMB` (hold) | Free-look camera |
| `Scroll` | Adjust camera/move speed |
| `LMB` | Select object |
| `W A S D Q E` | Move selected object |
| `Ctrl + C` | Duplicate selected object |
| UI buttons (top-right) | Spawn enemy / civilian / merchant / boss spawner, or collider |

---

## 🔧 Building

### Prerequisites
- **Windows** — [w64devkit](https://github.com/skeeto/w64devkit) (bundled `g++` path is `C:/raylib/w64devkit/bin`)
- **Linux** — `g++`, `make`, and standard Raylib system dependencies (`libGL`, `libX11`, etc.)
- **Raylib 5.0.0** — headers and static library expected in `lib/`

### Build & Run

**Windows (MinGW / w64devkit):**
```bat
mingw32-make
main.exe
```

**Linux:**
```bash
make PLATFORM=PLATFORM_DESKTOP
./game
```

**Debug build:**
```bash
make BUILD_MODE=DEBUG
```

**Clean:**
```bash
make clean
```

> The Makefile compiles `main.cpp` with `-std=c++14` and links against `raylib`, `opengl32`, `gdi32`, and `winmm` on Windows, or `raylib`, `GL`, `pthread`, `dl`, `rt`, and `X11` on Linux.

---

## 🧱 OOP Design Highlights

| Concept | Where it appears |
|---------|-----------------|
| **Encapsulation** | `Item`, `CameraMI`, `TransformMI` — private data with public accessors |
| **Inheritance** | `TransformMI → Box`, `Character → NPC → PossessedNPC / Merchant / Boss / Civilian` |
| **Abstract classes** | `I_Dialogueable` — pure virtual `DialogueSetup()` and `Dialogue()` |
| **Multiple inheritance** | `Merchant` and `Boss` extend both `NPC` and `I_Dialogueable` |
| **Polymorphism** | `NPC*` arrays, `dynamic_cast` for UI buttons, virtual `Update()` / `TakeDamage()` / `DrawCharacter()` |
| **Static singleton** | `GlobalInfo::instance` — single source of truth for all game state |
| **Friend functions** | `GlobalInfo::UnloadThings()` accesses private members of `PossessedNPC` and `Merchant` |
| **Dynamic memory** | `new` / `delete[]` throughout `Scene` for objects, colliders, UI, and NPCs |
| **Custom exceptions** | `out_of_space`, `empty_collection`, `failed_execution` with `what()` |

---

## 👥 Contributors

- **Abu Huraira** — [@ABUHURAIRA114](https://github.com/ABUHURAIRA114)

---

## 📄 License

This project was made for academic purposes (CS112 semester project). The Makefile is based on the [Raylib Makefile template](https://github.com/raysan5/raylib) by Ramon Santamaria, used under its original license terms.

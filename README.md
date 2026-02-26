# Captain Tsubasa II - Super Strike
## HD Remake for PlayStation 2

> Fan-made remake ທີ່ໄດ້ຮັບແຮງບັນດານໃຈຈາກ Captain Tsubasa II: Super Strike (NES, Konami)

---

## 📁 ໂຄງສ້າງ Files

```
captain_tsubasa/
├── src/
│   ├── main.c                  ← Entry point
│   ├── types.h                 ← Type definitions ທັງໝົດ
│   ├── game/
│   │   ├── engine.h/c          ← Main game engine + state machine
│   │   ├── player.h/c          ← ຂໍ້ມູນນັກເຕະ + ທີມ ທັງໝົດ
│   │   ├── battle.h/c          ← Turn-based battle system
│   │   └── match.h/c           ← ລະບົບ match + halftime + fulltime
│   ├── graphics/
│   │   └── renderer.h/c        ← PS2 GS renderer + UI ທັງໝົດ
│   └── input/
│       └── input.h/c           ← PS2 controller handler
├── assets/
│   ├── sprites/                ← ໃສ່ sprite PNG ທີ່ນີ້
│   ├── maps/                   ← ໃສ່ background PNG ທີ່ນີ້
│   └── audio/                  ← ໃສ່ BGM/SFX ທີ່ນີ້
├── Makefile                    ← Build system
├── SYSTEM.CNF                  ← PS2 boot config
├── build.sh                    ← Full build script
└── README.md                   ← ເຈົ້າກໍາລັງອ່ານ file ນີ້
```

---

## 🎮 Gameplay ທີ່ມີ

### ທີມທີ່ເລືອກໄດ້
- 🇯🇵 **Japan** (Tsubasa, Hyuga, Misaki, Wakabayashi...)
- 🇩🇪 **Germany** (Schneider, Kaltz, Levin...)
- 🇦🇷 **Argentina** (Diaz...)

### ລະບົບ Battle (Turn-Based)
| Action | ລາຍລະອຽດ |
|--------|---------|
| **Shoot** | ຍິງປະທະ GK — atk_roll vs def_roll |
| **Pass** | ສົ່ງໃຫ້ teammate |
| **Dribble** | ຜ່ານ DF |
| **Special Shot** | ໃຊ້ Stamina — Drive Shot, Tiger Shot, Fire Shot... |

### ລະບົບ Stamina
- ທຸກ Special Shot ໃຊ້ Stamina
- ເມື່ອ Stamina ໜ້ອຍ = power ຫຼຸດ
- ພັກເຄິ່ງ (Halftime) ຟື້ນ Stamina ບາງສ່ວນ

### ທ່າ Special ທັງໝົດ

| ນັກເຕະ | Special Shot | Power |
|--------|-------------|-------|
| Tsubasa | Drive Shot | 88 |
| Tsubasa | Sky Drive Shot | 95 |
| Tsubasa | Twin Shot | 92 |
| Hyuga | Tiger Shot | 95 |
| Hyuga | Neo Tiger Shot | 98 |
| Hyuga | Atomic Shot | 99 |
| Misaki | Eagle Shot | 90 |
| Misaki | Twin Shot | 92 |
| Schneider | Fire Shot | 96 |
| Schneider | Neo Fire Shot | 99 |
| Diaz | Overhead Kick | 93 |
| Diaz | Super Overhead | 97 |
| Wakabayashi | God Hand Catch | 90 |

---

## 🏗️ ວິທີ Build

### ຕ້ອງການ
- Ubuntu 20.04+ (ຫຼື WSL2 ໃນ Windows)
- Git, GCC, Make, CMake
- PS2DEV toolchain
- genisoimage (mkisofs)
- PCSX2 emulator (ສໍາລັບ test)

### Build ແບບໄວ
```bash
chmod +x build.sh
./build.sh
```

Script ຈະ:
1. ຕິດຕັ້ງ dependencies
2. Build PS2DEV toolchain
3. Compile ໂຕເກມ → `.elf`
4. Pack ເປັນ `.iso`

### Build Manual
```bash
# ຕັ້ງ environment
export PS2DEV=/usr/local/ps2dev
export PS2SDK=$PS2DEV/ps2sdk
export PATH=$PATH:$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin

# Compile
make

# Build ISO
make iso
```

---

## 🎮 ວິທີຫຼິ້ນ

### PCSX2 (PC Emulator)
1. ໂຫຼດ [PCSX2](https://pcsx2.net/) 1.7+
2. File → Boot ISO
3. ເລືອກ `CaptainTsubasa2.iso`
4. ຕ້ອງມີ PS2 BIOS (scph10000.bin ຫຼື SCPH-70012)

### PS2 ຈິງ (FreeMCBoot)
1. ຕ້ອງໄດ້ FreeMCBoot memory card
2. burn ISO ລົງ DVD-R
3. ໃສ່ disc ແລ້ວ boot ຜ່ານ uLaunchELF

---

## 🕹️ Controls

| ປຸ່ມ | Action |
|------|--------|
| ↑↓ | ເລືອກ menu |
| ❌ X | ຢືນຢັນ |
| ⭕ O | ຍ້ອນກັບ |
| START | ເລີ່ມ / Pause |

---

## 🔧 ວິທີເພີ່ມ Sprites (2D HD)

1. ວາດ sprite ດ້ວຍ **Aseprite** (ຂະໜາດ 64x64 ຫຼື 128x128)
2. Export ເປັນ PNG
3. ວາງໃສ່ `assets/sprites/`
4. ໃຊ້ `ps2stuff` ຫຼື `gskit` ເພື່ອ convert → TIM2 (tm2)
5. ໂຫຼດໃນ `renderer.c` ດ້ວຍ `gsKit_texture_send()`

---

## 🗺️ Roadmap ຕໍ່ໄປ

- [ ] ໂຫຼດ sprite 2D HD ຈິງ (TIM2 format)
- [ ] BGM system (ADPCM .adp)
- [ ] World Cup Tournament mode
- [ ] Save/Load (memory card)
- [ ] ເພີ່ມທີມ (Italy, France, Brazil...)
- [ ] Cutscene system
- [ ] Formation editor

---

## ⚠️ Legal Notice

ໂປຣເຈັກນີ້ເປັນ fan-made ທີ່ບໍ່ຫວັງຜົນກໍາໄລ.
Captain Tsubasa II ເປັນ trademark ຂອງ Bandai Namco / Konami.
ບໍ່ໄດ້ໃຊ້ assets ໃດຂອງ original game.

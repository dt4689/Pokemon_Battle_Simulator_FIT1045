# Pokemon Battle Simulator

A turn-based Pokemon battle game built with C++ and SplashKit, featuring user authentication, leaderboards, and strategic combat.

## Features

- 🔐 User login and registration system
- 🎮 3v3 turn-based Pokemon battles with strategic switching
- 🖥️ HD 1150×780 game window with scaled sprites/UI
- 📊 Leaderboard with statistics
- 🎨 Sprite-based graphics
- 🎵 Background music
- 📈 Win/loss tracking and streaks
- 🎯 Type effectiveness system

## Requirements

- C++ compiler (GCC, Clang, or MSVC)
- SplashKit SDK installed
- macOS, Linux, or Windows

## Building the Project

### Using SplashKit Manager (Recommended)

1. Install SplashKit if not already installed:
   ```bash
   # Follow SplashKit installation instructions for your platform
   ```

2. Compile the project:
   ```bash
   skm clang++ -o H3 H3.cpp
   # or
   skm g++ -o H3 H3.cpp
   ```

### Manual Compilation

If SplashKit is installed but not in PATH:

```bash
# macOS/Linux
clang++ -o H3 H3.cpp -I/path/to/splashkit/include -L/path/to/splashkit/lib -lsplashkit

# Windows (adjust paths accordingly)
g++ -o H3.exe H3.cpp -I"C:\path\to\splashkit\include" -L"C:\path\to\splashkit\lib" -lsplashkit
```

## Running the Game

```bash
./H3
# or on Windows
H3.exe
```

## Project Structure

```
Project_Pokemon/
├── H3.cpp              # Main game file (single-file implementation)
├── fighter.h            # Fighter class header (legacy, not used in H3.cpp)
├── userdata.txt        # User database (CSV format)
├── sprites/            # Pokemon sprite images
│   ├── usercharizard.png
│   ├── userblastoise.png
│   ├── uservenusaur.png
│   ├── enemycharizard.png
│   ├── enemyblastoise.png
│   └── enemyvenusaur.png
├── backgrounds/        # Battle background images
│   ├── bg1.png
│   ├── bg2.png
│   ├── bg3.png
│   ├── bg4.png
│   └── bg5.png
└── music/              # Background music files
    ├── Opening.ogg
    ├── Battle_vs_Trainer.ogg
    ├── Rival_Battle.ogg
    ├── Victory_Road.ogg
    └── Last_Battle.ogg
```

## Gameplay

1. **Login/Register**: Create an account or log in with existing credentials
2. **Main Menu**: Choose to play a battle, view leaderboard, or logout
3. **Battle**: 
   - Field a squad of 3 Pokemon selected at random
   - Select moves from the bottom panel or switch using the dedicated switch row
   - Manage your team to keep at least one Pokemon standing
   - Win by defeating all 3 enemy Pokemon
4. **Statistics**: Your wins, losses, and streaks are tracked automatically

## Controls

- **Mouse**: Click buttons and input fields
- **Keyboard**: Type in login fields, press TAB to switch fields
- **SPACE**: Continue after victory/defeat screen

## Pokemon Types

The game features three starter Pokemon:
- **Charizard** (Fire type)
- **Blastoise** (Water type)
- **Venusaur** (Grass type)

Each Pokemon has 4 unique moves with different types, damage, and accuracy.

## Type Effectiveness

- Fire > Grass (2x damage)
- Water > Fire (2x damage)
- Grass > Water (2x damage)
- Same type attacks: 0.5x damage
- Many other type matchups implemented

## Recent Improvements

- ✅ Fixed linter warnings
- ✅ Optimized `getMoves()` to return const reference (reduces copying)
- ✅ Added bounds checking for move selection
- ✅ Improved code documentation

See `IMPROVEMENTS.md` for detailed suggestions and future enhancements.

## Troubleshooting

### Game won't compile
- Ensure SplashKit is properly installed
- Check that all include paths are correct
- Verify C++ compiler is installed

### Font warning loops (`text_driver.cpp:119`)
- Update `DEFAULT_FONT_PATH` in `H3.cpp` to point to a font that exists on your system (defaults to `/Library/Fonts/Arial.ttf` on macOS)
- Or replace the constant with a relative path to a `.ttf` you place in the project (e.g., `resources/fonts/YourFont.ttf`)

### Missing sprites/music
- Ensure `sprites/`, `backgrounds/`, and `music/` folders are in the same directory as the executable
- Check file paths in the code match your file structure

### User data not saving
- Check file permissions for `userdata.txt`
- Ensure the game has write access to the directory

## Author

Dhruv Lalit Tahiliani | 36422304

## License

This project is for educational purposes.


/**
 * Pokemon Battle Simulator - Single File Version
 * 
 * A simple turn-based battle game with Pokemon sprites.
 * All code consolidated into one file for easy understanding.
 * 
 * Author: Dhruv Lalit Tahiliani | 36422304
 */

#include "splashkit.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// ============================================================================
// CONSTANTS - All game settings in one place
// ============================================================================

// Window settings
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int FRAME_RATE = 60;

// Fighter positions
const int PLAYER_X = 200;
const int PLAYER_Y = 280;
const int ENEMY_X = 600;
const int ENEMY_Y = 280;

// UI positions
const int HP_BAR_WIDTH = 200;
const int HP_BAR_HEIGHT = 25;
const int PLAYER_HP_BAR_X = 100;
const int PLAYER_HP_BAR_Y = 350;
const int ENEMY_HP_BAR_X = 500;
const int ENEMY_HP_BAR_Y = 350;

const int GROUND_Y = 400;
const int GROUND_HEIGHT = 200;

const int UI_PANEL_Y = 490;
const int UI_PANEL_HEIGHT = 110;

const int MOVE_BUTTON_START_X = 10;
const int MOVE_BUTTON_Y = 510;
const int MOVE_BUTTON_WIDTH = 192;
const int MOVE_BUTTON_HEIGHT = 68;
const int MOVE_BUTTON_SPACING = 197;

// Animation settings
const int ANIMATION_FRAMES = 16;
const int ANIMATION_SPEED = 3;

// Timing
const unsigned int AI_DELAY_MS = 2000;  // 2 seconds

// Fighter settings
const int FIGHTER_RADIUS = 60;
const int FIGHTER_HIGHLIGHT_RADIUS = 20;
const int FIGHTER_HIGHLIGHT_OFFSET = 15;

// HP thresholds
const double HP_YELLOW_THRESHOLD = 0.5;
const double HP_RED_THRESHOLD = 0.25;

// Damage calculation
const double RANDOM_FACTOR_MIN = 0.9;
const int MIN_DAMAGE = 1;

// Music settings
const int NUM_MUSIC_TRACKS = 5; // I have set the number of music trakcs to 5
const float MUSIC_VOLUME = 0.6f;  // Controls how loud the music is: Set to 60% currently.

const int NUM_BACKGROUNDS = 5; // Number of background images available

//=============================================================================
// DAMAGE RESULT STRUCTURE
//=============================================================================
struct DamageResult {
    int damage;
    bool critical;
    bool missed;
    double typeMultiplier;
};

// ============================================================================
// MUSIC DATA
// ============================================================================

// Music track paths
const string MUSIC_PATHS[NUM_MUSIC_TRACKS] = {
    "music/Opening.ogg",
    "music/Battle_vs_Trainer.ogg",
    "music/Rival_Battle.ogg",
    "music/Victory_Road.ogg",
    "music/Last_Battle.ogg"
};

int current_music_index = 0; //current music set as 0.
music current_music; //creates a variable with the music data type in splashkit.

// ============================================================================
// BACKGROUND DATA
// ============================================================================

const string BACKGROUND_PATHS[NUM_BACKGROUNDS] = {
    "backgrounds/bg1.png",
    "backgrounds/bg2.png",
    "backgrounds/bg3.png",
    "backgrounds/bg4.png",
    "backgrounds/bg5.png"
};

int current_background_index = 0; //current background set as 0.
bitmap current_background; //creates a variable with the bitmap data type in splashkit.

// ============================================================================
// MOVE CLASS - Represents a Pokemon attack
// ============================================================================

class Move {
private:
    string name;
    int damage;
    string type;
    int accuracy;
    double critChance;
    
public:
    Move(string n, int dmg, string t, int acc = 100, double crit = 0.0625)
        : name(n), damage(dmg), type(t), accuracy(acc), critChance(crit) {}
    
    string getName() const { return name; }
    int getDamage() const { return damage; }
    string getType() const { return type; }
    int getAccuracy() const { return accuracy; }
    double getCritChance() const { return critChance; }
};

// ============================================================================
// FIGHTER CLASS - Represents a Pokemon
// ============================================================================

class Fighter {
private:
    string name;
    int hp;
    int max_hp;
    int attack;
    int defense;
    int speed;
    string fighter_type;
    vector<Move> moves;
    string sprite_path;
    
public:
    // Constructor
    Fighter(string n, int maxHp, int atk, int def, int spd, string type)
        : name(n), hp(maxHp), max_hp(maxHp), attack(atk), defense(def), 
          speed(spd), fighter_type(type) {}
    
    // Move management
    void addMove(const Move& move) {
        moves.push_back(move);
    }
    
    vector<Move> getMoves() const {
        return moves;
    }
    
    // Health management
    void takeDamage(int damage) {
        hp -= damage;
        if (hp < 0) hp = 0;
    }
    
    void heal(int amount) {
        hp += amount;
        if (hp > max_hp) hp = max_hp;
    }
    
    bool isAlive() const {
        return hp > 0;
    }
    
    // Getters
    string getName() const { return name; }
    int getHP() const { return hp; }
    int getMaxHP() const { return max_hp; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getSpeed() const { return speed; }
    string getType() const { return fighter_type; }
    
    double getHPPercentage() const {
        return (double)hp / (double)max_hp;
    }
    
    // Sprite management
    void setSpritePath(string path) {
        sprite_path = path;
    }
    
    string getSpritePath() const {
        return sprite_path;
    }
};

// ============================================================================
// HELPER FUNCTIONS - Type effectiveness and damage calculation
// ============================================================================

double getTypeMultiplier(string attack_type, string defender_type) {

    // Fire Matchups
    if (attack_type == "Fire" && defender_type == "Grass") return 2.0;
    if (attack_type == "Fire" && defender_type == "Water") return 0.5;
    if (attack_type == "Fire" && defender_type == "Fire") return 0.5;
    if (attack_type == "Fire" && defender_type == "Flying") return 1.0; // neutral

    // Water Matchups
    if (attack_type == "Water" && defender_type == "Fire") return 2.0;
    if (attack_type == "Water" && defender_type == "Grass") return 0.5;
    if (attack_type == "Water" && defender_type == "Water") return 0.5;

    // Grass Matchups
    if (attack_type == "Grass" && defender_type == "Water") return 2.0;
    if (attack_type == "Grass" && defender_type == "Fire") return 0.5;
    if (attack_type == "Grass" && defender_type == "Flying") return 0.5; 
    if (attack_type == "Grass" && defender_type == "Grass") return 0.5;

    // Flying Matchups
    if (attack_type == "Flying" && defender_type == "Grass") return 2.0; 
    if (attack_type == "Flying" && defender_type == "Fire") return 1.0;
    if (attack_type == "Flying" && defender_type == "Water") return 1.0;
    if (attack_type == "Flying" && defender_type == "Flying") return 1.0;

    // Poison Matchups
    if (attack_type == "Poison" && defender_type == "Grass") return 2.0;
    if (attack_type == "Poison" && defender_type == "Poison") return 0.5;
    if (attack_type == "Poison" && defender_type == "Ground") return 0.5;

    // Ground Matchups
    if (attack_type == "Ground" && defender_type == "Fire") return 2.0;
    if (attack_type == "Ground" && defender_type == "Electric") return 2.0;
    if (attack_type == "Ground" && defender_type == "Grass") return 0.5;
    if (attack_type == "Ground" && defender_type == "Flying") return 0.0; 

    // Ice Matchups
    if (attack_type == "Ice" && defender_type == "Grass") return 2.0;
    if (attack_type == "Ice" && defender_type == "Water") return 0.5;
    if (attack_type == "Ice" && defender_type == "Fire") return 0.5;
    if (attack_type == "Ice" && defender_type == "Flying") return 2.0;

    // Dragon Matchups
    if (attack_type == "Dragon" && defender_type == "Dragon") return 2.0;
    if (attack_type == "Dragon" && defender_type != "Dragon") return 1.0;

    // Dark Matchups
    if (attack_type == "Dark" && defender_type == "Psychic") return 2.0;
    if (attack_type == "Dark" && defender_type == "Dark") return 0.5;
    if (attack_type == "Dark" && defender_type == "Fighting") return 0.5;

    // Neutral Damage
    return 1.0;
}


Fighter createRandomPlayer() {
    int random_choice = rand() % 3;

    if(random_choice == 0) {
        return Fighter("Charizard", 150, 55, 52, 100, "Fire");
    } else if(random_choice == 1) {
        return Fighter("Blastoise", 140, 48, 55, 78, "Water");
    } else {
        return Fighter("Venusaur", 145, 50, 50, 80, "Grass");
    }
}

Fighter createRandomEnemy() {
    int random_choice = rand() % 3;

    if(random_choice == 0) {
        return Fighter("Charizard", 150, 55, 52, 100, "Fire");
    } else if(random_choice == 1) {
        return Fighter("Blastoise", 140, 48, 55, 78, "Water");
    } else {
        return Fighter("Venusaur", 145, 50, 50, 80, "Grass");
    }
}

DamageResult calculateDamage(const Fighter& attacker, const Fighter& defender, const Move& move) {
    DamageResult result;
    result.damage = 0;
    result.critical = false;
    result.missed = false;
    result.typeMultiplier = getTypeMultiplier(move.getType(), defender.getType());

    // Accuracy roll
    int roll = rand() % 100 + 1;
    if (roll > move.getAccuracy()) {
        result.missed = true;
        return result;
    }

    // Base damage
    double base_damage = (attacker.getAttack() * move.getDamage()) / (double)defender.getDefense();

    // STAB bonus
    double stab = (move.getType() == attacker.getType()) ? 1.5 : 1.0;

    // Critical hit chance
    if (((rand() % 1000) / 1000.0) < move.getCritChance()) {
        result.critical = true;
    }

    // Random factor 0.9–1.1
    double random_factor = RANDOM_FACTOR_MIN + (rand() % 20) / 100.0;

    // Final damage
    result.damage = (int)(base_damage * stab * result.typeMultiplier *
                          (result.critical ? 1.5 : 1.0) * random_factor);

    // Minimum damage safeguard
    if (result.damage < MIN_DAMAGE) result.damage = MIN_DAMAGE;
    return result;
}


// ============================================================================
// MUSIC FUNCTIONS
// ============================================================================

int getRandomMusicIndex() {
    return rand() % NUM_MUSIC_TRACKS;
}

void playRandomMusic() {
    // Step 1: Stop any currently playing music
    if (music_valid(current_music)) {
        stop_music();
    }
    
    // Step 2: Pick a random song index
    current_music_index = getRandomMusicIndex();
    
    // Step 3: Get the file path for that song
    string music_path = MUSIC_PATHS[current_music_index];
    
    // Step 4: Load the music file
    current_music = load_music("battle_music_" + to_string(current_music_index), music_path);
    
    // Step 5: Play it if it loaded successfully
    if (music_valid(current_music)) {
        play_music(current_music, -1);  // -1 means loop forever
        set_music_volume(MUSIC_VOLUME);
    } else {
        write_line("Warning: Could not load music: " + music_path);
    }
}

void stopBattleMusic() {
    if (music_valid(current_music)) {
        stop_music();
    }
}

// ============================================================================
// BACKGROUND FUNCTIONS
// ============================================================================

int getRandomBackgroundIndex() {
    return rand() % NUM_BACKGROUNDS;
}

void loadRandomBackground() {
    // Pick random background
    current_background_index = getRandomBackgroundIndex();
    string bg_path = BACKGROUND_PATHS[current_background_index];
    
    // Load the image using path as unique cache key
    current_background = load_bitmap(bg_path, bg_path);
    
    // Check if loading failed (missing file)
    if (!bitmap_valid(current_background)) {
        write_line("Warning: Could not load background: " + bg_path);
    }
}

// ============================================================================
// DRAWING FUNCTIONS - All graphics rendering
// ============================================================================

color getHPBarColor(double hp_percentage) {
    if (hp_percentage > HP_YELLOW_THRESHOLD) return COLOR_GREEN;
    if (hp_percentage > HP_RED_THRESHOLD) return COLOR_YELLOW;
    return COLOR_RED;
}

color getFighterColor(string fighter_type) {
    if (fighter_type == "Fire") return COLOR_ORANGE;
    if (fighter_type == "Water") return COLOR_BLUE;
    if (fighter_type == "Grass") return COLOR_GREEN;
    if (fighter_type == "Electric") return COLOR_YELLOW;
    return COLOR_GRAY;
}

void drawFighter(const Fighter& fighter, int x, int y, bool use_sprite = true) {
    if (use_sprite && !fighter.getSpritePath().empty()) {
        // Try to load and draw sprite
        bitmap sprite = load_bitmap(fighter.getSpritePath(), fighter.getSpritePath());
        
        if (bitmap_valid(sprite)) {
            // Calculate size - scale sprite to fit nicely
            int sprite_width = bitmap_width(sprite);
            int sprite_height = bitmap_height(sprite);
            
            // Scale to reasonable size (120x120)
            double scale = 120.0 / max(sprite_width, sprite_height);
            
            // Draw sprite centered
            draw_bitmap(sprite, x - (sprite_width * scale) / 2, 
                       y - (sprite_height * scale) / 2,
                       option_scale_bmp(scale, scale));
            return;
        }
    }
    
    // Fallback to circles if sprite not found
    color fighter_color = getFighterColor(fighter.getType());
    
    // Draw shadow
    fill_circle(rgba_color(0, 0, 0, 100), x + 5, y + 5, FIGHTER_RADIUS);
    
    // Draw main body
    fill_circle(fighter_color, x, y, FIGHTER_RADIUS);
    
    // Draw outline
    draw_circle(COLOR_BLACK, x, y, FIGHTER_RADIUS);
    
    // Draw highlight
    fill_circle(rgba_color(255, 255, 255, 100), 
                x - FIGHTER_HIGHLIGHT_OFFSET, 
                y - FIGHTER_HIGHLIGHT_OFFSET, 
                FIGHTER_HIGHLIGHT_RADIUS);
}

void drawHPBar(const Fighter& fighter, int x, int y, int width, int height) {
    // Draw background
    fill_rectangle(COLOR_DARK_GRAY, x, y, width, height);
    
    // Calculate current width and get color
    int current_width = width * fighter.getHPPercentage();
    color bar_color = getHPBarColor(fighter.getHPPercentage());
    
    // Draw foreground
    fill_rectangle(bar_color, x, y, current_width, height);
    
    // Draw border
    draw_rectangle(COLOR_BLACK, x, y, width, height);
    
    // Display HP text with shadow
    string hp_text = to_string(fighter.getHP()) + "/" + to_string(fighter.getMaxHP());
    draw_text(hp_text, COLOR_BLACK, x + width / 2 - 29, y + height / 2 - 7);
    draw_text(hp_text, COLOR_WHITE, x + width / 2 - 30, y + height / 2 - 8);
}

void drawMoveButton(const Move& move, int x, int y, int width, int height, bool is_hovered) {
    // Background color
    color button_bg = is_hovered ? rgb_color(200, 220, 255) : COLOR_WHITE;
    
    // Draw shadow for depth
    fill_rectangle(rgba_color(0, 0, 0, 80), x + 2, y + 2, width, height);
    
    // Draw button background
    fill_rectangle(button_bg, x, y, width, height);
    
    // Draw border
    draw_rectangle(COLOR_BLACK, x, y, width, height);
    if (is_hovered) {
        draw_rectangle(COLOR_BLACK, x + 1, y + 1, width - 2, height - 2);
    }
    
    // Move name - top center, large and bold
    draw_text(move.getName(), COLOR_BLACK, "Arial", 18, x + 8, y + 8);
    
    // Divider line
    draw_line(COLOR_LIGHT_GRAY, x + 5, y + 32, x + width - 5, y + 32);
    
    // Type - bottom left
    draw_text(move.getType(), rgb_color(100, 100, 100), "Arial", 13, x + 8, y + 40);
    
    // Power - bottom right
    string power_text = "PWR " + to_string(move.getDamage());
    draw_text(power_text, rgb_color(100, 100, 100), "Arial", 13, x + width - 70, y + 40);

    // Accuracy - bottom left below type
    draw_text("ACC " + to_string(move.getAccuracy()) + "%", rgb_color(100, 100, 100), "Arial", 13, x + 8, y + 58);
}

void drawTurnInfo(int turn_number) {
    fill_rectangle(rgba_color(0, 0, 0, 180), 300, 20, 200, 40);
    string turn_text = "Turn " + to_string(turn_number);
    draw_text(turn_text, COLOR_WHITE, "Arial", 24, 340, 30);
}

void drawBattleLog(const string& log_text) {
    fill_rectangle(rgba_color(0, 0, 0, 180), 20, 430, 760, 50);
    draw_text(log_text, COLOR_WHITE, "Arial", 16, 30, 445);
}

void drawBackground() {
    // Draw the background image (already 800×600, no scaling!)
    if (bitmap_valid(current_background)) {
        draw_bitmap(current_background, 0, 0);  // ← Perfect!
    } else {
        // Fallback gradient
        clear_screen(rgb_color(135, 206, 235));
        fill_rectangle(rgb_color(34, 139, 34), 0, GROUND_Y, WINDOW_WIDTH, GROUND_HEIGHT);
    }
}

void drawUIPanel() {
    fill_rectangle(rgba_color(50, 50, 50, 220), 0, UI_PANEL_Y, WINDOW_WIDTH, UI_PANEL_HEIGHT);
}

void drawVictoryScreen() {
    // Darken screen
    fill_rectangle(rgba_color(0, 0, 0, 200), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Draw victory box
    fill_rectangle(rgba_color(0, 100, 0, 230), 150, 180, 500, 240);
    draw_rectangle(COLOR_BLACK, 150, 180, 500, 240);
    
    // Draw text
    draw_text("YOU WON!", COLOR_GREEN, "Arial", 56, 250, 240);
    draw_text("Congratulations!", COLOR_WHITE, "Arial", 20, 290, 300);
    draw_text("Press SPACE to restart", COLOR_YELLOW, "Arial", 22, 250, 360);
}

void drawDefeatScreen() {
    // Darken screen
    fill_rectangle(rgba_color(0, 0, 0, 200), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Draw loss box
    fill_rectangle(rgba_color(139, 0, 0, 230), 150, 180, 500, 240);
    draw_rectangle(COLOR_BLACK, 150, 180, 500, 240);
    
    // Draw text
    draw_text("YOU LOST!", COLOR_RED, "Arial", 56, 250, 240);
    draw_text("Better luck next time!", COLOR_WHITE, "Arial", 20, 270, 300);
    draw_text("Press SPACE to restart", COLOR_YELLOW, "Arial", 22, 250, 360);
}

// ============================================================================
// GAME STATE - Track current game status
// ============================================================================

enum class GameState {
    BATTLE,
    VICTORY,
    DEFEAT
};

// ============================================================================
// GLOBAL GAME VARIABLES
// ============================================================================

Fighter player = Fighter("Charizard", 150, 55, 52, 100, "Fire");
Fighter enemy = Fighter("Charizard", 150, 55, 52, 100, "Fire");

GameState state = GameState::BATTLE;
bool player_turn = true;
int turn_number = 1;
string battle_log = "Battle Start! Choose your move!";

int player_x = PLAYER_X;
int enemy_x = ENEMY_X;
bool animating = false;
int animation_frame = 0;

unsigned int ai_action_time = 0;
bool ai_waiting = false;

// ============================================================================
// GAME FUNCTIONS
// ============================================================================

void initializeFighters() {
    // The pokemon moves and sprite are set up based on the pokemon that was chosen for the user    
    player = createRandomPlayer();
    enemy = createRandomEnemy();

    playRandomMusic();
    loadRandomBackground();

    string player_name = player.getName();

    if (player_name == "Charizard") {
        // Charizard moves
        player.addMove(Move("Fire Blast", 40, "Fire",80));
        player.addMove(Move("Flamethrower", 32, "Fire",85));
        player.addMove(Move("Air Slash", 32, "Flying",85));
        player.addMove(Move("Dragon Claw", 24, "Dragon",90));
        player.setSpritePath("sprites/usercharizard.png");
        
    } else if (player_name == "Blastoise") {
        // Blastoise moves
        player.addMove(Move("Hydro Pump", 40, "Water",80));
        player.addMove(Move("Surf", 32, "Water",85));
        player.addMove(Move("Ice Beam", 32, "Ice",85));
        player.addMove(Move("Bite", 24, "Dark",90));
        player.setSpritePath("sprites/userblastoise.png");
        
    } else if (player_name == "Venusaur") {
        // Venusaur moves
        player.addMove(Move("Solar Beam", 40, "Grass",80));
        player.addMove(Move("Razor Leaf", 32, "Grass",85));
        player.addMove(Move("Sludge Bomb", 32, "Poison",85));
        player.addMove(Move("Earthquake", 24, "Ground",90));
        player.setSpritePath("sprites/uservenusaur.png");
    }
    
    // The enemy moves and sprite are set up based on the pokemon that was chosen for the enemy
    string enemy_name = enemy.getName();
    
    if (enemy_name == "Charizard") {
        // Charizard moves
        enemy.addMove(Move("Fire Blast", 40, "Fire",80));
        enemy.addMove(Move("Flamethrower", 32, "Fire",85));
        enemy.addMove(Move("Air Slash", 32, "Flying",85));
        enemy.addMove(Move("Dragon Claw", 24, "Dragon",90));
        enemy.setSpritePath("sprites/enemycharizard.png");

    } else if (enemy_name == "Blastoise") {
        // Blastoise moves
        enemy.addMove(Move("Hydro Pump", 40, "Water",80));
        enemy.addMove(Move("Surf", 32, "Water",85));
        enemy.addMove(Move("Ice Beam", 32, "Ice",85));
        enemy.addMove(Move("Bite", 24, "Dark",90));
        enemy.setSpritePath("sprites/enemyblastoise.png");

    } else if (enemy_name == "Venusaur") {
        // Venusaur moves
        enemy.addMove(Move("Solar Beam", 40, "Grass",80));
        enemy.addMove(Move("Razor Leaf", 32, "Grass",85));
        enemy.addMove(Move("Sludge Bomb", 32, "Poison",85));
        enemy.addMove(Move("Earthquake", 24, "Ground",90));
        enemy.setSpritePath("sprites/enemyvenusaur.png");
    }
}

void executePlayerMove(int move_index) {
    vector<Move> moves = player.getMoves();
    Move chosen_move = moves[move_index];

    DamageResult result = calculateDamage(player, enemy, chosen_move);

    if (result.missed) {
        // Attack missed
        battle_log = player.getName() + " used " + chosen_move.getName() + " but it missed!";
    } 
    else {
        enemy.takeDamage(result.damage);  // Apply damage
        battle_log = player.getName() + " used " + chosen_move.getName() + "! " +
                     to_string(result.damage) + " damage!";

        // Add battle feedback
        if (result.critical) battle_log += " A critical hit!";
        if (result.typeMultiplier > 1.0)
            battle_log += " It's super effective!";
        else if (result.typeMultiplier < 1.0)
            battle_log += " It's not very effective...";
    }

    // Switch turn and trigger animation
    player_turn = false;
    animating = true;
    animation_frame = 0;
    ai_action_time = current_ticks() + AI_DELAY_MS;
    ai_waiting = true;
}

void executeEnemyMove() {
    ai_waiting = false;

    vector<Move> enemy_moves = enemy.getMoves();
    int random_move_index = rand() % enemy_moves.size();
    Move ai_move = enemy_moves[random_move_index];

    DamageResult result = calculateDamage(enemy, player, ai_move);

    if (result.missed) {
        battle_log = enemy.getName() + " used " + ai_move.getName() + " but it missed!";
    } 
    else {
        player.takeDamage(result.damage);
        battle_log = enemy.getName() + " used " + ai_move.getName() + "! " +
                     to_string(result.damage) + " damage!";

        if (result.critical) battle_log += " A critical hit!";
        if (result.typeMultiplier > 1.0)
            battle_log += " It's super effective!";
        else if (result.typeMultiplier < 1.0)
            battle_log += " It's not very effective...";
    }

    player_turn = true;
    turn_number++;
}

void handleAnimation() {
    if (animating) {
        animation_frame++;
        
        if (animation_frame < 8) {
            player_x += ANIMATION_SPEED;
        } else if (animation_frame < ANIMATION_FRAMES) {
            player_x -= ANIMATION_SPEED;
        } else {
            animating = false;
            animation_frame = 0;
            player_x = PLAYER_X;
        }
    }
}

void handlePlayerTurn() {
    vector<Move> moves = player.getMoves();
    
    for (size_t i = 0; i < moves.size(); i++) {
        int button_x = MOVE_BUTTON_START_X + (i * MOVE_BUTTON_SPACING);
        int button_y = MOVE_BUTTON_Y;
        int button_width = MOVE_BUTTON_WIDTH;
        int button_height = MOVE_BUTTON_HEIGHT;
        
        // Check if button clicked
        bool is_hovered = (mouse_x() >= button_x && 
                          mouse_x() <= button_x + button_width &&
                          mouse_y() >= button_y && 
                          mouse_y() <= button_y + button_height);
        
        if (is_hovered && mouse_clicked(LEFT_BUTTON)) {
            executePlayerMove(i);
            break;
        }
    }
}

void handleEnemyTurn() {
    // Wait for AI delay
    if (ai_waiting && current_ticks() >= ai_action_time) {
        executeEnemyMove();
    }
}

void checkBattleEnd() {
    if (!player.isAlive()) {
        state = GameState::DEFEAT;
    } else if (!enemy.isAlive()) {
        state = GameState::VICTORY;
    }
}

void resetGame() {
    // Reset fighters
    initializeFighters();
    
    // Reset state
    state = GameState::BATTLE;
    turn_number = 1;
    player_turn = true;
    battle_log = "Battle Start! Choose your move!";
    player_x = PLAYER_X;
    enemy_x = ENEMY_X;
    ai_waiting = false;
    animating = false;
    animation_frame = 0;
}

void handleInput() {
    if (state == GameState::BATTLE) {
        if (player_turn && !ai_waiting) {
            handlePlayerTurn();
        } else if (!player_turn) {
            handleEnemyTurn();
        }
    } else {
        // Game over - wait for restart
        if (key_typed(SPACE_KEY)) {
            resetGame();
        }
    }
}

void render() {
    // Draw background
    drawBackground();
    
    // Draw fighters
    drawFighter(player, player_x, PLAYER_Y);
    draw_text(player.getName() + " (" + player.getType() + ")", 
              COLOR_BLACK, "Arial", 18, 120, 220);
    drawHPBar(player, PLAYER_HP_BAR_X, PLAYER_HP_BAR_Y, HP_BAR_WIDTH, HP_BAR_HEIGHT);
    
    drawFighter(enemy, enemy_x, ENEMY_Y);
    draw_text(enemy.getName() + " (" + enemy.getType() + ")", 
              COLOR_BLACK, "Arial", 18, 520, 220);
    drawHPBar(enemy, ENEMY_HP_BAR_X, ENEMY_HP_BAR_Y, HP_BAR_WIDTH, HP_BAR_HEIGHT);
    
    // Draw UI
    drawTurnInfo(turn_number);
    drawBattleLog(battle_log);
    
    // Render based on state
    if (state == GameState::VICTORY) {
        drawVictoryScreen();
    } else if (state == GameState::DEFEAT) {
        drawDefeatScreen();
    } else {
        // Draw gameplay UI
        drawUIPanel();
        
        if (player_turn && !ai_waiting) {
            draw_text("Your Turn! Choose a move:", COLOR_YELLOW, "Arial", 20, 280, 500);
            
            vector<Move> moves = player.getMoves();
            for (size_t i = 0; i < moves.size(); i++) {
                int button_x = MOVE_BUTTON_START_X + (i * MOVE_BUTTON_SPACING);
                int button_y = MOVE_BUTTON_Y;
                int button_width = MOVE_BUTTON_WIDTH;
                int button_height = MOVE_BUTTON_HEIGHT;
                
                bool is_hovered = (mouse_x() >= button_x && 
                                  mouse_x() <= button_x + button_width &&
                                  mouse_y() >= button_y && 
                                  mouse_y() <= button_y + button_height);
                
                drawMoveButton(moves[i], button_x, button_y, 
                              button_width, button_height, is_hovered);
            }
        } else if (!player_turn) {
            draw_text("Enemy's Turn...", COLOR_ORANGE, "Arial", 22, 300, 530);
            draw_text("Wait for enemy to attack", COLOR_WHITE, "Arial", 16, 280, 560);
        }
    }
}

// ============================================================================
// MAIN FUNCTION - Program entry point
// ============================================================================

int main() {
    // Initialize random number generator
    srand(time(nullptr));
    
    // Initialize fighters
    initializeFighters();
    
    // Open game window
    open_window("Battle Simulator - Enhanced", WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Game loop
    while (!quit_requested()) {
        process_events();
        
        handleAnimation();
        handleInput();
        checkBattleEnd();
        render();
        
        refresh_screen(FRAME_RATE);
    }
    
    // Cleanup
    stopBattleMusic();
    close_all_windows();
    
    return 0;
}

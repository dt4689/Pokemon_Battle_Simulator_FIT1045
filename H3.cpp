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
#include <fstream>
#include <sstream>
#include <algorithm>

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

// User data file
const string USER_DATA_FILE = "userdata.txt";

// Login UI constants
const int LOGIN_BOX_WIDTH = 400;
const int LOGIN_BOX_HEIGHT = 500;
const int LOGIN_BOX_X = (WINDOW_WIDTH - LOGIN_BOX_WIDTH) / 2;
const int LOGIN_BOX_Y = (WINDOW_HEIGHT - LOGIN_BOX_HEIGHT) / 2;
const int INPUT_FIELD_WIDTH = 300;
const int INPUT_FIELD_HEIGHT = 40;
const int BUTTON_WIDTH = 150;
const int BUTTON_HEIGHT = 45;

// Menu constants
const int MENU_BUTTON_WIDTH = 300;
const int MENU_BUTTON_HEIGHT = 60;
const int MENU_BUTTON_SPACING = 80;

// Leaderboard constants
const int LEADERBOARD_WIDTH = 700;
const int LEADERBOARD_HEIGHT = 500;
const int LEADERBOARD_X = (WINDOW_WIDTH - LEADERBOARD_WIDTH) / 2;
const int LEADERBOARD_Y = (WINDOW_HEIGHT - LEADERBOARD_HEIGHT) / 2;

//=============================================================================
// DAMAGE RESULT STRUCTURE
//=============================================================================
struct DamageResult {
    int damage;
    bool critical;
    bool missed;
    double typeMultiplier;
};

//=============================================================================
// USER CLASS - Stores user login and statistics
//=============================================================================
class User {
private:
    string username;
    string password;
    int wins;
    int losses;
    int current_streak;
    int best_streak;
    int total_battles;
    int total_score;

public:
    // Constructor
    User() : username(""), password(""), wins(0), losses(0),
             current_streak(0), best_streak(0), total_battles(0), total_score(0) {}

    User(string uname, string pass)
        : username(uname), password(pass), wins(0), losses(0),
          current_streak(0), best_streak(0), total_battles(0), total_score(0) {}

    User(string uname, string pass, int w, int l, int streak, int best, int battles, int score)
        : username(uname), password(pass), wins(w), losses(l),
          current_streak(streak), best_streak(best), total_battles(battles), total_score(score) {}

    // Getters
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    int getWins() const { return wins; }
    int getLosses() const { return losses; }
    int getCurrentStreak() const { return current_streak; }
    int getBestStreak() const { return best_streak; }
    int getTotalBattles() const { return total_battles; }
    int getTotalScore() const { return total_score; }

    double getWinRate() const {
        if (total_battles == 0) return 0.0;
        return (double)wins / (double)total_battles * 100.0;
    }

    // Update stats after battle
    void recordWin() {
        wins++;
        total_battles++;
        current_streak++;
        if (current_streak > best_streak) {
            best_streak = current_streak;
        }
        total_score += 100 + (current_streak * 10); // Base 100 + streak bonus
    }

    void recordLoss() {
        losses++;
        total_battles++;
        current_streak = 0;
        total_score += 10; // Small consolation points
    }

    // Serialize to string for file storage
    string serialize() const {
        return username + "," + password + "," +
               to_string(wins) + "," + to_string(losses) + "," +
               to_string(current_streak) + "," + to_string(best_streak) + "," +
               to_string(total_battles) + "," + to_string(total_score);
    }

    // Deserialize from string
    static User deserialize(const string& data) {
        stringstream ss(data);
        string uname, pass;
        int w, l, streak, best, battles, score;
        char delim;

        getline(ss, uname, ',');
        getline(ss, pass, ',');
        ss >> w >> delim >> l >> delim >> streak >> delim >> best >> delim >> battles >> delim >> score;

        return User(uname, pass, w, l, streak, best, battles, score);
    }
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
// USER DATA MANAGEMENT FUNCTIONS
// ============================================================================

vector<User> loadAllUsers() {
    vector<User> users;
    ifstream file(USER_DATA_FILE);

    if (!file.is_open()) {
        return users; // Return empty vector if file doesn't exist yet
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            users.push_back(User::deserialize(line));
        }
    }

    file.close();
    return users;
}

void saveAllUsers(const vector<User>& users) {
    ofstream file(USER_DATA_FILE);

    if (!file.is_open()) {
        write_line("Error: Could not save user data!");
        return;
    }

    for (const User& user : users) {
        file << user.serialize() << "\n";
    }

    file.close();
}

User* authenticateUser(vector<User>& users, const string& username, const string& password) {
    for (size_t i = 0; i < users.size(); i++) {
        if (users[i].getUsername() == username && users[i].getPassword() == password) {
            return &users[i];
        }
    }
    return nullptr;
}

bool usernameExists(const vector<User>& users, const string& username) {
    for (const User& user : users) {
        if (user.getUsername() == username) {
            return true;
        }
    }
    return false;
}

bool registerUser(vector<User>& users, const string& username, const string& password) {
    if (username.empty() || password.empty()) {
        return false;
    }

    if (usernameExists(users, username)) {
        return false;
    }

    users.push_back(User(username, password));
    saveAllUsers(users);
    return true;
}

void updateUserStats(vector<User>& users, User* current_user) {
    // Find and update the user in the vector
    for (size_t i = 0; i < users.size(); i++) {
        if (users[i].getUsername() == current_user->getUsername()) {
            users[i] = *current_user;
            break;
        }
    }
    saveAllUsers(users);
}

vector<User> getLeaderboard(const vector<User>& users) {
    vector<User> leaderboard = users;

    // Sort by total score (descending)
    sort(leaderboard.begin(), leaderboard.end(),
         [](const User& a, const User& b) {
             return a.getTotalScore() > b.getTotalScore();
         });

    return leaderboard;
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

void drawInputField(int x, int y, int width, int height, const string& text,
                    const string& placeholder, bool is_active, bool is_password = false) {
    // Draw field background
    color bg_color = is_active ? COLOR_WHITE : rgb_color(240, 240, 240);
    fill_rectangle(bg_color, x, y, width, height);

    // Draw border
    color border_color = is_active ? rgb_color(0, 100, 255) : COLOR_GRAY;
    draw_rectangle(border_color, x, y, width, height);
    if (is_active) {
        draw_rectangle(border_color, x + 1, y + 1, width - 2, height - 2);
    }

    // Draw text or placeholder
    string display_text = text;
    if (is_password && !text.empty()) {
        display_text = string(text.length(), '*');
    }

    if (display_text.empty()) {
        draw_text(placeholder, rgb_color(150, 150, 150), "Arial", 16, x + 10, y + 12);
    } else {
        draw_text(display_text, COLOR_BLACK, "Arial", 16, x + 10, y + 12);
    }

    // Draw cursor if active
    if (is_active) {
        int cursor_x = x + 10 + text_width(display_text, "Arial", 16);
        draw_line(COLOR_BLACK, cursor_x, y + 8, cursor_x, y + height - 8);
    }
}

void drawButton(int x, int y, int width, int height, const string& text, bool is_hovered) {
    // Draw shadow
    fill_rectangle(rgba_color(0, 0, 0, 80), x + 3, y + 3, width, height);

    // Draw button
    color button_color = is_hovered ? rgb_color(0, 120, 255) : rgb_color(0, 100, 200);
    fill_rectangle(button_color, x, y, width, height);
    draw_rectangle(COLOR_BLACK, x, y, width, height);

    // Center text
    int text_w = text_width(text, "Arial", 18);
    int text_x = x + (width - text_w) / 2;
    int text_y = y + (height - 18) / 2;
    draw_text(text, COLOR_WHITE, "Arial", 18, text_x, text_y);
}

void drawLoginScreen(const string& username_input, const string& password_input,
                     int active_field, const string& error_message, bool is_register_mode) {
    // Background
    clear_screen(rgb_color(50, 50, 80));

    // Draw login box
    fill_rectangle(rgba_color(255, 255, 255, 250), LOGIN_BOX_X, LOGIN_BOX_Y,
                   LOGIN_BOX_WIDTH, LOGIN_BOX_HEIGHT);
    draw_rectangle(COLOR_BLACK, LOGIN_BOX_X, LOGIN_BOX_Y, LOGIN_BOX_WIDTH, LOGIN_BOX_HEIGHT);

    // Title
    string title = is_register_mode ? "REGISTER" : "LOGIN";
    draw_text(title, rgb_color(0, 100, 200), "Arial", 36,
              LOGIN_BOX_X + LOGIN_BOX_WIDTH / 2 - 70, LOGIN_BOX_Y + 30);

    // Game title
    draw_text("Pokemon Battle Simulator", COLOR_BLACK, "Arial", 20,
              LOGIN_BOX_X + 75, LOGIN_BOX_Y + 80);

    // Username field
    draw_text("Username:", COLOR_BLACK, "Arial", 16, LOGIN_BOX_X + 50, LOGIN_BOX_Y + 140);
    drawInputField(LOGIN_BOX_X + 50, LOGIN_BOX_Y + 165, INPUT_FIELD_WIDTH,
                   INPUT_FIELD_HEIGHT, username_input, "Enter username", active_field == 0);

    // Password field
    draw_text("Password:", COLOR_BLACK, "Arial", 16, LOGIN_BOX_X + 50, LOGIN_BOX_Y + 230);
    drawInputField(LOGIN_BOX_X + 50, LOGIN_BOX_Y + 255, INPUT_FIELD_WIDTH,
                   INPUT_FIELD_HEIGHT, password_input, "Enter password", active_field == 1, true);

    // Error message
    if (!error_message.empty()) {
        draw_text(error_message, COLOR_RED, "Arial", 14,
                  LOGIN_BOX_X + 50, LOGIN_BOX_Y + 315);
    }

    // Buttons
    int btn1_x = LOGIN_BOX_X + 50;
    int btn2_x = LOGIN_BOX_X + 200;
    int btn_y = LOGIN_BOX_Y + 360;

    bool btn1_hover = mouse_x() >= btn1_x && mouse_x() <= btn1_x + BUTTON_WIDTH &&
                      mouse_y() >= btn_y && mouse_y() <= btn_y + BUTTON_HEIGHT;
    bool btn2_hover = mouse_x() >= btn2_x && mouse_x() <= btn2_x + BUTTON_WIDTH &&
                      mouse_y() >= btn_y && mouse_y() <= btn_y + BUTTON_HEIGHT;

    if (is_register_mode) {
        drawButton(btn1_x, btn_y, BUTTON_WIDTH, BUTTON_HEIGHT, "Register", btn1_hover);
        drawButton(btn2_x, btn_y, BUTTON_WIDTH, BUTTON_HEIGHT, "Back", btn2_hover);
    } else {
        drawButton(btn1_x, btn_y, BUTTON_WIDTH, BUTTON_HEIGHT, "Login", btn1_hover);
        drawButton(btn2_x, btn_y, BUTTON_WIDTH, BUTTON_HEIGHT, "Register", btn2_hover);
    }

    // Instructions
    draw_text("Press TAB to switch fields", rgb_color(100, 100, 100), "Arial", 12,
              LOGIN_BOX_X + 50, LOGIN_BOX_Y + 430);
}

void drawMainMenu(const string& username) {
    // Background
    clear_screen(rgb_color(50, 50, 80));

    // Title
    draw_text("POKEMON BATTLE SIMULATOR", rgb_color(255, 200, 0), "Arial", 40,
              WINDOW_WIDTH / 2 - 300, 80);

    // Welcome message
    draw_text("Welcome, " + username + "!", COLOR_WHITE, "Arial", 24,
              WINDOW_WIDTH / 2 - 100, 150);

    // Menu buttons
    int btn_x = WINDOW_WIDTH / 2 - MENU_BUTTON_WIDTH / 2;
    int btn1_y = 250;
    int btn2_y = btn1_y + MENU_BUTTON_SPACING;
    int btn3_y = btn2_y + MENU_BUTTON_SPACING;

    bool btn1_hover = mouse_x() >= btn_x && mouse_x() <= btn_x + MENU_BUTTON_WIDTH &&
                      mouse_y() >= btn1_y && mouse_y() <= btn1_y + MENU_BUTTON_HEIGHT;
    bool btn2_hover = mouse_x() >= btn_x && mouse_x() <= btn_x + MENU_BUTTON_WIDTH &&
                      mouse_y() >= btn2_y && mouse_y() <= btn2_y + MENU_BUTTON_HEIGHT;
    bool btn3_hover = mouse_x() >= btn_x && mouse_x() <= btn_x + MENU_BUTTON_WIDTH &&
                      mouse_y() >= btn3_y && mouse_y() <= btn3_y + MENU_BUTTON_HEIGHT;

    drawButton(btn_x, btn1_y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, "PLAY BATTLE", btn1_hover);
    drawButton(btn_x, btn2_y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, "LEADERBOARD", btn2_hover);
    drawButton(btn_x, btn3_y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, "LOGOUT", btn3_hover);
}

void drawLeaderboard(const vector<User>& leaderboard, const string& current_username) {
    // Background
    clear_screen(rgb_color(50, 50, 80));

    // Title
    draw_text("LEADERBOARD", rgb_color(255, 200, 0), "Arial", 40,
              WINDOW_WIDTH / 2 - 150, 30);

    // Draw leaderboard box
    fill_rectangle(rgba_color(255, 255, 255, 250), LEADERBOARD_X, LEADERBOARD_Y,
                   LEADERBOARD_WIDTH, LEADERBOARD_HEIGHT);
    draw_rectangle(COLOR_BLACK, LEADERBOARD_X, LEADERBOARD_Y,
                   LEADERBOARD_WIDTH, LEADERBOARD_HEIGHT);

    // Header
    int header_y = LEADERBOARD_Y + 20;
    draw_text("Rank", COLOR_BLACK, "Arial", 16, LEADERBOARD_X + 20, header_y);
    draw_text("Username", COLOR_BLACK, "Arial", 16, LEADERBOARD_X + 100, header_y);
    draw_text("Score", COLOR_BLACK, "Arial", 16, LEADERBOARD_X + 280, header_y);
    draw_text("W/L", COLOR_BLACK, "Arial", 16, LEADERBOARD_X + 380, header_y);
    draw_text("Win%", COLOR_BLACK, "Arial", 16, LEADERBOARD_X + 480, header_y);
    draw_text("Streak", COLOR_BLACK, "Arial", 16, LEADERBOARD_X + 580, header_y);

    // Divider line
    draw_line(COLOR_GRAY, LEADERBOARD_X + 10, header_y + 25,
              LEADERBOARD_X + LEADERBOARD_WIDTH - 10, header_y + 25);

    // Display top 10 users
    int max_display = min(10, (int)leaderboard.size());
    for (int i = 0; i < max_display; i++) {
        int row_y = header_y + 45 + (i * 35);
        const User& user = leaderboard[i];

        // Highlight current user
        bool is_current = (user.getUsername() == current_username);
        if (is_current) {
            fill_rectangle(rgba_color(255, 255, 0, 100), LEADERBOARD_X + 10, row_y - 5,
                          LEADERBOARD_WIDTH - 20, 30);
        }

        color text_color = is_current ? rgb_color(200, 0, 0) : COLOR_BLACK;

        // Rank
        draw_text(to_string(i + 1), text_color, "Arial", 14, LEADERBOARD_X + 30, row_y);

        // Username
        string display_name = user.getUsername();
        if (display_name.length() > 15) {
            display_name = display_name.substr(0, 12) + "...";
        }
        draw_text(display_name, text_color, "Arial", 14, LEADERBOARD_X + 100, row_y);

        // Score
        draw_text(to_string(user.getTotalScore()), text_color, "Arial", 14,
                  LEADERBOARD_X + 280, row_y);

        // W/L
        string wl = to_string(user.getWins()) + "/" + to_string(user.getLosses());
        draw_text(wl, text_color, "Arial", 14, LEADERBOARD_X + 380, row_y);

        // Win%
        char win_rate[10];
        snprintf(win_rate, sizeof(win_rate), "%.1f%%", user.getWinRate());
        draw_text(string(win_rate), text_color, "Arial", 14, LEADERBOARD_X + 480, row_y);

        // Best Streak
        draw_text(to_string(user.getBestStreak()), text_color, "Arial", 14,
                  LEADERBOARD_X + 600, row_y);
    }

    // Back button
    int back_btn_x = WINDOW_WIDTH / 2 - 75;
    int back_btn_y = LEADERBOARD_Y + LEADERBOARD_HEIGHT + 20;
    bool back_hover = mouse_x() >= back_btn_x && mouse_x() <= back_btn_x + 150 &&
                      mouse_y() >= back_btn_y && mouse_y() <= back_btn_y + 45;

    drawButton(back_btn_x, back_btn_y, 150, 45, "BACK", back_hover);
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
    draw_text("Press SPACE to continue", COLOR_YELLOW, "Arial", 22, 240, 360);
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
    draw_text("Press SPACE to continue", COLOR_YELLOW, "Arial", 22, 240, 360);
}

// ============================================================================
// GAME STATE - Track current game status
// ============================================================================

enum class GameState {
    LOGIN,
    MAIN_MENU,
    LEADERBOARD,
    BATTLE,
    VICTORY,
    DEFEAT
};

// ============================================================================
// GLOBAL GAME VARIABLES
// ============================================================================

// User system variables
vector<User> all_users;
User* current_user = nullptr;
string username_input = "";
string password_input = "";
int active_input_field = 0; // 0 = username, 1 = password
string login_error_message = "";
bool is_register_mode = false;

// Battle variables
Fighter player = Fighter("Charizard", 150, 55, 52, 100, "Fire");
Fighter enemy = Fighter("Charizard", 150, 55, 52, 100, "Fire");

GameState state = GameState::LOGIN;
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
        if (current_user != nullptr) {
            current_user->recordLoss();
            updateUserStats(all_users, current_user);
        }
    } else if (!enemy.isAlive()) {
        state = GameState::VICTORY;
        if (current_user != nullptr) {
            current_user->recordWin();
            updateUserStats(all_users, current_user);
        }
    }
}

void resetBattle() {
    // Reset fighters
    initializeFighters();

    // Reset battle state
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

void handleLoginInput() {
    // Handle keyboard input for typing
    // Check for alphanumeric keys and common symbols
    for (int key = 32; key <= 126; key++) {  // ASCII printable characters
        key_code kc = static_cast<key_code>(key);
        if (key_typed(kc)) {
            char typed_char = static_cast<char>(key);

            if (active_input_field == 0) {
                if (username_input.length() < 20) {
                    username_input += typed_char;
                }
            } else {
                if (password_input.length() < 20) {
                    password_input += typed_char;
                }
            }
        }
    }

    // Handle backspace
    if (key_typed(BACKSPACE_KEY)) {
        if (active_input_field == 0 && !username_input.empty()) {
            username_input.pop_back();
        } else if (active_input_field == 1 && !password_input.empty()) {
            password_input.pop_back();
        }
    }

    // Handle tab to switch fields
    if (key_typed(TAB_KEY)) {
        active_input_field = (active_input_field + 1) % 2;
    }

    // Handle mouse clicks on input fields
    if (mouse_clicked(LEFT_BUTTON)) {
        int username_field_y = LOGIN_BOX_Y + 165;
        int password_field_y = LOGIN_BOX_Y + 255;
        int field_x = LOGIN_BOX_X + 50;

        if (mouse_x() >= field_x && mouse_x() <= field_x + INPUT_FIELD_WIDTH) {
            if (mouse_y() >= username_field_y && mouse_y() <= username_field_y + INPUT_FIELD_HEIGHT) {
                active_input_field = 0;
            } else if (mouse_y() >= password_field_y && mouse_y() <= password_field_y + INPUT_FIELD_HEIGHT) {
                active_input_field = 1;
            }
        }

        // Handle button clicks
        int btn1_x = LOGIN_BOX_X + 50;
        int btn2_x = LOGIN_BOX_X + 200;
        int btn_y = LOGIN_BOX_Y + 360;

        // Button 1 (Login or Register)
        if (mouse_x() >= btn1_x && mouse_x() <= btn1_x + BUTTON_WIDTH &&
            mouse_y() >= btn_y && mouse_y() <= btn_y + BUTTON_HEIGHT) {

            if (is_register_mode) {
                // Register user
                if (registerUser(all_users, username_input, password_input)) {
                    current_user = authenticateUser(all_users, username_input, password_input);
                    state = GameState::MAIN_MENU;
                    username_input = "";
                    password_input = "";
                    login_error_message = "";
                    is_register_mode = false;
                } else {
                    if (username_input.empty() || password_input.empty()) {
                        login_error_message = "Username and password cannot be empty!";
                    } else {
                        login_error_message = "Username already exists!";
                    }
                }
            } else {
                // Login user
                current_user = authenticateUser(all_users, username_input, password_input);
                if (current_user != nullptr) {
                    state = GameState::MAIN_MENU;
                    username_input = "";
                    password_input = "";
                    login_error_message = "";
                } else {
                    login_error_message = "Invalid username or password!";
                }
            }
        }

        // Button 2 (Register or Back)
        if (mouse_x() >= btn2_x && mouse_x() <= btn2_x + BUTTON_WIDTH &&
            mouse_y() >= btn_y && mouse_y() <= btn_y + BUTTON_HEIGHT) {

            is_register_mode = !is_register_mode;
            login_error_message = "";
        }
    }
}

void handleMainMenuInput() {
    if (!mouse_clicked(LEFT_BUTTON)) return;

    int btn_x = WINDOW_WIDTH / 2 - MENU_BUTTON_WIDTH / 2;
    int btn1_y = 250;
    int btn2_y = btn1_y + MENU_BUTTON_SPACING;
    int btn3_y = btn2_y + MENU_BUTTON_SPACING;

    // Play Battle button
    if (mouse_x() >= btn_x && mouse_x() <= btn_x + MENU_BUTTON_WIDTH &&
        mouse_y() >= btn1_y && mouse_y() <= btn1_y + MENU_BUTTON_HEIGHT) {
        resetBattle();
    }

    // Leaderboard button
    if (mouse_x() >= btn_x && mouse_x() <= btn_x + MENU_BUTTON_WIDTH &&
        mouse_y() >= btn2_y && mouse_y() <= btn2_y + MENU_BUTTON_HEIGHT) {
        state = GameState::LEADERBOARD;
    }

    // Logout button
    if (mouse_x() >= btn_x && mouse_x() <= btn_x + MENU_BUTTON_WIDTH &&
        mouse_y() >= btn3_y && mouse_y() <= btn3_y + MENU_BUTTON_HEIGHT) {
        current_user = nullptr;
        username_input = "";
        password_input = "";
        login_error_message = "";
        state = GameState::LOGIN;
        stopBattleMusic();
    }
}

void handleLeaderboardInput() {
    if (!mouse_clicked(LEFT_BUTTON)) return;

    int back_btn_x = WINDOW_WIDTH / 2 - 75;
    int back_btn_y = LEADERBOARD_Y + LEADERBOARD_HEIGHT + 20;

    // Back button
    if (mouse_x() >= back_btn_x && mouse_x() <= back_btn_x + 150 &&
        mouse_y() >= back_btn_y && mouse_y() <= back_btn_y + 45) {
        state = GameState::MAIN_MENU;
    }
}

void handleInput() {
    if (state == GameState::LOGIN) {
        handleLoginInput();
    } else if (state == GameState::MAIN_MENU) {
        handleMainMenuInput();
    } else if (state == GameState::LEADERBOARD) {
        handleLeaderboardInput();
    } else if (state == GameState::BATTLE) {
        if (player_turn && !ai_waiting) {
            handlePlayerTurn();
        } else if (!player_turn) {
            handleEnemyTurn();
        }
    } else if (state == GameState::VICTORY || state == GameState::DEFEAT) {
        // Battle ended - go back to main menu
        if (key_typed(SPACE_KEY)) {
            state = GameState::MAIN_MENU;
        }
    }
}

void render() {
    if (state == GameState::LOGIN) {
        drawLoginScreen(username_input, password_input, active_input_field,
                       login_error_message, is_register_mode);
    } else if (state == GameState::MAIN_MENU) {
        if (current_user != nullptr) {
            drawMainMenu(current_user->getUsername());
        }
    } else if (state == GameState::LEADERBOARD) {
        vector<User> leaderboard = getLeaderboard(all_users);
        string current_username = (current_user != nullptr) ? current_user->getUsername() : "";
        drawLeaderboard(leaderboard, current_username);
    } else if (state == GameState::BATTLE || state == GameState::VICTORY || state == GameState::DEFEAT) {
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
}

// ============================================================================
// MAIN FUNCTION - Program entry point
// ============================================================================

int main() {
    // Initialize random number generator
    srand(time(nullptr));

    // Load user data from file
    all_users = loadAllUsers();

    // Open game window
    open_window("Pokemon Battle Simulator - Login", WINDOW_WIDTH, WINDOW_HEIGHT);

    // Game loop
    while (!quit_requested()) {
        process_events();

        // Only run battle logic when in battle
        if (state == GameState::BATTLE) {
            handleAnimation();
            checkBattleEnd();
        }

        handleInput();
        render();

        refresh_screen(FRAME_RATE);
    }

    // Cleanup
    stopBattleMusic();
    close_all_windows();

    return 0;
}

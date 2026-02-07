/**
 * main.cpp - Main driver file for the Battle Simulator.
 *
 * Turn-based RPG battle simulator with proper UI flow and visible move buttons.
 * Added delay system for better pacing and user feedback.
 *
 * Author: Dhruv Lalit Tahiliani | 36422304
 */

#include "splashkit.h"
#include "fighter.h"
#include <iostream>

using namespace std;

/**
 * get_type_multiplier - Calculates damage multiplier based on type matchup.
 * Returns: 2.0 (super effective), 0.5 (not very effective), or 1.0 (neutral)
 */
double get_type_multiplier(string attack_type, string defender_type) {
    if (attack_type == "Fire" && defender_type == "Grass") return 2.0;
    if (attack_type == "Fire" && defender_type == "Water") return 0.5;
    if (attack_type == "Water" && defender_type == "Fire") return 2.0;
    if (attack_type == "Water" && defender_type == "Grass") return 0.5;
    if (attack_type == "Grass" && defender_type == "Water") return 2.0;
    if (attack_type == "Grass" && defender_type == "Fire") return 0.5;
    return 1.0;
}

/**
 * get_hp_bar_color - Returns color based on HP percentage.
 * Green when healthy, yellow at 50%, red when critical.
 */
color get_hp_bar_color(double hp_percentage) {
    if (hp_percentage > 0.5) {
        return COLOR_GREEN;
    } else if (hp_percentage > 0.25) {
        return COLOR_YELLOW;
    } else {
        return COLOR_RED;
    }
}

/**
 * draw_hp_bar - Renders an animated health bar with color transitions.
 */
void draw_hp_bar(Fighter &fighter, int x, int y, int width, int height) {
    // Draw background
    fill_rectangle(COLOR_DARK_GRAY, x, y, width, height);

    // Calculate current width and get color
    int current_width = width * fighter.getHPPercentage();
    color bar_color = get_hp_bar_color(fighter.getHPPercentage());

    // Draw foreground
    fill_rectangle(bar_color, x, y, current_width, height);

    // Draw border
    draw_rectangle(COLOR_BLACK, x, y, width, height);

    // Display HP text with shadow
    string hp_text = to_string(fighter.getHP()) + "/" + to_string(fighter.getMaxHP());
    draw_text(hp_text, COLOR_BLACK, x + width / 2 - 29, y + height / 2 - 7);
    draw_text(hp_text, COLOR_WHITE, x + width / 2 - 30, y + height / 2 - 8);
}

/**
 * calculate_damage - Determines final damage value for an attack.
 */
int calculate_damage(Fighter &attacker, Fighter &defender, Move &move) {
    double base_damage = (attacker.getAttack() * move.damage) / (double)defender.getDefense();
    double type_multiplier = get_type_multiplier(move.type, defender.getType());
    double random_factor = 0.9 + (rand() % 20) / 100.0;
    int final_damage = (int)(base_damage * type_multiplier * random_factor);

    if (final_damage < 1) {
        final_damage = 1;
    }

    return final_damage;
}

/**
 * get_fighter_color - Returns color for fighter based on type.
 */
color get_fighter_color(string fighter_type) {
    if (fighter_type == "Fire") return COLOR_ORANGE;
    if (fighter_type == "Water") return COLOR_BLUE;
    if (fighter_type == "Grass") return COLOR_GREEN;
    if (fighter_type == "Electric") return COLOR_YELLOW;
    return COLOR_GRAY;
}

/**
 * draw_fighter - Draws fighter with shadow and 3D effect.
 */
void draw_fighter(Fighter &fighter, int x, int y) {
    color fighter_color = get_fighter_color(fighter.getType());

    // Draw shadow
    fill_circle(rgba_color(0, 0, 0, 100), x + 5, y + 5, 60);

    // Draw main body
    fill_circle(fighter_color, x, y, 60);

    // Draw outline
    draw_circle(COLOR_BLACK, x, y, 60);

    // Draw highlight
    fill_circle(rgba_color(255, 255, 255, 100), x - 15, y - 15, 20);
}

/**
 * draw_move_button - Draws enhanced move button with hover effect.
 */
void draw_move_button(Move &move, int x, int y, int width, int height, bool is_hovered) {
    color button_bg = is_hovered ? COLOR_LIGHT_BLUE : COLOR_WHITE;

    // Draw shadow
    fill_rectangle(rgba_color(0, 0, 0, 50), x + 3, y + 3, width, height);

    // Draw background
    fill_rectangle(button_bg, x, y, width, height);

    // Draw border (thicker if hovered)
    if (is_hovered) {
        draw_rectangle(COLOR_BLUE, x - 1, y - 1, width + 2, height + 2);
    }
    draw_rectangle(COLOR_BLACK, x, y, width, height);

    // Draw move name
    draw_text(move.name, COLOR_BLACK, "Arial", 16, x + 10, y + 8);

    // Draw type and power
    string info_text = "Type: " + move.type + " | Power: " + to_string(move.damage);
    draw_text(info_text, COLOR_DARK_GRAY, "Arial", 11, x + 10, y + 28);
}

/**
 * Main Function - Fixed UI with proper turn flow.
 */
int main() {
    open_window("Battle Simulator - Enhanced", 800, 600);

    // Create fighters
    Fighter player("Charizard", 100, 52, 43, 100, "Fire");
    player.addMove(Move("Fire Blast", 45, "Fire"));
    player.addMove(Move("Tackle", 20, "Normal"));
    player.addMove(Move("Flamethrower", 35, "Fire"));

    Fighter enemy("Blastoise", 100, 48, 50, 78, "Water");
    enemy.addMove(Move("Water Gun", 40, "Water"));
    enemy.addMove(Move("Tackle", 20, "Normal"));
    enemy.addMove(Move("Hydro Pump", 50, "Water"));

    // Game state
    bool player_turn = true;
    int turn_number = 1;
    string battle_log = "Battle Start! Choose your move!";

    // Animation variables
    int player_x = 200;
    int enemy_x = 600;
    bool animating = false;
    int animation_frame = 0;

    // AI delay system
    unsigned int ai_action_time = 0;
    bool ai_waiting = false;

    // Main game loop
    while (!quit_requested()) {
        process_events();

        // Clear with gradient background
        clear_screen(rgb_color(135, 206, 235));

        // Draw ground
        fill_rectangle(rgb_color(34, 139, 34), 0, 400, 800, 200);

        // Handle attack animation
        if (animating) {
            animation_frame++;
            if (animation_frame < 8) {
                player_x += 3; // Move forward
            } else if (animation_frame < 16) {
                player_x -= 3; // Move back
            } else {
                animating = false;
                animation_frame = 0;
                player_x = 200; // Reset position
            }
        }

        // Draw fighters
        draw_fighter(player, player_x, 280);
        draw_text(player.getName() + " (" + player.getType() + ")", COLOR_BLACK, "Arial", 18, 120, 220);
        draw_hp_bar(player, 100, 350, 200, 25);

        draw_fighter(enemy, enemy_x, 280);
        draw_text(enemy.getName() + " (" + enemy.getType() + ")", COLOR_BLACK, "Arial", 18, 520, 220);
        draw_hp_bar(enemy, 500, 350, 200, 25);

        // Draw turn info
        fill_rectangle(rgba_color(0, 0, 0, 180), 300, 20, 200, 40);
        string turn_text = "Turn " + to_string(turn_number);
        draw_text(turn_text, COLOR_WHITE, "Arial", 24, 340, 30);

        // Draw battle log
        fill_rectangle(rgba_color(0, 0, 0, 180), 20, 430, 760, 50);
        draw_text(battle_log, COLOR_WHITE, "Arial", 16, 30, 445);

        // Check for battle end
        if (!player.isAlive()) {
            fill_rectangle(rgba_color(0, 0, 0, 200), 0, 0, 800, 600);
            fill_rectangle(rgba_color(139, 0, 0, 230), 150, 180, 500, 240);
            draw_rectangle(COLOR_BLACK, 150, 180, 500, 240);

            draw_text("YOU LOST!", COLOR_RED, "Arial", 56, 250, 240);
            draw_text("Better luck next time!", COLOR_WHITE, "Arial", 20, 270, 300);
            draw_text("Press SPACE to restart", COLOR_YELLOW, "Arial", 22, 250, 360);

            if (key_typed(SPACE_KEY)) {
                player = Fighter("Charizard", 100, 52, 43, 100, "Fire");
                player.addMove(Move("Fire Blast", 45, "Fire"));
                player.addMove(Move("Tackle", 20, "Normal"));
                player.addMove(Move("Flamethrower", 35, "Fire"));

                enemy = Fighter("Blastoise", 100, 48, 50, 78, "Water");
                enemy.addMove(Move("Water Gun", 40, "Water"));
                enemy.addMove(Move("Tackle", 20, "Normal"));
                enemy.addMove(Move("Hydro Pump", 50, "Water"));

                turn_number = 1;
                player_turn = true;
                battle_log = "Battle Start! Choose your move!";
                player_x = 200;
                ai_waiting = false;
            }
        } else if (!enemy.isAlive()) {
            fill_rectangle(rgba_color(0, 0, 0, 200), 0, 0, 800, 600);
            fill_rectangle(rgba_color(0, 100, 0, 230), 150, 180, 500, 240);
            draw_rectangle(COLOR_BLACK, 150, 180, 500, 240);

            draw_text("YOU WON!", COLOR_GREEN, "Arial", 56, 250, 240);
            draw_text("Congratulations!", COLOR_WHITE, "Arial", 20, 290, 300);
            draw_text("Press SPACE to restart", COLOR_YELLOW, "Arial", 22, 250, 360);

            if (key_typed(SPACE_KEY)) {
                player = Fighter("Charizard", 100, 52, 43, 100, "Fire");
                player.addMove(Move("Fire Blast", 45, "Fire"));
                player.addMove(Move("Tackle", 20, "Normal"));
                player.addMove(Move("Flamethrower", 35, "Fire"));

                enemy = Fighter("Blastoise", 100, 48, 50, 78, "Water");
                enemy.addMove(Move("Water Gun", 40, "Water"));
                enemy.addMove(Move("Tackle", 20, "Normal"));
                enemy.addMove(Move("Hydro Pump", 50, "Water"));

                turn_number = 1;
                player_turn = true;
                battle_log = "Battle Start! Choose your move!";
                enemy_x = 600;
                ai_waiting = false;
            }
        } else {
            // Gameplay UI
            fill_rectangle(rgba_color(50, 50, 50, 220), 0, 490, 800, 110);

            if (player_turn && !ai_waiting) {
                draw_text("Your Turn! Choose a move:", COLOR_YELLOW, "Arial", 20, 280, 500);

                vector<Move> moves = player.getMoves();

                for (int i = 0; i < moves.size(); i++) {
                    int button_x = 50 + (i * 250);
                    int button_y = 530;
                    int button_width = 230;
                    int button_height = 55;

                    bool is_hovered = (mouse_x() >= button_x && mouse_x() <= button_x + button_width &&
                                       mouse_y() >= button_y && mouse_y() <= button_y + button_height);

                    draw_move_button(moves[i], button_x, button_y, button_width, button_height, is_hovered);

                    if (is_hovered && mouse_clicked(LEFT_BUTTON)) {
                        Move chosen_move = moves[i];
                        int damage = calculate_damage(player, enemy, chosen_move);
                        double type_mult = get_type_multiplier(chosen_move.type, enemy.getType());

                        enemy.takeDamage(damage);

                        battle_log = player.getName() + " used " + chosen_move.name + "! " + to_string(damage) + " damage!";

                        if (type_mult > 1.0) {
                            battle_log += " It's super effective!";
                        } else if (type_mult < 1.0) {
                            battle_log += " It's not very effective...";
                        }

                        player_turn = false;
                        animating = true;
                        animation_frame = 0;

                        ai_action_time = current_ticks() + 2000;
                        ai_waiting = true;
                    }
                }
            } else if (!player_turn) {
                draw_text("Enemy's Turn...", COLOR_ORANGE, "Arial", 22, 300, 530);
                draw_text("Wait for enemy to attack", COLOR_WHITE, "Arial", 16, 280, 560);

                if (ai_waiting && current_ticks() >= ai_action_time) {
                    ai_waiting = false;

                    vector<Move> enemy_moves = enemy.getMoves();
                    int random_move_index = rand() % enemy_moves.size();
                    Move ai_move = enemy_moves[random_move_index];

                    int damage = calculate_damage(enemy, player, ai_move);
                    double type_mult = get_type_multiplier(ai_move.type, player.getType());

                    player.takeDamage(damage);

                    battle_log = enemy.getName() + " used " + ai_move.name + "! " + to_string(damage) + " damage!";

                    if (type_mult > 1.0) {
                        battle_log += " It's super effective!";
                    } else if (type_mult < 1.0) {
                        battle_log += " It's not very effective...";
                    }

                    player_turn = true;
                    turn_number++;
                }
            }
        }

        refresh_screen(60);
    }
}

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Pong.h"
#include "raylib.h"
using namespace std;

/**
 * Initializes the game window, audio, and resources.
 */
void initialize_window(){
    InitWindow(WIDTH, HEIGHT, "Pong");
    SetTargetFPS(FPS);
    InitAudioDevice();
    // Load sound effects and textures for the game
    lost_heart = LoadSound("../resources/Lost_Heart.wav");
    lost_game = LoadSound("../resources/Lost_Game.wav");
    paddle_hard_hit = LoadSound("../resources/Paddle_Hard_Hit.wav");
    paddle_hit = LoadSound("../resources/Paddle_Hit.wav");
    Heart = LoadTexture("../resources/Heart.png");
}

/**
 * Closes the game window and unloads all resources.
 */
void close_window(){
    UnloadTexture(Heart);
    UnloadSound(paddle_hit);
    UnloadSound(paddle_hard_hit);
    UnloadSound(lost_game);
    UnloadSound(lost_heart);
    CloseAudioDevice();
    CloseWindow();
}

/**
 * Draws the game scene including the ball, paddles, lives, and score.
 * 
 * @param Ball_Color The color of the ball.
 */
void draw_game(Color Ball_Color){
    BeginDrawing();
    ClearBackground(Dark_Blue);
    DrawRectangle(WIDTH / 2, 0, WIDTH/2, HEIGHT, Blue);  // Game field
    DrawCircle(WIDTH / 2, HEIGHT / 2, 150, Light_Blue);  // Background circle
    // Draw remaining lives as heart icons
    for (int life = stats.lives; life > 0; life--) {
        float xPos = (float)life * 50 + 520;
        float yPos = 2.5;
        DrawTexturePro(Heart,
                       (Rectangle){0, 0, (float)Heart.width, (float)Heart.height},
                       (Rectangle){xPos, yPos, 50.0, 50.0},
                       (Vector2){0, 0}, 0.0, WHITE);
    }
    // Draw texts: Lives and rally score
    DrawText(TextFormat("Lives Remaining:"), 325, 15, 30, BLACK);
    DrawText(TextFormat("Rally Score: %d", stats.rally), 380, 550, 30, YELLOW);
    ball.draw(Ball_Color);  // Draw the ball
    paddle.draw();          // Draw player paddle
    cpuPaddle.draw();       // Draw CPU paddle
    DrawFPS(925, 0);        // Display FPS
    EndDrawing();
}

/**
 * Draws the main menu screen.
 * Listens for button presses to switch to game or settings.
 */
void draw_main_menu_screen() {
    BeginDrawing();
    ClearBackground(Dark_Blue);
    start_button.draw();    // Draw the start button
    settings_button.draw(); // Draw the settings button
    // Transition to game or settings screen based on button press
    if (start_button.isPressed()) {main_menu = false; start_button.reset();}
    if (settings_button.isPressed()) {main_menu = false; settings = true; settings_button.reset();}
    EndDrawing();
}

/**
 * Draws the settings screen and allows the user to adjust speed settings.
 */
void draw_settings_screen(){
    BeginDrawing();
    ClearBackground(Dark_Blue);
    set_speed_button.draw();   // Draw the set speed button
    speed_box.draw();          // Draw input box for speed
    speed_box.Update();        // Update the speed box input
    exit_settings.draw();      // Draw the exit settings button
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << stats.speed;
    DrawText(TextFormat("Speed Increase Rate = %s", ss.str().c_str()), 280, 0, 30, BLACK);
    // Set the speed based on input
    if (set_speed_button.isPressed()) {stats.speed = atof(speed_box.getText()); set_speed_button.reset();}
    if (exit_settings.isPressed()) {stats.lives = LIVES; settings = false; main_menu = true; exit_settings.reset();}
    EndDrawing();
}

/**
 * Draws the end screen when the game is over and handles replay options.
 */
void draw_end_screen() {
    BeginDrawing();
    ClearBackground(Dark_Blue);
    if (play_sound_once) {PlaySound(lost_game); play_sound_once = false;}
    exit_end_screen.draw();   // Draw exit button on the end screen
    // Reset game when exit button is pressed
    if (exit_end_screen.isPressed()) {stats.lives = LIVES; settings = false; main_menu = true; exit_end_screen.reset();play_sound_once = true;}
    EndDrawing();
}

/**
 * Handles the ball's position, checking for collisions and updating scores.
 */
void ball_pos(){
    // Handle paddle-ball collisions
    if (paddle.collided(ball.position, ball.radius) || cpuPaddle.collided(ball.position, ball.radius)){
        ball.speed_x *= -stats.speed; //increase speed
        ball.speed_y *= stats.speed; //increase speed
        stats.rally += 1;
        if (paddle.collided(ball.position, ball.radius)) {
            PlaySound(paddle_hard_hit);  // Play sound if player paddle is hit
        } else {
            PlaySound(paddle_hit);  // Play sound if CPU paddle is hit
        }
    } 
    // Handle scoring
    else if (ball.position.x + ball.radius >= WIDTH - cpuPaddle.size.x + 10 || ball.position.x - ball.radius <= 0 + paddle.size.x + 10) {
        ball.reset((float) WIDTH / 2, (float) HEIGHT / 2, ball.speed_x *= -stats.speed, ball.speed_y *= -stats.speed);
        draw_game(RED);
        ball.speed_x = 5;  // Reset ball speed
        ball.speed_y = 5;
        // Update player or computer score
        if (ball.position.x + ball.radius >= WIDTH - cpuPaddle.size.x + 10) {stats.player_score += 1;}
        else{stats.computer_score += 1;}
        PlaySound(lost_heart);  // Play sound on scoring
        WaitTime(.25);  // Wait after scoring
        // Track highest rally
        if (stats.rally >= stats.highest_rally) {stats.highest_rally = stats.rally;}
        stats.lives -= 1; // Decrease lives
        stats.rally = 0;  // Reset rally
    }
    // Handle vertical ball bounce
    if (ball.position.y + ball.radius >= HEIGHT || ball.position.y - ball.radius <= 0) {
        ball.speed_y *= -1;
    }
    ball.move();  // Move the ball
}

/**
 * Handles user input to move paddles.
 */
void key_inputs(){
    if (IsKeyDown(KEY_W)) paddle.move(-15.0); //move up
    if (IsKeyDown(KEY_S)) paddle.move(15.0);  //move down
    cpuPaddle.update(ball.position, ball.speed_y); // Update CPU paddle position
}

/**
 * Main game loop. Initializes window, handles screen transitions, and updates game state.
 */
void run(){
    initialize_window();  // Initialize window and resources
    while (!WindowShouldClose()) {
        if (main_menu) {
            draw_main_menu_screen();  // Draw main menu
        } else if (settings) {
            draw_settings_screen();  // Draw settings
        } else if (stats.lives == 0) {
            draw_end_screen();  // Draw end screen if no lives remain
        } else if (!main_menu && !settings){
            ball_pos();     // Update ball position
            key_inputs();   // Handle user input
            draw_game(Yellow); // Draw the game
        }
    }
    close_window(); // Close window and resources
}

/**
 * Main function to start the game.
 */
int main(){
    run(); // Start game
    return 0;
}

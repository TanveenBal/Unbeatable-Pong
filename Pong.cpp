#include <iostream>
#include "Pong.h"
#include "src/raylib.h"
using namespace std;


void initialize_window(){
    InitWindow(WIDTH, HEIGHT, "Pong");
    SetTargetFPS(FPS);
    InitAudioDevice();
    lost_heart = LoadSound("resources/Lost_Heart.wav");
    lost_game = LoadSound("resources/Lost_Game.wav");
    paddle_hard_hit = LoadSound("resources/Paddle_Hard_Hit.wav");
    paddle_hit = LoadSound("resources/Paddle_Hit.wav");
    Heart = LoadTexture("resources/Heart.png");
}

void close_window(){
    UnloadTexture(Heart);
    UnloadSound(paddle_hit);
    UnloadSound(paddle_hard_hit);
    UnloadSound(lost_game);
    UnloadSound(lost_heart);
    CloseAudioDevice();
    CloseWindow();
}

void draw_game(Color Ball_Color){
    BeginDrawing();
    ClearBackground(Dark_Blue);
    DrawRectangle(WIDTH / 2, 0, WIDTH/2, HEIGHT, Blue);
    DrawCircle(WIDTH / 2, HEIGHT / 2, 150, Light_Blue);
    for (int life = stats.lives; life > 0; life--) {
        float xPos = (float)life * 50 + 520;
        float yPos = 2.5;
        DrawTexturePro(Heart,
                       (Rectangle){0, 0, (float)Heart.width, (float)Heart.height},
                       (Rectangle){xPos, yPos, 50.0, 50.0},
                       (Vector2){0, 0}, 0.0, WHITE);
    }
    DrawText(TextFormat("Lives Remaining:"), 325, 15, 30, BLACK);
    DrawText(TextFormat("Rally Score: %d", stats.rally), 380, 550, 30, YELLOW);
    ball.draw(Ball_Color);
    paddle.draw();
    cpuPaddle.draw();
    DrawFPS(925, 0);
    EndDrawing();
}

void draw_main_menu_screen() {
    BeginDrawing();
    ClearBackground(Dark_Blue);
    start_button.draw();
    settings_button.draw();
    if (start_button.isPressed()) {main_menu = false; start_button.reset();}
    if (settings_button.isPressed()) {main_menu = false; settings = true; settings_button.reset();}
    EndDrawing();
}

void draw_settings_screen(){
    BeginDrawing();
    ClearBackground(Dark_Blue);
    set_speed_button.draw();
    speed_box.draw();
    speed_box.Update();
    exit_settings.draw();
    DrawText(TextFormat("Speed Increase Rate = %f", stats.speed), 280, 0, 30, BLACK);
    if (set_speed_button.isPressed()) {stats.speed = atof(speed_box.getText()); set_speed_button.reset();}
    if (exit_settings.isPressed()) {settings = false; main_menu = true; exit_settings.reset();}
    EndDrawing();
}

void draw_end_screen(){
    BeginDrawing();
    ClearBackground(Dark_Blue);
    if (play_sound_once) {PlaySound(lost_game); play_sound_once = false;}

    EndDrawing();
}

void ball_pos(){
    if (paddle.collided(ball.position, ball.radius) || cpuPaddle.collided(ball.position, ball.radius)){
        ball.speed_x *= -stats.speed; //increase speed
        ball.speed_y *= stats.speed; //increase speed
        stats.rally += 1;
        if (paddle.collided(ball.position, ball.radius)) {
            PlaySound(paddle_hard_hit);
        } else {
            PlaySound(paddle_hit);
        }
    }else if (ball.position.x + ball.radius >= WIDTH - cpuPaddle.size.x + 10 || ball.position.x - ball.radius <= 0 + paddle.size.x + 10) {
        ball.reset((float) WIDTH / 2, (float) HEIGHT / 2, ball.speed_x *= -stats.speed, ball.speed_y *= -stats.speed);
        draw_game(RED);
        ball.speed_x = 5;
        ball.speed_y = 5;
        if (ball.position.x + ball.radius >= WIDTH - cpuPaddle.size.x + 10) {stats.player_score += 1;}
        else{stats.computer_score += 1;}
        PlaySound(lost_heart);
        WaitTime(.25);
        if (stats.rally >= stats.highest_rally) {stats.highest_rally = stats.rally;}
        stats.lives -= 1;
        stats.rally = 0;
    }
    if (ball.position.y + ball.radius >= HEIGHT || ball.position.y - ball.radius <= 0) {
        ball.speed_y *= -1;
    }
    ball.move();
}

void key_inputs(){
    if (IsKeyDown(KEY_W)) paddle.move(-15.0); //move up
    if (IsKeyDown(KEY_S)) paddle.move(15.0); //move down
    cpuPaddle.update(ball.position, ball.speed_y);
}

void run(){
    initialize_window();
    while (!WindowShouldClose()) {
        if (main_menu) {
            draw_main_menu_screen();
        } else if (settings) {
            draw_settings_screen();
        } else if (stats.lives == 0) {
            draw_end_screen();
        } else if (!main_menu && !settings){
            ball_pos();
            key_inputs();
            draw_game(Yellow);
        }

    }
    close_window();
}

int main(){
    run();
    return 0;
}

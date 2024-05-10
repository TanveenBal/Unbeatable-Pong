#include <iostream>
#include "Pong.h"
#include "src/raylib.h"
using namespace std;

Ball ball((float)WIDTH / 2, (float)HEIGHT / 2, 5, 5, 15);
Paddle paddle(10.0, (float)HEIGHT / 2 - 50, 10.0, 100.0);
CPU_Paddle cpuPaddle(980.0, (float)HEIGHT / 2 - 50, 10.0, 100.0);
Stats stats;
Button start_button({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT},
                   GRAY, BLACK, "Start");
Button settings_button({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2 + 100, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT},
                      GRAY, BLACK, "Settings");

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
    ClearBackground(BLACK);
    for (int life = 3; life > stats.computer_score; life--) {
        float xPos = (float)-life * 30 + 655;
        float yPos = 2.5;
        DrawTexturePro(Heart,
                       (Rectangle){30, 0, (float)Heart.width, (float)Heart.height},
                       (Rectangle){xPos, yPos, 30.0, 30.0},
                       (Vector2){0, 0}, 0.0, WHITE);
    }
    DrawText(TextFormat("Lives Remaining:"), 325, 0, 30, BLUE);
    DrawText(TextFormat("Rally Score: %d", stats.rally), 380, 550, 30, YELLOW);
    ball.draw(Ball_Color);
    paddle.draw(BLUE);
    cpuPaddle.draw(RED);
    DrawFPS(925, 0);
    EndDrawing();
}

void draw_start_screen(){
    BeginDrawing();
    start_button.draw();
    if (start_button.isPressed()) {
        start = false;
    }
    settings_button.draw();
    if (settings_button.isPressed()) {
        settings = true;
    }
    EndDrawing();
}

void draw_end_screen(){
    BeginDrawing();
    if (play_sound_once){
        PlaySound(lost_game);
        play_sound_once = false;
    }
    ClearBackground(BLACK);
    EndDrawing();
}

void ball_pos(){
    if (paddle.collided(ball.position, ball.radius) || cpuPaddle.collided(ball.position, ball.radius)){
        ball.speed_x *= -SPEED; //increase speed
        ball.speed_y *= SPEED; //increase speed
        stats.rally += 1;
        if (paddle.collided(ball.position, ball.radius)){
            PlaySound(paddle_hard_hit);
        }else{
            PlaySound(paddle_hit);
        }
    }else if (ball.position.x + ball.radius >= WIDTH - cpuPaddle.size.x + 10 || ball.position.x - ball.radius <= 0 + paddle.size.x + 10){
        ball.reset((float) WIDTH / 2, (float) HEIGHT / 2, ball.speed_x *= -SPEED, ball.speed_y *= -SPEED);
        draw_game(YELLOW);
        ball.speed_x = 5;
        ball.speed_y = 5;
        if (ball.position.x + ball.radius >= WIDTH - cpuPaddle.size.x + 10){stats.player_score += 1;}
        else{stats.computer_score += 1;}
        PlaySound(lost_heart);
        WaitTime(.25);
        if (stats.rally >= stats.highest_rally){stats.highest_rally = stats.rally;}
        stats.lives -= 1;
        stats.rally = 0;
    }
    if (ball.position.y + ball.radius >= HEIGHT || ball.position.y - ball.radius <= 0){
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
    while (!WindowShouldClose()){
        if (start){
            draw_start_screen();
        }
        else if (stats.lives == 0){
            draw_end_screen();
        } else {
            ball_pos();
            key_inputs();
            draw_game(WHITE);
        }

    }
    close_window();
}

int main(){
    run();
    return 0;
}

#include "src/raylib.h"

#ifndef UNBEATABLE_PONG_PONG_H
bool start = true;
bool settings = false;
bool play_sound_once = true;
const int WIDTH = 1000;
const int HEIGHT = 600;
const int BUTTON_HEIGHT = 50;
const int BUTTON_WIDTH = 400;
const int FPS = 60;
float SPEED = 1.1;
Sound lost_heart;
Sound lost_game;
Sound paddle_hard_hit;
Sound paddle_hit;
Texture2D Heart;

struct Stats {
    int rally = 0;
    int highest_rally = 0;
    int player_score = 0;
    int computer_score = 0;
    int lives = 3;
};

class Ball {
public:
    Vector2 position;
    float speed_x;
    float speed_y;
    const float radius;

    Ball(const float & X, const float & Y, const float & initialSpeedX, const float & initialSpeedY, const float & ballRadius)
            : position({X, Y}), speed_x(initialSpeedX), speed_y(initialSpeedY), radius(ballRadius) {}

    void move() {
        position.x += speed_x;
        position.y += speed_y;
    }

    void draw(const Color & color) const {
        DrawCircleV(position, radius, color);
    }

    void reset(const float & X, const float &Y, const float & newSpeedX, const float & newSpeedY) {
        position = {X, Y};
        speed_x = newSpeedX;
        speed_y = newSpeedY;
    }
};

class Paddle {
public:
    Vector2 position;
    Vector2 size;

    Paddle(const float & X, const float & Y, const float & width, const float & height)
            : position({X, Y}), size({width, height}) {}

    void draw(const Color & color) const {
        DrawRectangleV(position, size, color);
    }

    void limit_movement() {
        if (position.y + size.y >= HEIGHT){
            position.y = HEIGHT - size.y;
        } else if (position.y <= 0){
            position.y = 0;
        }
    }

    void move(const float & dy) {
        position.y += dy;
        limit_movement();
    }

    bool collided(const Vector2 & ballPos, const float & ballRad){
        return CheckCollisionCircleRec(ballPos, ballRad, Rectangle {position.x, position.y, size.x, size.y});
    }
};

class CPU_Paddle: public Paddle{
public:
    CPU_Paddle(const float & X, const float & Y, const float & width, const float & height) : Paddle(X, Y, width, height) {}

    void update(const Vector2 & ballPos, const float & ballSpeedY){
        if (ballPos.x >= 650){
            if (position.y + size.y / 2 <= ballPos.y) {
                position.y += std::abs(ballSpeedY);
            } else if (position.y + size.y / 2 > ballPos.y) {
                position.y -= std::abs(ballSpeedY);
            }
        } else {
            if (position.y < HEIGHT/2 - size.y/2) {
                position.y += 1;
            } else if (position.y > HEIGHT/2 - size.y/2){
                position.y -= 1;
            }
        }
        limit_movement();
    }
};


class Button {
private:
    Rectangle bounds;
    Color color;
    Color textColor;
    std::string text;
    bool pressed;

public:
    Button(Rectangle bounds, Color color, Color textColor, std::string text)
            : bounds(bounds), color(color), textColor(textColor), text(text), pressed(false) {}

    void draw() {
        DrawRectangleRec(bounds, color);
        if (CheckCollisionPointRec(GetMousePosition(), bounds)) {
            DrawRectangleLinesEx(bounds, 3, RED);
            DrawText(text.c_str(), (int)(bounds.x + bounds.width / 2 - MeasureText(text.c_str(), 20) / 2),
                     (int)(bounds.y + bounds.height / 2 - 10), 20, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                pressed = true;
            }
        } else {
            DrawText(text.c_str(), (int)(bounds.x + bounds.width / 2 - MeasureText(text.c_str(), 20) / 2),
                     (int)(bounds.y + bounds.height / 2 - 10), 20, textColor);
        }
    }

    bool isPressed() const {
        return pressed;
    }

    void reset() {
        pressed = false;
    }
};

#define UNBEATABLE_PONG_PONG_H

#endif

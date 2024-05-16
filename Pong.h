#include "src/raylib.h"
using namespace std;

#ifndef UNBEATABLE_PONG_PONG_H
bool main_menu = true;
bool settings = false;
bool play_sound_once = true;
const int WIDTH = 1000;
const int HEIGHT = 600;
const int BUTTON_HEIGHT = 50;
const int BUTTON_WIDTH = 400;
const int FPS = 60;
Sound lost_heart;
Sound lost_game;
Sound paddle_hard_hit;
Sound paddle_hit;
Texture2D Heart;

Color Blue = Color{38, 154, 185, 255};
Color Dark_Blue = Color{20, 133, 160, 255};
Color Light_Blue = Color{129, 184, 204, 150};
Color Yellow = Color{243, 213, 91, 255};


struct Stats {
    int rally = 0;
    int highest_rally = 0;
    int player_score = 0;
    int computer_score = 0;
    int lives = 3;
    float speed = 1.1;
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
    Color color;

    Paddle(const float & X, const float & Y, const float & width, const float & height, const Color & color)
            : position({X, Y}), size({width, height}), color(color) {}

    void draw() const {
        DrawRectangleRounded(Rectangle{position.x, position.y, size.x, size.y}, 0.8, 0, color);
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
    CPU_Paddle(const float & X, const float & Y, const float & width, const float & height, const Color & color) : Paddle(X, Y, width, height, color) {}

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
    Button(const Rectangle & bounds, const Color & color, const Color & textColor, const string & text)
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

    bool isPressed() const {return pressed;}

    void reset() {pressed = false;}
};


class TextBox {
private:
    Rectangle box;
    char* text;
    int maxChars;
    int letterCount;
    int framesCounter;
    bool active;

public:
    TextBox(const float & x, const float & y, const float & width, const float & height, const int & maxChars) : box({x, y, width, height}), maxChars(maxChars), letterCount(0), framesCounter(0), active(false)
    {text = new char[maxChars + 1]();}

    void Update() {
        if (CheckCollisionPointRec(GetMousePosition(), box)) {
            active = true;
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
        } else {
            active = false;
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

        if (active) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125 && letterCount < maxChars) {
                    text[letterCount] = (char)key;
                    text[++letterCount] = '\0';
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                text[letterCount] = '\0';
            }
        }
    }

    void draw() const {
        DrawRectangleRec(box, active ? Light_Blue : Blue);
        DrawRectangleLinesEx(box, 1, active ? Yellow : Light_Blue);
        DrawText(text, (int)box.x + 5, (int)box.y + 8, 20, Yellow);

        if (active && letterCount < maxChars && (framesCounter / 20) % 2 == 0) {
            DrawText("_", (int)box.x + 5 + MeasureText(text, 20), (int)box.y + 8, 20, MAROON);
        }
    }

    const char* getText() const {return text;}

    ~TextBox() {delete[] text;}
};

Ball ball((float)WIDTH / 2, (float)HEIGHT / 2, 5, 5, 15);
Paddle paddle(10.0, (float)HEIGHT / 2 - 50, 20.0, 100.0, WHITE);
CPU_Paddle cpuPaddle(970.0, (float)HEIGHT / 2 - 50, 20.0, 100.0, WHITE);
Stats stats;
Button start_button({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT},
                    Light_Blue, BLACK, "Start");
Button settings_button({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2 + 100, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT},
                       Light_Blue, BLACK, "Settings");
Button set_speed_button({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2 + 100, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT},
                       Light_Blue, RED, "Confirm");
Button exit_settings({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2 + 200, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT},
                     Light_Blue, BLACK, "Exit");
TextBox speed_box((float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT, 10);


#define UNBEATABLE_PONG_PONG_H

#endif

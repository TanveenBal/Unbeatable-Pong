#include "raylib.h"
using namespace std;

#ifndef UNBEATABLE_PONG_PONG_H

// Global variables for the game state and resources
bool main_menu = true; // Flag for showing main menu
bool settings = false; // Flag for settings screen
bool play_sound_once = true; // Flag for playing the sound only once
const int WIDTH = 1000; // Window width
const int HEIGHT = 600; // Window height
const int BUTTON_HEIGHT = 50; // Button height
const int BUTTON_WIDTH = 400; // Button width
const int FPS = 60; // Frames per second
const int LIVES = 1; // Initial number of lives
Sound lost_heart; // Sound for lost heart
Sound lost_game; // Sound for lost game
Sound paddle_hard_hit; // Sound for hard paddle hit
Sound paddle_hit; // Sound for regular paddle hit
Texture2D Heart; // Heart image texture

// Customizable Color constants
Color Blue = Color{38, 154, 185, 255};
Color Dark_Blue = Color{20, 133, 160, 255};
Color Light_Blue = Color{129, 184, 204, 150};
Color Yellow = Color{243, 213, 91, 255};

/**
 * Stats structure for storing game statistics.
 */
struct Stats {
    int rally = 0;         // Current rally score
    int highest_rally = 0; // Highest rally score
    int player_score = 0;  // Player score
    int computer_score = 0; // Computer score
    int lives = LIVES;     // Number of lives
    float speed = 1.1f;    // Ball speed multiplier
};

class Ball {
public:
    Vector2 position;  // Ball position
    float speed_x;     // Speed in X direction
    float speed_y;     // Speed in Y direction
    const float radius; // Ball radius

    /**
     * Constructor for the Ball class.
     *
     * @param X Initial X position
     * @param Y Initial Y position
     * @param initialSpeedX Initial speed in X direction
     * @param initialSpeedY Initial speed in Y direction
     * @param ballRadius Ball radius
     */
    Ball(const float & X, const float & Y, const float & initialSpeedX, const float & initialSpeedY, const float & ballRadius)
            : position({X, Y}), speed_x(initialSpeedX), speed_y(initialSpeedY), radius(ballRadius) {}

    /** Moves the ball based on current speed. */
    void move() {
        position.x += speed_x;
        position.y += speed_y;
    }

    /**
     * Draws the ball to the screen.
     * @param color Ball color
     */
    void draw(const Color & color) const {
        DrawCircleV(position, radius, color);
    }

    /**
     * Resets the ball position and speed.
     *
     * @param X New X position
     * @param Y New Y position
     * @param newSpeedX New speed in X direction
     * @param newSpeedY New speed in Y direction
     */
    void reset(const float & X, const float &Y, const float & newSpeedX, const float & newSpeedY) {
        position = {X, Y};
        speed_x = newSpeedX;
        speed_y = newSpeedY;
    }
};

class Paddle {
public:
    Vector2 position; // Paddle position
    Vector2 size;     // Paddle size
    Color color;      // Paddle color

    /**
     * Constructor for Paddle class.
     *
     * @param X Initial X position
     * @param Y Initial Y position
     * @param width Paddle width
     * @param height Paddle height
     * @param color Paddle color
     */
    Paddle(const float & X, const float & Y, const float & width, const float & height, const Color & color)
            : position({X, Y}), size({width, height}), color(color) {}

    /** Draws the paddle to the screen. */
    void draw() const {
        DrawRectangleRounded(Rectangle{position.x, position.y, size.x, size.y}, 0.8, 0, color);
    }

    /** Restricts the paddle's movement within the window bounds. */
    void limit_movement() {
        if (position.y + size.y >= HEIGHT){
            position.y = HEIGHT - size.y;
        } else if (position.y <= 0){
            position.y = 0;
        }
    }

    /**
     * Moves the paddle.
     * 
     * @param dy Delta Y value to move the paddle
     */
    void move(const float & dy) {
        position.y += dy;
        limit_movement();
    }

    /**
     * Checks if the paddle collided with the ball.
     * 
     * @param ballPos Ball position
     * @param ballRad Ball radius
     * @return True if there is a collision, false otherwise
     */
    bool collided(const Vector2 & ballPos, const float & ballRad){
        return CheckCollisionCircleRec(ballPos, ballRad, Rectangle {position.x, position.y, size.x, size.y});
    }
};

class CPU_Paddle: public Paddle {
public:
    /**
     * Constructor for CPU_Paddle class.
     * 
     * @param X Initial X position
     * @param Y Initial Y position
     * @param width Paddle width
     * @param height Paddle height
     * @param color Paddle color
     */
    CPU_Paddle(const float & X, const float & Y, const float & width, const float & height, const Color & color) : Paddle(X, Y, width, height, color) {}

    /**
     * Updates CPU paddle's position based on the ball position and movement.
     * 
     * @param ballPos Ball position
     * @param ballSpeedY Ball's vertical speed
     */
    void update(const Vector2 & ballPos, const float & ballSpeedY) {
        if (ballPos.x >= 650){ // Follow ball only when it crosses a certain threshold
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
    Rectangle bounds; // Button bounds
    Color color;      // Button background color
    Color textColor;  // Button text color
    std::string text; // Button text
    bool pressed;     // Button press state

public:
    /**
     * Constructor for Button class.
     *
     * @param bounds Button bounds as a Rectangle
     * @param color Button color
     * @param textColor Button text color
     * @param text Text to display on the button
     */
    Button(const Rectangle & bounds, const Color & color, const Color & textColor, const string & text)
            : bounds(bounds), color(color), textColor(textColor), text(text), pressed(false) {}

    /** Draws the button to the screen. */
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

    /** Returns whether the button was pressed. */
    bool isPressed() const {return pressed;}

    /** Resets the button press state. */
    void reset() {pressed = false;}
};

class TextBox {
private:
    Rectangle box;       // Text box bounds
    char* text;          // Text in the text box
    int maxChars;        // Max number of characters in text box
    int letterCount;     // Current number of letters
    int framesCounter;   // Frames counter for cursor blinking
    bool active;         // Active state of text box

public:
    /**
     * Constructor for TextBox class.
     *
     * @param x Initial X position
     * @param y Initial Y position
     * @param width Text box width
     * @param height Text box height
     * @param maxChars Maximum number of characters allowed
     */
    TextBox(const float & x, const float & y, const float & width, const float & height, const int & maxChars)
        : box({x, y, width, height}), maxChars(maxChars), letterCount(0), framesCounter(0), active(false) {
        text = new char[maxChars + 1]();
    }

    /** Updates the text box based on user input. */
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

    /** Draws the text box on screen. */
    void draw() const {
        DrawRectangleRec(box, active ? Light_Blue : Blue);
        DrawRectangleLinesEx(box, 1, active ? Yellow : Light_Blue);
        DrawText(text, (int)box.x + 5, (int)box.y + 8, 20, Yellow);

        if (active && letterCount < maxChars && (framesCounter / 20) % 2 == 0) {
            DrawText("_", (int)box.x + 5 + MeasureText(text, 20), (int)box.y + 8, 20, MAROON);
        }
    }

    /** Returns the text currently in the text box. */
    const char* getText() const {return text;}

    ~TextBox() {delete[] text;} // Destructor to clean up dynamically allocated text memory
};

// Instantiate game objects like Ball, Paddles, Stats, and Buttons
Ball ball((float)WIDTH / 2, (float)HEIGHT / 2, 5, 5, 15);
Paddle paddle(10.0, (float)HEIGHT / 2 - 50, 20.0, 100.0, WHITE);
CPU_Paddle cpuPaddle(970.0, (float)HEIGHT / 2 - 50, 20.0, 100.0, WHITE);
Stats stats;
Button start_button({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT}, Light_Blue, BLACK, "Start");
Button settings_button({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2 + 100, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT}, Light_Blue, BLACK, "Settings");
Button set_speed_button({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2 + 100, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT}, Light_Blue, RED, "Confirm");
Button exit_settings({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2 + 200, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT}, Light_Blue, BLACK, "Exit");
TextBox speed_box((float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT, 10);
Button exit_end_screen({(float)(WIDTH - BUTTON_WIDTH) / 2, (float)(HEIGHT - BUTTON_HEIGHT) / 2 + 200, (float)BUTTON_WIDTH, (float)BUTTON_HEIGHT}, Light_Blue, BLACK, "Exit");

#define UNBEATABLE_PONG_PONG_H

#endif

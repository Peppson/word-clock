#pragma once
#include "config.h"
#include "core/neoPixels.h"

class Snake : public NeoPixels 
{
    public:
        Snake(NeoPixels* pixel) : px(pixel) {};

        enum Direction 
        { 
            UP, DOWN, LEFT, RIGHT, NONE 
        };
        
        void newGame();
        void gameWon();
        void gameOver();
        bool newFrame();
        void loop(uint32_t& endTime);
        void setDirection(Direction direction);
        bool debugKeyboardInput(); 
        
    private:
        NeoPixels* px; 

        void colorSnakeBody(Color color, uint8_t index = 0);
        void colorSnakeHead(Color color);
        void colorSnakeTail(Color color);
        void colorFood(Color color);
        void updateHeadPosition();
        void updateBodyPosition(uint8_t headPos);
        bool checkCollisionWalls();
        bool checkCollisionSelf(const uint8_t inputPos);
        void spawnFood();
        void resetGame();
        bool inline processDirection(Direction direction);

        int _headX;
        int _headY;
        bool _pauseSnake = true;
        uint8_t _bodyPosition[TOTAL_LEDS] = {};
        uint8_t _bodySize;
        uint8_t _foodPosition;
        uint8_t _foodRandomSeed;
        uint8_t _directionBuf[2] = {NONE, NONE};
        uint32_t _newGameDelay = 0;
};

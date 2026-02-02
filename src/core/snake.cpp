#include "core/snake.h"


inline void Snake::colorSnakeHead(Color color) 
{
	px->led[ _bodyPosition[0] ] = color;
}

void Snake::colorSnakeBody(Color color, uint8_t index) 
{
	for (uint8_t i = index; i < (_bodySize - 1); i++) 
	{
		px->led[ _bodyPosition[i] ] = color;
	}
}

inline void Snake::colorSnakeTail(Color color) 
{
	uint8_t tailPos = _bodyPosition[_bodySize - 1];
	if (_foodPosition != tailPos) 
	{
		px->led[tailPos] = color;
	}
}

inline void Snake::colorFood(Color color) 
{
	px->led[_foodPosition] = color;
}

inline void Snake::updateHeadPosition() 
{
	switch (_directionBuf[0]) 
	{
		case UP:	_headY--;		break;
		case DOWN:	_headY++;		break;
		case LEFT:	_headX--;		break;
		case RIGHT:	_headX++;		break;
		default:					break;
	}
}

void Snake::updateBodyPosition(uint8_t headPos) 
{
	// Shift back body pos by 1, add new head pos 
	for (uint8_t i = (_bodySize - 1); i > 0; i--) 
	{
		_bodyPosition[i] = _bodyPosition[i - 1];
	}
	_bodyPosition[0] = headPos;
}

inline bool Snake::checkCollisionSelf(const uint8_t newPos) 
{
	for (uint8_t i = 0; i < (_bodySize - 1); i++) 
	{
		if (newPos == _bodyPosition[i]) 
			return true;
	}

	return false;
}

inline bool Snake::checkCollisionWalls() 
{
	if ((_headX > MATRIX_WIDTH - 1) ||
		(_headX == -1) ||
		(_headY > MATRIX_HEIGHT - 1) ||
		(_headY == -1)) 
	{
		return true;
	}

	return false;
}

inline void Snake::spawnFood() 
{
	uint8_t food;
	// Don't spawn food ontop of snake
	// Worst case here is a few μs on ESP32, good enough
	do 
	{
		food = (random8() + _foodRandomSeed) % TOTAL_LEDS;
	} while (checkCollisionSelf( ledIndexByIndex(food) ));

	_foodPosition = ledIndexByIndex(food);
	colorFood(Color::RED);
}

void Snake::resetGame() 
{
	uint32_t curTime = millis();
	_pauseSnake = true;
	_headX = 5;
	_headY = 5;
	_bodySize = 2;
	_bodyPosition[0] = ledIndexByXY(_headX, _headY);
	_foodRandomSeed = curTime % 256;
	_newGameDelay = curTime + 125; // ms
	_directionBuf[0] = NONE;
	_directionBuf[1] = NONE;
}

void Snake::newGame() 
{
	px->setLedsColor(Color::OFF, true);
	px->animation_spiral(px->getCurrentColor(), 10);
	px->setLedsColor(Color::OFF);
	resetGame();
	colorSnakeHead(Color::LIGHT_GREEN);
	spawnFood();
	FastLED.show();
}

void Snake::gameWon() 
{
	px->setLedsColor(Color::OFF, true);
	px->animation_fireworks(4, 4, LIGHT_BLUE, 72);
	px->animation_fireworks(8, 2, GREEN, 75);
	px->animation_fireworks(5, 3, ORANGE, 69);
	px->animation_fireworks(7, 4, LIGHT_BLUE, 72);
	px->animation_fireworks(3, 4, GREEN, 73);
	px->animation_fireworks(5, 4, LIGHT_GREEN, 72);
}

void Snake::gameOver()  
{
	colorFood(Color::OFF);
	colorSnakeBody(Color::RED); 
	FastLED.show();
	delay(300);

	// "Delete" snake back to front
	for (int16_t i = (_bodySize - 1); i >= 0; i--) 
	{
		px->led[ _bodyPosition[i] ] = OFF;
		FastLED.show();
		delay( (_bodySize < 50) ? 40 : 10 ); // ms
	}
}

bool Snake::newFrame() 
{
	uint8_t headPos = ledIndexByXY(_headX, _headY);

	// Victory?
	if (_bodySize == TOTAL_LEDS - 1) 
	{
		gameWon();
		return false;
	}
	// !Victory?
	if (checkCollisionSelf(headPos) || checkCollisionWalls()) 
	{
		gameOver();
		return false;
	}
	// Progress?
	if (headPos == _foodPosition) 
	{
		spawnFood();
		_bodySize++;
	}

	// Draw snake
	updateBodyPosition(headPos);
	colorSnakeHead(Color::LIGHT_GREEN);
	colorSnakeBody(Color::GREEN, 1);
	colorSnakeTail(Color::OFF);
	FastLED.show();
	return true;
}

void Snake::loop(uint32_t& endTime) 
{
	static uint32_t nextTime = 0;
	if (_pauseSnake || millis() < nextTime)
	{
		FastLED.show();
		return;
	}

	updateHeadPosition();
	if (!newFrame()) 
	{ 
		newGame();
		return; 
	}
	
	uint32_t now = millis();
	nextTime = now + SNAKE_GAME_SPEED;
	endTime = now + ACCESSPOINT_UPTIME;
}

void Snake::setDirection(Direction direction) 
{
	if (!processDirection(direction)) return; 

	// Change direction as soon as user inputs new direction,
	// instead of waiting the full loop() delay
	updateHeadPosition();
	_pauseSnake = false;

	if (!newFrame()) 
	{ 
		newGame(); 
	}
}

bool inline Snake::processDirection(Direction direction) 
{
	// Ignore inputs for 125ms after newGame(), cheap guard against "old" inputs from client
	if (_newGameDelay > millis()) return false; 

	// Move freely when only 1 "pixel" big
	if (_bodySize > 2) 
	{ 
		_directionBuf[1] = _directionBuf[0]; 
	}

	// Dont allow opposite direction (self collision)
	switch (direction) 
	{
		case UP:
			if (_directionBuf[1] != DOWN) 
			{ 
				_directionBuf[0] = UP; 
				return true;
			}
			break;
		case DOWN:
			if (_directionBuf[1] != UP) 
			{ 
				_directionBuf[0] = DOWN; 
				return true;
			}
			break;
		case LEFT:
			if (_directionBuf[1] != RIGHT) 
			{ 
				_directionBuf[0] = LEFT; 
				return true;
			}
			break;
		case RIGHT:
			if (_directionBuf[1] != LEFT) 
			{ 
				_directionBuf[0] = RIGHT; 
				return true;
			}
			break;
		default:
			break;
	}	

	return false; // Invalid direction
}


// ######## Debug ########
#ifdef DEBUG_SNAKE
	bool Snake::debugKeyboardInput() 
	{
		if (Serial.available() > 0) 
		{
			char key = Serial.read();
			switch (key) 
			{
				case 'w':	_headY--; 	break;
				case 'a':	_headX--;	break;
				case 's':	_headY++; 	break;
				case 'd':	_headX++;	break;
				default:				break;
			}
			return true;
		}

		return false;
	}
#endif // DEBUG_SNAKE

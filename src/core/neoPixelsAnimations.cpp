#include "core/neoPixels.h"
#include "core/hardware.h"
#include "ledMapping/ledMapping.h"
#include "ledMapping/ledDataShapes.h"
#if FRIEND_CLOCK
	#include "ledMapping/ledFriendAnimation.h"
#endif


void NeoPixels::animation_startup() 
{
	uint8_t initialLedBrightness = ledBrightness;

	#if FRIEND_CLOCK
		friend_startup_animation(this);
	#else		
		animation_startupFillIn(StartupShape::DOTS, 0, _currentColor);	
		/* 
		switch (esp_random() % 5) 
		{
			case 0: animation_startupFillIn(StartupShape::DOTS); break;
			case 1: animation_gameOfLife(3*1000); break;
			case 2: animation_waterDrops(); break;
			case 3: animation_startupFadeIn(StartupShape::HEART); break;
			case 4: animation_flag(StartupShape::FLAG); break;
		}  
		*/		  
	#endif

	ledBrightness = initialLedBrightness;
	setLedBrightness(ledBrightness);
}

void NeoPixels::animation_gameOfLife(uint32_t maxDuration) 
{
	bool curState[COL][ROW] = {};
	bool newState[COL][ROW] = {};
	bool oldState[COL][ROW] = {};

	maxDuration += millis();
	setRandomState(curState);

	while (maxDuration > millis()) 
	{
		calculateNewState(curState, newState);

		// Game ended? Catch some oscillating states
		if (isArrayEqual(curState, newState) || isArrayEqual(oldState, newState)) 
		{
			break;
		}

		drawGameOfLife(newState);

		std::swap(curState, newState);
		std::swap(newState, oldState);
		delay(60);
	}

	animation_fadeAll(FADE_OUT);
}


/* ########## Game of life helpers ########## */


void NeoPixels::calculateNewState(bool (&curState)[COL][ROW], bool (&newState)[COL][ROW]) 
{
	for (uint8_t y = 0; y < COL; y++) 
	{
		for (uint8_t x = 0; x < ROW; x++) 
		{
			bool aliveCell = curState[y][x];
			uint8_t aliveNeighbours = getAliveNeighbours(curState, x, y);

			// Any dead cell with exactly 3 alive neighbours becomes a live cell
			if (!aliveCell && aliveNeighbours == 3) 
			{
				newState[y][x] = true;
			} 
			// Any live cell with fewer than 2, or more than 3 live neighbours dies
			else if (aliveCell && (aliveNeighbours < 2 || aliveNeighbours > 3)) 
			{
				newState[y][x] = false;
			
			} 
			else // No change 
			{
				newState[y][x] = curState[y][x];
			}
		}
	}
}

uint8_t NeoPixels::getAliveNeighbours(bool (&state)[COL][ROW], uint8_t posX, uint8_t posY) 
{
	uint8_t aliveNeighbours = 0;

	// Check the state of each adjacent cells
	//			#  #  #
	//			#  X  #
	//			#  #  #
	for (int8_t i = - 1; i < 2; i++) 
	{
		for (int8_t j = - 1; j < 2; j++) 
		{	
			if (i == 0 && j == 0) continue; 

			// Let positions wrap around if on edge
			int8_t x = posX + j;  
			int8_t y = posY + i;
			uint8_t curRow = (x + ROW) % ROW;
			uint8_t curCol = (y + COL) % COL;

			if (state[curCol][curRow]) 
			{
				aliveNeighbours++;
			}
		}
	}

	return aliveNeighbours;
}

void NeoPixels::setRandomState(bool (&state)[COL][ROW]) 
{
	random16_set_seed(esp_random());

	for (uint8_t y = 0; y < COL; y++) 
	{
		for (uint8_t x = 0; x < ROW; x++) 
		{
			state[y][x] = random8() % 2;
		}
	}
}

bool NeoPixels::isArrayEqual(bool (&state1)[COL][ROW], bool (&state_2)[COL][ROW]) 
{
	for (uint8_t y = 0; y < COL; y++) 
	{
		for (uint8_t x = 0; x < ROW; x++) 
		{
			if (!(state1[y][x] == state_2[y][x])) return false;
		}
	}

	return true;
}

void NeoPixels::drawGameOfLife(bool (&state)[COL][ROW]) 
{
	for (uint8_t y = 0; y < COL; y++) 
	{
		for (uint8_t x = 0; x < ROW; x++) 
		{
			uint32_t color = state[y][x] ? _currentColor : (uint32_t)Color::OFF;
			drawPixel(x, y, color, 0, false);
		}
	}
	
	FastLED.show();
}


/* ########## Animations ########## */


void NeoPixels::connectWiFiTASK(void* ptr) 
{
    // FreeRTOS thread task
    NeoPixels* pixel_ptr = static_cast<NeoPixels*>(ptr);

    while (true)
	{ 
        pixel_ptr->animation_connectWiFi();
        vTaskDelay(450 / portTICK_PERIOD_MS);
    }
}

void NeoPixels::animation_spiral(uint32_t color, uint16_t delay) 
{
	uint8_t startX = 0;
	uint8_t startY = 0;
	uint8_t endX = ROW - 1;
	uint8_t endY = COL - 1;
	uint16_t count = 0;
	
	while (count != TOTAL_LEDS) 
	{
		// Left to right
		for (int8_t i = startX; i <= endX; i++) 
		{
			drawPixel(i, startY, color, delay);
			count++;
		}
		startY++; 

		// Top to bottom
		for (int8_t i = startY; i <= endY; i++) 
		{
			drawPixel(endX, i, color, delay);
			count++;
		}
		endX --; 

		// Right to left
		for (int8_t i = endX; i >= startX; i--) 
		{
			drawPixel(i, endY, color, delay);
			count++;
		}
		endY --; 
		
		// Bottom to top
		for (int8_t i = endY; i >= startY; i--) 
		{
			drawPixel(startX, i, color, delay);
			count++;
		}
		startX++; 
	}
}

void NeoPixels::animation_waterDrops() 
{
	bool usedIndex[TOTAL_LEDS] = {false};
	uint8_t randomIndex = 0;
	ledBrightness = 1;
	random16_set_seed(esp_random()); 

	for (size_t i = 0; i < TOTAL_LEDS; i++) 
	{
		do 
		{
			randomIndex = random8() % TOTAL_LEDS; 
		} while (usedIndex[randomIndex]);

		// Store lit positions
		usedIndex[randomIndex] = true;

		// Draw position and fade in 
		led[ ledIndexByIndex(randomIndex) ] = _currentColor;
		setLedBrightness(ledBrightness++);
		FastLED.show();
		
		// Decrease the delay between updates
		uint16_t delayMs = (TOTAL_LEDS - i) / 5;
		delay(delayMs);
	}

	delay(500);
	animation_fadeAll(FADE_OUT, 2000);
}

void NeoPixels::animation_startupFadeIn(StartupShape shape, bool allLeds) 
{
	ledBrightness = 220;

	if (allLeds) 
	{
		for (size_t i = 0; i < TOTAL_LEDS; i++) 
		{
			led[ ledIndexByIndex(i) ] = _currentColor;
		}
		return;
	}
	// Shape from ledMapping/led_startup_shape.h 
	for (size_t i = 0; i < TOTAL_LEDS; i++) 
	{
		if (startupLedMap[shape][i]) 
		{
			led[ ledIndexByIndex(i) ] = _currentColor;
		}
	}
	
	constexpr uint32_t delay_uS = 3000;
	animation_fadeAll(FADE_IN, delay_uS * 2);
	animation_fadeAll(FADE_OUT, delay_uS);
}

void NeoPixels::animation_startupFillIn(StartupShape shape, int delay_mS, uint32_t color)
{	
	uint8_t savedBrightness = ledBrightness;
	ledBrightness = 0;
	setLedBrightness(ledBrightness);

	for (size_t i = 0; i < TOTAL_LEDS; i++) 
	{
		if (startupLedMap[shape][i])
		{
			setLedBrightness(ledBrightness += 2);
			led[ ledIndexByIndex(i) ] = color;
			FastLED.show();
			delay(10);
		}
	}

	delay(delay_mS);
	animation_fadeAll(FADE_OUT, 1000);
	ledBrightness = savedBrightness;
}

void NeoPixels::animation_startupFillInStatic(StartupShape shape, int delay_mS, uint32_t color) 
{	
	for (size_t i = 0; i < TOTAL_LEDS; i++) 
	{
		if (startupLedMap[shape][i]) 
		{
			led[ ledIndexByIndex(i) ] = color;
			FastLED.show();
			delay(40);
		}
	}

	delay(delay_mS);
	animation_fadeAll(FADE_OUT, 1000);
}

void NeoPixels::animation_flag(StartupShape shape) 
{
	int color;
	for (size_t i = 0; i < TOTAL_LEDS; i++) 
	{
		color = startupLedMap[shape][i] ? Color::YELLOW : Color::BLUE;
		led[ ledIndexByIndex(i) ] = color;
	}

	animation_fadeAll(FADE_IN);
	delay(1000);
	animation_fadeAll(FADE_OUT);
}

void NeoPixels::animation_fadeAll(const bool fadeIn, uint32_t delay_uS)
{	
	// Makes sure fade takes the same amount of time regardless of ledBrightness
	// Account for about 3mS in "FastLED.show()" for each iteration
	uint totalDelay_mS = 1000 - (3 * ledBrightness);
	uint dynamicDelay_uS = (totalDelay_mS * 1000) / ledBrightness; 

	// Fade
	for (uint8_t i = 1; i < ledBrightness; i++)
	{
		if (fadeIn)
			FastLED.setBrightness(i);
		else
			FastLED.setBrightness(ledBrightness - i - 1);

		FastLED.show(); 
		delayMicroseconds(dynamicDelay_uS + delay_uS);
	}

	if (!fadeIn) 
	{ 
		setLedsColor(Color::OFF);
	}
}

void NeoPixels::animation_webServer(bool isPaused, bool resetAnimation) 
{	
	using State = AnimationWebServerStates;
	static State animationState = State::INIT;
	static uint nextTime = 0;
	static uint count = 0;

	if (resetAnimation)
	{
		setLedsColor(Color::OFF);
		animationState = State::INIT;
		nextTime = 0;
		count = 0;
		return;
	}

	// Hotfix for color picker
	static uint8_t hour = 0;
	static uint8_t minute = 0;
	setLedsColor(Color::OFF);
	drawTimeStaticHourMinute(hour, minute);
	FastLED.show();
	if (isPaused) return;

	if (!shouldUpdateWebServerAnimation(nextTime)) return;

	switch (animationState)
	{
		case State::INIT: 
			animationState = webServerInit(hour, minute);
			break;
		case State::FADE_IN: 
			animationState = webServerFadeIn(count);
			break;
		case State::WAIT: 
			animationState = webServerWait(count);
			break;
		case State::FADE_OUT: 
			animationState = webServerFadeOut(count);
			break;
	}
}

NeoPixels::AnimationWebServerStates NeoPixels::webServerInit(uint8_t& hour, uint8_t& minute)
{
	hour = getRandomUniqueTime(HOUR);
	minute = getRandomUniqueTime(MINUTE);
	drawTimeStaticHourMinute(hour, minute);

	return AnimationWebServerStates::FADE_IN;
}

NeoPixels::AnimationWebServerStates NeoPixels::webServerFadeIn(uint& fadeAmount)
{	
	if (fadeAmount <= ledBrightness)
	{
		FastLED.setBrightness(fadeAmount); 
		animationLedBrightness = fadeAmount;
		fadeAmount++;

		return AnimationWebServerStates::FADE_IN;
	} 
	else 
	{
		fadeAmount = 0; 
		return AnimationWebServerStates::WAIT;
	}	
}

NeoPixels::AnimationWebServerStates NeoPixels::webServerWait(uint& count)
{	
	if (count < ledBrightness)
	{
		count++;
		return AnimationWebServerStates::WAIT;
	}
	else 
	{
		count = 0;
		return AnimationWebServerStates::FADE_OUT;	
	}
}

NeoPixels::AnimationWebServerStates NeoPixels::webServerFadeOut(uint& fadeAmount)
{	
	if (fadeAmount <= ledBrightness) 
	{
		FastLED.setBrightness(ledBrightness - fadeAmount); 
		animationLedBrightness = ledBrightness - fadeAmount;
		fadeAmount++;

		return AnimationWebServerStates::FADE_OUT;
	} 
	else 
	{
		setLedsColor(Color::OFF, true);
		fadeAmount = 0;
		return AnimationWebServerStates::INIT;
	}
}

void NeoPixels::animation_connectWiFi() 
{
	static uint8_t steps = 0;

	if (steps == 3) 
	{
		steps = 0;
		setLedsColor(Color::OFF, true);
		return;
	}
	// Connecting animation ".    ..    ..." 
	led[ ledIndexByXY(4 + steps, 8) ] = _currentColor;
	FastLED.show();	
	steps++;
}

void NeoPixels::animation_connectWiFiStatus(WiFiConnectionStatus status)
{
	setLedsColor(Color::OFF, true);

	if (status == WiFiConnectionStatus::SUCCESS) 
		animation_startupFillInStatic(StartupShape::CHECKMARK, 1000, Color::GREEN);
	else if (status == WiFiConnectionStatus::FAILED) 
		animation_startupFillInStatic(StartupShape::FAIL, 1000, Color::RED);
}

void NeoPixels::animation_fireworks(uint8_t X, uint8_t height, Color color, uint8_t speed) 
{
	int8_t Y; 

	// Rocket
	for (Y = (COL - 1); Y >= height; Y--) 
	{
		drawPixel(X, Y, Color::RED, 0, false);
		drawPixel(X, (Y + 3), Color::OFF, 0, false);
		FastLED.show();
		delay(speed);
	}
	// Firework
	setLedsColor(Color::OFF);
	drawPixel(X, (Y + 1), color, speed, true);
	delay(30);
	setLedsColor(Color::OFF);

	drawPixel(X, 		(Y + 2), 	color, 0, 0);
	drawPixel(X, 		(Y), 	   	color, 0, 0);
	drawPixel(X - 1, 	(Y + 1), 	color, 0, 0);
	drawPixel(X + 1, 	(Y + 1), 	color, 0, 0);
	FastLED.show();
	delay(speed + 5);
	setLedsColor(Color::OFF);

	drawPixel(X, 		(Y + 3), 	color, 0, 0);
	drawPixel(X, 		(Y - 1), 	color, 0, 0);
	drawPixel(X - 2, 	(Y + 1), 	color, 0, 0);
	drawPixel(X + 2, 	(Y + 1), 	color, 0, 0);
	FastLED.show();
	delay(speed + 5);
	setLedsColor(Color::OFF);

	drawPixel(X, 		(Y + 4), 	color, 0, 0);
	drawPixel(X, 		(Y - 2), 	color, 0, 0);
	drawPixel(X - 3, 	(Y + 1), 	color, 0, 0);
	drawPixel(X + 3, 	(Y + 1), 	color, 0, 0);
	drawPixel(X - 2, 	(Y + 3), 	color, 0, 0);
	drawPixel(X + 2, 	(Y + 3), 	color, 0, 0);
	drawPixel(X - 2, 	(Y - 1), 	color, 0, 0);
	drawPixel(X + 2, 	(Y - 1), 	color, 0, 0);

	FastLED.show();
	delay(speed + 5);
	setLedsColor(Color::OFF, true);
}

// Debug
void NeoPixels::testRandom() 
{ 
	log("testRandom() = empty\n");
}

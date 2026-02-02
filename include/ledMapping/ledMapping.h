#if defined(ledMapping_INCLUDED)
    #pragma Warning: File ledMapping.h included multiple times 
#else
    #define ledMapping_INCLUDED

#include "config.h"
#include "ledMapping/ledDataHours.h"
#include "ledMapping/ledDataMinutes.h"

// Remap binary led matrixes to "compressed" uint8_t arrays at compile time.
// 1: Get column size and the largest row size for both minute and hour arrays
// 2: Create 2 new arrays with the sizes from above
// 3: Only include lit led positions in the new arrays to limit size.
constexpr size_t getLargestRow(const bool targetMap[][TOTAL_LEDS], const size_t numOfCols) 
{
    uint rowCount[TOTAL_LEDS] = {0}; 
    
    // Columns / rows
    for (size_t col = 0; col < numOfCols; col++) 
    {
        for (size_t row = 0; row < TOTAL_LEDS; row++) 
        {
            if (targetMap[col][row]) 
                rowCount[col]++;
        }
    }
    // Find the index of the largest row
    uint largestRow = 0;
    for (size_t i = 0; i < numOfCols; i++) 
    {
        if (rowCount[i] > rowCount[largestRow])
            largestRow = i;
    }
    
    // "+1" used as termination while drawing time to leds (TOTAL_LEDS + 1) 
    return rowCount[largestRow] + 1;
}

// Sizes
constexpr size_t HOURS_COLUMN_SIZE = std::size(hours::oldLedMap);
constexpr size_t MINUTES_COLUMN_SIZE = std::size(minutes::oldLedMap);
constexpr size_t HOURS_LARGEST_ROW = getLargestRow(hours::oldLedMap, HOURS_COLUMN_SIZE);
constexpr size_t MINUTES_LARGEST_ROW = getLargestRow(minutes::oldLedMap, MINUTES_COLUMN_SIZE);

/*  
    #########  Led layout  #########

    SERPENTINE_LED_LAYOUT true
    0  >  1  >  2  >  3  >  4 ... 11
                                  |
    22 ... 15  <  14  <  13  <  12
    |
    23 ...

    SERPENTINE_LED_LAYOUT false
    0  >  1  >  2  >  3  >  4 ... 11
    12  >  13  >  14  >  15 ...
*/

// Generate the new "compressed" arrays
#if SERPENTINE_LED_LAYOUT 
    constexpr uint8_t calculateIndex(uint8_t col) 
    {
        const uint8_t Xpos = col % MATRIX_WIDTH;
        const uint8_t Ypos = (col / MATRIX_WIDTH) % MATRIX_HEIGHT;
        uint8_t index = 0;

        // Even rows run forwards 
        if (Ypos % 2 == 0)
        {
            index = (Ypos * MATRIX_WIDTH) + Xpos;
        }
        else // Odd rows run backwards 
        {
            uint8_t reverseX = (MATRIX_WIDTH - 1) - Xpos;
            index = (Ypos * MATRIX_WIDTH) + reverseX;		
        }
        
        return index;
    }


    // Return a 2D array with the sizes from above, only include lit led positions
    template<size_t NumCols, size_t NumRows> 
    constexpr std::array<std::array<uint8_t, NumRows>, NumCols> getLedMap(const bool oldLedMap[][TOTAL_LEDS]) 
    {
        std::array<std::array<uint8_t, NumRows>, NumCols> newLedMap = {};

        // Columns
        for (size_t col = 0; col < NumCols; col++) 
        {
            size_t index = 0;
            // Rows, add only lit leds
            for (size_t row = 0; row < TOTAL_LEDS; row++) 
            {
                if (oldLedMap[col][row]) 
                {
                    newLedMap[col][index] = calculateIndex(row);
                    index++;
                }  
            }
            newLedMap[col][index] = (TOTAL_LEDS + 1); // "End of array" termination constant
        }

        return newLedMap;
    }

#else // SERPENTINE_LED_LAYOUT false 

    // Return a 2D array with the sizes from above, only include lit led positions
    template<size_t NumCols, size_t NumRows> 
    constexpr std::array<std::array<uint8_t, NumRows>, NumCols> getLedMap(const bool oldLedMap[][TOTAL_LEDS]) {
        std::array<std::array<uint8_t, NumRows>, NumCols> newLedMap = {};

        // Columns
        for (size_t col = 0; col < NumCols; col++) 
        {
            size_t index = 0;
            // Rows, add only lit leds
            for (size_t row = 0; row < TOTAL_LEDS; row++) 
            {
                if (oldLedMap[col][row]) 
                {
                    newLedMap[col][index] = row;
                    index++;
                }  
            }
            newLedMap[col][index] = (TOTAL_LEDS + 1); // "End of array" termination constant
        }

        return newLedMap;
    }
#endif // SERPENTINE_LED_LAYOUT
#endif // ledMapping_INCLUDED

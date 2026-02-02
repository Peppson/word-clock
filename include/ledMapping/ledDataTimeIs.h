#if defined(LED_DATA_TIME_IS_INCLUDED)
    #pragma Warning: File ledDataTimeIs.h included multiple times
#else
    #define LED_DATA_TIME_IS_INCLUDED

// "Klockan är" (The time is)
constexpr bool o = false; // Visibility hack
constexpr bool theTimeIsLedMap[MATRIX_WIDTH]
{ 
    1, 1, 1, 1, 1, 1, 1, o, 1, 1, o
};

#endif

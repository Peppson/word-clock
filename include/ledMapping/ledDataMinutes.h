#if defined(LED_DATA_MINUTES_INCLUDED)
    #pragma Warning: File ledDataMinutes.h included multiple times 
#else
    #define LED_DATA_MINUTES_INCLUDED

// Binary matrix for each 5 minutes step
// Compressed and remapped at compile time, see ledMapping.h
namespace minutes 
{
    constexpr bool o = false; // Visibility hack
    constexpr bool oldLedMap[][TOTAL_LEDS] 
    {
        // 00:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },
        
        // 00:05
        {
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, 1, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:10
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, 1, 1, 1, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, 1, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:15
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, 1, 1, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, 1, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:20
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, 1, 1, o, o, o, o, o, o,
        1, 1, 1, 1, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:25
        {
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, o, 1, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, 1, 1, 1, 1,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:30
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, 1, 1, 1, 1,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:35
        {
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, 1, o, o, o, 1, 1, 1, 1,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:40
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, 1, 1, o, 1, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:45
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, 1, 1, o, 1, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:50
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, 1, 1, 1, o, 1,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 00:55
        {
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, o, 1, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        }
    };
} // namespace minutes
#endif // LED_DATA_MINUTES_INCLUDED

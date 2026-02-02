#if defined(LED_DATA_HOURS_INCLUDED)
    #pragma Warning: File ledDataHours.h included multiple times 
#else
    #define LED_DATA_HOURS_INCLUDED

// Binary matrix for each hour, in 12h format
// Compressed and remapped at compile time, see ledMapping.h
namespace hours 
{
    constexpr bool o = false; // Visibility hack    
    constexpr bool oldLedMap[][TOTAL_LEDS] 
    {
        // 12:00
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
        o, o, o, o, o, o, o, 1, 1, 1, 1
        },

        // 01:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        },

        // 02:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, 1, 1, 1,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 03:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 04:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, 1, 1, 1, 1,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 05:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 06:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, 1, 1, 1,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 07:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        1, 1, 1, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 08:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, 1, 1, 1, 1, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 09:00
        {
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, 1, 1, 1,
        o, o, o, o, o, o, o, o, o, o, o
        },

        // 10:00
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
        1, 1, 1, o, o, o, o, o, o, o, o
        },

        // 11:00
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
        o, o, o, 1, 1, 1, 1, o, o, o, o,
        } 
    };
} // namespace hours
#endif // LED_DATA_HOURS_INCLUDED

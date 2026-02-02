#if defined(LED_STARTUP_SHAPES_INCLUDED)
    #pragma Warning: File ledDataShapes.h included multiple times
#else
    #define LED_STARTUP_SHAPES_INCLUDED

// Startup shapes
namespace 
{
    constexpr bool o = false; // Visibility hack
    constexpr bool startupLedMap[][TOTAL_LEDS] 
    { 
        // Dots
        {
        o, 1, o, 1, o, 1, o, 1, o, 1, o,
        1, o, 1, o, 1, o, 1, o, 1, o, 1,
        o, 1, o, 1, o, 1, o, 1, o, 1, o,
        1, o, 1, o, 1, o, 1, o, 1, o, 1,
        o, 1, o, 1, o, 1, o, 1, o, 1, o,
        1, o, 1, o, 1, o, 1, o, 1, o, 1,
        o, 1, o, 1, o, 1, o, 1, o, 1, o,
        1, o, 1, o, 1, o, 1, o, 1, o, 1,
        o, 1, o, 1, o, 1, o, 1, o, 1, o,
        1, o, 1, o, 1, o, 1, o, 1, o, 1,
        },

        // Flag
        { 
        o, o, o, 1, 1, o, o, o, o, o, o,
        o, o, o, 1, 1, o, o, o, o, o, o,
        o, o, o, 1, 1, o, o, o, o, o, o,
        o, o, o, 1, 1, o, o, o, o, o, o,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        o, o, o, 1, 1, o, o, o, o, o, o,
        o, o, o, 1, 1, o, o, o, o, o, o,
        o, o, o, 1, 1, o, o, o, o, o, o,
        o, o, o, 1, 1, o, o, o, o, o, o 
        }, 

        // Heart
        { 
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, 1, 1, o, o, o, 1, 1, o, o,
        o, 1, 1, 1, 1, o, 1, 1, 1, 1, o,
        o, 1, 1, 1, 1, 1, 1, 1, 1, 1, o,
        o, 1, 1, 1, 1, 1, 1, 1, 1, 1, o,
        o, o, 1, 1, 1, 1, 1, 1, 1, o, o,
        o, o, o, 1, 1, 1, 1, 1, o, o, o,
        o, o, o, o, 1, 1, 1, o, o, o, o,
        o, o, o, o, o, 1, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o, 
        }, 

        // Checkmark
        { 
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, 1, o, o,
        o, o, o, o, o, o, o, 1, o, o, o,
        o, o, 1, o, o, o, 1, o, o, o, o,
        o, o, o, 1, o, 1, o, o, o, o, o,
        o, o, o, o, 1, o, o, o, o, o, o, 
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        }, 

        // Fail
        { 
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, 1, o, o, o, o, o, 1, o, o,
        o, o, o, 1, o, o, o, 1, o, o, o,
        o, o, o, o, 1, o, 1, o, o, o, o,
        o, o, o, o, o, 1, o, o, o, o, o,
        o, o, o, o, 1, o, 1, o, o, o, o,
        o, o, o, 1, o, o, o, 1, o, o, o,
        o, o, 1, o, o, o, o, o, 1, o, o,
        o, o, o, o, o, o, o, o, o, o, o,
        o, o, o, o, o, o, o, o, o, o, o, 
        }
    };
}
#endif // LED_STARTUP_SHAPES_INCLUDED

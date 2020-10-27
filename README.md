# Skate following underglow

Using a Hall effect sensor and magnets embedded into a wheel, we measure the movement of a skateboard and use that information to animate LEDs.

This project uses WS2812B LEDs, a Teensy LC and a TLE4966 dual Hall effect sensor.

Since the timing for the WS2812B LEDs is very strict, the excellent [WS2812Serial library](https://github.com/PaulStoffregen/WS2812Serial) by Paul Stoffregen is used to keep interrupts available for the Hall effect sensor.

The [FastLED library](https://github.com/FastLED/FastLED) is also used so the LEDs can be controlled using HSV values instead of RGB.

Sketch uploaded to Teensy board with Arduino 1.8.13 & Teensyduino 1.53.

TODO:
- General clean up of code
- Optimization: use memcpy in the LED patterns. Currently every LED value is calculated every frame, only LEDs at the tip of the board actually need to ba calculated
- Implement actual speed calculations, only distance is used right now.  

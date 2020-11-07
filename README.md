# Skate following underglow

Using a Hall effect sensor and magnets embedded into a wheel, we measure the movement of a skateboard and use that information to animate LEDs.

This project uses WS2812B LEDs, a Teensy LC and a TLE4966 dual Hall effect sensor.

Since the timing for the WS2812B LEDs is very strict, the excellent [WS2812Serial library](https://github.com/PaulStoffregen/WS2812Serial) by Paul Stoffregen is used to keep interrupts available for the Hall effect sensor.

The [FastLED library](https://github.com/FastLED/FastLED) is also used so the LEDs can be controlled using HSV values instead of RGB.

Sketch uploaded to Teensy board with Arduino 1.8.13 & Teensyduino 1.53.

TODO:
- Optimization: use memcpy in the LED patterns. Currently every LED value is calculated every frame, despite most of the values just moving along on the strip. Really the only values that need to be calculated are the those at the tip of the board (when moving forward) the other values can just be copied downwards. This is only implemented in theaterLights() so far.
- Implement actual speed calculations, only distance is used right now.  Add animations that use speed values.
- Miniaturize with an actual PCB? 


Lessons learned:
-The adhesive back on the LED strips can be pretty crap. My strips would unstick after a day or so. Hot glue on left and right of the strip on its entire length seems to work fine.
-Don't trust the LED strip manufacturer when they say the LEDs are GRB, the actual order was BRG. Testing this earlier would have avoided a lot of confusion.

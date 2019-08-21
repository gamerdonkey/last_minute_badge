# Crappy Last-Minute DEFCON Badge

For years, I've wanted to design and build my very own badge for DEFCON. But I always put the work off until it is too late. This year, I decided to make a badge, come hell or high water. So I threw together components that I had lying, unused, around the house into something resembling an electronic badge.

![Front and back image of a stack of electronics with a numeric display on one side and an exposed Huzzah32 microcontroller on the other.](images/last_minute_badge.jpg?raw=true "Looks kind of like one of those fancy badges, right?")

# Programming

Due to the constrained timetable of this project, I programmed the ESP32 using the [Espressif Arduino Core](https://github.com/espressif/arduino-esp32#using-through-arduino-ide). The code is mostly together but not perfect, much like the rest of the badge.

# Assembly

![Hand-drawn schematic on grid paper showing how the display and Huzzah32 are connected. These connections may also be determined from the code, but note that there is a 4.7k pull-up resistor on the button.](images/schematic.jpg?raw=true "So analog.")

I know that the ESP32 is enormously overkill for this project, but I was working with what I had lying around. And the built-in LiPo battery management was just impossible to pass up. Also, this keeps the door open for WiFi and Bluetooth fun in the future.

## Bill of Materials

- [Huzzah32 - Adafruit's ESP32 Feather Board](https://www.adafruit.com/product/3405)
- [150 mAh LiPo Battery](https://www.adafruit.com/product/1317)
- [TM1637 Seven-Segment Display](https://www.amazon.com/diymore-Display-7-Segment-4-Digit-Digital/dp/B078J6452J)
- Some kind of circuit prototyping Board
- Tactile button
- 4.7k ohm resistor
- Wires

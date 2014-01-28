AKC6955-M6955--DSP-radio-with-full-colour-LCD
=============================================

Time and station informations are displayed in addition to current receiving frequency. Station information is prepared in header file, and its on LCD is renewed every hour according to header file (timetable.h). To complete this circuit, you will need Arduino 328p working 3.3V (or 5V-3.3V level converter), M6955 radio module (I2C), rotary encoder (D2, D4) with band switch (D3), DS1307 RTC (I2C), and 1.8inch SPI LCD (connection of LCD is same as sample pprogram of TFT.h in Arduino IDE). Station info of FM and shortwave are written in separate files. RTC related two libraries is required to use RTC(http://www.pjrc.com/teensy/arduino_libraries/Time.zip , http://www.pjrc.com/teensy/arduino_libraries/DS1307RTC.zip ) Receiving operation can be seen in http://www.youtube.com/watch?v=DB5WFJkqZIQ
Enjoy listening!

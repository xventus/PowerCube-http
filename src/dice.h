
/**
 * @file dice.h
 * @author Petr Vanek (petr@fotoventus.cz)
 * @brief wrapping the status display in LED dice
 * @version 0.1
 * @date 2022-04-05
 * 
 * @copyright Copyright (c) 2022 Petr Vanek
 * 
 */

#pragma once

#include <FastLED.h>

const uint16_t _gledNumOfPins {25};
CRGB _gledsMatrix[_gledNumOfPins];  

/**
 * @brief LED visualiser, running in extra core, we don't need to stop this job
 *            
 */ 
class Dice {
public:

enum class Info { ok, error, wait, none };

private:
    static const uint16_t  _delay {1000};     ///> LED wait 
    TaskHandle_t _task;                ///> task handle     
    int8_t       _power {0};           ///> value of actual power 
    Info         _info {Info::none};   ///> special status

public:
    static const uint16_t  _ledPin {27};
public:  
    
    /**
     * @brief run visualiser
     * 
     */
    void init() {
        xTaskCreatePinnedToCore(
        task, 
        "visualiser", 
        10000,  
        this,  
        0, 
        &_task, 
        0); 
    }

    void showErrorState() { _info = Info::error; }
    void showSucessState() { _info = Info::ok; }
    void showWaitState() { _info = Info::wait; }
    void clearState() { _info = Info::none; }
    void showPower(int8_t value) { _power = value;}
    Info getState() const  { return _info; }
    int8_t getPower() const  { return _power; }
    void data(bool isok) { 
         if (isok) _gledsMatrix[0] = 0x008B00; 
          _gledsMatrix[0] = 0x8B3A3A; 
         FastLED.show(); 
    }
 
private:

    static void task( void * pvParameters ){
        Dice *pdice = (Dice *) pvParameters;
        bool state = true;
        
        FastLED.addLeds<WS2812,pdice->_ledPin>(_gledsMatrix, _gledNumOfPins);
        FastLED.setBrightness(20);

        for(;;) {

            switch (pdice->getState()) {
                case Info::ok:      
                    FastLED.clear(true);
                    if (state) ok(0x00FFFF);
                    else ok(0x0000FF);
                    state = !state;
                    delay(_delay);
                break;
                case Info::error:   
                    FastLED.clear(true);
                    if (state)  err(0xFF00FF);
                    else err(0x0000FF);
                    state = !state;
                    delay(_delay);
                break;
                case Info::wait:     
                    FastLED.clear(true);
                    snake(0x0000FF);   
                    snake(0x000000); 
                    break;
                case Info::none: 
                       state = true;   
                     break;
            }

            if (pdice->getState()==Info::none) {
                FastLED.clear(true);
                if (pdice->getPower() < 0) {
                    pixels(abs(pdice->getPower()), 0x00FF00);  
                } else if (pdice->getPower() > 0) {
                    pixels(pdice->getPower(), 0xFF0000);  
                } else {
                    pixels(pdice->getPower(), 0x0000FF);  
                }

                 delay(_delay);
            } 

        }
    }

    static void snake(const CRGB colour, uint16_t wait=100, uint8_t max=4) {
        for (auto z = 0; z < 4; z++ ) {
            for (auto i = 0; i < max;i++) { 
            switch(z) {
                case 0:  _gledsMatrix[i] = colour; break;
                case 1:  _gledsMatrix[(i*5)+4] = colour; break;
                case 2:  _gledsMatrix[24-i] = colour; break;
                case 3:  _gledsMatrix[20-(i*5)] = colour; break;                                                                                                                                         ; break;
            }
            FastLED.show();
            delay(wait);
            }
        }
    }

    static void ok(const CRGB colour) {
        _gledsMatrix[12] =  colour;
        _gledsMatrix[11] = colour;
        _gledsMatrix[13] = colour;
        _gledsMatrix[7] = colour;
        _gledsMatrix[17] = colour;
        FastLED.show();
    }

    static void err(const CRGB colour) {
        _gledsMatrix[12] = colour;
        _gledsMatrix[6] = colour;
        _gledsMatrix[18] = colour;
        _gledsMatrix[16] = colour;
        _gledsMatrix[8] = colour; 
        FastLED.show();
    }

    static void pixels(uint8_t value, const CRGB colour) {
        
        if (value == 0 || value == 1 || value == 3 || value == 5 || value == 7 || value == 8) {
            _gledsMatrix[12] = colour;
        }

        if (value == 2 || value == 3 || value == 4 || value == 5) {
            _gledsMatrix[16] = colour;
            _gledsMatrix[8] = colour;
        }

        if (value == 4 || value == 5 ) {
            _gledsMatrix[6] = colour;
            _gledsMatrix[18] = colour;
        }

        if (value == 6 || value == 7  || value == 8) {
            _gledsMatrix[6] = colour;
            _gledsMatrix[11] = colour;
            _gledsMatrix[16] = colour;
            _gledsMatrix[8] = colour;
            _gledsMatrix[13] = colour;
            _gledsMatrix[18] = colour;
        }

        if (value == 8) {
            _gledsMatrix[7] = colour;
            _gledsMatrix[17] = colour;
        }

        if (value > 8) {
            FastLED.showColor(colour, FastLED.getBrightness()); 
        }  else { 
            FastLED.show();
        }
    }
};
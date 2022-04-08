
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

#include <M5Atom.h>

/**
 * @brief LED visualiser, running in extra core, we don't need to stop this job
 *        requires initialized M5 framework    
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
    int8_t getEnergy() const  { return _power; }
    void data(bool isok) { 
         if (isok) M5.dis.drawpix(0, 0x008B00); 
          M5.dis.drawpix(0, 0x8B3A3A); 
    }
 
private:

    static void task( void * pvParameters ){
        Dice *pdice = (Dice *) pvParameters;
        bool state = true;
        for(;;) {

            switch (pdice->getState()) {
                case Info::ok:      
                    M5.dis.clear();
                    if (state) ok(0x00FFFF);
                    else ok(0x0000FF);
                    state = !state;
                    delay(_delay);
                break;
                case Info::error:   
                    M5.dis.clear();
                    if (state)  err(0xFF00FF);
                    else err(0x0000FF);
                    state = !state;
                    delay(_delay);
                break;
                case Info::wait:     
                    M5.dis.clear();
                    snake(0x0000FF);   
                    snake(0x000000); 
                    break;
                case Info::none: 
                       state = true;   
                     break;
            }

            if (pdice->getState()==Info::none) {
                M5.dis.clear();
                if (pdice->getEnergy() < 0) {
                    pixels(abs(pdice->getEnergy()), 0xFF0000);  
                } else if (pdice->getEnergy() > 0) {
                    pixels(pdice->getEnergy(), 0x00FF00);  
                } else {
                    pixels(pdice->getEnergy(), 0x0000FF);  
                }

                 delay(_delay);
            } 

        }
    }

    static void snake(const CRGB colour, uint16_t wait=100, uint8_t max=4) {
        for (auto z = 0; z < 4; z++ ) {
            for (auto i = 0; i < max;i++) { 
            switch(z) {
                case 0:  M5.dis.drawpix(0,i, colour); break;
                case 1:  M5.dis.drawpix(i,4, colour) ; break;
                case 2:  M5.dis.drawpix(4,4-i, colour); break;
                case 3:  M5.dis.drawpix(4-i,0, colour)                                                                                                                                         ; break;
            }
            delay(100);
            }
        }
    }

    static void ok(const CRGB colour) {
          M5.dis.drawpix(12, colour);
          M5.dis.drawpix(11, colour);
          M5.dis.drawpix(13, colour);
          M5.dis.drawpix(7, colour);
          M5.dis.drawpix(17, colour);
    }

    static void err(const CRGB colour) {
          M5.dis.drawpix(12, colour);
          M5.dis.drawpix(6, colour);
          M5.dis.drawpix(18, colour);
          M5.dis.drawpix(16, colour);
          M5.dis.drawpix(8, colour); 
    }

    static void pixels(uint8_t value, const CRGB colour) {
        
        if (value == 0) {
            M5.dis.drawpix(12, colour); 
        }

        if (value == 1 || value == 3 || value == 5 || value == 7 || value == 8) {
            M5.dis.drawpix(12, colour);
        }

        if (value == 2 || value == 3 || value == 4 || value == 5) {
            M5.dis.drawpix(16, colour);
            M5.dis.drawpix(8, colour);
        }

        if (value == 4 || value == 5 ) {
            M5.dis.drawpix(6, colour);
            M5.dis.drawpix(18, colour);
        }

        if (value == 6 || value == 7  || value == 8) {
            M5.dis.drawpix(6, colour);
            M5.dis.drawpix(11, colour);
            M5.dis.drawpix(16, colour);
            M5.dis.drawpix(8, colour);
            M5.dis.drawpix(13, colour);
            M5.dis.drawpix(18, colour);
        }

        if (value == 8) {
            M5.dis.drawpix(7, colour);
            M5.dis.drawpix(17, colour);
        }

        if (value > 8) {
            M5.dis.fillpix(colour); 
        }  
    }
};
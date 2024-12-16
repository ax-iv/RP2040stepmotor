#pragma once

#include <Arduino.h>

// RP2040_PWM stuff
#define _PWM_LOGLEVEL_ 0
#include "RP2040_PWM.h"

//#define PS_DEBUG

#ifdef PS_DEBUG
    #define PSPrint(x) Serial.print(x)
    #define PSPrintln(x) Serial.println(x)
    #define PSPrint2(x,y) Serial.print(x);Serial.println(y);
#else
    #define PSPrint(x) 
    #define PSPrintln(x) 
    #define PSPrint2(x,y) 
#endif

enum stage{
    STOPPED = 0,
    ACCELERATION = 1,
    RUN = 2,
    SLOWDOWN = 3,
    STARTED = 4
};

typedef void(* change_stage_callback_t) (stage aStage);

class RP2040stepmotor
{
  public:
    
    RP2040stepmotor(uint32_t stepsPerRev,uint8_t pul, uint8_t dir, uint8_t ena, bool invert_enable_pins=0);
    stage Stage = STOPPED;
    void setTarget(float pos);
    float getTarget(void);

    void setMaxSpeed(float speed);
    float getMaxSpeed(void);

    float getSpeed(void);
    
    void setAcceleration(uint32_t accel);    
    uint32_t getAcceleration(void);

    
    double getCurrentDeg();

    void stop(void);
    void brake(void);
    void reset(void);
    void invertEn(bool inv);
    void enable(void);
    void disable(void);
    uint8_t isEnable(void);
    stage getState(void);
    uint8_t ready(void);
    void onChangeStage(change_stage_callback_t cb);    
    void irq_cb();
  private:
    void run(void);
    uint32_t _step_cnt=0;
    void  setCurrentDeg(double pos);
    void _tick();
    void callbackChangeStage();
    change_stage_callback_t _onChangeStage = nullptr;
       
    
    uint8_t _ready=0;
    double _pulsePerSec=0;

    uint8_t _toStop=0;
    uint8_t _isRun=0;
    uint8_t _isEnable=0;
    float _target = 0;
    float _maxSpeed=1000.0;
    float _curSpeed=0;
    uint32_t _accel = 5;

    uint32_t _curDeg=0;
    uint32_t _stepsPerRev = 0;
    

    void _stop(void);
    void setSpeed(float speed);
    
    
    uint8_t accel_count=0;


    // pinout
    uint8_t PUL_PIN;
    uint8_t DIR_PIN;
    uint8_t ENA_PIN;

    // pwm
    RP2040_PWM* pwm;

    // motion
    bool invert_enable;
    
};

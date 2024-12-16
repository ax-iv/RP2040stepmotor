#include <stdint.h>
#include "RP2040stepmotor.h"
#include "hardware/irq.h"


RP2040stepmotor* PRP2040stepmotor=nullptr;

void on_pwm_wrap() {
  PRP2040stepmotor->irq_cb();
  
}

RP2040stepmotor::RP2040stepmotor(uint32_t stepsPerRev,uint8_t pul, uint8_t dir, uint8_t ena, bool invert_enable_pins){
  _stepsPerRev=stepsPerRev;
  PUL_PIN = pul;
  ENA_PIN = ena;  
  DIR_PIN = dir;  
  invert_enable = invert_enable_pins;
  pinMode(ENA_PIN, OUTPUT);
  PRP2040stepmotor=this;  
    pwm_set_irq_enabled(pwm_gpio_to_slice_num(PUL_PIN),true);  
  irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), on_pwm_wrap);
  irq_set_enabled(PWM_DEFAULT_IRQ_NUM(),true);
  pwm = new RP2040_PWM(PUL_PIN, 1000, 0);  
}

void RP2040stepmotor::irq_cb(){
  _step_cnt++;
  pwm_clear_irq(pwm_gpio_to_slice_num(PUL_PIN));
  _tick();
}

void RP2040stepmotor::onChangeStage(change_stage_callback_t cb){
  _onChangeStage=cb;
}

void RP2040stepmotor::callbackChangeStage(){
  if (_onChangeStage == nullptr) {
      return;
    }
    _onChangeStage(Stage);
}

void RP2040stepmotor::_tick(){
    _curDeg = 360.0d*((double)_step_cnt / (double)_stepsPerRev)*100.0d;
    
    switch(Stage){
        case ACCELERATION:               
          if(_curDeg>=accel_count*_accel*10){
            accel_count++;
            setSpeed(_curSpeed + _maxSpeed/10.0d);
            run();
          }                  
          if(_curSpeed>=_maxSpeed){
              setSpeed(_maxSpeed);              
              Stage=RUN;      
              callbackChangeStage();        
              run();
          }
          break;
        case RUN:           
          
          if(_accel==0){
            if(_curDeg>=(uint32_t)round(100.0d * abs(_target))){
              _stop();
              Stage=STOPPED;
              callbackChangeStage();
              
            }
          }else{
            if((round(100.0d * abs(_target)) - _curDeg) <=  (_accel * 100)){
              accel_count=1;     
              run();
              Stage=SLOWDOWN;              
              callbackChangeStage();
            }
          }
          break;
        case SLOWDOWN:           

          if(_curDeg  >=  (100.0d * (abs(_target) - _accel + (accel_count*_accel/10.0d))  ) ){
            accel_count++;
            setSpeed(_curSpeed - _maxSpeed/10.0d);
            run();
          }
         
          if(_curSpeed <= 0){
              Stage=STOPPED;
              _stop();
              callbackChangeStage();              
          }  
          break;
        
        default: break;
    }
}

uint8_t RP2040stepmotor::ready(){
  if(_ready) {
    _ready=0;
    return 1;
  } 
  return 0;  
}

void RP2040stepmotor::_stop(void){  
  uint32_t cnt = pwm->disablePWM();  
  _isRun=0; 
  if(!_toStop)
    _ready=1;
}

void RP2040stepmotor::run(){    
    pwm->setPWM(PUL_PIN, _pulsePerSec, 50);  
}

void RP2040stepmotor::setTarget(float pos){
  _step_cnt=0;
  _ready=0;
  _toStop=0;
  accel_count=0;  
  setSpeed(0);
  setCurrentDeg(0);
    if(pos>0) digitalWriteFast(DIR_PIN,0); else digitalWriteFast(DIR_PIN,1);
  _target = pos;
  _isRun=1;  
  Stage=STARTED;
  callbackChangeStage();
  if(_accel==0){    
    setSpeed(_maxSpeed);
    Stage=RUN;
    callbackChangeStage();
  }
  else{
    accel_count=1;
    setSpeed(_maxSpeed/10.0);    
    Stage=ACCELERATION;    
    callbackChangeStage();
  }
  pwm_set_counter(pwm_gpio_to_slice_num(PUL_PIN),0);   
  run();  
}

void RP2040stepmotor::setSpeed(float speed){      
  _curSpeed = speed;
  if(_curSpeed>0){
    _pulsePerSec = (double)_stepsPerRev / 360.0d * (double)speed;
  }
  else{
    _pulsePerSec=0;
  }

}

/********************************************************************************************************* */


float RP2040stepmotor::getTarget(void){
  return _target;
}
void RP2040stepmotor::setMaxSpeed(float speed){
  _maxSpeed=speed;
}
float RP2040stepmotor::getMaxSpeed(void){
  return _maxSpeed;
}
void RP2040stepmotor::setAcceleration(uint32_t accel){
  _accel=accel;
}
uint32_t RP2040stepmotor::getAcceleration(void){
  return _accel;
}


void RP2040stepmotor::setCurrentDeg(double pos){
  _curDeg=round(pos*100.0d);
}
double RP2040stepmotor::getCurrentDeg(){
  if(_target<0) return (double)_curDeg / 100.0d * -1.0d;
  return (double)_curDeg / 100.0d;
}

float RP2040stepmotor::getSpeed(void){
  return _curSpeed;
}

void RP2040stepmotor::brake(){
  _step_cnt=0;
  pwm->disablePWM();
  setSpeed(0);    
  _isRun=0;
}

void RP2040stepmotor::reset(){
  brake();
  setCurrentDeg(0);
}

void RP2040stepmotor::stop(){
  Stage=SLOWDOWN;
  _toStop=1;
}


void RP2040stepmotor::invertEn(bool inv){
  invert_enable = inv;
}

void RP2040stepmotor::enable(){
  digitalWrite(ENA_PIN,invert_enable);
  Serial.println(invert_enable);
  _isEnable=1;
}
void RP2040stepmotor::disable(){
  digitalWrite(ENA_PIN,!invert_enable);
  _isEnable=0;
}

stage RP2040stepmotor::getState(void){
  return Stage;
}



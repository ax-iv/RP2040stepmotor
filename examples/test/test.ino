#include "global.h"
#include "RP2040stepmotor.h"

#define DPrint2(x,y) Serial.print(x);Serial.println(y)

#define MOTOR_STEPS 10000

#define MAX_ACCEL  20
#define MAX_SPEED  100
#define MAX_ANGLE  720

#define PIN_PUL    2
#define PIN_DIR    3 
#define PIN_EN     4


RP2040stepmotor st(MOTOR_STEPS,PIN_PUL, PIN_DIR, PIN_EN, 0);

float dir=1;

void changeStage(stage aStage){
    switch(aStage){
        case STARTED: DPrint2("Goto STAGE: ","STARTED");break;
        case RUN: DPrint2("Goto STAGE: ","RUN");break;
        case ACCELERATION: DPrint2("Goto STAGE: ","ACCELERATION");break;
        case SLOWDOWN: DPrint2("Goto STAGE: ","SLOWDOWN");break;
        case STOPPED: DPrint2("Goto STAGE: ","STOPPED");break;
        default:break;
    }
}

void setup()
{
    pin_setup();
	Serial.begin(115200);
    while(!Serial){}
    digitalWrite(PIN_EN, 0);
    st.onChangeStage(changeStage);
    st.setAcceleration(MAX_ACCEL);
    st.setMaxSpeed(MAX_SPEED);
    st.setTarget(1440.0);    
}


void loop()
{     
    if(st.ready()){
        if(dir>0) dir=-1.0; else dir=1.0;
        delay(1000);
        st.setTarget(dir * 1440.0);
        
    }
}


void pin_setup()
{
	pinMode(PIN_EN,OUTPUT);
    digitalWrite(PIN_EN, 1);
    pinMode(PIN_DIR,OUTPUT);
    pinMode(PIN_PUL,OUTPUT);
}




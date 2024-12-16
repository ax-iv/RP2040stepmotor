# RP2040stepmotor

RP2040stepmotor это Arduino-библиотека для управления PUL/DIR-шаговым двигателем на платах основанных на контроллере RP2040. Реализовывалась для управлением поворотным столом. Использовался мотор аля Nema17 и драйвер 3D580S. 
Управление основано на ШИМ.

Данная библиотека зависима от https://github.com/khoih-prog/RP2040_PWM/ .
This library depends on https://github.com/khoih-prog/RP2040_PWM/ for hardware pwm control.

Задание рабочего цикла PULSE (STEP) реализовано ШИМ-ом с заполнением (duty) 50% с помощью библиотки RP2040_PWM.
Подсчет прошедшего мотором угла реализован с помощью подсчета шагов мотора, которое получается от прерывания ШИМ


# Реализация

```cpp
    
    RP2040stepmotor st(MOTOR_STEPS,PIN_PUL, PIN_DIR, PIN_EN, 0);
    //Объявление объекта: MOTOR_STEPS - количество шагов на оборот, PIN_PUL - пин Pulse(Step), PIN_DIR - пин направления (Direction,Dir), PIN_EN - пин ENA, false (0) - инверсия ENA
    //TODO: управление ENA-пином - авто отключение мотора, etc.

    void setup()
    {    
        st.onChangeStage(changeStage);  // callback-функция срабатывающая при измениении состояния
        st.setAcceleration(ACCEL); // задание ускорения: 0 - без ускорения (сразу искомая скорость). 
                                   // ACCEL - количество градусов за которое наберется искомая скорость. 
                                   // Скорость увеличивается 10 раз по 1/10 от заданной MAX_SPEED
        st.setMaxSpeed(MAX_SPEED); // Искомая скорость в градусах в секунду
        st.setTarget(1440);        // Задание угла поворота в градусах (может быть отрицательной для смены неаправления движения) 
    }
    void loop()
    {     
        if(st.ready()){     // ready() - сообщает о достижении заданного угла поворота            
            if(dir>0) dir=-1.0; else dir=1.0;
            delay(1000);
            st.setTarget(dir * 1440.0);            
        }
    }
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
```

```cpp
float getSpeed();       // выдаст текущую скорость
double getCurrentDeg(); // выдаст текущую скорость
void stop(void);        // плавная остановка с ускорением, заданным setAcceleration
void brake(void);       // резкая остановка
void enable(void);      // управление ENA-пином
void disable(void);     // управление ENA-пином
uint8_t isEnable(void); // статус ENA-пина
stage getState(void);   // выдаст сотояние: 
/*
enum stage{
    STOPPED = 0,        // не движется
    ACCELERATION = 1,   // разгоняется
    RUN = 2,            // движется с заданной скоростью
    SLOWDOWN = 3,       // тормозит
    STARTED = 4         // старт
};
*/
uint8_t ready(void);    // сообщает о достижении заданного угла поворота (однократный опрос флага)
void onChangeStage(change_stage_callback_t cb);  // callback-функция срабатывающая при измениении состояния
```
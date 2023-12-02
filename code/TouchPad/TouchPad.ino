#include <BleMouse.h>

BleMouse bleMouse;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
// Just test touch pin - Touch0 is T0 which is on GPIO 1,T0~T10为GPIO0~10.

#define MAX_NO_TOUCH_VALUE 40000
typedef struct{
    int curXValue[5];
    int curYValue[5];
    
    int threShold;

  
}TouchPin_t;


//触摸状态
typedef enum touch_sta_e{
    RELEASE = 0,
    PRESSED = 1
}touch_sta;

//触摸事件
enum {
    ACT_NO        = 0,
    ACT_PRESS,
    ACT_SHORT_CLICKED,
    ACT_LONG_CLICKED,
    ACT_LONG_PRESSING,
}_touch_evt;

typedef struct{
    int curXPos;
    int curYPos;
    int lastXPos;
    int lastYPos;

    touch_sta curState;
    touch_sta lastState;
    
    int moveSpeed;
    int moveX;
    int moveY;
}TouchPad_t;




TouchPin_t touchInput; 
TouchPad_t TouchPad;

void ARDUINO_ISR_ATTR onTimer()
{
    portENTER_CRITICAL_ISR(&timerMux);
    if (_touch_evt == ACT_PRESS || _touch_evt ==  ACT_LONG_PRESSING)
        GetMoveDistance();
    portEXIT_CRITICAL_ISR(&timerMux);
}

void setup()
{
    Serial.begin(115200);
    bleMouse.begin();
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 20000, true);  // 20ms
    timerAlarmEnable(timer);
    delay(1000); // give me time to bring up serial monitor
    TouchInit();
    Serial.println("ESP32 Touch Test");
}

void loop()
{
    GetTouchInput();
    TouchProcess();
   // bleMouse.move(5, 5);
    //ReadTchInfo();
}

void TouchInit(void)
{
    int i;
    
    for (int i = 0;i < 5;i++)
    {
      touchInput.curXValue[i] = 0;
      touchInput.curYValue[i] = 0;
    }
    touchInput.threShold = MAX_NO_TOUCH_VALUE;
  //  touchAttachInterrupt(T1, gotTouchX1, touchInput.threShold); 
  //  touchAttachInterrupt(T2, gotTouchX2, touchInput.threShold); 
  //  touchAttachInterrupt(T3, gotTouchX3, touchInput.threShold); 
  //  touchAttachInterrupt(T4, gotTouchX4, touchInput.threShold);
  //  touchAttachInterrupt(T5, gotTouchX5, touchInput.threShold); 

    TouchPad.curState = RELEASE;
    
    TouchPad.lastState = RELEASE;
    
}

void GetTouchInput(void)
{
    /* x输入获取 */
    touchInput.curXValue[0] = touchRead(T1);
    touchInput.curXValue[1] = touchRead(T2);
    touchInput.curXValue[2] = touchRead(T3);
    touchInput.curXValue[3] = touchRead(T4);
    touchInput.curXValue[4] = touchRead(T5);

    /* y输入获取 */
    touchInput.curXValue[5] = touchRead(T6);
    touchInput.curXValue[6] = touchRead(T7);
    touchInput.curXValue[7] = touchRead(T8);
    touchInput.curXValue[8] = touchRead(T9);
    touchInput.curXValue[9] = touchRead(T10);
  
}

void ReadTchInfo(void)
{
    int i;
    Serial.println("                  "); 
    Serial.println("----------------------"); 

    for (i = 0;i < 5;i++)
    { 
      Serial.print("touchRead chx"); 
      Serial.print(i); 
      Serial.print(":  "); 
      Serial.println(touchInput.curXValue[i]); // get value using T1
    }
    Serial.println("----------------------"); 
    for (i = 0;i < 5;i++)
    { 
      Serial.print("touchRead chy"); 
      Serial.print(i); 
      Serial.print(":  "); 
      Serial.println(touchInput.curYValue[i]); // get value using T1
    }
    Serial.println("----------------------"); 
    Serial.println("                  "); 
    delay(500);


}

/***************
 * function: TouchProcess
 * descript: 触摸处理函数
 * return: 
 * date: 2023-12-03
 **************/
void TouchProcess(void)
{
    uint8_t keyIndex;
    static uint32_t startPrTimes; 
    for (keyIndex = 0;keyIndex <5;keyIndex++)
    {
        if ((touchInput.curXValue[keyIndex] >= touchInput.threShold) 
            || (touchInput.curYValue[keyIndex] >= touchInput.threShold))
        {
          TouchPad.curState = PRESSED;
        }
        else
        {
          TouchPad.curState = RELEASE;
        }
    }

    if (TouchPad.lastState == RELEASE)
    {
        switch (TouchPad.curState)
        {
        case RELEASE:
          /* code */
          _touch_evt = ACT_NO;
          break;
        case PRESSED:
          /* code */
          _touch_evt = ACT_PRESS;
          startPrTimes = Get_Sys_Time_tick();
          break;   
        default:
          break;
        }
    }
    else if (TouchPad.lastState == PRESSED)
    {
        switch (TouchPad.curState)
        {
            case RELEASE:
                if(Get_Sys_Time_tick() - startPrTimes > 1000)
                {
                    _touch_evt = ACT_LONG_CLICKED;
                }
                else 
                {
                    _touch_evt = ACT_SHORT_CLICKED;
                }
            break;
            
            case PRESSED:
                if(Get_Sys_Time_tick() - startPrTimes > 1000)
                {
                    _touch_evt = ACT_LONG_PRESSING;
                }
            break;
        }

    }

    TouchPad.lastState = TouchPad.curState;

    switch(_touch_evt)
    {
        case ACT_NO:
            break;
         
        case ACT_SHORT_CLICKED:
             touch_evt_proc_short_click();
        break;
            
        case ACT_LONG_CLICKED:
             touch_evt_proc_long_click();
        break;
            
        case ACT_PRESS:
             touch_evt_proc_press();
        break;
           
        case ACT_LONG_PRESSING:
             touch_evt_proc_long_pressing();
             
         break;
     }
    

   
}

uint32_t Get_Sys_Time_tick(void)
{
    uint32_t sysTickMs;

    sysTickMs = millis();

    return sysTickMs;
}

uint8_t GetTouchXPos(void)
{

    int count;
    const int*p = touchInput.curXValue;
    for(count = 1;count < 5;count++)
    {
      if(*p < touchInput.curXValue[count])
        p = &touchInput.curXValue[count];
    }
    
    return (uint8_t)(p-touchInput.curXValue);

}

uint8_t GetTouchYPos(void)
{

    int count;
    const int*p = touchInput.curYValue;
    for(count = 1;count < 5;count++)
    {
      if(*p < touchInput.curYValue[count])
        p = &touchInput.curYValue[count];
    }
    
    return (uint8_t)(p - touchInput.curYValue);

}

void GetMoveDistance()
{
    static uint32_t beginTime;
    int x_move, y_move;
    // beginTime = Get_Sys_Time_tick();
    TouchPad.curXPos = GetTouchXPos();
    TouchPad.curYPos = GetTouchYPos();

    // if ((Get_Sys_Time_tick() - beginTime) >= 10)
    // {
    x_move = TouchPad.curXPos - TouchPad.lastXPos;
    y_move = TouchPad.curYPos - TouchPad.lastYPos;

    TouchPad.moveX = x_move;
    TouchPad.moveY = y_move;
    TouchPad.lastXPos = TouchPad.curXPos;
    TouchPad.lastYPos = TouchPad.curYPos;
    // beginTime = Get_Sys_Time_tick();
    // }
}

void touch_evt_proc_short_click()
{
    Serial.println("TouchPad is short_click!!");
}

void touch_evt_proc_long_click()
{
    Serial.println("TouchPad is long_click!!");
}

void touch_evt_proc_press()
{
    int x, y;
    Serial.println("TouchPad is press!!");
    if(bleMouse.isConnected())
    {
        Serial.println("x:");
        Serial.println(TouchPad.moveX);
        Serial.println("y:");
        Serial.println(TouchPad.moveY);
        bleMouse.move(TouchPad.moveX*5, TouchPad.moveY*5);
//        bleMouse.move(0.5*x, 0.5*y);
 

    }
}

void touch_evt_proc_long_pressing()
{
    int x, y;
    if(bleMouse.isConnected())
    {
        Serial.println("x:");
        Serial.println(TouchPad.moveX);
        Serial.println("y:");
        Serial.println(TouchPad.moveY);
        bleMouse.move(TouchPad.moveX*5, TouchPad.moveY*5);
        
//        bleMouse.move(0.5*x, 0.5*y);
 

    }
    Serial.println("TouchPad is long press!!");
    Serial.print("Press x:");
    Serial.println(GetTouchXPos());
    Serial.print("Press y:");
    Serial.println(GetTouchYPos());
}

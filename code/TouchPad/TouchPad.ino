// ESP32 Touch Test
// Just test touch pin - Touch0 is T0 which is on GPIO 1,T0~T10为GPIO0~10.
#define MAX_NO_TOUCH_VALUE 4000
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

  touch_sta curSate;
  touch_sta lastSate;
  
  int moveSpeed;
  int moveX;
  int moveY;
}TouchPad_t;




TouchPin_t touchInput; 
TouchPad_t TouchPad;
void setup()
{
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  TouchInit();
  Serial.println("ESP32 Touch Test");
}

void loop()
{
  GetTouchInput();
  ReadTchInfo();
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

  TouchPad.curSate = RELEASE;
  
  TouchPad.lastSate = RELEASE;
    
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
  for (keyIndex = 0;keyIndex <5;keyIndex++)
  {
    if ((touchInput.curXValue[i] >= touchInput.threShold) 
        || (touchInput.curYValue[i] >= touchInput.threShold))
    {
      TouchPad.curSate = PRESSED;
      Serial.println("TouchPad is PRESSED!!"); 
    }
    else
    {
      TouchPad.lastSate = RELEASE;
    }
  }
  

   
}

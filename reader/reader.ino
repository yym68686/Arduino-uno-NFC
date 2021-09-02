#include "RC522.h"   //RC522模块所需头文件
#include <SPI.h>
#include <Servo.h>
#include <string.h>
#include <math.h>

Servo myservo;  // 创建一个伺服电机对象
unsigned char serNum[5]; //ic卡的id码
int decid=0;
int i=0;


void setup() 
{ 
    Serial.begin(9600); 
    Serial.println("welcome!");
    SPI.begin();   
    pinMode(chipSelectPin,OUTPUT); 
    digitalWrite(chipSelectPin, LOW); 
    pinMode(NRSTPD,OUTPUT); 
    MFRC522_Init();  
}

int time=0;

void loop()   //持续检测是否感应到ic卡
{   
    decid=0;
    unsigned char status;   //状态变量
    unsigned char str[MAX_LEN];
    status = MFRC522_Request(PICC_REQIDL, str);  //传入卡状态，2为无卡，0为感应到卡
    Serial.print(status);
    if (status == MI_OK)      //读取到ID卡时候，MI_OK=0
    {   
      Serial.print("MI_OK!");
      status = MFRC522_Anticoll(str);    
      if (status == MI_OK)    //下一个时刻仍能读取到ID卡
      {
          memcpy(serNum, str, 5);
          Serial.print("ID:");  
          ShowCardID(serNum);   //输出卡ID，16进制
          
          unsigned char* id = serNum;
          
          for(i=0;i<4;i++)
          {  
            decid=decid+id[i]*pow(16,i);    //16转10进制
          }
          Serial.print("DECID:");
          Serial.println(decid);   //输出卡DECID，10进制
      }
    }
    MFRC522_Halt();
    delay(500);   //每0.5s检测一次
}

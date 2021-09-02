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
    myservo.attach(8);  // 8号引脚输出舵机的控制信号
    MFRC522_Init();  
    myservo.write(0);  //舵机初始角设为0
    pinMode(8,OUTPUT);
}

int time=0;

void loop()   //持续检测是否感应到ic卡
{   
    decid=0;
    myservo.write(0);
    unsigned char status;   //状态变量
    unsigned char str[MAX_LEN];
    status = MFRC522_Request(PICC_REQIDL, str);  //传入卡状态，2为无卡，0为感应到卡
    Serial.print(status);
    if (status == MI_OK)      //读取到ID卡时候，MI_OK=0
    {   
      status = MFRC522_Anticoll(str);    
      if (status == MI_OK)    //下一个时刻仍能读取到ID卡
      {
          memcpy(serNum, str, 5);
          Serial.print("ID:");  
          ShowCardID(serNum);   //输出卡ID，16进制
          
      unsigned char* id = serNum;
      if( id[0]==0x70 && id[1]==0x52 && id[2]==0x22 && id[3]==0x85 ) //这里和下面的星号都是需要替换的。每一个循环是一张卡的id，如果要增加或者减少卡片张数自己修改代码改变循环就行
      {
          myservo.write(90);     // dakaiduoji
          Serial.println("The Host 1!");
          delay(1300);
          myservo.write(0);
      }
          for(i=0;i<4;i++){  
            decid=decid+id[i]*pow(16,i);    //16转10进制
          }
          Serial.print("DECID:");
          Serial.println(decid);   //输出卡DECID，10进制

          switch(decid){
            case 26989://lzs card    //允许通过的卡ID
            case 6005://smk phone
            case -27841://ycl phone
            //若需要添加新卡，在此处新添加一个case XXXX:

               Serial.println("Pass!");
               myservo.write(180);   //舵机转180度

               for(;status == MI_OK;)  //若卡一直贴在感应器上，保证门锁始终打开
               {
                  delay(2000);  //添加2s的刷卡延迟，方便单手开门
                  MFRC522_Halt();
                  delay(100);
                  status = MFRC522_Request(PICC_REQIDL, str);   //读取记录此时的卡状态
                  memcpy(serNum, str, 5);
               }

               myservo.write(0);   //舵机转回
             break;  //跳出case


             default:   //如果不在允许的ID中

                  Serial.println("Strange Card");
                  delay(10000); //暂停10秒
             break;  //跳出default
          }
      }
    }
    MFRC522_Halt();
    delay(500);   //每0.5s检测一次
}

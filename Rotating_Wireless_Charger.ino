#include <Stepper.h> // 스텝 모터 라이브러리 사용

// 모터의 한스텝 5.625도이므로 360도이면 64스텝이

const int steps = 2048;

// myStepper 객체 생성(스텝 수, IN4, IN3, IN2, IN1)
// IN1~4까지 각각 아두이노 D8~D11번에 연결
Stepper myStepper(steps,11,9,10,8);

int movecount=0;
int moveArray[]={410,409,410,409,410};
int LEDArray[]={0,0,0,0,0};

int count=0;
int level=0;

int mode = 0;
int infinite_mode = 0;

int Charge_job = 0;
long Charge_old_time = 0;
long Charge_new_time = 0;

long algorithm_old_time = 0;
long algorithm_new_time = 0;

int job = 0;
long old_time = 0;
long new_time = 0;

int LED_job = 0;
long LED_old_time = 0;
long LED_new_time = 0;

int move_jop = 0;
long move_old_time = 0;
long move_new_time = 0;


int readValue=0;

int Temp=0;
int l=0;



void setup() 
{
 // 스텝 모터 속도설정
 // 스텝 수와 연결되어 있음
 myStepper.setSpeed(10); // 15RPM구동
 Serial.begin(9600); // 9600bps로 시리얼 통신 시작
 pinMode(3, OUTPUT); // 충전기 바로 앞 LED(기준)
 pinMode(4, OUTPUT); // (기준 LED + 1) LED
 pinMode(5, OUTPUT); // (기준 LED + 2) LED
 pinMode(6, OUTPUT); // (기준 LED + 3) LED
 pinMode(7, OUTPUT); // (기준 LED + 4) LED
 pinMode(2, INPUT);  // 버튼
}

void loop() 
{
   algorithm();
   
}


void algorithm()
{
  readValue = digitalRead(2);
  level = analogRead(A3);
  
 
   switch(mode) 
  { 
   case 0: 
        if(readValue == HIGH)
        {
        
        mode++;
        for(int i=0;i<5;i++) LEDArray[i]=0;
        Illumination();
        algorithm_old_time = millis();

        }
        break;
   
   case 1: 
      algorithm_new_time = 0;
      if((algorithm_new_time - algorithm_old_time)>500)break;
    
      Charge_Signal_Management(); 
      LED_sparkle();
      LED_status();
      movement();

      if(movecount==5){
        mode=0;
        movecount=0;
        Illumination();
      }
      
      if(readValue == HIGH){
        mode++;
        infinite_mode=1;
        Illumination2();
        algorithm_old_time = millis();
        
       }
       
      break;
      
   case 2:
      algorithm_new_time = 0;
      if((algorithm_new_time - algorithm_old_time)>500) break;

      Charge_Signal_Management(); 
      LED_sparkle();
      LED_status();
      movement();
      if(movecount==5){
        movecount=0;
      }
      if(readValue == HIGH){
        mode=0;
        Illumination();
        movecount=0;
       }
      
      break;
   
   deault:
      mode=0;
      break; 
      
}
}


void Charge_Signal_Management() // 충전중 신호관리
{ 
  switch(Charge_job) 
  { 
   case 0: 
      Charge_old_time = millis();          
      Charge_job++; 
      break;  
   case 1: 
      Charge_new_time = millis(); 
      if((Charge_new_time - Charge_old_time)< 3000)break;
      
      
      movement();
      Charge_job = 0; 
      break;
   deault: 
      Charge_job = 0; 
      break; 
    }
 }

 void movement() // 회전
{ 
  switch(move_jop){
    
 
    case 0:
        if(level < 520) break;
        move_old_time = millis();
        move_jop++;
        break;
    
    case 1:
      move_new_time = millis();
      if((move_new_time - move_old_time)<4000)break;
      if(level > 520 ) {
        digitalWrite(3, LOW );
       
        myStepper.step(moveArray[movecount]);
        movecount++;
        Temp = LEDArray[4];
        LEDArray[4] = LEDArray[3] ;
        LEDArray[3] = LEDArray[2] ;
        LEDArray[2] = LEDArray[1] ;
        LEDArray[1] = LEDArray[0] ;
        LEDArray[0] = 0;
        if(mode==1)LEDArray[0] = Temp;
    }
    move_jop=0;
    break;
    }
}

void LED_sparkle() // LED 상황판 3
{ 
  if(level <= 520){ 
    switch(LED_job) 
    { 
       case 0: 
          LED_old_time = millis();          
          LED_job++; 
         break;  
      
      case 1: 
         LED_new_time = millis(); 
         if((LED_new_time - LED_old_time)< 1000)break;
         digitalWrite(3, HIGH);
         LED_job++; 
         LED_old_time = millis();   
         break;
         
      case 2: 
         LED_new_time = millis(); 
         if((LED_new_time - LED_old_time)< 1000)break;    
         digitalWrite(3, LOW);
         LED_job = 0; 
         break;
        }
    }
}

 void LED_status() // LED 상황판 4,5,6,7
{ 
  if(level < 520) LEDArray[0] = 1;
  for(int i = 4; i<8; i++) digitalWrite(i, LEDArray[i-3]);
  for(int i = 0; i<5; i++) Serial.print( LEDArray[i] );Serial.print(" ");
  Serial.print( movecount );Serial.print(" "); Serial.print( level  );
  Serial.println();
}

 void Illumination(){ // 켜질때 꺼질때 동작
   for(int j = 0; j<2; j++){
      for(int i = 3; i<8; i++) digitalWrite(i, HIGH );
      delay(500);
      for(int i = 3; i<8; i++) digitalWrite(i, LOW);
      delay(500); 
      for(int i = 3; i<8; i++) digitalWrite(i, LEDArray[i-3]);
   }
 }

 void Illumination2(){ // 무한 모드 동작 LED
   if(infinite_mode%2==1){
    for(int i = 3; i<8; i++){
      digitalWrite(i, HIGH); delay(50);
      digitalWrite(i, LOW); delay(50);
     }
   }
   else{
    for(int i = 7; i>2; i--){
      digitalWrite(i, HIGH); delay(50);
      digitalWrite(i, LOW); delay(50);
      }
   }
   }


// 버튼 and 모드 불빛 
// LED 불빛 제어(판이 돌면 같이 상황값 전달)
// 모터  

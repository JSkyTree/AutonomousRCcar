#include <LiquidCrystal_I2C.h>
#include <Wire.h>
//////////////////////I2C
#define SLAVE 4 // 슬레이브 주소
int i=0;
int Lat = 0, Long = 0;
int Lat1 = 0, Lat2 = 0;
int Long1 = 0, Long2 = 0;

//////////////////////LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

////////////////////// 초음파센서
const int trig1 = 12;
const int echo1 = 13;

const int trig2 = 4;
const int echo2 = 5;

const int trig3 = 2;
const int echo3 = 3;

//////////////////////// 모터
const int L_M_status = 9;
const int L_M_L = 10;
const int L_M_R = 11;

const int R_M_status = 6;
const int R_M_L = 7;
const int R_M_R = 8;

//////////////////////////
bool flag = 0, M_flag = 0, T_flag=0;
int cnt=0;

int Target_Lat = 5532, Target_Long = 5979;
int last_Error_Lat = 0, last_Error_Long = 0;
int first_Error_Lat = 0, first_Error_Long = 0;

int Error_code = 0;

void setup()
{
  Serial.begin(9600);
  Wire.begin();// Wire 라이브러리 초기화
  //////////////////////// LCD
  lcd.init();
  lcd.backlight();
  //////////////////////// 초음파센서set
  pinMode(trig1,OUTPUT);
  pinMode(echo1,INPUT);
  pinMode(trig2,OUTPUT);
  pinMode(echo2,INPUT);
  pinMode(trig3,OUTPUT);
  pinMode(echo3,INPUT);
  
  ///////////////////////// 모터set
  
  pinMode(L_M_status,OUTPUT);
  pinMode(L_M_L,OUTPUT);
  pinMode(L_M_R,OUTPUT);
  
  pinMode(R_M_status,OUTPUT);
  pinMode(R_M_L,OUTPUT);
  pinMode(R_M_R,OUTPUT);
}

/////////////////////////// LCD
void print_LCD()
{
  lcd.clear();
  
  if(D_sensor(trig1) < 30 || D_sensor(trig2) < 30 || D_sensor(trig3) < 30)
  {
    lcd.setCursor(8,1);
    lcd.print(D_sensor(trig1));
    lcd.setCursor(4,1);
    lcd.print(D_sensor(trig2));
    lcd.setCursor(0,1);
    lcd.print(D_sensor(trig3));
  } 
  
    lcd.setCursor(0,0);
    lcd.print(Lat);
    lcd.setCursor(5,0);
    lcd.print(Long);
    
}
/////////////////////////// Distance
int D_sensor(int trig_num)
{
  int distance = 0; 
  digitalWrite(trig_num, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_num, LOW);
  
  distance = pulseIn(trig_num+1,HIGH)/58;

  if(distance > 100)  distance = 100;

  
  
  return distance;
}
//////////////////////////// Movement

void M_Start(int L_now_speed, int R_now_speed, int rate)
{
  if(flag==0)  flag=1;
  else  rate = 1;
    
  float L_val=0, R_val=0;
  float i = L_now_speed, j = R_now_speed;
  
  L_val = L_now_speed/(float)rate;
  R_val = R_now_speed/(float)rate;   
  
  while(1)
  { 
    i = i-L_val;
    j = j-R_val;
    
    analogWrite(L_M_status, L_now_speed-i);
    analogWrite(R_M_status, R_now_speed-j);
    
    if(i<=0 && j<=0)  break;
  }
}

void M_Brake(int L_now_speed, int R_now_speed, int rate)
{
  if(flag==1) flag=0;
  
  float L_val=0, R_val=0;
  
  float i = L_now_speed, j = R_now_speed;
  
  L_val = L_now_speed/(float)rate;
  R_val = R_now_speed/(float)rate;
  
  while(1)
  { 
    i = i-L_val;
    j = j-R_val;
    
    analogWrite(L_M_status, i);
    analogWrite(R_M_status, j);
    
    if(i<=0 && j<=0)  break;
  }
}


void Custom_Move(int L_get_speed, int R_get_speed, char LM_set, char RM_set, int rate)
{
  
  if(LM_set == 0)
  {
    digitalWrite(L_M_L, LOW);
    digitalWrite(L_M_R, LOW);
  }
  else if(LM_set == 1)
  {
    digitalWrite(L_M_L, HIGH);
    digitalWrite(L_M_R, LOW);
  }
  else
  {
    digitalWrite(L_M_L, LOW);
    digitalWrite(L_M_R, HIGH);
  }
  
  if(RM_set == 0)
  {
    digitalWrite(R_M_L, LOW);
    digitalWrite(R_M_R, LOW);
  }
  else if(RM_set == 1)
  {
    digitalWrite(R_M_L, HIGH);
    digitalWrite(R_M_R, LOW);
  }
  else
  {
    digitalWrite(R_M_L, LOW);
    digitalWrite(R_M_R, HIGH);
  }
  
  if(L_get_speed>255) L_get_speed = 255;
  if(R_get_speed>255) R_get_speed = 255;
    
  M_Start(L_get_speed, R_get_speed, rate);
  
}
//////////////////////////////////////////

void Evasion_Move()
{
  if(D_sensor(trig2) > 30)
  {
    M_flag = 1;
    if(D_sensor(trig1) < 30 || D_sensor(trig3) < 30)
    {
      if(D_sensor(trig1) > D_sensor(trig3))
      {
        Custom_Move(100, 200, 1, -1,100);
      }
      else
      {
        Custom_Move(200, 100, 1, -1,100);
      }
    }
    else Custom_Move(200, 200, 1, -1,100);
  }
  else if(D_sensor(trig2) <= 30)
  {
    if(M_flag == 1)
    {
      M_Brake(200,200,1000);
      M_flag = 0;
    }

    if(D_sensor(trig1) < 20 && D_sensor(trig3) < 20)
    {
      Custom_Move(200, 200, 1, 1,100);
    }
    else if(D_sensor(trig1) < 30 && D_sensor(trig3) < 30)
    {
      Custom_Move(200, 0, 1, 0,100);
    }
    else
    {
      if(D_sensor(trig1) > D_sensor(trig3))
      {
        Custom_Move(0, 200, 0, -1,100);
      }
      else
      {
        Custom_Move(200, 0, 1, 0,100);
      }
    }
  }
}

bool return_val(int Error_Lat, int Error_Long)
{
  if(Error_Lat < 0)
  {
    Error_Lat *= -1; 
  }

  if(Error_Long < 0)
  {
    Error_Long *= -1;
  }

  if(Error_Lat < 3 && Error_Long < 3)
  {
    Serial.println("gool");
    return 1;
  }
  else return 0;
}

void Target_Move()
{
  int Error_Lat = Target_Lat - Lat;
  int Error_Long = Target_Long - Long;

  if(return_val(Error_Lat, Error_Long))
  {
    while(1)
    {
      
    Custom_Move(0, 0, 1, -1,100);
    }
  }
  

  if(T_flag == 0)
  {
    first_Error_Lat = Error_Lat;
    first_Error_Long = Error_Long;
    T_flag = 1;
  }

  if(cnt <= 1)
  {
    last_Error_Lat = Error_Lat;
    last_Error_Long = Error_Long;

  Serial.print("last_Error_Lat : ");
  Serial.println(last_Error_Lat);
  Serial.print("last_Error_Long : ");
  Serial.println(last_Error_Long);
  }

  if(Error_Lat >= 0 && Error_Long >= 0)//우상
  {
    
    if(cnt >= 100)
    {
      Serial.println("RightUP");
      if(last_Error_Lat - Error_Lat < -2 && last_Error_Long - Error_Long < -2)
      {
        Error_code = 3;
      }
      else if(last_Error_Lat - Error_Lat < -2)
      {
        Error_code = 2;
      }
      else if(last_Error_Long - Error_Long < -2)
      {
        Error_code = 1;
      }
      else Error_code = 0;
      
      cnt = 0;
      Serial.println(Error_code);
    }

  }
  else if(Error_Lat <= 0 && Error_Long >= 0)//우하
  {
    
    if(cnt >= 100)
    {
      Serial.println("RightDown");
      if(last_Error_Lat - Error_Lat > 2 && last_Error_Long - Error_Long < -2)
      {
        Error_code = 3;
      }
      else if(last_Error_Lat - Error_Lat > 2)
      {
        Error_code = 1;
      }
      else if(last_Error_Long - Error_Long < -2)
      {
        Error_code = 2;
      }
      else Error_code = 0;
      cnt = 0;
      Serial.println(Error_code);
    }
  }
  else if(Error_Lat >= 0 && Error_Long <= 0)//좌상
  {
    
    if(cnt >= 100)
    {
      Serial.println("leftUP");
      if(last_Error_Lat - Error_Lat < -2 && last_Error_Long - Error_Long > 2)
      {
        Error_code = 3;
      }
      else if(last_Error_Lat - Error_Lat < -2)
      {
        Error_code = 1;
      }
      else if(last_Error_Long - Error_Long > 2)
      {
        Error_code = 2;
      }
      else Error_code = 0;
      cnt = 0;
      Serial.println(Error_code);
    }
  }
  else if(Error_Lat <= 0 && Error_Long <= 0)//좌하
  {
    if(cnt >= 100)
    {
      Serial.println("leftDown");
      if(last_Error_Lat - Error_Lat > 2 && last_Error_Long - Error_Long > 2)
      {
        Error_code = 3;
      }
      else if(last_Error_Lat - Error_Lat > 2)
      {
        Error_code = 2;
      }
      else if(last_Error_Long - Error_Long > 2)
      {
        Error_code = 1;
      }
      else Error_code = 0;
      cnt = 0;
      Serial.println(Error_code);
    }
  }
  

  if(Error_code == 3)
  {
    
      Custom_Move(200, 200, 1, 1,100);
      delay(500);
    Error_code = 0;
    cnt = 0;
  }
  else if(Error_code == 2)
  {
    Custom_Move(50, 200, 1, -1,100);
  }
  else if(Error_code == 1)
  {
    Custom_Move(200, 50, 1, -1,100);
  }
  else if(Error_code == 0)
  {
    Custom_Move(200, 200, 1, -1,100);
   // Serial.println("go");
  }
  
  cnt++;
}

void I2C_GPS_Set()
{
  i2c_communication();
  Lat_Long();
}

void Lat_Long()
{
  Lat = (Lat1*100) + Lat2;
  Long = (Long1*100) + Long2;
  
  Serial.print("Lat : ");
  Serial.println(Lat);
  Serial.print("Long : ");
  Serial.println(Long);
  
}


void i2c_communication() {
  Wire.beginTransmission(SLAVE);
  Wire.write(i);
  Wire.endTransmission();
 
  Wire.requestFrom(SLAVE, 4); // 4 바이트 크기의 데이터 요청
  if(Wire.available()>1)
  {
    int c = Wire.read(); // 수신 데이터 읽기
    if(i==0)  Lat1 = c;
    else if(i==1) Lat2 = c;
    else if(i==2) Long1 = c;
    else if(i==3) Long2 = c;
    
    if(i>3) i=0;
    else  i++;
    
  }
}


void loop()
{
  I2C_GPS_Set();
  print_LCD();

  if(D_sensor(trig1) < 30 || D_sensor(trig2) < 30 || D_sensor(trig3) < 30)
  {
    Evasion_Move();
  }
  else
  {
    Target_Move();
  }
  
  
}

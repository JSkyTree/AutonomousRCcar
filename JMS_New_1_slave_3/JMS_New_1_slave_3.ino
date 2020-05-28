#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial gpsSerial(A0,A1);

char c = ""; // Wn 인지 구분 및 str에 저장.
String str = ""; // \n 전까지 c 값을 저장.
String targetStr = "GPGGA"; // str의 값이 NMEA의 GPGGA 값인지 타겟
int LatI=0;
int LongI=0;
byte rec[128];
int i=0;

/////////////////////////////
#define SLAVE 4


void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  // Wire 라이브러리 초기화
  // 슬레이브로 참여하기 위해서는 주소를 지정해야 한다.
  Wire.begin(SLAVE);
  // 마스터의 데이터 전송 요구가 있을 때 처리할 함수 등록
  Wire.onRequest(Send_gps);
}

void loop () {
 receive();
 setgps();

}
void receive()
{
  Wire.onReceive(record);
}
void record(int receiveNum)
{
  i = Wire.read();
}

void Send_gps()
{
    if(i==0)
    {
      int Lat1 = LatI/100;
      Wire.write(Lat1);
    }
    else if(i==1)
    {
      int Lat2 = LatI%100;
      Wire.write(Lat2);
    }
    else if(i==2)
    {
      int Long1 = LongI/100;
      Wire.write(Long1);
    }
    else if(i==3)
    {
      int Long2 = LongI%100;
      Wire.write(Long2);
    }
}
void setgps()
{
  if(gpsSerial.available()) // gps 센서 통신 가능?
    {
      c=gpsSerial.read(); // 센서의 값 읽기

      if(c == '\n'){ // \n 값인지 구분.

        // \n 일시. 지금까지 저장된 str 값이 targetStr과 맞는지 구분

        if(targetStr.equals(str.substring(1, 6))){

          // , 를 토큰으로서 파싱.

          int first = str.indexOf(",");

          int two = str.indexOf(",", first+1);

          int three = str.indexOf(",", two+1);

          int four = str.indexOf(",", three+1);

          int five = str.indexOf(",", four+1);

          // Lat과 Long 위치에 있는 값들을 index로 추출

          String Lat = str.substring(two+1, three);

          String Long = str.substring(four+1, five);

          // Lat의 앞값과 뒷값을 구분

          String Lat1 = Lat.substring(0, 2);

          String Lat2 = Lat.substring(2);

          // Long의 앞값과 뒷값을 구분

          String Long1 = Long.substring(0, 3);

          String Long2 = Long.substring(3);

          // 좌표 계산.

          double LatF = Lat1.toDouble() + Lat2.toDouble()/60;

          float LongF = Long1.toFloat() + Long2.toFloat()/60;


          LatI = (LatF*10-(int)(LatF*10))*10000+1;
          LongI = (LongF*10-(int)(LongF*10))*10000+1;


          // 좌표 출력.

          Serial.print("Lat : ");

          Serial.println(LatI);

          Serial.print("Long : ");

          Serial.println(LongI);

        }

        // str 값 초기화 

        str = "";

      }else{ // \n 아닐시, str에 문자를 계속 더하기

        str += c;

      }

    }
}

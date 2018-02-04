/*
 * Author: Zixiao Liu
 * Acc：加速度， Gryo：陀螺仪，Ang：角度，Sta：状态
 */

# include <Wire.h>
# include <JY901.h>
# include <SoftwareSerial.h>
# define maxLength 1500

typedef struct data {
    struct data *next;
    struct data *prev;
    float acc[3];
    float gry[3];
    float ang[3];
//    int sta[4];
} *Data;

Data head;
Data tail;
static int len;

//union SeFrame {
//  float Float;
//  byte Byte[4];
//};
//
//SeFrame Sefram;



SoftwareSerial BT(10, 11); 

void freeData(Data data) {
    data -> prev -> next = data -> next;
    data -> next -> prev = data -> prev;
    len --;
    free(data);
}

void addDataToTail(Data data) {
  data -> next = tail;
  data -> prev = tail -> prev;
  tail -> prev -> next = data;
  tail -> prev = data;
  len ++;
}
  
//void Send_float(float FLOAT) { 
//    Sefram.Float= FLOAT;
//    Serial.write(Sefram.Byte[0]);
//    Serial.write(Sefram.Byte[1]);
//    Serial.write(Sefram.Byte[2]);
//    Serial.write(Sefram.Byte[3]); 
//}

void SendToSerial(Data data) {
  Serial.print("a");
  for (int i = 0; i < 3; i ++) {
    Serial.print(data -> acc[i]);
    Serial.print(" ");
  }
  Serial.print("g");
  for (int i = 0; i < 3; i ++) {
    Serial.print(data -> gry[i]);
    Serial.print(" ");
  }
  Serial.print("A");
  for (int i = 0; i < 3; i ++) {
    Serial.print(data -> ang[i]);
    Serial.print(" ");
  }
//  Serial.print("s");
//  for (int i = 0; i < 4; i ++) {
//    Serial.print(data -> sta[i]);
//    Serial.print(" ");
//  }
}

void getAcc(Data data) {
  data -> acc[0] = (float)JY901.stcAcc.a[0] / 32768 * 16;
  data -> acc[1] = (float)JY901.stcAcc.a[1] / 32768 * 16;
  data -> acc[2] = (float)JY901.stcAcc.a[2] / 32768 * 16;
}

void getGryo(Data data) {
  data -> gry[0] = (float)JY901.stcGyro.w[0] / 32768 * 2000;
  data -> gry[1] = (float)JY901.stcGyro.w[1] / 32768 * 2000;
  data -> gry[2] = (float)JY901.stcGyro.w[2] / 32768 * 2000;
}

void getAng(Data data) {
  data -> ang[0] = (float)JY901.stcAngle.Angle[0] / 32768 * 180;
  data -> ang[1] = (float)JY901.stcAngle.Angle[1] / 32768 * 180;
  data -> ang[2] = (float)JY901.stcAngle.Angle[2] / 32768 * 180;
}

//void getSta(Data data) {
//  data -> sta[0] = JY901.stcDStatus.sDStatus[0];
//  data -> sta[1] = JY901.stcDStatus.sDStatus[1];
//  data -> sta[2] = JY901.stcDStatus.sDStatus[2];
//  data -> sta[3] = JY901.stcDStatus.sDStatus[3];
//}

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  len = 0;
  head -> next = tail;
  tail -> prev = head;
}

void loop() {
  Data data;
  getAcc(data);
  getGryo(data);
  getAng(data);
//  getSta(data);
  if (len >= maxLength) {
    freeData(head -> next);
  }
  addDataToTail(data);
  if (Serial.available()) {
    delay(500);
    int l = len;
    for (int i = 0; i < l; i ++) {
      Serial.write("//");
      SendToSerial(head -> next);
      freeData(head -> next);
    }
  }
  delay(20); //50 hz
}
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */


void serialEvent() {
  while (Serial.available()) {
    JY901.CopeSerialData(Serial.read()); //Call JY901 data cope function
  }
}


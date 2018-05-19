/*
 * Author: Zixiao Liu
 * Acc：加速度， Gryo：陀螺仪，Ang：角度
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
  unsigned long tim;
} Data;

static Data *head = new Data();
static Data *tail = new Data();
static int len;

SoftwareSerial BTSerial(10, 11);

// left listen, right write
void freeData(Data *data) {
  if (data == tail) {
	 return;
  }
  data->prev->next = data->next;
  data->next->prev = data->prev;
  len--;
  data->prev = NULL;
  data->next = NULL;
  delete (data);
}

void addDataToTail(Data *data) {
  data->next = tail;
  data->prev = tail->prev;
  data->prev->next = data;
  tail->prev = data;
  len++;
}

/* Use hardware serial*/

//void SendToSerial(Data *data) {
//  
//  Serial.print("a");
//  for (int i = 0; i < 3; i ++) {
//    Serial.print(data -> acc[i]);
//    Serial.print(" ");
//  }
//  Serial.print("g");
//  for (int i = 0; i < 3; i ++) {
//    Serial.print(data -> gry[i]);
//    Serial.print(" ");
//  }
//  Serial.print("A");
//  for (int i = 0; i < 3; i ++) {
//    Serial.print(data -> ang[i]);
//    Serial.print(" ");
//  }
//  Serial.print("[");
//  Serial.print(data -> tim);
//  Serial.print("]");
//}

/* Use software serial */
void SendToSerial(Data *data) {

  BTSerial.print("a");
  for (int i = 0; i < 3; i++) {
	 BTSerial.print(data->acc[i]);
	 BTSerial.print(" ");
  }
  BTSerial.print("g");
  for (int i = 0; i < 3; i++) {
	 BTSerial.print(data->gry[i]);
	 BTSerial.print(" ");
  }
  BTSerial.print("A");
  for (int i = 0; i < 3; i++) {
	 BTSerial.print(data->ang[i]);
	 BTSerial.print(" ");
  }
  BTSerial.print("[");
  BTSerial.print(data->tim);
  BTSerial.print("]");
}

void getAcc(Data *data) {
  data->acc[0] = (float) JY901.stcAcc.a[0] / 32768 * 16;
  data->acc[1] = (float) JY901.stcAcc.a[1] / 32768 * 16;
  data->acc[2] = (float) JY901.stcAcc.a[2] / 32768 * 16;
}

void getGryo(Data *data) {
  data->gry[0] = (float) JY901.stcGyro.w[0] / 32768 * 2000;
  data->gry[1] = (float) JY901.stcGyro.w[1] / 32768 * 2000;
  data->gry[2] = (float) JY901.stcGyro.w[2] / 32768 * 2000;
}

void getAng(Data *data) {
  data->ang[0] = (float) JY901.stcAngle.Angle[0] / 32768 * 180;
  data->ang[1] = (float) JY901.stcAngle.Angle[1] / 32768 * 180;
  data->ang[2] = (float) JY901.stcAngle.Angle[2] / 32768 * 180;
}

void getTime(Data *data) {
  data->tim = millis();
}

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  len = 0;
  head->next = tail;
  tail->prev = head;
  tail->next = NULL;
  head->prev = NULL;
}

void loop() {
  Data *data = new Data();
  getAcc(data);
  getGryo(data);
  getAng(data);
  getTime(data);

  if (len >= maxLength) {
	 freeData(head->next);
  }
  addDataToTail(data);
  if (BTSerial.available()) {
//  if (Serial.available()) {
	 int l = len;
	 for (int i = 0; i < l; i++) {
		SendToSerial(head->next);
		freeData(head->next);
	 }
  }

  /* Should be changed, after setup passback freqency */
  delay(500); //0.5s
}

void serialEvent() {
  while (Serial.available()) {
	 JY901.CopeSerialData(Serial.read()); //Call JY901 data cope function
  }
}


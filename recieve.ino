#define PIN_IRRECEIVER 4

void setup()
{
  Serial.begin(115200); // PCとの通信速度
  pinMode(PIN_IRRECEIVER, INPUT);
}

void loop()
{
  static int s_iState_prev = HIGH;
  static unsigned long s_ulMicros_prev = 0;

  int iState = digitalRead(PIN_IRRECEIVER); // センサー値を読み取る

  if( iState != s_iState_prev )
  { // 前回センサー値が変化した時間からの経過時間を算出してシリアルに書き出し
    s_iState_prev = iState;
    unsigned long ulMicros = micros();
    Serial.print( ulMicros - s_ulMicros_prev );
    Serial.print( ", " );
    s_ulMicros_prev = ulMicros;
  }
}
//http://www.hiramine.com/physicalcomputing/arduino/irreceiver.html

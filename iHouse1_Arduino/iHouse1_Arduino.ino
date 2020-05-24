
//RFID程式庫---------------
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
//-------------------------
#include <StepperMotor.h> //步進馬達程式庫
#include <SoftwareSerial.h> //藍牙程式庫
#include <Adafruit_NeoPixel.h> //LED燈環&燈條程式庫
#define STEPS 4076 //步進馬達步數
#define redLED 3
#define greenLED 5
#define blueLED 6
#define SS_PIN A4
#define RST_PIN A5
//建立藍牙物件
SoftwareSerial bluetooth(8, 7);
//建立燈環物件, 16顆LED, pin腳=10
Adafruit_NeoPixel circleLED = Adafruit_NeoPixel(16, 10);
//建立燈條物件, 15顆LED, pin腳=11
Adafruit_NeoPixel barLED = Adafruit_NeoPixel(15, 9);
//建立步進馬達物件, pin腳=4,5,6,7
StepperMotor stepper(A0,A1,A2,A3);
int motorSpeed = 1; // 速度，數字愈大，速度愈慢，在5v電力下，最快可達15 rpm
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int val = 255;
int currentCircle = 0;
const int PIRSensor = 4; //人體感測器 , pin=2
int sensorValue = LOW;
void setup() {
  Serial.begin(9600);   // 與電腦序列埠連線
  Serial.println("BT is ready!");
  bluetooth.begin(9600);
  stepper.setStepDuration(motorSpeed); //設定步進馬達轉速

  //設定室內LED燈為白燈
  analogWrite(redLED, 0);
  analogWrite(greenLED, 0);
  analogWrite(blueLED, 0);

  //燈條初始化, 設為全暗
  barLED.begin();
  barLED.setBrightness(32);
  barLED.show();

  //燈環初始化
//  pinMode(buttonPin, OUTPUT);  // 將按鈕腳位設為數位輸入腳位
  // 指定 1 號中斷發生 RISING (電位由低變高) 時,
  // 呼叫自訂的函式 myISR
//  attachInterrupt(1, myISR, RISING);
  circleLED.begin();              // 將燈條物件初始化, 並設為全暗
  circleLED.setBrightness(32);    // 將整體亮度降為 1/8
  circleLED.show();               // 讓燈條顯示出所設定的狀態

  //RFID初始化
  SPI.begin();      // 設定  SPI bus
  mfrc522.PCD_Init();   // 設定 MFRC522
  Serial.println(" Put your card close to the reader...");
  Serial.println();

  //PIRSensor初始化
  pinMode(PIRSensor, INPUT);
}
bool checkSensor(){
  sensorValue = digitalRead(PIRSensor);
  if(sensorValue == HIGH){
//    Serial.println("Body Detected");
    bluetooth.print("1");
    return true;
  }else if(sensorValue == LOW){
//    Serial.println("Body Disappeared");
    bluetooth.print("0");
    return false;
  }
}
void loop() {
  RFID();
  checkSensor();
  btData();
  byte isUser = val/100;
  byte controll = (val%100)/10;
  byte option = (val%100)%10;
  if(isUser == 1){
    switch(controll){
      case 1: //控制門
        if(option == 0)
          stepper.step(STEPS/4.5);
        if(option == 1)
          stepper.step(-STEPS/4.5);
        break;
      case 2://控制室內LED的Red
        analogWrite(redLED,(255/9)*option);
        break;
      case 3://控制室內LED的Green
        analogWrite(greenLED,(255/9)*option);
        break;
      case 4://控制室內LED的Blue
        analogWrite(blueLED,(255/9)*option);
        break;
      case 5:
        startCircleLED(option);
        break;
      case 6:
        startBarLED();
        break;
    }
  }
  val=255;
//  stepper.step(STEPS/4.5); //步進馬達轉動
//  delay(1000);
//  stepper.step(-STEPS/4.5); //步進馬達轉動
//  delay(1000);
}

void btData(){
  // 若收到「序列埠監控視窗」的資料，則送到藍牙模組
  if (Serial.available()) {
    val= Serial.read();
    bluetooth.print(val);
  }
  // 若收到藍牙模組的資料，則送到「序列埠監控視窗」，check if anything in UART buffer
  if (bluetooth.available()) {
    val=bluetooth.read();
    Serial.println(val);
  }
}
//燈條模組
void startBarLED(){
  int r=random(64,255);
  int g=random(64,255);
  int b=random(64,255);
  meteorLamp(r, g,b, 8, 35); // 顯示白色長度8的流星,移動間隔35ms
  delay(500);
}

//燈條迴圈1
void meteorLamp(int r, int g, int b, int len, int delayMs) {
  for(int i = (0-len); i <= (int)barLED.numPixels(); i++) {
    meteor(r, g, b, i, len);  // 顯示靜態的流星
//    val=bluetooth.read();
//    byte isUser = val/100;
//    byte controll = (val%100)/10;
//    if(controll == 6){
//      byte option = (val%100)%10;
//      if(option == 1){
//      barLED.clear();
//      barLED.show();
//      }
//    }           
      delay(delayMs);// 暫停指定的毫秒
  }
}

//燈條迴圈2
void meteor(int r, int g, int b, int pos, int len) {
  int n     = 255 / ((1+len)*len/2);      // 算出梯形的 n
  int bright= 255 % ((1+len)*len/2);      // 算出餘數做為初始亮度
  int cnt = 1;                            // 計數 (由第 1*n 開始計算增量)
  barLED.clear();                          // 先清為全暗
  for (int i = pos; i < pos+len; i++) {   // 由最暗到最亮
    bright += cnt*n;                      // 計算新亮度 (= 原亮度+增量)
    cnt++;                                // 將計數加 1
    if(i>=0 && i<(int)barLED.numPixels()){ // 位置在燈條的範圍內才點亮
      barLED.setPixelColor(i,              // 點亮目前位置的 LED
            map(bright, 0, 255, 0, r),    // 利用 map() 做紅色的範圍對映
            map(bright, 0, 255, 0, g),    // 利用 map() 做綠色的範圍對映 
            map(bright, 0, 255, 0, b));   // 利用 map() 做藍色的範圍對映
    }
  }
  barLED.show();  // 將設定實際顯示出來
}

//燈環
void startCircleLED(int n){
  byte rr=255, gg=255, bb=255;  // 設定特效要使用的顏色
  currentCircle = n;
  // 依照 n 值來判斷應該執行哪一個特效
  switch (n) {
    case 0: 
      circleLED.clear(); circleLED.show();  // 關閉 LED
      break;
    case 1: 
      rgbLoop();  // 漸變循環
      break;
    case 2: 
      theaterChase(circleLED.Color(rr, gg, bb), 50);  // 劇院追逐
      break;
    case 3: 
      theaterChaseRainbow(50);  // 劇院追逐-彩虹色
      break;
    case 4: 
      rainbow(20);  // 彩虹
      break;
    case 5: 
      rainbowCycle(20);  // 彩虹圓圈
      break;
    case 6: 
      strobe(rr, gg, bb, 10, 50, 1000);  // 閃光
      break;
    case 7: 
      runningLights(rr, gg, bb, 80);  // 流光
      break;
    case 8: 
      cylonBounce(rr, gg, bb, 2, 70, 70);  // 賽隆人
      break;
    case 9: 
      sparkle(rr, gg, bb, 0);  // 發泡
      break;
  }
}

//燈環自訂的中斷處理函式
//void myISR() {
//  // 不同的按壓開關硬體, 可能需使用不同的時間間隔來消除彈跳, 
//  // 若測試按鈕時發現異常, 請嘗試調整時間間隔
//  if (millis() - lastTime > 200) { // 距上次中斷已超過一段時間
//    n = (n + 1) % 9;  // 每按一次鈕則 n 值加 1, n 值會在 0~8 之間循環
//    buttonState = HIGH;
//    lastTime = millis();  // 記錄本次中斷的時間
//  } 
//}
//

//// 燈環檢查按鈕是否被按下
bool isButtonPressed() {
  
    val=bluetooth.read();
  byte isUser = val/100;
  byte controll = (val%100)/10;
  byte option = (val%100)%10;
//  if(isUser==1){
  if(controll == 5){
    circleLED.clear();
    circleLED.show();
    startCircleLED(option);
    return true;
    }
  else if(controll == 1){
    if(option == 0)
      stepper.step(STEPS/4.5);
    else if(option == 1)
      stepper.step(-STEPS/4.5);
    circleLED.clear();
    circleLED.show();
    startCircleLED(currentCircle);
    return true;
  }
  else if(RFID()){
    circleLED.clear();
    circleLED.show();
    startCircleLED(currentCircle);
    return true;
  }
  else if(checkSensor()){
    circleLED.clear();
    circleLED.show();
    startCircleLED(currentCircle);
    return true;
  }
  else if(controll == 6){
    startBarLED();
    circleLED.clear();
    circleLED.show();
    startCircleLED(currentCircle);
    return true;
  }else if(controll == 2){
    analogWrite(redLED,(255/9)*option);
    circleLED.clear();
    circleLED.show();
    startCircleLED(currentCircle);
    return true;
  }else if(controll == 3){
    analogWrite(greenLED,(255/9)*option);
    circleLED.clear();
    circleLED.show();
    startCircleLED(currentCircle);
    return true;
  }else if(controll == 4){
    analogWrite(blueLED,(255/9)*option);
    circleLED.clear();
    circleLED.show();
    startCircleLED(currentCircle);
    return true;
  }
  else
    return false;
  }


// 燈環點亮所有 LED, 並顯示出來
void showAll(int r, int g, int b) {
  for (int i = 0; i < circleLED.numPixels(); i++) {
    circleLED.setPixelColor(i, r, g, b);
  }
  circleLED.show();    // 讓燈條顯示出來
}

// 燈環漸變循環特效
void rgbLoop() {
  while(true){
  for (int j = 0; j < 3; j++ ) {
    // Fade IN 漸強
    for (int k = 0; k < 256; k++) {
      switch (j) {
        case 0: showAll(k, 0, 0); break;
        case 1: showAll(0, k, 0); break;
        case 2: showAll(0, 0, k); break;
      }
      delay(5);
      if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
    }

    // Fade OUT 漸弱
    for (int k = 255; k >= 0; k--) {
      switch (j) {
        case 0: showAll(k, 0, 0); break;
        case 1: showAll(0, k, 0); break;
        case 2: showAll(0, 0, k); break;
      }
      delay(5);
      if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
    }
    if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
  }
  }
}

// 燈環劇院追逐特效
void theaterChase(uint32_t c, uint8_t wait) {
  while(true){
  for (int j = 0; j < 10; j++) {
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < circleLED.numPixels(); i = i + 3) {
        circleLED.setPixelColor(i + q, c);
      }
      circleLED.show();

      delay(wait);

      for (int i = 0; i < circleLED.numPixels(); i = i + 3) {
        circleLED.setPixelColor(i + q, 0);
      }
    }
  }
  
      if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
  }
}

// 燈環劇院追逐-彩虹色 特效
void theaterChaseRainbow(uint8_t wait) {
  while(true){
  for (int j = 0; j < 256; j++) {
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < circleLED.numPixels(); i = i + 3) {
        circleLED.setPixelColor(i + q, Wheel( (i + j) % 255));
      }
      circleLED.show();

      delay(wait);

      for (int i = 0; i < circleLED.numPixels(); i = i + 3) {
        circleLED.setPixelColor(i + q, 0);
      }
      if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
    }
  }
  if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
  }
}

// 燈環彩虹特效
void rainbow(uint8_t wait) {
  uint16_t i, j;
  while(true){
  for (j = 0; j < 256; j++) {
    for (i = 0; i < circleLED.numPixels(); i++) {
      circleLED.setPixelColor(i, Wheel((i + j) & 255));
    }
    circleLED.show();
    delay(wait);
    if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
  }
    if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
  }
}

// 燈環彩虹圓圈特效
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  while(true){
  for (j = 0; j < 256 * 5; j++) {
    for (i = 0; i < circleLED.numPixels(); i++) {
      circleLED.setPixelColor(i, 
                  Wheel(((i * 256 / circleLED.numPixels()) + j) & 255));
    }
    circleLED.show();
    delay(wait);
    if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
  }
    if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
  }
}

// 燈環閃光特效
void strobe(byte red, byte green, byte blue, 
              int StrobeCount, int FlashDelay, int EndPause) {
  while(true){
  for (int j = 0; j < StrobeCount; j++) {
    showAll(red, green, blue);
    delay(FlashDelay);
    showAll(0, 0, 0);
    delay(FlashDelay);
  }
  delay(EndPause);
  if (isButtonPressed()) return;}  // 若偵測到按鈕按下則停止目前特效
}

// 燈環流光特效
void runningLights(byte red, byte green, byte blue, int WaveDelay) {
  int pos;
  while (true) {
    pos = 0;
    for (int i = 0; i < circleLED.numPixels() * 2; i++)
    {
      pos++; // = 0; //pos + Rate;
      for (int i = 0; i < circleLED.numPixels(); i++) {
        circleLED.setPixelColor(i, ((sin(i + pos) * 127 + 128) / 255)*red,
                             ((sin(i + pos) * 127 + 128) / 255)*green,
                              ((sin(i + pos) * 127 + 128) / 255)*blue);
      }
      circleLED.show();
      delay(WaveDelay);
      if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
    }
//  }
}
}
// 燈環賽隆人特效
void cylonBounce(byte red, byte green, byte blue, 
                  int EyeSize, int SpeedDelay, int ReturnDelay) {
  while (true) {
    for (int i = 0; i < circleLED.numPixels() - EyeSize - 2; i++) {
      showAll(0, 0, 0);
      circleLED.setPixelColor(i, red / 10, green / 10, blue / 10);
      for (int j = 1; j <= EyeSize; j++) {
        circleLED.setPixelColor(i + j, red, green, blue);
      }
      circleLED.setPixelColor(i + EyeSize + 1, 
                            red / 10, green / 10, blue / 10);
      circleLED.show();
      delay(SpeedDelay);
    }
    delay(ReturnDelay);

    for (int i = circleLED.numPixels() - EyeSize - 2; i > 0; i--) {
      showAll(0, 0, 0);
      for (int j = 1; j <= EyeSize; j++) {
        circleLED.setPixelColor(i + j, red, green, blue);
      }
      circleLED.setPixelColor(i + EyeSize + 1, 
                            red / 10, green / 10, blue / 10);
      circleLED.show();
      delay(SpeedDelay);
    }
    delay(ReturnDelay);
    if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
  }
}

// 燈環發泡特效
void sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  while (true) {
    int px = random(circleLED.numPixels());
    circleLED.setPixelColor(px, red, green, blue); circleLED.show();
    delay(SpeedDelay);
    circleLED.setPixelColor(px, 0, 0, 0); circleLED.show();

    if (isButtonPressed()) return;  // 若偵測到按鈕按下則停止目前特效
  }
}

// 燈環有關彩虹的特效中, 產生漸變顏色值的函式
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return circleLED.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return circleLED.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return circleLED.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//RFID讀卡器(學生證、磁扣)
bool RFID(){
   // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("Card No. :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print(" Message : ");
  content.toUpperCase();
  mfrc522.PICC_HaltA(); // 讓卡片進入停止模式

  //需要改成可通行的卡號
  if (content.substring(1) == 
  "A6 27 D8 48"   //A6 27 D8 48 磁扣
  || content.substring(1) =="89 A1 34 00"   //89 A1 34 00 俊豪學生證
  || content.substring(1) =="85 48 26 9C"   //85 48 26 9C 梓謙學生證
  || content.substring(1) =="E9 04 33 00"   //E9 04 33 00 丹廷學生證
  || content.substring(1) == "D9 92 31 00" // 育辰學生證
  || content.substring(1) == "C9 95 30 00" // 慧格學生證
  || content.substring(1) == "A9 C9 32 00" // 皖淩學生證
  || content.substring(1) == "79 36 33 00" // 婉華學生證
  || content.substring(1) == "89 13 35 00" // 郁昕學生證
  )  
  {
    Serial.println(" Pass... ");
    stepper.step(STEPS/4.5);
    delay(2000);
    stepper.step(-STEPS/4.5);
    return true;
  }

  else   {
    Serial.println("Error...");
    return false;
  }
}

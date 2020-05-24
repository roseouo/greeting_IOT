// the setup function runs once when you press reset or power the board

int LedPin = 4;
void setup() {
  
  // initialize digital pin LED_BUILTIN as an output.

  Serial.begin(9600); // Begen listening on port 9600 for serial
  
  LedPin = LED_BUILTIN;
  //LedPin = 4;
  
  pinMode(LedPin, OUTPUT);

  digitalWrite(LedPin, LOW);
}

// the loop function runs over and over again forever
void loop() {

   if(Serial.available() > 0) // Read from serial port
    {
      char ReaderFromNode; // Store current character
      ReaderFromNode = (char) Serial.read();
      convertToState(ReaderFromNode); // Convert character to state  
    }
  delay(1000); 
}

void convertToState(char chr) {
  if(chr=='o'){
    digitalWrite(LedPin, HIGH);
    delay(100); 
  }
  if(chr=='f'){
    digitalWrite(LedPin, LOW);
    delay(100); 
  }
}

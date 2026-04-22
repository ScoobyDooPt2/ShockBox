//AI was used to make the code more readable and to add comments

// Pins, use whatever pins work on your arduino
const int trigPin = 13;
const int echoPin = 12;
const int btn15 = 2;
const int btn30 = 8;
const int btn60 = 6;
const int transistorPin = 7;

// Timer Variables
unsigned long armingEndTime = 0;
bool isArmed = false;

// Time Constants 
const unsigned long MIN_15 = 15ULL * 60 * 1000;
const unsigned long MIN_30 = 30ULL * 60 * 1000;
const unsigned long MIN_60 = 60ULL * 60 * 1000;

void setup() {
  Serial.begin(9600);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  //Buttons
  pinMode(btn15, INPUT_PULLUP);
  pinMode(btn30, INPUT_PULLUP);
  pinMode(btn60, INPUT_PULLUP);
  
  pinMode(transistorPin, OUTPUT);
  digitalWrite(transistorPin, LOW); 
  
  Serial.println("System Ready. Pins 2, 8, 6 are Active-Low.");
  
  // STARTUP GUARD: Wait for all buttons to be HIGH before starting
  while(digitalRead(btn15) == LOW || digitalRead(btn30) == LOW || digitalRead(btn60) == LOW) {
    Serial.println("Waiting for buttons to be released/stable...");
    delay(500);
  }
  Serial.println("System Initialized & Clear.");
}

void loop() {
  unsigned long currentTime = millis();

  // 1. Serial output for troubleshooting
  if (digitalRead(btn15) == LOW) {
    delay(50); // Debounce
    armingEndTime = currentTime + MIN_15;
    isArmed = true;
    Serial.println("--- SYSTEM ARMED: 15 MINUTES ---");
    while(digitalRead(btn15) == LOW); 
  } 
  else if (digitalRead(btn30) == LOW) {
    delay(50);
    armingEndTime = currentTime + MIN_30;
    isArmed = true;
    Serial.println("--- SYSTEM ARMED: 30 MINUTES ---");
    while(digitalRead(btn30) == LOW);
  } 
  else if (digitalRead(btn60) == LOW) {
    delay(50);
    armingEndTime = currentTime + MIN_60;
    isArmed = true;
    Serial.println("--- SYSTEM ARMED: 60 MINUTES ---");
    while(digitalRead(btn60) == LOW);
  }
  if (isArmed && currentTime >= armingEndTime) {
    isArmed = false;
    digitalWrite(transistorPin, LOW);
    Serial.println("--- SYSTEM DISARMED: TIMER EXPIRED ---");
  }

  //Sensor Logic
  if (isArmed) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH);
    int distance = duration * 0.034 / 2;

    if (distance > 0 && distance <= 10) {
      digitalWrite(transistorPin, HIGH);
    } else {
      digitalWrite(transistorPin, LOW);
    }

    // Countdown Display
    static unsigned long lastUpdate = 0;
    if (currentTime - lastUpdate >= 1000) {
      unsigned long remaining = (armingEndTime - currentTime) / 1000;
      Serial.print("Countdown: ");
      Serial.print(remaining / 60);
      Serial.print("m ");
      Serial.print(remaining % 60);
      Serial.println("s");
      lastUpdate = currentTime;
    }
  }

  delay(50); 
}
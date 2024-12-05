int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long previousEncoderValue = 0;
unsigned long previousTime = 0;

const float pulleyDiameter = 10.0; // in mm
const int pulsesPerRevolution = 800;

// Calculate distance per pulse
const float distancePerPulse = (pulleyDiameter * 3.14159) / pulsesPerRevolution;

void setup() {
  Serial.begin(115200);

  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);

  digitalWrite(encoderPin1, HIGH); // Turn pull-up resistor on
  digitalWrite(encoderPin2, HIGH); // Turn pull-up resistor on

  // Call updateEncoder() when any high/low change seen
  attachInterrupt(digitalPinToInterrupt(encoderPin1), updateEncoder, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(encoderPin2), updateEncoder, CHANGE);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Calculate speed every 100 ms
  if (currentTime - previousTime >= 100) {
    long currentEncoderValue = encoderValue;
    long deltaPulses = currentEncoderValue - previousEncoderValue;
    float distanceMoved = deltaPulses * distancePerPulse; // in mm
    float speed = distanceMoved / ((currentTime - previousTime) / 1000.0); // mm/s
    
    Serial.print("Belt Speed: ");
    Serial.print(speed);
    Serial.println(" mm/s");
    
    previousEncoderValue = currentEncoderValue;
    previousTime = currentTime;
  }
}

void updateEncoder() {
  int MSB = digitalRead(encoderPin1); // MSB = most significant bit
  int LSB = digitalRead(encoderPin2); // LSB = least significant bit

  int encoded = (MSB << 1) | LSB; // Convert the 2 pin value to single number
  int sum = (lastEncoded << 2) | encoded; // Add it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue--;

  lastEncoded = encoded; // Store this value for next time
}

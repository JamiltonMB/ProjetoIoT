int pinoVibracao = 34;

void setup() {
  Serial.begin(115200);
  pinMode(pinoVibracao, INPUT);

}

void loop() {
  long measurement = vibration();
  Serial.println(measurement);

}

long vibration(){
  long measurement = pulseIn (pinoVibracao, HIGH);
  return measurement;
  }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);
  Serial3.begin(9600,SERIAL_8N1);

  pinMode(9,OUTPUT);digitalWrite(/*M0*/9,LOW);
  pinMode(8,OUTPUT);digitalWrite(/*M1*/8,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( Serial.available() ) { Serial3.write(Serial.read()); }
  if ( Serial3.available() ) { Serial.write(Serial3.read()); }
}

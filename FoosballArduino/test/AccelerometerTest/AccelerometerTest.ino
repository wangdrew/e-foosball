int x = A0;
int y = A1;
int z = A2;

void setup() {
 Serial.begin(9600);
}

void loop() {
  int xVal = analogRead(x);
  int yVal = analogRead(y);
  int zVal = analogRead(z);
  Serial.print("x: ");
  Serial.print(xVal);
  Serial.print(" y: ");
  Serial.print(yVal);
  Serial.print(" z: ");
  Serial.print(zVal);
  Serial.print("\n");
  delay(100);
}

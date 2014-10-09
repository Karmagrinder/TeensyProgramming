#define trigPin 0
#define echoPin 1
#define led 13

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
 
}

void loop()
{
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration/58.2;
  
  if (distance < 4) {  // This is where the LED On/Off happens
    digitalWrite(led,HIGH);
  }
  
  else
  digitalWrite(led,LOW);
  Serial.print("\n");
  Serial.print(distance);
  

}


const int led1 = 22; 
const int led2 = 23; 

void setup(){
  Serial.begin(115200);
  pinMode(led1,OUTPUT); digitalWrite(led1,LOW);
  pinMode(led2,OUTPUT); digitalWrite(led2,LOW);
  Serial.println("Successful Initialization.");
}

void loop(){
  delay(3000);
  digitalWrite(led1,HIGH); Serial.println(led1, "HIGH");
  digitalWrite(led2,LOW); Serial.println(led2, "LOW");
  
  delay(3000);
  digitalWrite(led2,HIGH); Serial.println(led2, "HIGH");
  digitalWrite(led1,LOW); Serial.println(led1, "LOW");
}

int analogPin0 = A0;
                    
int val[6] ;  // variable to store the value read

void setup() {
  Serial.begin(9600);           //  setup serial
}

void loop() {
  int analogPinAddress;
  int analogPinIndex;
  
  for(analogPinIndex=0;analogPinIndex<=5;analogPinIndex++){
    analogPinAddress = A0 + analogPinIndex; // @A0=14
    val[analogPinIndex] = analogRead(analogPinAddress);  // read the input pin
    Serial.println((String)"Pin[A"+analogPinIndex+"]<"+val[analogPinIndex]+">");          // debug value
  }
  
  Serial.println((String)"-----------------");          // debug value
  delay(2000);
}

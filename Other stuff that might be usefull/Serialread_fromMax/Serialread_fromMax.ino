// By Mark Meeuwenoord


int offSet = 0;
int maxPins = 13;
boolean debug = false;

void setup() {
  
  Serial.begin(9600);                         // initialize serial. Really slow!
  
   for (int i = 0; i <= maxPins ; i++) {      // initialize the digital pins as an output.
    pinMode(offSet + i, OUTPUT);
  }
}

void loop() {
  
  if (Serial.available() > 0) {               // if there's any serial available, read it:

    int pinNo = Serial.parseInt();            // look for the next valid integer in the incoming serial stream:
    int pinValue = Serial.parseInt();         // do it again:
    if (Serial.read() == '\n') {              // look for the newline. That's the end of your message in Max:
    digitalWrite (pinNo + offSet, pinValue);
      
    if (debug) {
        Serial.print("Pin: ");
        Serial.print(pinNo);
        Serial.print("    is active: ");
        Serial.println(pinValue);
      }
      delay(5);                               // A short delay... when debug is true.
    }
  }
}

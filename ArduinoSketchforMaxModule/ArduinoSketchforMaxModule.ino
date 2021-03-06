/* 
 VERSIE maart 2021
 ---- CMD SimpleMessageSystem ----
 Control Arduino board functions with the following messages:
 
 r a -> read analog pins
 r d -> read digital pins
 w d [pin] [value] -> write digital pin
 w a [pin] [value] -> write analog pin
 w s [pin] [value] -> write servo pin
 
 
 Base: Thomas Ouellet Fredericks 
 Additions: Alexandre Quessy, Thijs Eerens, Mark Meeuwenoord, Thomas Huster
 
 */

 /*
  ---- Grove Starter Kit ----

  Grove_Button, 2, 3, 4, 5, 6, 7, 8 || Read digital
  Grove_Buzzer, Advanced, Check Arduino Code
  Grove_LED,  3, 5, 6 || Write Analog
  Grove_Light_Sensor, A0, A1, A2, A3 || Read Analog
  Grove_Relay, 2, 3, 4, 5, 6, 7, 8  || Write digital
  Grove_RGB_Backlight_LCD, Advanced, Check Arduino Code
  Grove_Rotary_Angle_Sensor, A0, A1, A2, A3 || Read Analog
  Grove_Servo,  3, 5, 6 || Write Servo
  Grove_Grove_Sound_Sensor, A0, A1, A2, A3 || Read Analog
  Grove_Temperature_Sensor, A0, A1, A2, A3 || Read Analog + for accurate result check Arduino Code. 
 
 */

#include <Servo.h> 

Servo myservo;  // create servo object to control a servo  


// list the digital ports which are used as input // CHANGE THIS IF YOU NEED MORE THEN ONE SERVO OR ANOTHER I/O SETTING
int digitalPorts[] = {2,3,4,5,6,7,8,9,10,11,12,13};
//int digitalPorts[] = {2,4,7,8,9,12,13}; pins 3 5 6 10 11 are reserved for servo output

// list the analog ports which are used as input
int analogPorts[] = {0,1,2,3,4,5};


void setup()
{

  // The following command initiates the serial port at 9600 baud. 
  // If you have any problem connecting try lower baud rate. 

  Serial.begin(57600); //

// Enables pullup resistors on the arduino board. Which means you can connect buttons ed. directely to the pins. (comment out for non pullup use) 

//for (int i = 2;i<14;i++){ // pin 2 to 13
//    digitalWrite(i,HIGH); 
//}

}

void loop()
{

  if (messageBuild() > 0) { // Checks to see if the message is complete and erases any previous messages
    switch (messageGetChar()) { // Gets the first word as a character
    case 'r': // Read pins (analog or digital)
      readpins(); // Call the readpins function
      break; // Break from the switch
    case 'w': // Write pin
      writepin(); // Call the writepin function
    }

  }

}

void readpins(){ // Read pins (analog or digital)

  int size;
  
  switch (messageGetChar()) { // Gets the next word as a character

    case 'd': // READ digital pins
    messageSendChar('d');  // Echo what is being read
    
    
    size = sizeof(digitalPorts) / sizeof(int); // get number of ports in digitalPorts list
    
    for (int thisPin = 0; thisPin < size; thisPin++) {
      messageSendInt(digitalRead(digitalPorts[thisPin])); // read each digital pin in the list
    }
    

      messageEnd(); // Terminate the message being sent
    break; // Break from the switch

    case 'a': // READ analog pins
    messageSendChar('a');  // Echo what is being read
    
    size = sizeof(analogPorts) / sizeof(int); // get number of ports in analogPorts list
    
    for (int thisPin = 0; thisPin < size; thisPin++) {
      messageSendInt(analogRead(analogPorts[thisPin])); // read each digital pin in the list
    }


      messageEnd(); // Terminate the message being sent
  }

}

void writepin() { // Write pin

    int pin;
  int state;

  switch (messageGetChar()) { // Gets the next word as a character

    case 'a' : // WRITE an analog pin

    pin = messageGetInt(); // Gets the next word as an integer
    state = messageGetInt(); // Gets the next word as an integer
    pinMode(pin, OUTPUT); //Sets the state of the pin to an output
    analogWrite(pin, state); //Sets the PWM of the pin 
    break;  // Break from the switch


    // WRITE a digital pin
  case 'd' : 

    pin = messageGetInt();  // Gets the next word as an integer
    state = messageGetInt();  // Gets the next word as an integer
    pinMode(pin,OUTPUT);  //Sets the state of the pin to an output
    digitalWrite(pin,state);  //Sets the state of the pin HIGH (1) or LOW (0)


      // WRITE a servo pin 
  case 's' : 

    pin = messageGetInt();  // Gets the next word as an integer
    state = messageGetInt();  // Gets the next word as an integer
    myservo.attach(pin);  // attaches the servo on pin to the servo object 
    myservo.write(state);  // sets the servo position according to the scaled value 

  }

}

/////////////////////////////////// LIBRARY ///////////////////////////////////////////

#define MESSAGE_BUFFER_SIZE 64
#define MESSAGE_BUFFER_LAST_ELEMENT 63

char messageState = 0;
char messageBufferSerialByte;
int messageBufferIndex = 0;
char messageBuffer[MESSAGE_BUFFER_SIZE];
char *messageBufferLast;
char *messageBufferWord;
char messageSendState = 0;

int messageGetInt() {

    if (messageNext()) return atoi(messageBufferWord);

  return 0;

}

char messageNext() {

  char * temppointer= NULL;
  switch (messageState) {
  case 0:
    return 0;
  case 1:
    temppointer = messageBuffer;
    messageState = 2;
  default:
    messageBufferWord = strtok_r(temppointer," ",&messageBufferLast);
    if (messageBufferWord != NULL) return 1;
  }
  return 0;
}

char messageGetChar() {

 
    if (messageNext()) return messageBufferWord[0];
  return 0;

}

int messageBuild() {

  int size = 0;
  messageState = 0;
  while (Serial.available() > 0) {
    messageBufferSerialByte = Serial.read();

    switch (messageBufferSerialByte) {
    case 10:
      break;
    case 13:
      size = messageBufferIndex;
      messageBuffer[messageBufferIndex]=0;
      messageBufferIndex=0;
      messageState = 1;

      break;
    default:
      messageBuffer[messageBufferIndex]=messageBufferSerialByte;
      messageBufferIndex = messageBufferIndex + 1;
    }
    if (messageBufferIndex >= MESSAGE_BUFFER_LAST_ELEMENT) messageBufferIndex=0;
  }
  
  
  
  return size;
}

// OUTPUT MESSAGES CODE

void messageSendChar(char value) {

  messageSpacer();
  Serial.print(value);
}

void messageSendInt(int value) {

  messageSpacer();
  Serial.print(value); 
}

void messageEnd() {

  messageSendState = 0;
  Serial.println();
}

void messageSpacer() {

  if (messageSendState==1) Serial.print((char) 32);
  messageSendState = 1;

}

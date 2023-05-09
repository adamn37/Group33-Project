// *** The arduino is the prototyping hardware ***

// Includes all files that are needed for program to run
#include <rgb_lcd.h>
#include <Wire.h>
#include <DHT.h>
#include <Servo.h>

// Defines the port number and type for humidity and temperature sensor
#define dht_apin A0
#define dht_type DHT11

// Declares variables for lcd display and humidity and temperature sensor
rgb_lcd lcd;
DHT dht(dht_apin, dht_type);

// Declares buzzer and button port numbers
const int buzzer = 8; // D8
const int buttonpin = 7; // D6

// Declares variables for states and count
int buttonState;
int buzzerState;
const int colorR = 0;
const int colorG = 255;
const int colorB = 0;
int count = 0;

// Declares variables for the motor and joystick
Servo servo1;
int servoVal;
int joyY = 3;
bool raised;

// Function to setup the device
// *** This is our architecture setup ***
void setup() {
  // Declares the pinmode for buzzer and button
  pinMode(buzzer, OUTPUT);
  pinMode(buttonpin, INPUT);

  // Starts the serial, humidity and temperature sensor and lcd display
  Serial.begin(9600);
  delay(1500);
  dht.begin();
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("hello, world!");

  // Sets the buzzer to off and the motor to 0 degrees
  digitalWrite(buzzer, LOW);
  servo1.attach(4);
  servo1.write(0);

  // Starts the bluetooth connection
  while(!Serial);
  setupBlueToothConnection();
}

// Main loop run by the program
void loop() {
  
  // Count increases each iteration to periodically display the current humidity and temperature to the connected bluetooth device
  count = count + 1;

  // Sets up bluetooth connection
  char recvChar;
  String WholeCommand = "";
  String EnteredPassword = "";

  // Debugging code to monitor errors with the bluetooth
  /*DEBUG START*/
  while(Serial1.available()){
    recvChar = Serial1.read();
    Serial.print(recvChar);
  }

  while(Serial.available()){
    WholeCommand = SerialString();
    Serial1.print(WholeCommand);
    delay(400);
  }

  // Reads the input of the joystick and maps it to a useable variable
  servoVal = analogRead(joyY);
  servoVal = map(servoVal, 0, 1023, 0, 180);  

  // If the joystick is being being pushed up, set the raised variable to true otherwise false
  if (servoVal < 87) {
    raised = true;
  } else if (servoVal > 88) {
    raised = false;
  }

  // If the raised variable is true, set the motor to 0 degrees
  // If the raised variable is false, set the motor to 45 degrees
  // *** This is our sensing and acutation ***
  if (raised == true) {
    servo1.write(0);
  } else if (raised == false) {
    servo1.write(45);
  }

  // Get the current state of the button and buzzer
  buzzerState = digitalRead(buzzer);
  buttonState = digitalRead(buttonpin);

  // If the button is on and buzzer is off, turn the buzzer on and send a message to the connected bluetooth devices terminal
  // If the button is on and the buzzer is on, turn the buzzer off
  if (buttonState == LOW) {
    if (buzzerState == LOW) {
      digitalWrite(buzzer, HIGH);
      Serial1.println("PATIENT REQUESTING HELP");
      count = 0;
      delay(500);
    } else if (buzzerState == HIGH) {
      digitalWrite(buzzer, LOW);
      count = 0;
      delay(500);
    }
  }

  // Read the current humidity and temperature from the sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Prints the humidity to the lcd screen
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.print(h);

  // Prints the temperature to the lcd screen
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(t);
  
  // Print the current humidity and temperature to the bluetooth device when an adequate amount of time has passed
  // *** This is our sensing and acutation ***
  if (count == 500 & buzzerState == LOW) {  
    Serial1.print("Current humidity = ");
    Serial1.print(h);
    Serial1.print(" & ");
    Serial1.print("Temperature = ");
    Serial1.print(t); 
    Serial1.print("C  ");
    Serial1.print("\n");
    count = 0; 
  }
}

// Function used to create String from what is entered into serial
String SerialString()
{
  String inputString = "";
  while (Serial.available()){
    char inputChar = (char)Serial.read();
    inputString += inputChar;
  }
  return inputString;
}

// Used to setup the bluetooth connection
// *** This is our Network and Communication ***
void setupBlueToothConnection()
{
  Serial1.begin(9600);
  Serial1.print("AT");
  delay(400);
  Serial1.print("AT+ROLE0"); // set the role as peripheral.
  delay(400);
  Serial1.print("AT+NAMEGroup33");
  delay(400);
  
  // *** This is our privacy and security PIN ***
  Serial1.print("AT+PSWD123456"); // setPI the name as group33
  Serial1.flush();
}

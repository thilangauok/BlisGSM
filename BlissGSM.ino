/*
 * 
 * Author: Thilanga Ariyarathna, thilanga@blis4.co.nz
 * Blis4 Technologies Ltd.
 * 5 oct 2025
 * adapted from Elecrow Keen, Email:keen@elecrow.com
 * Please note:
    When the the board receive "R1O" by SMS, the relay 1 will be open and the "R1C" that it will be close. 
    
    "R1O"/""R1C" means:
      R ->  Relay
      1 ->  Relay 1 
      O ->  Open
      C ->  Close
      
    It also used Relay2,Relay3 and Relay4.
 */

#define DEBUG true    //Open the debug information 

 // the number of the Relay pin
const int Relay1 = 3;
const int Relay2 = 2;
const int Relay3 = 4;
const int Relay4 = 5;
/*
R1O means open  relay 1
R1C means close relay 1
The same as others 
*/
String R1O = "R1O";
String R1C = "R1C";
String R2O = "R2O";
String R2C = "R2C";
String R3O = "R3O";
String R3C = "R3C";
String R4O = "R4O";
String R4C = "R4C";

bool mRecived = 0;
String serialMessage = "";
String mSeq = "";

char target[] ="+CMTI";  
int sms_no;
String get_message = "";   
bool waitForModem(unsigned long timeout = 10000);
void resetModem();
void updateSerial();
String readSerial3();
void sendSMS(String number, String message);
bool debug=DEBUG;

String runCommand(String command, const int timeout, boolean debug);

void setup() {  
      Serial.begin(19200); 
      Serial1.begin(19200); 
      for(int i=2;i<6;i++){ // initialize the Relay pins as an output:
        pinMode(i,OUTPUT);
      }
      for(int i=2;i<6;i++){// initialize the Relay pins status:
        digitalWrite(i,LOW);
      }
      //Power on the SIM800C
      pinMode(9,OUTPUT);
      resetModem();
      sendData("AT",2000,DEBUG);
      sendData("AT+CMGF=1",1000,DEBUG);        //Set the SMS in text mode
      
      Serial.println("Waiting 10 s for modem to turn on...");
      resetModem();
      if (waitForModem(10000)) {
        Serial.println("Modem is ON and responsive.");
      } else {
        Serial.println("Modem not responding.");
      }

      initNZ();
} 


void loop() {
    serialMessage = readSerial3();
    if(serialMessage.indexOf("+CMT:")>=0){
      Serial.println("Message received!");
      Data_handling(serialMessage);
      if(serialMessage.indexOf("R1O")){  
          Serial.println("R1 Relay Open");
      }else{
          Serial.println("Different Message");
      }
    }
    
    if(serialMessage.indexOf("+CMTI")>=0){
      Serial.println("Message received!");  
      mSeq = serialMessage.substring(serialMessage.indexOf(",")+1, serialMessage.length()-1);
      Serial.println("Message # "+mSeq); 
      serialMessage = runCommand("AT+CMGR="+mSeq, 500, DEBUG);
      Serial.println("from method"+serialMessage);

      //Serial.println(serialMessage.indexOf("R1O"));
      //Serial.println("Extracted Message "+serialMessage.substring(serialMessage.indexOf("+")+1,serialMessage.length()-1));
      //delay(2000);
      //Serial.println("Index of R10 "+serialMessage.indexOf("R1O"));
    }
}

void Data_handling(String response) {
  response.toUpperCase();  // Convert input to uppercase once

  // Extract sender's number from +CMT or +CMGR format
  String senderNumber = "";
  int startIndex = response.indexOf("\"") + 1;
  int endIndex = response.indexOf("\"", startIndex);
  if (startIndex > 0 && endIndex > startIndex) {
    senderNumber = response.substring(startIndex, endIndex);
    if (debug) Serial.println("Sender Number: " + senderNumber);
  }

  String ackMessage = "";

  if (response.indexOf(R1C) >= 0) {
    digitalWrite(Relay1, HIGH);
    ackMessage = "Relay 1 Closed";
  }
  else if (response.indexOf(R1O) >= 0) {
    digitalWrite(Relay1, LOW);
    ackMessage = "Relay 1 Opened";
  }
  else if (response.indexOf(R2C) >= 0) {
    digitalWrite(Relay2, HIGH);
    ackMessage = "Relay 2 Closed";
  }
  else if (response.indexOf(R2O) >= 0) {
    digitalWrite(Relay2, LOW);
    ackMessage = "Relay 2 Opened";
  }
  else if (response.indexOf(R3C) >= 0) {
    digitalWrite(Relay3, HIGH);
    ackMessage = "Relay 3 Closed";
  }
  else if (response.indexOf(R3O) >= 0) {
    digitalWrite(Relay3, LOW);
    ackMessage = "Relay 3 Opened";
  }
  else if (response.indexOf(R4C) >= 0) {
    digitalWrite(Relay4, HIGH);
    ackMessage = "Relay 4 Closed";
  }
  else if (response.indexOf(R4O) >= 0) {
    digitalWrite(Relay4, LOW);
    ackMessage = "Relay 4 Opened";
  }
  else {
    ackMessage = "Unrecognized command.";
  }

  if (senderNumber.length() > 0) {
    sendSMS(senderNumber, ackMessage);
  }

  if (debug) Serial.println("Ack: " + ackMessage);
}


void sendData(String command, const int timeout, boolean debug)  //Send command function
{
    String response = "";    
    Serial1.println(command); 
    long int time = millis();
    while( (time+timeout) > millis()){
      while(Serial1.available()){       
        response += (char)Serial1.read(); 
      }  
    }    
    if(debug){
      Serial.print(response);
    }    
}

bool waitForModem(unsigned long timeout) {
    unsigned long startTime = millis();
    String response = "";

    while (millis() - startTime < timeout) {
        // Flush input before sending command
        while (Serial1.available()) Serial1.read();

        Serial1.println("AT");
        delay(500);  // Give modem time to respond

        response = "";
        while (Serial1.available()) {
            char c = Serial1.read();
            response += c;
        }

        // Check for OK in response
        if (response.indexOf("OK") != -1) {
            Serial.println("Modem is ready.");
            return true;
        }

        delay(500);  // Small delay before retry
    }

    Serial.println("Modem did not respond.");
    return false;
}

void initNZ(){

  Serial.println("******Configuring SIM800*******");
  delay(1000);
  Serial1.print("AT\r\n");                    // Test
  delay(1000);
  Serial1.print("AT+CSTT=\"vodafone\"\r\n");  // Set APN
  delay(2000);
  Serial1.print("AT+CMGF=1\r\n");//Set message mode to text
  delay(2000);
  Serial1.print("AT+CNMI=2,2,0,0,0\r\n");    // Show SMS instantly when received
  delay(2000);
  
  /*
  Serial1.print("AT+CIICR\r\n");              // Bring up wireless
  delay(2000);
  Serial1.print("AT+CIFSR\r\n");              // Get IP
  Serial.println("Initialized for NZ.");
  */
  
}

void resetModem(){
  digitalWrite(9,HIGH);
  delay(3000);
  digitalWrite(9,LOW);
  delay(1000);
  Serial.println("modem reset...");
}


String runCommand(String command, const int timeout, boolean debug)  //data handling function
{
    String response = "";    
    Serial1.println(command); 
    long int time = millis();
    /*
    while( (time+timeout) > millis()){
      while(Serial1.available()){       
        response += (char)Serial1.read(); 
      }  
    }
    */
    /*
    if(Serial1.available()){
        response = Serial1.readString();
        Serial.println(response);
    }
    */
  return response;
}

String readSerial3()
{
    String mSerial1 = "";
    if(Serial1.available()){
        mSerial1 = Serial1.readString();
        Serial.println("m: "+mSerial1);
    }
    return mSerial1;
}

void sendSMS(String number, String message) {
  Serial1.println("AT+CMGF=1");  // Set SMS to text mode
  delay(500);
  Serial1.println("AT+CMGS=\"" + number + "\"");
  delay(500);
  Serial1.print(message);
  Serial1.write(26);  // Ctrl+Z to send
  if (debug) {
    Serial.println("Sent SMS to " + number + ": " + message);
  }
}

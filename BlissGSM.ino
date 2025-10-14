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
void readSerial1();
void readSerial2();
String readSerial3();

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
    if(serialMessage.indexOf("+CMTI")>=0){
      Serial.println("Message received!");  
      mSeq = serialMessage.substring(serialMessage.indexOf(",")+1, serialMessage.length()-1);
      Serial.println("Message # "+mSeq); 
      serialMessage = runCommand("AT+CMGR="+mSeq, 500, DEBUG);
      //Serial.println(serialMessage);

      //Serial.println(serialMessage.indexOf("R1O"));
      //Serial.println("Extracted Message "+serialMessage.substring(serialMessage.indexOf("+")+1,serialMessage.length()-1));
      //delay(2000);
      //Serial.println("Index of R10 "+serialMessage.indexOf("R1O"));
    }
}

void Data_handling(String command, const int timeout, boolean debug)  //data handling function
{
    String response = "";    
    //Serial1.println(command); 
    long int time = millis();
    while( (time+timeout) > millis()){
      while(Serial1.available()){       
        response += (char)Serial1.read(); 
      }  
    }  
    
    Serial.println(response);
      
   if (response.indexOf(R1O)>=0) {
    digitalWrite(Relay1,HIGH);
    if(debug){
    Serial.println("Open Relay 1");
    }
    }
   else if (response.indexOf(R1C)>=0) {
    digitalWrite(Relay1,LOW);
    if(debug){
    Serial.println("Close Relay 1");
    }
   }
   else if(response.indexOf(R2O)>=0) {
    digitalWrite(Relay2,HIGH);
    if(debug){
    Serial.println("Open Relay 2");
    }
   }
   else if(response.indexOf(R2C)>=0) {
    digitalWrite(Relay2,LOW);
    if(debug){
    Serial.println("Close Relay 2");
    }
   }
   else if (response.indexOf(R3O)>=0) {
    digitalWrite(Relay3,HIGH);
    if(debug){
    Serial.println("Open Relay 3");
    }
   }
   else if(response.indexOf(R3C)>=0) {
    digitalWrite(Relay3,LOW);
    if(debug){
    Serial.println("Close Relay 3");
    }
   }
   else if(response.indexOf(R4O)>=0) {
    digitalWrite(Relay4,HIGH);
    if(debug){
    Serial.println("Open Relay 4");
    }
   } 
   else if(response.indexOf(R4C)>=0) {
    digitalWrite(Relay4,LOW);
    if(debug){
    Serial.println("Close Relay 4");
    }
   }else
   Serial.println("....Error message....");
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
  Serial1.print("AT+CIICR\r\n");              // Bring up wireless
  delay(2000);
  
  Serial1.print("AT+CIFSR\r\n");              // Get IP

  Serial.println("Initialized for NZ.");
  
}

void resetModem(){
  digitalWrite(9,HIGH);
  delay(3000);
  digitalWrite(9,LOW);
  delay(1000);
  Serial.println("modem reset...");
}

void checkSerial(){
  //put your main code here, to run repeatedly:
  if(Serial1.available()>0){   
     
          //Serial.println("available"); 
          char incomingByte = Serial1.read();
          //Serial.println(incomingByte); 
          
          if(Serial1.find(target)){                  //If receive a new SMS
             Serial.println("SMS received!"); 
             sms_no = Serial1.parseInt();            //Get the SMS ID        
             get_message = "AT+CMGR="+(String)sms_no; //The command of the content of the SMS
             Serial.println("******************** Print the relay status *********************" );
             Data_handling(get_message,500,DEBUG);    //Get the content of the SMS 
             Serial.println("*****************************END*********************************" );
         } 
     }
     while(Serial1.read() >= 0){} 
}



void updateSerial()
{
  delay(500);
  while (Serial1.available())
  {
    Serial.write(Serial1.read());//Forward what Serial received to Software Serial Port
  }
}



void readSerial2()
{
    String mSerial1 = "";
    if(Serial1.available()){
        mSerial1 = Serial1.readString();
        Serial.println(mSerial1);
        if(mRecived){
            Serial.println("Message is:");
            Serial.println(mSerial1);
            mRecived = 0;
        }
        Serial.println(mSerial1);
        if( mSerial1.indexOf("+CMTI: \"ME\",")>=0){
            mRecived =1;
            String mNo = mSerial1.substring(mSerial1.indexOf("=")+1,mSerial1.length()-1);
            String cReadMessage = "AT+CMGR=" + mNo;
            Serial1.write(cReadMessage.c_str());
            Serial.println("New message received..! cmd sent"+cReadMessage);
        }
    }
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

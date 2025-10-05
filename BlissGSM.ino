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

char target[] ="+CMTI";  
int sms_no;
String get_message = "";   

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
      digitalWrite(9,HIGH);
      delay(3000);
      digitalWrite(9,LOW);
      delay(1000);
      sendData("AT",2000,DEBUG);
      sendData("AT+CMGF=1",1000,DEBUG);        //Set the SMS in text mode
} 


void loop() {
  // put your main code here, to run repeatedly:
  if(Serial1.available()>0){    
          if(Serial1.find(target)){                  //If receive a new SMS
             sms_no = Serial1.parseInt();            //Get the SMS ID        
             get_message = "AT+CMGR="+(String)sms_no; //The command of the content of the SMS
             Serial.println("******************** Print the relay status *********************" );
             Data_handling(get_message,500,DEBUG);    //Get the content of the SMS 
             Serial.println("*****************************END*********************************" );
         } 
     }
     while(Serial1.read() >= 0){} 

}

void Data_handling(String command, const int timeout, boolean debug)  //data handling function
{
    String response = "";    
    Serial1.println(command); 
    long int time = millis();
    while( (time+timeout) > millis()){
      while(Serial1.available()){       
        response += (char)Serial1.read(); 
      }  
    }    
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

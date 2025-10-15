/*
 * Author: Thilanga Ariyarathna, thilanga@blis4.co.nz
 * Company: Blis4 Technologies Ltd.
 * Date: 5 Oct 2025
 * Adapted from: Elecrow Keen
 *
 * Description:
 *  This sketch receives SMS commands using the SIM800C module to control 4 relays.
 *  It supports commands like "R1O" (Relay 1 Open), "R1C" (Relay 1 Close), etc.
 *  After executing a command, it sends an SMS acknowledgment back to the sender.
 */

#define DEBUG true  // Enable debug output

// Relay pin assignments
const int Relay1 = 3;
const int Relay2 = 2;
const int Relay3 = 4;
const int Relay4 = 5;

// Command strings
const String R1O = "R1O";
const String R1C = "R1C";
const String R2O = "R2O";
const String R2C = "R2C";
const String R3O = "R3O";
const String R3C = "R3C";
const String R4O = "R4O";
const String R4C = "R4C";

// Global variables
String serialMessage = "";
String mSeq = "";
bool debug = DEBUG;

// Function declarations
void resetModem();
void initNZ();
bool waitForModem(unsigned long timeout = 10000);
void sendData(String command, const int timeout, bool debug);
String readSerial3();
String runCommand(String command, const int timeout, bool debug);
void Data_handling(String response);
void sendSMS(String number, String message);

void setup() {
  Serial.begin(19200);    // Debug serial
  Serial1.begin(19200);   // SIM800C serial

  // Initialize relay pins
  for (int i = 2; i <= 5; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW); // Set initial state to LOW (relay off)
  }

  // Power control pin for SIM800
  pinMode(9, OUTPUT);

  resetModem();  // Hard reset SIM800

  sendData("AT", 2000, DEBUG);
  sendData("AT+CMGF=1", 1000, DEBUG); // Set SMS mode to text

  Serial.println("Waiting 10 s for modem to turn on...");
  resetModem();

  if (waitForModem(10000)) {
    Serial.println("Modem is ON and responsive.");
  } else {
    Serial.println("Modem not responding.");
  }

  initNZ();  // Setup SIM800 for Vodafone NZ
}

void loop() {
  serialMessage = readSerial3();

  // SMS content received directly
  if (serialMessage.indexOf("+CMT:") >= 0) {
    Serial.println("Direct message received!");
    Data_handling(serialMessage);
  }

  // SMS notification (memory reference)
  if (serialMessage.indexOf("+CMTI") >= 0) {
    Serial.println("SMS memory notification received!");

    // Extract SMS index from +CMTI: "SM",1
    mSeq = serialMessage.substring(serialMessage.indexOf(",") + 1);
    Serial.println("Message index: " + mSeq);

    // Read the SMS using AT+CMGR
    serialMessage = runCommand("AT+CMGR=" + mSeq, 500, DEBUG);
    Serial.println("Full SMS: " + serialMessage);

    // Handle the SMS content
    Data_handling(serialMessage);
  }
}

/**
 * Handles SMS commands and sends acknowledgements.
 */
void Data_handling(String response) {
  response.toUpperCase();  // Normalize for case-insensitive matching

  // Extract sender number from response: +CMT: "+1234567890", ...
  String senderNumber = "";
  int startIndex = response.indexOf("\"") + 1;
  int endIndex = response.indexOf("\"", startIndex);

  if (startIndex > 0 && endIndex > startIndex) {
    senderNumber = response.substring(startIndex, endIndex);
    if (debug) Serial.println("Sender Number: " + senderNumber);
  }

  String ackMessage = "";

  // Relay command processing
  if (response.indexOf(R1C) >= 0) {
    digitalWrite(Relay1, HIGH);
    ackMessage = "Relay 1 Closed";
  } else if (response.indexOf(R1O) >= 0) {
    digitalWrite(Relay1, LOW);
    ackMessage = "Relay 1 Opened";
  } else if (response.indexOf(R2C) >= 0) {
    digitalWrite(Relay2, HIGH);
    ackMessage = "Relay 2 Closed";
  } else if (response.indexOf(R2O) >= 0) {
    digitalWrite(Relay2, LOW);
    ackMessage = "Relay 2 Opened";
  } else if (response.indexOf(R3C) >= 0) {
    digitalWrite(Relay3, HIGH);
    ackMessage = "Relay 3 Closed";
  } else if (response.indexOf(R3O) >= 0) {
    digitalWrite(Relay3, LOW);
    ackMessage = "Relay 3 Opened";
  } else if (response.indexOf(R4C) >= 0) {
    digitalWrite(Relay4, HIGH);
    ackMessage = "Relay 4 Closed";
  } else if (response.indexOf(R4O) >= 0) {
    digitalWrite(Relay4, LOW);
    ackMessage = "Relay 4 Opened";
  } else {
    ackMessage = "Unrecognized command.";
  }

  // Send acknowledgment SMS
  if (senderNumber.length() > 0) {
    sendSMS(senderNumber, ackMessage);
  }

  if (debug) Serial.println("Ack: " + ackMessage);
}

/**
 * Sends a command to the SIM800 and reads back the response.
 */
void sendData(String command, const int timeout, bool debug) {
  String response = "";
  Serial1.println(command);
  long int time = millis();

  while ((time + timeout) > millis()) {
    while (Serial1.available()) {
      response += (char)Serial1.read();
    }
  }

  if (debug) {
    Serial.print(response);
  }
}

/**
 * Waits for SIM800 to respond with "OK" within a timeout.
 */
bool waitForModem(unsigned long timeout) {
  unsigned long startTime = millis();
  String response = "";

  while (millis() - startTime < timeout) {
    // Clear input
    while (Serial1.available()) Serial1.read();

    Serial1.println("AT");
    delay(500);

    response = "";
    while (Serial1.available()) {
      char c = Serial1.read();
      response += c;
    }

    if (response.indexOf("OK") != -1) {
      Serial.println("Modem is ready.");
      return true;
    }

    delay(500);
  }

  return false;
}

/**
 * Sends an SMS to a given phone number.
 */
void sendSMS(String number, String message) {
  Serial1.println("AT+CMGF=1");  // SMS text mode
  delay(500);
  Serial1.println("AT+CMGS=\"" + number + "\"");
  delay(500);
  Serial1.print(message);
  Serial1.write(26);  // Ctrl+Z to send

  if (debug) {
    Serial.println("Sent SMS to " + number + ": " + message);
  }
}

/**
 * Initializes SIM800C for Vodafone NZ.
 */
void initNZ() {
  Serial.println("****** Configuring SIM800 *******");
  delay(1000);

  Serial1.print("AT\r\n");
  delay(1000);

  Serial1.print("AT+CSTT=\"vodafone\"\r\n");  // Set APN
  delay(2000);

  Serial1.print("AT+CMGF=1\r\n");  // SMS text mode
  delay(2000);

  Serial1.print("AT+CNMI=2,2,0,0,0\r\n");  // Auto display SMS on receipt
  delay(2000);
}

/**
 * Performs a hardware reset of SIM800C.
 */
void resetModem() {
  digitalWrite(9, HIGH);
  delay(3000);
  digitalWrite(9, LOW);
  delay(1000);
  Serial.println("Modem reset...");
}

/**
 * Sends an AT command and returns the response (not implemented fully).
 */
String runCommand(String command, const int timeout, bool debug) {
  String response = "";
  Serial1.println(command);
  long int time = millis();

  while ((time + timeout) > millis()) {
    while (Serial1.available()) {
      response += (char)Serial1.read();
    }
  }

  if (debug && response.length() > 0) {
    Serial.println("Command Response: " + response);
  }

  return response;
}

/**
 * Reads full string from Serial1 (SIM800) if available.
 */
String readSerial3() {
  String mSerial1 = "";
  if (Serial1.available()) {
    mSerial1 = Serial1.readString();
    Serial.println("Received: " + mSerial1);
  }
  return mSerial1;
}

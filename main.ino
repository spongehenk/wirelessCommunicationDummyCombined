const long BAUDRATE = 250000; // Computer <-> Arduino serial Baudrate.
const int DELAY_TIME = 100; // ms
enum SYS_STATUS { STOPPED, PAUSED, RUNNING, EMERGENCY };
int systemStatus = STOPPED;

#include <XBee.h>

//for esp32
#define RXD2 16
#define TXD2 17

//xbee numbers
#define xbee1 0x41A26FAA
#define xbee2 0x41A26FB7

//sending signals
#define START1  "START"
#define STOP1  "STOP"
#define SETPOINT1  "SETPOINT"
#define POSITION1 "POSITION"+

// create the XBee object
XBee xbee = XBee();

//Global variables 
int statusLed = 13;
int errorLed = 13;
bool wantInfoFromGripper = false; //Not necessarily needed. See example code.

void setup() {
  Serial.begin(BAUDRATE);
////////////////////////////////////////
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  //Serial.begin(115200);
  //serial begin voor arduino 
  //Serial1.begin(9600);
  //serial begin voor esp32
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  xbee.setSerial(Serial1);
  /* Zigbee initialisation code should come here
   *  
   */
}

void loop() {
  checkSerial();
}

/* 
 *  This function checks if data is sent from the computer over 
 *  the serial port. If so, it checks the validity of the command
 *  and acts accordingly.
 */
 
void checkSerial() {

  // Check if new data is available
  if (Serial.available()) 
  {
    String data = Serial.readString(); // Read \r\n ending serial string
    int dataLength = data.length() - 1; // Somehow, arduino counts 1 extra when using the standard Serial Monitor

    // Error marking boolean; used throughout the serial data processing code:
    boolean error = false;
    
    if (dataLength > 1)  // Check if the string has at least two characters:
    {
      
      // Extract the first two bits from the sent string:
      String identifier = data.substring(0,2);
      
      // Act according to command sent:
      // ------------------------------------------------------------

      if (identifier == "Rp") // Receive positions command
      {
        String gripperIDText = data.substring(3, dataLength); // take IdGripper value
        long gripperID = gripperIDText.toInt(); // type conversion
        // (will be 0 if unable to convert)
        
        receivePositionsCommand(&gripperID); // in commands.ino
      }

      else if (identifier == "Rc") // Receive controller info command
      {
        String gripperIDText = data.substring(3, dataLength); // take IdGripper value
        long gripperID = gripperIDText.toInt(); // type conversion
        // (will be 0 if unable to convert)
        
        receiveControllerInfoCommand(&gripperID); // in commands.ino
      }
      
      else if (identifier == "Sp") // Send position/setpoint command
      {
        int semicolonIndex = findNextSemicolon(data, 3); // find next semicolon char index

        if (semicolonIndex < 0) // function returns -1 in case no semicolon is found
        {
          error = true; // if this is the case, throw an error
        }
        else 
        {
          String gripperIDText = data.substring(3, semicolonIndex + 1); // take IdGripper value
          long gripperID = gripperIDText.toInt(); // type conversion
          // (will be 0 if unable to convert)
          
          int dataEnd = findNextSemicolon(data, semicolonIndex+1);
          if (dataEnd < 0) // function returns -1 in case no semicolon is found
          {
            error = true; // if this is the case, throw an error
          }
          else 
          {
            String setpointText = data.substring(semicolonIndex, dataEnd-1);
            long setpoint = setpointText.toInt();
            semicolonIndex = findNextSemicolon(data, dataEnd+1); // find next semicolon char index
            
            if (semicolonIndex < 0) // function returns -1 in case no semicolon is found
            {
              error = true; // if this is the case, throw an error
            }
            else 
            {
              String accText = data.substring(dataEnd, semicolonIndex-1);
              long acc = accText.toInt();
              
              String velText = data.substring(semicolonIndex, dataLength);
              long vel = velText.toInt();
              
              sendPositionCommand(&gripperID, &setpoint, &acc, &vel); // in commands.ino
              
            }
          }
        }
      }

      else if (identifier == "Rs") // Get status function
      {
        receiveStatusCommand(); // in commands.ino
      }
      
      else if (identifier == "Ss") // Send status function
      {
        String statusText = data.substring(3, dataLength); // The rest of the data is the status
        long statusInteger = statusText.toInt(); // type conversion
        // (will be 0 if unable to convert), which corresponds to "stopped"

        sendStatusCommand(&statusInteger);  // in commands.ino 
      }
      
      else if (identifier == "Sc") // Send controller settings function
      {
        int dataEnd = findNextSemicolon(data, 3); // find next semicolon
        
        if (dataEnd < 0) // function returns -1 in case no semicolon is found
        {
          error = true; // if this is the case, throw an error
        }
        else 
        {
          String gripperIDText = data.substring(3, dataEnd); // take IdGripper value
          long gripperID = gripperIDText.toInt(); // type conversion
          // (will be 0 if unable to convert)
  
          int semicolonIndex = findNextSemicolon(data, dataEnd+1); // find next semicolon 
          
          if (semicolonIndex < 0) // function returns -1 in case no semicolon is found
          {
            error = true; // if this is the case, throw an error
          }
          else
          {
            String KpText = data.substring(dataEnd, semicolonIndex-1); // Extract "Kp";
            long Kp = KpText.toInt(); // type conversion
            // (will be 0 if unable to convert)
    
            // dataEnd value is re-used here to save precious RAM
            dataEnd = findNextSemicolon(data, semicolonIndex+1); // find next semicolon 
            if (dataEnd < 0) // function returns -1 in case no semicolon is found
            {
              error = true; // if this is the case, throw an error
            }
            else 
            {
              String KiText = data.substring(semicolonIndex, dataEnd-1); // Extract "Ki";
              long Ki = KiText.toInt(); // type conversion
              // (will be 0 if unable to convert)
              
              // semicolonIndex value is re-used too
              semicolonIndex = findNextSemicolon(data, dataEnd+1); // find next semicolon 
              if (semicolonIndex < 0) // function returns -1 in case no semicolon is found
              {
                error = true; // if this is the case, throw an error
              }
              else
              {
                String KdText = data.substring(dataEnd, semicolonIndex-1); // Extract "Ki";
                long Kd = KdText.toInt(); // type conversion
                // (will be 0 if unable to convert)
                
                String LPFText = data.substring(semicolonIndex, dataLength);
                long LPF = LPFText.toInt(); // type conversion
                // (will be 0 if unable to convert)
                sendControllerSettingsCommand(&gripperID, &Kp, &Ki, &Kd, &LPF); // in commands.ino
               
              }
            }
          }
        }
      }
      
    } 
    else 
    { // String received has less than two characters, print error:
      error = true;
    }

    // This makes it possible to enhance the "data checks" later on.
    // For example, if the [x]th semicolon couldn't be found, but it is 
    // not expected, you can easily set [error] to true
    // This is not implemented yet nevertheless
    if (error == true) 
    {
      Serial.println("INVALID");
    }
  }
  
}

/*
 * This function loops the characters of the given string [data] 
 * from index [start] onwards, and tries to find the next 
 * semicolon. It returns the index of this semicolon
 */
int findNextSemicolon (String data, int start)
{
  // now loop through all characters until the next semicolon
  boolean semicolonReached = false;
  
  while (semicolonReached == false && start < data.length())
  {
    if (data.charAt(start) == ';') {
      semicolonReached = true;
    }
    start++;
  }

  // return -1 when no semicolon is found
  if (semicolonReached == false) 
  {
    return -1;
  }
  else // return found semicolon index if found
  {
    return start;
  }
}


//--------------------------------------------------------------------------------------------------

//flash led function (Also not necessarily needed but don't forget to delete al the places this function is called) 
void flashLed(int pin, int times, int wait) { 
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

//sending function
void Sending(String DATA,unsigned long xbeeNumber) {// NEED Data and XBee receiver number
  uint8_t payload[] = { 0, 0 }; //sending buffer
  if (DATA == "START") //to start the motor
  {
    // break down 10-bit reading into two bytes and place in payload
    payload[0] = 'S';
    payload[1] = 'A';
    wantInfoFromGripper = false; //Need nothing in return
  }
  else if (DATA == "STOP") //to stop the motor
  {
    // break down 10-bit reading into two bytes and place in payload
    payload[0] = 'S';
    payload[1] = 'T';
    wantInfoFromGripper = false;    
  }
  else if (DATA == "SETPOINT") //request to receive the setpoint
  {
    // break down 10-bit reading into two bytes and place in payload
    payload[0] = 'S';
    payload[1] = 'E';
    wantInfoFromGripper = true;
  }
  else if (DATA == "POSITION") //request to receive the position
  {
    // break down 10-bit reading into two bytes and place in payload
    payload[0] = 'P';
    payload[1] = 'O';
    wantInfoFromGripper = true;
  }
  else //Data was not "START", "STOP" , "SETPOINT" or "POSITION"
  {
    //Serial.print("send data niet herkend");
  }

  // SH + SL Address of receiving XBee
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, xbeeNumber); //xbee adress
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload)); //transmitter request
  ZBTxStatusResponse txStatus = ZBTxStatusResponse(); //transmitter status
  //Serial.print("send: ");
  //Serial.println(DATA);
  xbee.send(zbTx); // sending transmitter reuest

  while (xbee.readPacket(500) == false) // Waiting till packet is received
  {   }             
  if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) // get api id
    {
      xbee.getResponse().getZBTxStatusResponse(txStatus); // get transmitter status

      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) //status is succesfull
      {
        // success.  time to celebrate
        //Serial.println("succes");
        flashLed(statusLed, 5, 50);
      } 
      else // status is not succesfull
      {
        // the remote XBee did not receive the packet. is it powered on?
        flashLed(errorLed, 3, 500);
        //Serial.println("xbee did not receive packet");
      }
    }
}

unsigned long returnInfo() { //for setpoint or positie request
  XBeeResponse response = XBeeResponse(); //for the responce
  unsigned long signalInDec = 0; //for converting ASCII signal to DECIMAL
  // create reusable response objects for responses expected to handle 
  ZBRxResponse rx = ZBRxResponse(); 
  ModemStatusResponse msr = ModemStatusResponse();
  
  xbee.readPacket(); //for listening
    if (xbee.getResponse().isAvailable()) { //Is information available?
      // got something
       //Serial.println("Got return data");
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) { 
        // got a zb rx packet
        
        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx); 
            
        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) { 
            // the sender got an ACK
            flashLed(statusLed, 10, 10);
        } else {
            // Receiver received but sender didn't get an ACK
            flashLed(errorLed, 2, 20);
        }
        // received pieces of data in ASCII.
        unsigned long SIGNAL4 = int(rx.getData(4))*100000000; //  xx  00  00  00  00
        unsigned long SIGNAL3 = int(rx.getData(3))*1000000; //        xx  00  00  00
        unsigned long SIGNAL2 = int(rx.getData(2))*10000;//               xx  00  00
        unsigned long SIGNAL1 = int(rx.getData(1))*100;//                     xx  00
        unsigned long SIGNAL0 = int(rx.getData(0));//                             xx
        signalInDec = SIGNAL4 +  SIGNAL3 +  SIGNAL2 +  SIGNAL1 +  SIGNAL0  ; // xx + xx + xx + xx + xx = 32 bit number
      } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) 
      {
        xbee.getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation
        
        if (msr.getStatus() == ASSOCIATED) 
        {// This is good.  flash led
          flashLed(statusLed, 10, 10);
        } else if (msr.getStatus() == DISASSOCIATED) 
        {// Not good. flash led to show discontent
          flashLed(errorLed, 10, 10);
        } else 
        {// another status
          flashLed(statusLed, 5, 10);
        }
      } else 
      {// not something expecting
        flashLed(errorLed, 1, 25);    
      }
    } 
    else if (xbee.getResponse().isError()) 
    { //ERROR 
      //Serial.println("ERROR");
     }
  return signalInDec;
}

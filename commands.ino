/*
 * This file holds all the "dummy" data
 */
int DUMMY_ACCELERATION = 2500; // mm/s^2 ?
int DUMMY_VELOCITY = 50; // mm/s^2 ?
long DUMMY_SETPOINT = 500000; // in um?

int DUMMY_KP = 20; // mm/s^2 ?
int DUMMY_KI = 15; // mm/s^2 ?
int DUMMY_KD = 5; // mm/s^2 ?

int DUMMY_LPF = 150; // rad/s or hz ?
/*
 * This file holds the functions for making contact 
 * with the computer application. 
 * 
 * 
 */


/*
 * This function should send:
 *  1;IdGripper;SetPoint;Time;ActualpositonBuffer
 *  (1 is the check bit)
 *  
 *  With input "IdGripper"
 */

void receivePositionsCommand(long* gripperID)
{
  delay(DELAY_TIME); // this delay is there to avoid "too quick" responses

  int DUMMY_TIME = 0; // time in ms!
  unsigned long DUMMY_POSITION = 0;
    Sending(POSITION1, xbee1);
  if (wantInfoFromGripper)
  {
    DUMMY_POSITION = returnInfo();
  }

  // Send 50 data points, so repeat loop that many times
  for (int i = 0; i < 50; i++)
  {
    Serial.print("1;"); // check byte
    Serial.print(int(*gripperID));
    Serial.print(";");
    Serial.print(DUMMY_SETPOINT);
    Serial.print(";");
    Serial.print(DUMMY_TIME);
    Serial.print(";");
    Serial.print(DUMMY_POSITION);
    Serial.print("\n");

    // Increase every dummy "sample step"
    DUMMY_TIME++;
    DUMMY_POSITION = random(0, 3000);
  }
}




/*
 * This function should send:
 * 2;IdGripper;Setpoint;Kp;Ki;Kd;LPF;Acc;Vel
 * 
 * (2 is the check bit)
 */
void receiveControllerInfoCommand(long* gripperID)
{
  delay(DELAY_TIME); // this delay is there to avoid "too quick" responses
  unsigned long DUMMY_SETPOINT = 0; // in um?
  const int DUMMY_TIME = random(0, 1000); // time in ms!
  unsigned long  DUMMY_POSITION = 0;
  Sending(SETPOINT1, xbee1);
  if (wantInfoFromGripper)
  {
    DUMMY_SETPOINT = returnInfo();
  }
  Sending(POSITION1, xbee1);
  if (wantInfoFromGripper)
  {
    DUMMY_POSITION = returnInfo();
  }
  
  Serial.print("2;"); // check byte
  Serial.print(int(*gripperID));
  Serial.print(";");
  Serial.print(DUMMY_SETPOINT);

  Serial.print(";");
  Serial.print(DUMMY_KP); 
  Serial.print(";");
  Serial.print(DUMMY_KI); 
  Serial.print(";");
  Serial.print(DUMMY_KD);
  Serial.print(";");
  Serial.print(DUMMY_LPF); 

  Serial.print(";");
  Serial.print(DUMMY_ACCELERATION); // in dummy_data
  Serial.print(";");
  Serial.print(DUMMY_VELOCITY); // in dummy_data
  
  Serial.print("\n");
}




/*
 * This function should send:
 * 3;idGripper;Setpoint
 * Which is basically a command echo
 * 
 * (3 is the check bit)
 */
void sendPositionCommand(long* gripperID, long* setpoint, long* acc, long* vel)
{
  DUMMY_ACCELERATION = *acc;
  DUMMY_VELOCITY = *vel;
  delay(DELAY_TIME); // this delay is there to avoid "too quick" responses
  
  Serial.print("3;");
  Serial.print(int(*gripperID));
  Serial.print(";");
  Serial.print(int(*setpoint));
  Serial.print(";");
  Serial.print(int(DUMMY_ACCELERATION));
  Serial.print(";");
  Serial.print(int(DUMMY_VELOCITY));
  Serial.print("\n");
  
}

/*
 * This function should send:
 * 4;Status;Gripper1Setpoint;Gripper1Position;Gripper2Setpoint;Gripper2Position;
 * [……]Gripper20Setpoint;Gripper20Position
 * 
 * Where status is the "echo"
 * (4 is the check bit)
 */
void receiveStatusCommand()
{
  delay(DELAY_TIME); // this delay is there to avoid "too quick" responses
  Serial.print("4;");
  Serial.print(systemStatus); // in main file, integer (enum)
  unsigned long SETPOINT = 0;

  Sending(SETPOINT1, xbee1);
  if (wantInfoFromGripper)
  {
    SETPOINT = returnInfo();
  }
  for(int i = 0; i < 20; i++) 
  {
    Serial.print(";");
    Serial.print(SETPOINT);
    Serial.print(";");
    Serial.print(random(0,3000));
    
    SETPOINT += 20;
  }

  Serial.print("\n"); // close with newline char  
}

/*
 * This function should send:
 *
 * 5;Status
 * Which is basically an echo
 * (5 is the check bit)
 */
void sendStatusCommand(long* statusSignal)
{
  delay(DELAY_TIME); // this delay is there to avoid "too quick" responses
  systemStatus = int(*statusSignal);
  Serial.print("5;");
  Serial.print(systemStatus);
  Serial.print("\n");
}


/*
 * This function should send:
 * 6;IdGripper;Kp;Ki;Kd;LPF
 * 
 * (6 is the check bit)
 */
void sendControllerSettingsCommand(long* gripperID, long* Kp, long* Ki, long* Kd, long* LPF)
{
  delay(DELAY_TIME); // this delay is there to avoid "too quick" responses
  DUMMY_KP = int(*Kp);
  DUMMY_KI = int(*Ki);
  DUMMY_KD = int(*Kd);

  DUMMY_LPF = int(*LPF);
  
  Serial.print("6;");
  Serial.print(int(*gripperID));
  Serial.print(";");
  Serial.print(DUMMY_KP);
  Serial.print(";");
  Serial.print(DUMMY_KI);
  Serial.print(";");
  Serial.print(DUMMY_KD);
  Serial.print(";");
  Serial.print(DUMMY_LPF);
  Serial.print("\n");
}

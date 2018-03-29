/*
 * 
 * 
 * Added functions for uBitx
 * GPS
 * Timing
 * 
 * 
 * 
 */
 

void setTimeGPS(){      // Sets system time based on GPS if available
  while (Serial1.available() > 0)
  gps.encode(Serial1.read());
  setTime(gps.time.hour(),gps.time.minute(),gps.time.second(),gps.date.month(),gps.date.day(),gps.date.year()); // sets current time to hour, minute, second, month, day, year to UTC.
      // values returned by: hour(); minute(); second(); day(); month(); year();
      // calling timeStatus(); will return on of three possibilities timeNotSet, timeSet,timeNeedsSync (sync failed)

}

void setTimeSerial(){ // set time through serial, see https://www.pjrc.com/teensy/td_libs_Time.html
  //#include <TimeLib.h>

  #define TIME_HEADER  "T"   // Header tag for serial time sync message
  #define TIME_REQUEST  7    // ASCII bell character requests a time sync message 
  Serial.begin(9600);
  setSyncProvider( requestSync);  //set function to call when sync required
  printLine(2, "Waiting for time sync");
   if (Serial.available()) {
    processSyncMessage();
  }
  if (timeStatus() == timeSet) {
    printLine(3, "Time sync success");
  } else {
    printLine(3, "Time sync failed");
  }
  delay(1000);
}


void processSyncMessage() { // for time serial sync
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync() // for time serial sync
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}




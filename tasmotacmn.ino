
// get the current time from ntp server
void digitalClockDisplay(){
  Serial.println("digitalClockDisplay");
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 

  currentTimeStr = "" ;
  currentTimeStr = currentTimeStr +  day() + "/" ;
  currentTimeStr = currentTimeStr + month() + " " ;
  //currentTimeStr = currentTimeStr + hour() + ":" ;

  if (hour() < 10){
    currentTimeStr = currentTimeStr + "0" + hour() +":";
  }
  else{
    currentTimeStr = currentTimeStr + hour() +":";
  }

  if (minute() < 10){
    currentTimeStr = currentTimeStr + "0" + minute();
  }
  else{
    currentTimeStr = currentTimeStr + minute();
  }

   Serial.println(currentTimeStr);
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
  
}


String getSwitchStatus(String queue){

  mqtt.publish("brownstone/livingrm/switch/cmnd/STATUS", "0"); 
}

time_t requestSync()
{
  return cmd.GetTime(); // the time will be sent later in response to serial mesg
}

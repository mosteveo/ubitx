/*
 * 
 * Display mod - heavily modified - compiles
 * 
 * 
 * /
 /** Menus
 *  The Radio menus are accessed by tapping on the function button. 
 *  - The main loop() constantly looks for a button press and calls doMenu() when it detects
 *  a function button press. 
 *  - As the encoder is rotated, at every 10th pulse, the next or the previous menu
 *  item is displayed. Each menu item is controlled by it's own function.
 *  - Eache menu function may be called to display itself
 *  - Each of these menu routines is called with a button parameter. 
 *  - The btn flag denotes if the menu itme was clicked on or not.
 *  - If the menu item is clicked on, then it is selected,
 *  - If the menu item is NOT clicked on, then the menu's prompt is to be displayed
 */
#include "ubitx.h"

//Current Frequency and mode to active VFO by KD8CEC
void FrequencyToVFO(byte isSaveFreq)
{
  //Save Frequency & Mode Information
  if (vfoActive == VFO_A)
  {
    vfoA = frequency;
    vfoA_mode = modeToByte();

    if (isSaveFreq)
      storeFrequencyAndMode(1);
  }
  else
  {
    vfoB = frequency;
    vfoB_mode = modeToByte();
    
    if (isSaveFreq)
      storeFrequencyAndMode(2);
  }
}

//Commonly called functions when exiting menus by KD8CEC
void menuClearExit(int delayTime)
{
  if (delayTime > 0)
    delay_background(delayTime, 0);
    
  printLine2ClearAndUpdate();
  menuOn = 0;
}

//Ham band or general band movement by KD8CEC
void menuBand(int btn){
  int knob = 0;
  int stepChangeCount = 0;
  byte btnPressCount = 0;

  if (!btn){
   // printLineF2(F("Band Select?"));
   printLine(3,"Band Select?");
   return;
  }

  //printLineF2(F("Press to confirm"));
  //wait for the button menu select button to be lifted)
  while (btnDown()) {
     delay_background(50, 0);
    if (btnPressCount++ > 20) {
      btnPressCount = 0;
      if (tuneTXType > 0) { //Just toggle 0 <-> 2, if tuneTXType is 100, 100 -> 0 -> 2
        tuneTXType = 0;
        //printLineF2(F("General mode"));
        //printLineF2(F("General"));
        printLine(3,"General");
      }
      else {
        tuneTXType = 2;
        //printLineF2(F("Ham band mode"));
        //printLineF2(F("Ham band"));
        printLine(3,"Ham Band");
      }
      delay_background(1000, 0);
      printLine2ClearAndUpdate();
    }
  }
  //printLineF2(F("Press to confirm"));
  printLine(3,"Press to confirm");
  
  char currentBandIndex = -1;
  //Save Band Information
  if (tuneTXType == 2 || tuneTXType == 3 || tuneTXType == 102 || tuneTXType == 103) { //only ham band move
    //Get Now Band Index
    currentBandIndex = getIndexHambanBbyFreq(frequency);
    
    if (currentBandIndex >= 0) {
      //Save Frequency to Band Frequncy Record
      saveBandFreqByIndex(frequency, modeToByte(), currentBandIndex);
    }
  }
  
  //delay(50);    
  ritDisable();

  while(!btnDown()){
    knob = enc_read();
    if (knob != 0){
      if (tuneTXType == 2 || tuneTXType == 3 || tuneTXType == 102 || tuneTXType == 103) { //only ham band move
        if (knob < 0) {
          if (stepChangeCount-- < -3) {
            setNextHamBandFreq(frequency, -1);  //Prior Band
            stepChangeCount = 0;
          }
        }
        else if (knob > 0) {
          if (stepChangeCount++ > 3) {
            setNextHamBandFreq(frequency, 1); //Next Band
            stepChangeCount = 0;
          }
        }
      }       //end of only ham band move
      else {  //original source
        if (knob < 0 && frequency > 3000000l)
          setFrequency(frequency - 200000l);
        if (knob > 0 && frequency < 30000000l)
          setFrequency(frequency + 200000l);

        if (frequency > 10000000l)
          isUSB = true;
        else
          isUSB = false;
      }

      updateDisplay();
    }
    
    delay_background(20, 0);
  }

  FrequencyToVFO(1);
  menuClearExit(500);
}

//Convert Mode, Number by KD8CEC
//0: default, 1:not use, 2:LSB, 3:USB, 4:CWL, 5:CWU, 6:FM
byte modeToByte(){
  if (cwMode == 0)
  {
    if (isUSB)
      return 3;
    else
      return 2;
  }
  else if (cwMode == 1)
  {
    return 4;
  }
  else
  {
    return 5;
  }
}

//Convert Number to Mode by KD8CEC
//autoSetModebyFreq : 0
//autoSetModebyFreq : 1, if (modValue is not set, set mode by frequency)
void byteToMode(byte modeValue, byte autoSetModebyFreq){
  if (modeValue == 4)
    cwMode = 1;
  else if (modeValue == 5)
    cwMode = 2;
  else
  {
    cwMode = 0;
    if (modeValue == 3)
      isUSB = 1;
    else if (autoSetModebyFreq == 1 && (modeValue == 0))
      isUSB = (frequency > 10000000l) ? true : false;
    else
      isUSB = 0;
  }
}

//IF Shift function, BFO Change like RIT, by KD8CEC
void menuIFSSetup(int btn){
  int knob = 0;
  char needApplyChangeValue = 1;
  
  if (!btn){
    if (isIFShift == 1)
      //printLineF2(F("IF Shift Change?"));
      printLine(3,"IF Shift Change?");
    else
      //printLineF2(F("IF Shift On?"));
      printLine(3,"IF Shift On?");
  }
  else {
      isIFShift = 1;

      delay_background(500, 0);
      updateLine2Buffer(1);
      setFrequency(frequency);

      //Off or Change Value
      while(!btnDown() ){
        if (needApplyChangeValue ==1)
        {
          updateLine2Buffer(1);
          setFrequency(frequency);
        /*
          if (cwMode == 0)
            si5351bx_setfreq(0, usbCarrier + (isIFShift ? ifShiftValue : 0));  //set back the carrier oscillator anyway, cw tx switches it off
          else
            si5351bx_setfreq(0, cwmCarrier + (isIFShift ? ifShiftValue : 0));  //set back the carrier oscillator anyway, cw tx switches it off
        */
          SetCarrierFreq();

          needApplyChangeValue = 0;
        }
        
        knob = enc_read();
        if (knob != 0){
          if (knob < 0)
            ifShiftValue -= 50;
          else if (knob > 0)
            ifShiftValue += 50;

          needApplyChangeValue = 1;
        }
        Check_Cat(0);  //To prevent disconnections
      }

      delay_background(500, 0); //for check Long Press function key
      
      if (btnDown() || ifShiftValue == 0)
      {
        isIFShift = 0;
        ifShiftValue = 0;
        //printLineF2(F("IF Shift is OFF"));
        //printLineF2(F("OFF"));
        //clearLine2();
        setFrequency(frequency);
        SetCarrierFreq();
        //delay_background(1500, 0);
      }

      //Store IF Shiift
      EEPROM.put(IF_SHIFTVALUE, ifShiftValue);
      
      menuClearExit(0);
  }
}

//Functions for CWL and CWU by KD8CEC
void menuSelectMode(int btn){
  int knob = 0;
  int selectModeType = 0;
  int beforeMode = 0;
  int moveStep = 0;
  
  if (!btn){
      //printLineF2(F("Select Mode?"));
      printLine(3, "Select Mode?");
  }
  else {
    delay_background(500, 0);

    //LSB, USB, CWL, CWU
    if (cwMode == 0 && isUSB == 0)
      selectModeType = 0;
    else if (cwMode == 0 && isUSB == 1)
      selectModeType = 1;
    else if (cwMode == 1)
      selectModeType = 2;
    else
      selectModeType = 3;

    beforeMode = selectModeType;

    while(!btnDown()){
      //Display Mode Name
      memset(c, 0, sizeof(c));
      strcpy(c, " LSB USB CWL CWU");
      c[selectModeType * 4] = '>';
      //printLine1(c);
      printLine(2, c);
      
      knob = enc_read();

      if (knob != 0)
      {
        moveStep += (knob > 0 ? 1 : -1);
        if (moveStep < -3) {
          if (selectModeType > 0)
            selectModeType--;
            
          moveStep = 0;
        }
        else if (moveStep > 3) {
          if (selectModeType < 3)
            selectModeType++;
            
          moveStep = 0;
        }
      }

      //Check_Cat(0);  //To prevent disconnections
      delay_background(50, 0);
    }

    if (beforeMode != selectModeType) {
      //printLineF1(F("Changed Mode"));
      if (selectModeType == 0) {
        cwMode = 0; isUSB = 0;
      }
      else if (selectModeType == 1) {
        cwMode = 0; isUSB = 1;
      }
      else if (selectModeType == 2) {
        cwMode = 1;
      }
      else if (selectModeType == 3) {
        cwMode = 2;
      }

      FrequencyToVFO(1);
    }

  /*
  if (cwMode == 0)
    si5351bx_setfreq(0, usbCarrier + (isIFShift ? ifShiftValue : 0));  //set back the carrier oscillator anyway, cw tx switches it off
  else
    si5351bx_setfreq(0, cwmCarrier + (isIFShift ? ifShiftValue : 0));  //set back the carrier oscillator anyway, cw tx switches it off
  */
    SetCarrierFreq();
    
    setFrequency(frequency);
    menuClearExit(500);
  }
}


//Memory to VFO, VFO to Memory by KD8CEC
void menuCHMemory(int btn, byte isMemoryToVfo){
  int knob = 0;
  int selectChannel = 0;
  byte isDisplayInfo = 1;
  int moveStep = 0;
  unsigned long resultFreq, tmpFreq = 0;
  byte loadMode = 0;
  
  if (!btn){
    if (isMemoryToVfo == 1)
      //printLineF2(F("Channel To VFO?"));
      printLine(3, "Channel To VFO?");
   else 
      //printLineF2(F("VFO To Channel?"));
      printLine(3, "VFO To Channel?");
  }
  else {
    delay_background(500, 0);

    while(!btnDown()){
      if (isDisplayInfo == 1) {
        //Display Channel info *********************************
        memset(c, 0, sizeof(c));

        if (selectChannel >= 20 || selectChannel <=-1)
        {
          //strcpy(c, "Exit setup?");
          strcpy(c, "Exit?");
        }
        else
        {
          //Read Frequency from eeprom
          EEPROM.get(CHANNEL_FREQ + 4 * selectChannel, resultFreq);
          
          loadMode = (byte)(resultFreq >> 29);
          resultFreq = resultFreq & 0x1FFFFFFF;

          //display channel description
          if (selectChannel < 10 && EEPROM.read(CHANNEL_DESC + 6 * selectChannel) == 0x03) {  //0x03 is display Chnnel Name
            //display Channel Name
            for (int i = 0; i < 5; i++)
              c[i] = EEPROM.read(CHANNEL_DESC + 6 * selectChannel + i + 1);

           c[5] = ':';
          }
          else {
            //Display frequency
            //1 LINE : Channel Information : CH00
            strcpy(c, "CH");
            if (selectChannel < 9)
              c[2] = '0';
            
            ltoa(selectChannel + 1, b, 10);
            strcat(c, b); //append channel Number;
            strcat(c, " :"); //append channel Number;
          }
  
          //display frequency
          tmpFreq = resultFreq;
          for (int i = 15; i >= 6; i--) {
            if (tmpFreq > 0) {
              if (i == 12 || i == 8) c[i] = '.';
              else {
                c[i] = tmpFreq % 10 + 0x30;
                tmpFreq /= 10;
              }
            }
            else
              c[i] = ' ';
          }
        }

        //printLine2(c);
        printLine(3, c);
        
        isDisplayInfo = 0;
      }

      knob = enc_read();

      if (knob != 0)
      {
        moveStep += (knob > 0 ? 1 : -1);
        if (moveStep < -3) {
          if (selectChannel > -1)
            selectChannel--;

          isDisplayInfo = 1;
          moveStep = 0;
        }
        else if (moveStep > 3) {
          if (selectChannel < 20)
            selectChannel++;
            
          isDisplayInfo = 1;
          moveStep = 0;
        }
      }

      Check_Cat(0);  //To prevent disconnections
    } //end of while (knob)

    if (selectChannel < 20 && selectChannel >= 0)
    {
      if (isMemoryToVfo == 1)
      {
        if (resultFreq > 3000 && resultFreq < 60000000)
        setFrequency(resultFreq);
        byteToMode(loadMode, 1);
      }
      else
      {
        //Save current Frequency to Channel (selectChannel)
        EEPROM.put(CHANNEL_FREQ + 4 * selectChannel, (frequency & 0x1FFFFFFF) | (((unsigned long)modeToByte()) << 29) );
        //printLine2("Saved Frequency");
        printLine(3, "Saved Frequency");
      }
    }
    
    menuClearExit(500);
  }
}


//Select CW Key Type by KD8CEC
void menuSetupKeyType(int btn){
  int knob = 0;
  int selectedKeyType = 0;
  int moveStep = 0;
  if (!btn){
        //printLineF2(F("Change Key Type?"));
        printLine(3, "Change Key Type?");
  }
  else {
    //printLineF2(F("Press to set Key")); //for reduce usable flash memory
    delay_background(500, 0);
    selectedKeyType = cwKeyType;
    
    while(!btnDown()){

      //Display Key Type
      if (selectedKeyType == 0)
        //printLineF1(F("Straight"));
        printLine(2, "Straight");
      else if (selectedKeyType == 1)
        //printLineF1(F("IAMBICA"));
        printLine(2, "IAMBICA");
      else if (selectedKeyType == 2)
        //printLineF1(F("IAMBICB"));
        printLine(2, "IAMBICB");

      knob = enc_read();

      if (knob != 0)
      {
        moveStep += (knob > 0 ? 1 : -1);
        if (moveStep < -3) {
          if (selectedKeyType > 0)
            selectedKeyType--;
          moveStep = 0;
        }
        else if (moveStep > 3) {
          if (selectedKeyType < 2)
            selectedKeyType++;
          moveStep = 0;
        }
      }

      Check_Cat(0);  //To prevent disconnections
    }
    
    //printLineF2(F("CW Key Type set!"));
    printLine(3, "CW Key Type set!");
    cwKeyType = selectedKeyType;
    EEPROM.put(CW_KEY_TYPE, cwKeyType);

    if (cwKeyType == 0)
      Iambic_Key = false;
    else
    {
      Iambic_Key = true;
      if (cwKeyType == 1)
        keyerControl &= ~IAMBICB;
      else
        keyerControl |= IAMBICB;
    }
    
    menuClearExit(1000);
  }
}

//Analog pin monitoring with CW Key and function keys connected.
//by KD8CEC
void menuADCMonitor(int btn){
  int adcPinA0 = 0;  //A0(BLACK, EncoderA)
  int adcPinA1 = 0;  //A1(BROWN, EncoderB)
  int adcPinA2 = 0;  //A2(RED, Function Key)
  int adcPinA3 = 0;  //A3(ORANGE, CW Key)
  int adcPinA6 = 0;  //A6(BLUE, Ptt)
  int adcPinA7 = 0;  //A7(VIOLET, Spare)
  unsigned long pressKeyTime = 0;
  
  if (!btn){
        //printLineF2(F("ADC Line Monitor"));
        printLine(3, "ADC Line Monitor");
        return;
  }
  
  //printLineF2(F("Exit:Long PTT"));
  printLine(3, "Exit:Long PTT");
  delay_background(2000, 0);
  //printLineF1(F("A0   A1   A2"));
  printLine(2, "A0   A1   A2");
  //printLineF2(F("A3   A6   A7"));
  printLine(3, "A3   A6   A7");
  delay_background(3000, 0);
  
  while (true) {
    adcPinA0 = analogRead(A0);  //A0(BLACK, EncoderA)
    adcPinA1 = analogRead(A1);  //A1(BROWN, EncoderB)
    adcPinA2 = analogRead(A2);  //A2(RED, Function Key)
    adcPinA3 = analogRead(A3);  //A3(PTT)
    adcPinA6 = analogRead(A6);  //A6(KEYER)
    adcPinA7 = analogRead(A7);  //A7(VIOLET, Spare)

    if (adcPinA3 < 50) {
      if (pressKeyTime == 0)
        pressKeyTime = millis();
      else if (pressKeyTime < (millis() - 3000))
          break;
    }
    else
      pressKeyTime = 0;
    
    ltoa(adcPinA0, c, 10);
    //strcat(b, c);
    strcpy(b, c);
    strcat(b, ", ");
    
    ltoa(adcPinA1, c, 10);
    strcat(b, c);
    strcat(b, ", ");
    
    ltoa(adcPinA2, c, 10);
    strcat(b, c);

    //printLine1(b);
    printLine(2, b);

    //strcpy(b, " ");
    ltoa(adcPinA3, c, 10);
    strcpy(b, c);
    strcat(b, ", ");

    ltoa(adcPinA6, c, 10);
    strcat(b, c);
    strcat(b, ", ");
    
    ltoa(adcPinA7, c, 10);
    strcat(b, c);
    //printLine2(b);
    printLine(3, b);
    
    delay_background(200, 0);
  } //end of while
      
  menuClearExit(0);
}

//VFO Toggle and save VFO Information, modified by KD8CEC
void menuVfoToggle(int btn)
{
  if (!btn){
    if (vfoActive == VFO_A)
      //printLineF2(F("Select VFO B?"));
      printLine(3, "Select VFO B?");
    else
      //printLineF2(F("Select VFO A?")); 
      printLine(3, "Select VFO A?");   
  }
  else {
      FrequencyToVFO(1);
    
      if (vfoActive == VFO_B){
        vfoActive = VFO_A;
        frequency = vfoA;
        saveCheckFreq = frequency;
        byteToMode(vfoA_mode, 0);
      }
      else {
        vfoActive = VFO_B;
        frequency = vfoB;
        saveCheckFreq = frequency;
        byteToMode(vfoB_mode, 0);
      }

      ritDisable();
      setFrequency(frequency);
      menuClearExit(0);
  }
}

//modified for reduce used flash memory by KD8CEC
void menuRitToggle(int btn){
  if (!btn){
    if (ritOn == 1)
      //printLineF2(F("RIT Off?"));
      printLine(3, "RIT Off?");
    else
      //printLineF2(F("RIT On?"));
      printLine(3, "RIT On?");
  }
  else {
      if (ritOn == 0){
        //printLineF2(F("RIT is ON"));
        //enable RIT so the current frequency is used at transmit
        ritEnable(frequency);
      }
      else{
        //printLineF2(F("RIT is OFF"));
        ritDisable();
      }
      
      menuClearExit(500);
  }
}

//Split communication using VFOA and VFOB by KD8CEC
void menuSplitOnOff(int btn){
  if (!btn){
    if (splitOn == 0)
      //printLineF2(F("Split On?"));
      printLine(3, "Split On?");
    else
      //printLineF2(F("Split Off?"));
      printLine(3, "Split Off?");
  }
  else {
      if (splitOn == 1){
        splitOn = 0;
        //printLineF2(F("Split Off!"));
        //printLineF2(F("[OFF]"));
        printLine(3, "[OFF]");
      }
      else {
        splitOn = 1;
        if (ritOn == 1)
          ritOn = 0;
        //printLineF2(F("Split On!"));
        //printLineF2(F("[ON]"));
        printLine(3, "[ON]");
      }
      
    menuClearExit(500);
  }
}

//Function to disbled transmission
//by KD8CEC
void menuTxOnOff(int btn, byte optionType){
  if (!btn){
    if ((isTxType & optionType) == 0)
      //printLineF2(F("TX OFF?"));
      printLine(3, "TX OFF?");
    else
      //printLineF2(F("TX ON?"));
      printLine(3, "TX ON?");
  }
  else {
      if ((isTxType & optionType) == 0){
        isTxType |= optionType;
        //printLineF2(F("TX OFF!"));
        printLine(3, "TX OFF!");
      }
      else {
        isTxType &= ~(optionType);
       // printLineF2(F("TX ON!"));
        printLine(3, "TX ON!");
      }
      
    menuClearExit(500);
  }
}

/**
 * The calibration routines are not normally shown in the menu as they are rarely used
 * They can be enabled by choosing this menu option
 */
void menuSetup(int btn){
  if (!btn){
    if (!modeCalibrate)
      //printLineF2(F("Setup On?"));
      printLine(3, "Setup On?");
      
    else
      //printLineF2(F("Setup Off?"));
      printLine(3, "Setup Off?");
  }else {
    modeCalibrate = ! modeCalibrate;
    /*
    if (!modeCalibrate){
      modeCalibrate = true;
      //printLineF2(F("Setup:On"));
    }
    else {
      modeCalibrate = false;
      //printLineF2(F("Setup:Off"));
    }
    */
    
   menuClearExit(1000);
  }
}

void menuExit(int btn){
  if (!btn){
      //printLineF2(F("Exit Menu?"));
      printLine(3, "Exit Menu?");
  }
  else
   menuClearExit(0);
}

void menuCWSpeed(int btn){
    int knob = 0;
    int wpm;

    wpm = 1200/cwSpeed;
     
    if (!btn){
     strcpy(b, "CW:");
     itoa(wpm,c, 10);
     strcat(b, c);
     strcat(b, "WPM Change?");
     //printLine2(b);
     printLine(3, b);
     return;
    }

    //printLineF1(F("Press to set WPM"));
    printLine(2, "Press to set WPM");
    strcpy(b, "WPM:");
    itoa(wpm,c, 10);
    strcat(b, c);
    //printLine2(b);
    printLine(3, b);
    delay_background(300, 0);

    while(!btnDown()){

      knob = enc_read();
      if (knob != 0){
        if (wpm > 3 && knob < 0)
          wpm--;
        if (wpm < 50 && knob > 0)
          wpm++;

        strcpy(b, "WPM:");
        itoa(wpm,c, 10);
        strcat(b, c);
        //printLine2(b);
        printLine(3, b);
      }
      //abort if this button is down
      //if (btnDown())
      //re-enable the clock1 and clock 2
      //  break;

      Check_Cat(0);  //To prevent disconnections
    }
    
  //save the setting
  //printLineF2(F("CW Speed set!"));
  cwSpeed = 1200 / wpm;
  EEPROM.put(CW_SPEED, cwSpeed);
  menuClearExit(1000);
}

void displayEmptyData(void){
  //printLineF2(F("Empty data"));
  printLine(3, "Empty data");
  delay_background(2000, 0);
}


//Builtin CW Keyer Logic by KD8CEC
void menuCWAutoKey(int btn){
    if (!btn){
      //printLineF2(F("Memory Keyer"));
      printLine(3, "Memory Keyer");
      return;
    }
    
    //Check CW_AUTO_MAGIC_KEY and CW Text Count
    EEPROM.get(CW_AUTO_COUNT, cwAutoTextCount);
    if (EEPROM.read(CW_AUTO_MAGIC_KEY) != 0x73 || cwAutoTextCount < 1)
    {
      displayEmptyData();
     return;
    }

    //printLineF1(F("Press PTT to Send"));
    //printLineF1(F("PTT to Send"));
    printLine(2, "PTT to Send");
    
    delay_background(500, 0);
    updateDisplay();
    beforeCWTextIndex = 255;  //255 value is for start check
    isCWAutoMode = 1;
    menuOn = 0;
}

//Standalone WSPR Beacone
void menuWSPRSend(int btn){
  if (!btn){
     //printLineF2(F("WSPR Beacon"));
     printLine(3, "WSPR Beacon");
     return;
  }

  WsprMSGCount = EEPROM.read(WSPR_COUNT);

  if (WsprMSGCount < 1)
  {
    displayEmptyData();
    return;
  }

  SendWSPRManage();
  menuClearExit(1000);
}



//Modified by KD8CEC
void menuSetupCwDelay(int btn){
    int knob = 0;
    int tmpCWDelay = cwDelayTime * 10;
     
    if (!btn){
      //printLineF2(F("CW TX->RX Delay"));
      printLine(3, "CW TX->RX Delay");
     return;
    }

    //printLineF1(F("Press, set Delay"));
    printLine(2, "Press, set Delay");
    strcpy(b, "DELAY:");
    itoa(tmpCWDelay,c, 10);
    strcat(b, c);
    //printLine2(b);
    printLine(3, b);
    delay_background(300, 0);

    while(!btnDown()){
      knob = enc_read();
      if (knob != 0){
        if (tmpCWDelay > 3 && knob < 0)
          tmpCWDelay -= 10;
        if (tmpCWDelay < 2500 && knob > 0)
          tmpCWDelay += 10;

        strcpy(b, "DELAY:");
        itoa(tmpCWDelay,c, 10);
        strcat(b, c);
        //printLine2(b);
        printLine(3, b);
      }
      //abort if this button is down
      if (btnDown())
        break;

      Check_Cat(0);  //To prevent disconnections
    }
    
    //save the setting
    //printLineF2(F("CW Delay set!"));
    cwDelayTime = tmpCWDelay / 10;
    EEPROM.put(CW_DELAY, cwDelayTime);
   menuClearExit(1000);
}

//CW Time delay by KD8CEC
void menuSetupTXCWInterval(int btn){
    char needDisplayInformation = 1;
    int knob = 0;
    int tmpTXCWInterval = delayBeforeCWStartTime * 2;
     
    if (!btn){
      //printLineF2(F("CW Start Delay"));
      printLine(3, "CW Start Delay");
     return;
    }

    //printLineF1(F("Press, set Delay"));
    printLine(2, "Press, set Delay");
    delay_background(300, 0);

    while(!btnDown()){

      if (needDisplayInformation == 1) {
        strcpy(b, "Start Delay:");
        itoa(tmpTXCWInterval,c, 10);
        strcat(b, c);
        //printLine2(b);
        printLine(3, b);
        needDisplayInformation = 0;
      }
      
      knob = enc_read();
      if (knob != 0){
        if (tmpTXCWInterval > 0 && knob < 0)
          tmpTXCWInterval -= 2;
        if (tmpTXCWInterval < 500 && knob > 0)
          tmpTXCWInterval += 2;
        /*
        strcpy(b, "Start Delay:");
        itoa(tmpTXCWInterval,c, 10);
        strcat(b, c);
        printLine2(b);
        */
        needDisplayInformation = 1;
      }
      //abort if this button is down
      //if (btnDown())
      //  break;

      Check_Cat(0);  //To prevent disconnections
    }
    
    //save the setting
   //printLineF2(F("CW Start set!"));
   delayBeforeCWStartTime = tmpTXCWInterval / 2;
   EEPROM.put(CW_START, delayBeforeCWStartTime);
   
   menuClearExit(1000);
}


/**
 * Take a deep breath, math(ematics) ahead
 * The 25 mhz oscillator is multiplied by 35 to run the vco at 875 mhz
 * This is divided by a number to generate different frequencies.
 * If we divide it by 875, we will get 1 mhz signal
 * So, if the vco is shifted up by 875 hz, the generated frequency of 1 mhz is shifted by 1 hz (875/875)
 * At 12 Mhz, the carrier will needed to be shifted down by 12 hz for every 875 hz of shift up of the vco
 * 
 */

 //this is used by the si5351 routines in the ubitx_5351 file
extern int32_t calibration;
extern uint32_t si5351bx_vcoa;

void factoryCalibration(int btn){
  int knob = 0;

  //keep clear of any previous button press
  while (btnDown())
    delay(100);
  delay(100);
   
  if (!btn){
    //printLineF2(F("Set Calibration?"));
    printLine(3, "Set Calibration?");
    return;
  }

  calibration = 0;

  cwMode = 0;
  isUSB = true;

  //turn off the second local oscillator and the bfo
  si5351_set_calibration(calibration);
  startTx(TX_CW, 1);
  si5351bx_setfreq(2, 10000000l); 
  
  strcpy(b, "#1 10 MHz cal:");
  ltoa(calibration/8750, c, 10);
  strcat(b, c);
  //printLine2(b);
  printLine(3, b);     

  while (!btnDown())
  {

    if (digitalRead(PTT) == LOW && !keyDown)
      cwKeydown();
    if (digitalRead(PTT)  == HIGH && keyDown)
      cwKeyUp();
      
    knob = enc_read();

    if (knob > 0)
      calibration += 875;
    else if (knob < 0)
      calibration -= 875;
    else 
      continue; //don't update the frequency or the display
      
    si5351_set_calibration(calibration);
    si5351bx_setfreq(2, 10000000l);
    strcpy(b, "#1 10 MHz cal:");
    ltoa(calibration/8750, c, 10);
    strcat(b, c);
    //printLine2(b); 
    printLine(3, b);    
  }

  cwTimeout = 0;
  keyDown = 0;
  stopTx();

  //printLineF2(F("Calibration set!"));
  printLine(3, "Calibration set!");
  EEPROM.put(MASTER_CAL, calibration);
  initOscillators();
  setFrequency(frequency);
  updateDisplay();

  while(btnDown())
    delay(50);

  menuClearExit(100);
}

void menuSetupCalibration(int btn){
  int knob = 0;
  int32_t prev_calibration;
   
  if (!btn){
    //printLineF2(F("Set Calibration?"));
    printLine(3, "Set Calibration?");
    return;
  }

  //printLineF1(F("Set to Zero-beat,"));
  printLine(2, "Set to Zero-beat,");
  //printLineF2(F("press PTT to save"));
  printLine(3, "press PTT to save");
  delay_background(1000, 0);
  
  prev_calibration = calibration;
  calibration = 0;
  si5351_set_calibration(calibration);
  setFrequency(frequency);    
  
  strcpy(b, "cal:");
  ltoa(calibration/8750, c, 10);
  strcat(b, c);
  //printLine2(b);
  printLine(3, b);     

  while (digitalRead(PTT) == HIGH && !btnDown())
  {
    knob = enc_read();

    if (knob > 0){
      calibration += 8750;
      usbCarrier += 120;
    }
    else if (knob < 0){
      calibration -= 8750;
      usbCarrier -= 120;
    }
    else
      continue; //don't update the frequency or the display

    si5351_set_calibration(calibration);
    si5351bx_setfreq(0, usbCarrier);
    setFrequency(frequency);    

    strcpy(b, "cal:");
    ltoa(calibration/8750, c, 10);
    strcat(b, c);
    //printLine2(b); 
    printLine(3, b);    
  }
  
  //save the setting
  if (digitalRead(PTT) == LOW){
    //printLineF1(F("Calibration set!"));
    printLine(2, "Calibration set!");
    //printLineF2(F("Set Carrier now"));
    printLine(3, "Set Carrier now");
    EEPROM.put(MASTER_CAL, calibration);
    delay_background(2000, 0);
  }
  else
    calibration = prev_calibration;

  initOscillators();
  //si5351_set_calibration(calibration);
  setFrequency(frequency);    
  //printLine2ClearAndUpdate();
  //menuOn = 0;
  menuClearExit(0);
}

void printCarrierFreq(unsigned long freq){

  memset(c, 0, sizeof(c));
  memset(b, 0, sizeof(b));

  ultoa(freq, b, DEC);
  
  strncat(c, b, 2);
  strcat(c, ".");
  strncat(c, &b[2], 3);
  strcat(c, ".");
  strncat(c, &b[5], 3);
  //printLine2(c); 
  printLine(3, c);   
}

//modified by KD8CEC (just 1 line remarked //usbCarrier = ...
void menuSetupCarrier(int btn){
  int knob = 0;
  unsigned long prevCarrier;
   
  if (!btn){
      //printLineF2(F("Set the BFO"));
      printLine(3, "Set the BFO");
    return;
  }

  prevCarrier = usbCarrier;
  //printLineF1(F("Tune to best Signal"));  
  printLine(2, "Tune to best Signal");
  //printLineF1(F("PTT to confirm. "));
  printLine(2, "PTT to confirm. ");
  delay_background(1000, 0);

  //usbCarrier = 11995000l; //Remarked by KD8CEC, Suggest from many user, if entry routine factoryrest
  
  si5351bx_setfreq(0, usbCarrier);
  printCarrierFreq(usbCarrier);

  //disable all clock 1 and clock 2 
  while (digitalRead(PTT) == HIGH && !btnDown())
  {
    knob = enc_read();

    if (knob > 0)
      usbCarrier -= 5;
    else if (knob < 0)
      usbCarrier += 5;
    else
      continue; //don't update the frequency or the display
      
    si5351bx_setfreq(0, usbCarrier);
    printCarrierFreq(usbCarrier);

    Check_Cat(0);  //To prevent disconnections
    delay(100);
  }

  //save the setting
  if (digitalRead(PTT) == LOW){
    //printLineF2(F("Carrier set!"));
    printLine(3, "Carrier set!");
    EEPROM.put(USB_CAL, usbCarrier);
    delay_background(1000, 0);
  }
  else 
    usbCarrier = prevCarrier;

  //si5351bx_setfreq(0, usbCarrier);          
  if (cwMode == 0)
    si5351bx_setfreq(0, usbCarrier);  //set back the carrier oscillator anyway, cw tx switches it off
  else
    si5351bx_setfreq(0, cwmCarrier);  //set back the carrier oscillator anyway, cw tx switches it off
  
  setFrequency(frequency);    
  //printLine2ClearAndUpdate();
  //menuOn = 0; 
  menuClearExit(0);
}

//Append by KD8CEC
void menuSetupCWCarrier(int btn){
  int knob = 0;
  unsigned long prevCarrier;
   
  if (!btn){
      //printLineF2(F("Set CW RX BFO"));
      printLine(3, "Set CW RX BFO");
    return;
  }

  prevCarrier = cwmCarrier;
  //printLineF1(F("PTT to confirm. "));
  printLine(2, "PTT to confirm. ");
  delay_background(1000, 0);

  si5351bx_setfreq(0, cwmCarrier);
  printCarrierFreq(cwmCarrier);

  //disable all clock 1 and clock 2 
  while (digitalRead(PTT) == HIGH && !btnDown())
  {
    knob = enc_read();

    if (knob > 0)
      cwmCarrier -= 5;
    else if (knob < 0)
      cwmCarrier += 5;
    else
      continue; //don't update the frequency or the display
      
    si5351bx_setfreq(0, cwmCarrier);
    printCarrierFreq(cwmCarrier);

    delay_background(100, 0);
  }

  //save the setting
  if (digitalRead(PTT) == LOW){
    //printLineF2(F("Carrier set!"));
    printLine(3, "Carrier set!");
    EEPROM.put(CW_CAL, cwmCarrier);
    delay_background(1000, 0);
  }
  else 
    cwmCarrier = prevCarrier;

  if (cwMode == 0)
    si5351bx_setfreq(0, usbCarrier);  //set back the carrier oscillator anyway, cw tx switches it off
  else
    si5351bx_setfreq(0, cwmCarrier);  //set back the carrier oscillator anyway, cw tx switches it off
  
  setFrequency(frequency);
  menuClearExit(0);
}

//Modified by KD8CEC
void menuSetupCwTone(int btn){
    int knob = 0;
    int prev_sideTone;
     
    if (!btn){
      //printLineF2(F("Change CW Tone"));
      printLine(3, "Change CW Tone");
      return;
    }

    prev_sideTone = sideTone;
    //printLineF1(F("Tune CW tone"));
    printLine(2, "Tune CW tone");
    //printLineF2(F("PTT to confirm."));
    printLine(3, "PTT to confirm.");
    delay_background(1000, 0);
    tone(CW_TONE, sideTone);

    //disable all clock 1 and clock 2 
    while (digitalRead(PTT) == HIGH && !btnDown())
    {
      knob = enc_read();

      if (knob > 0 && sideTone < 2000)
        sideTone += 10;
      else if (knob < 0 && sideTone > 100 )
        sideTone -= 10;
      else
        continue; //don't update the frequency or the display
        
      tone(CW_TONE, sideTone);
      itoa(sideTone, b, 10);
      //printLine2(b);
      printLine(3, b);

      delay_background(100, 0);
    }
    noTone(CW_TONE);
    //save the setting
    if (digitalRead(PTT) == LOW){
      //printLineF2(F("Sidetone set!"));
      printLine(3, "Sidetone set!");
      EEPROM.put(CW_SIDETONE, sideTone);
      delay_background(2000, 0);
    }
    else
      sideTone = prev_sideTone;
    
  menuClearExit(0);
 }

//Lock Dial move by KD8CEC
void setDialLock(byte tmpLock, byte fromMode) {
  if (tmpLock == 1)
    isDialLock |= (vfoActive == VFO_A ? 0x01 : 0x02);
  else
    isDialLock &= ~(vfoActive == VFO_A ? 0x01 : 0x02);
    
  if (fromMode == 2 || fromMode == 3) return;

  //delay_background(1000, 0);
  printLine2ClearAndUpdate();
}

byte btnDownTimeCount;

#define PRESS_ADJUST_TUNE 20 //1000msec 20 * 50 = 1000milisec
#define PRESS_LOCK_CONTROL 40 //2000msec 40 * 50 = 2000milisec

//Modified by KD8CEC
void doMenu(){
  int select=0, i,btnState;
  char isNeedDisplay = 0;
  
  //for DialLock On/Off function
  btnDownTimeCount = 0;
  
  //wait for the button to be raised up

  //Appened Lines by KD8CEC for Adjust Tune step and Set Dial lock
  while(btnDown()){
    delay_background(50, 0);
    
    if (btnDownTimeCount++ == (PRESS_ADJUST_TUNE)) { //Set Tune Step 
      //printLineF2(F("Set Tune Step?"));
      printLine(3, "Set Tune Step?");
    }
    else if (btnDownTimeCount > (PRESS_LOCK_CONTROL)) {  //check long time Down Button -> 2.5 Second => Lock
      if (vfoActive == VFO_A)
        setDialLock((isDialLock & 0x01) == 0x01 ? 0 : 1, 0); //Reverse Dial lock
      else
        setDialLock((isDialLock & 0x02) == 0x02 ? 0 : 1, 0); //Reverse Dial lock
      return;
    }
  }
  delay(50);  //debounce

  //ADJUST TUNE STEP 
  if (btnDownTimeCount > PRESS_ADJUST_TUNE)
  {
    //printLineF1(F("Press to set"));
    printLine(2, "Press to set");
    isNeedDisplay = 1; //check to need display for display current value
    
    while (!btnDown())
    {
      delay_background(50, 0);

      if (isNeedDisplay) {
        strcpy(b, "Tune Step:");
        itoa(arTuneStep[tuneStepIndex -1], c, 10);
        strcat(b, c);
        //printLine2(b);
        printLine(3, b);
        isNeedDisplay = 0;
      }
        
      i = enc_read();

      if (i != 0) {
        select += (i > 0 ? 1 : -1);

        if (select * select >= 25) {  //Threshold 5 * 5 = 25
          if (select < 0) {
            if (tuneStepIndex > 1)
              tuneStepIndex--;
          }
          else {
            if (tuneStepIndex < 5)
              tuneStepIndex++;
          }
          select = 0;
          isNeedDisplay = 1;
        }
      }
    } //end of while

    EEPROM.put(TUNING_STEP, tuneStepIndex);
    delay_background(500, 0);
    printLine2ClearAndUpdate();
    return;
  }   //set tune step

  //Below codes are origial code with modified by KD8CEC
  menuOn = 2;
  
  while (menuOn){
    i = enc_read();
    btnState = btnDown();

    if (i > 0){
      if (modeCalibrate && select + i < 240)
        select += i;
      if (!modeCalibrate && select + i < 130)
        select += i;
    }

    if (i < 0 && select - i >= -10)
      select += i;      //caught ya, i is already -ve here, so you add it

    //if -> switch reduce program memory 200byte
    switch (select / 10)
    {
      case 0 : 
        menuBand(btnState); 
        break;
      case 1 : 
        menuVfoToggle(btnState); 
        break;
      case 2 : 
        menuSelectMode(btnState); 
        break;
      case 3 : 
        menuRitToggle(btnState); 
        break;
      case 4 : 
        menuIFSSetup(btnState); 
        break;
      case 5 : 
        menuCWSpeed(btnState); 
        break;
      case 6 : 
        menuSplitOnOff(btnState);        //SplitOn / off
        break;
      case 7 : 
        menuCHMemory(btnState, 0);       //VFO to Memroy
        break;
      case 8 : 
        menuCHMemory(btnState, 1);       //Memory to VFO
        break;
      case 9 : 
        menuCWAutoKey(btnState);  
        break;
      case 10 : 
        menuWSPRSend(btnState);
        break;
      case 11 : 
        menuSetup(btnState);
        break;
      case 12 : 
        menuExit(btnState);
        break;
      case 13 : 
        menuSetupCalibration(btnState);  //crystal
        break;
      case 14 : 
        menuSetupCarrier(btnState);        //lsb
        break;
      case 15 : 
        menuSetupCWCarrier(btnState);        //lsb
        break;
      case 16 : 
        menuSetupCwTone(btnState);  
        break;
      case 17 : 
        menuSetupCwDelay(btnState);  
        break;
      case 18 : 
        menuSetupTXCWInterval(btnState);  
        break;
      case 19 :
        menuSetupKeyType(btnState);  
        break;
      case 20 :
        menuADCMonitor(btnState);  
        break;
      case 21 :
        menuTxOnOff(btnState, 0x01);       //TX OFF / ON
        break;
      default :
        menuExit(btnState);  break;
    }
    /*
    else if (select < 130 && modeCalibrate)
      menuSetupCalibration(btnState);   //crystal
    else if (select < 140 && modeCalibrate)
      menuSetupCarrier(btnState);       //lsb
    else if (select < 150 && modeCalibrate)
      menuSetupCWCarrier(btnState);       //lsb
    else if (select < 160 && modeCalibrate)
      menuSetupCwTone(btnState);
    else if (select < 170 && modeCalibrate)
      menuSetupCwDelay(btnState);
    else if (select < 180 && modeCalibrate)
      menuSetupTXCWInterval(btnState);
    else if (select < 190 && modeCalibrate)
      menuSetupKeyType(btnState);
    else if (select < 200 && modeCalibrate)
      menuADCMonitor(btnState);
    else if (select < 210 && modeCalibrate)
      menuTxOnOff(btnState, 0x01);      //TX OFF / ON
    else if (select < 220 && modeCalibrate)
      menuExit(btnState);
    */

    Check_Cat(0);  //To prevent disconnections
  }

/*
  //debounce the button
  while(btnDown()){
    delay_background(50, 0);  //To prevent disconnections
  }
*/  
}


// ============================================================================================
/* 

Connect the Bluetooth module GND and 5V and to pin 0 and 1 or 6 and 7 as noted in the comment of the digital pin assignment.
When connected to pin 0 and 1 two digital ports and the SoftwareSerial library are saved. Also the Bluetooth subroutine can be omitted.
This ommision is accomplished bij commenting out the #define BLUETOOTHMOD with two slashes // 
A disadvantage of connecting to pin 0 & 1 is that upload to the Arduino when the Bluetooth module is connected is not possible because the module
inteferes the upload.

 Author .: Ed Nieuwenhuys
 Changes.: 0.0.1 Inital program 
*/
// ============================================================================================
//                                                                                            //


#define BLUETOOTHMOD                               // Use  this define if Bluetooth needs other pins than pin 0 and pin 1
                                                   // In this example the Bluetooth module is connected to pin 6 and 7
char VERSION[] = "BLE_HM-10_V01";

                     #ifdef BLUETOOTHMOD
#include <SoftwareSerial.h>                        // Arduino for Bluetooth communication
                     #endif //BLUETOOTHMOD

//------------------------------------------------------------------------------
// PIN Assigments
//------------------------------------------------------------------------------ 

enum DigitalPinAssignments {      // Digital hardware constants 
 RX           = 0,                // Optionally onnects to Bluetooth TX
 TX           = 1,                // Optionally onnects to Bluetooth RX
 PIN02        = 2,                // Empty
 PIN03        = 3,                // Empty
 PIN04        = 4,                // Empty
 PIN05        = 5,                // Empty
 BT_TX        = 6,                // Connects to Bluetooth RX
 BT_RX        = 7,                // Connects to Bluetooth TX
 PIN08        = 8,                // Empty
 PIN09        = 9,                // Empty
 PIN10        = 10,               // Empty  
 PIN11        = 11,               // Empty
 PIN12        = 12,               // Empty
 secondsPin   = 13,               // LED on Arduino
 };
 
enum AnaloguePinAssignments {     // Analogue hardware constants ----
 EmptyA0      = 0,                // Empty
 EmptyA1      = 1,                // Empty
 EmptyA2      = 2,                // Empty
 EmptyA3      = 3,                // Empty
 SDA_pin      = 4,                // SDA pin
 SCL_pin      = 5,                // SCL pin
 EmptyA6     =  6,                // Empty
 EmptyA7     =  7};               // Empty

char     sptext[100];                                                                         // For common print use
uint32_t msTick;                                                                              // The number of millisecond ticks since we last incremented the second counter


//------------------------------------------------------------------------------
// BLUETOOTH
//------------------------------------------------------------------------------                                     
                           #ifdef BLUETOOTHMOD                                                // Bluetooth ---------------------
SoftwareSerial Bluetooth(BT_RX, BT_TX);                                                       // BT_RX <=> TXD on BT module, BT_TX <=> RXD on BT module
                           #endif  //BLUETOOTHMOD                                       

//------------------------------------------------------------------------------
// Menu
//------------------------------------------------------------------------------  
//0        1         2         3         4         5
//12345678901234567890123456789012345678901234567890  
 char menu[][42] = {                                                                         // menu[][nn]  nn is largest length of sentence in the menu 
 "Menu",
 "A Action A",
 "D Enter Date DDDMMYYYY (D25122021", 
 "I For this info menu",
 "R Action R",
 "T Enter time as THHMMSS (T071500)",
 "Jan 2022" };
//  -------------------------------------   End Definitions  ---------------------------------------
//                                                                                            //
//------------------------------------------------------------------------------
// ARDUINO Loop
//------------------------------------------------------------------------------
void loop()
{
   InputDevicesCheck();                                                                       // Check for input from input devices 
   EverySecondCheck();                                                                        // This subroutine is not necessary but an example
}  
//------------------------------------------------------------------------------
// ARDUINO Setup
//------------------------------------------------------------------------------
//                                                                                            //
void setup()
{                                                            
 pinMode(secondsPin,   OUTPUT );  
 Serial.begin(9600);                                                                          // Setup the serial port to 9600 baud      
 Tekstprintln("\n*********\nSerial started");   

                          #ifdef BLUETOOTHMOD 
 Bluetooth.begin(9600);
 Tekstprintln("Bluetooth enabled");
                          #endif  //BLUETOOTHMOD
 } 

//------------------------------------------------------------------------------
// Version info
//------------------------------------------------------------------------------
void SWversion(void) 
{ 
 unsigned int i;
 PrintLine(40);
 for (i = 0; i < sizeof(menu) / sizeof(menu[0]); Tekstprintln(menu[i++]));
 PrintLine(40);
}

void PrintLine(byte Lengte)
{
 for (int n=0; n<Lengte; n++) {Serial.print(F("-"));} Serial.println(); 
}

//                                                                                            //
//------------------------------------------------------------------------------
// Update routine done every second
//------------------------------------------------------------------------------
void EverySecondCheck(void)
{
 uint32_t ms = millis() - msTick; 
 static bool Dpin;                                                                            // Only write once to improve program speed in the loop()
 if (ms > 1 && Dpin)                                                                          // With ms>5 LED is ON very short time
        {Dpin = LOW; digitalWrite(secondsPin,LOW);}                                           // Turn OFF the second on pin 13 
 if (ms > 999)                                                                                // Every second enter the loop
  {
    msTick = millis();
    digitalWrite(secondsPin,Dpin = HIGH);                                                     // Turn ON the second on pin     

   }
}

//------------------------------------------------------------------------------
// Check for input from devices
// This fubction is called from with the loop()
//------------------------------------------------------------------------------
//                                                                                            //
void InputDevicesCheck(void)
{
 SerialCheck();
                           #ifdef BLUETOOTHMOD   
 BluetoothCheck(); 
                           #endif  //BLUETOOTHMOD

}
//------------------------------------------------------------------------------
// common print routines
//------------------------------------------------------------------------------
//                                                                                            //
void Tekstprint(char const *tekst)
{
 Serial.print(tekst);    
                          #ifdef BLUETOOTHMOD   
 Bluetooth.print(tekst);  
                          #endif  //BLUETOOTHMOD
}

void Tekstprintln(char const *tekst)
{
 strcpy(sptext,tekst);
 strcat(sptext,"\n");          //sprintf(sptext,"%s\n",tekst);
 Tekstprint(sptext);    
}

//------------------------------------------------------------------------------
// Check for serial input
//------------------------------------------------------------------------------
void SerialCheck(void)
{
 String SerialString = "";
 while (Serial.available())
  {
   char c = Serial.read();  delay(3); 
   if (c>31 && c<127) SerialString += c;                                                   // Allow input from Space - Del
   else c = 0;
  }
 if (SerialString.length()>0)     ReworkInputString(SerialString);                         // Rework ReworkInputString();
 SerialString = "";
}
                           #ifdef BLUETOOTHMOD
//------------------------------------------------------------------------------
// Check for Bluetooth input
//------------------------------------------------------------------------------                           
void BluetoothCheck(void)
{ 
 String  BluetoothString = "";
 char c = 0;
 Bluetooth.listen();                                                                       //  When using two software serial ports, you have to switch ports by listen()ing on each one in turn.
 while (Bluetooth.available()) 
  { 
   c = Bluetooth.read();
   Serial.print(c);
   if (c>31 && c<127) BluetoothString += c;
   else c = 0;
   delay(3);
  }
 if (BluetoothString.length()>0) 
   {   
    ReworkInputString(BluetoothString);                                                      // Rework ReworkInputString();
    BluetoothString = "";
   }
}
                           #endif  //BLUETOOTHMOD
//                                                                                            //
//------------------------------------------------------------------------------
// Constrain a string with integers
// The value between the first and last character in a string is returned between the  low and up bounderies
//------------------------------------------------------------------------------
int SConstrainInt(String s,byte first,byte last,int low,int up){return constrain(s.substring(first, last).toInt(), low, up);}
int SConstrainInt(String s,byte first,          int low,int up){return constrain(s.substring(first).toInt(), low, up);}

//------------------------------------------------------------------------------
//  Input from Bluetooth or Serial
//------------------------------------------------------------------------------
//                                                                                            //
void ReworkInputString(String InputString)
{
 InputString.trim();                                                                          // Remove trailing spaces
 if (InputString.length()>10) return;                                                         // If string is too long for some reason
 if (InputString[0] > 64 && InputString[0] <123)                                              // If the first charater is a letter
  {
  sprintf(sptext,"**** Wrong entry ****");                                                    // Default message 
  Serial.println(InputString);
  switch (InputString[0]) 
   {
    case 'A':
    case 'a':
            if (InputString.length() == 1)
              {
               sprintf(sptext,"Action A");
              }
             break;  
    case 'D':
    case 'd':  
            if (InputString.length() == 9 )
             {
              int Day, Month, Year;
              Day   = (byte) SConstrainInt(InputString,1,3,0,31);
              Month = (byte) SConstrainInt(InputString,3,5, 0, 12); 
              Year  =        SConstrainInt(InputString,5,9, 2000, 3000); 
              sprintf(sptext,"%02d-%02d-%04d",Day, Month, Year);
             }
            break;
    case 'I':
    case 'i': 
            if (InputString.length() == 1)
            {
             SWversion();
             sptext[0] = 0;                                                                      // Clear sptext    

            }
            break;

    case 'R':
    case 'r':
            if (InputString.length() == 1)
              {
               // Reset();                                                                       // Reset all settings 
               Tekstprintln("\n**** Reset to default settings ****"); 
              }
            break;
    case 'T':
    case 't':
            if(InputString.length() >= 7)  // T125500
              {  
              int Hour, Minute, Second ;           
              Hour   = (byte) SConstrainInt(InputString,1,3,0,23);
              Minute = (byte) SConstrainInt(InputString,3,5,0,59); 
              Second = (byte) SConstrainInt(InputString,5,7,0,59); 
              sprintf(sptext,"%02d:%02d:%02d",Hour, Minute, Second);

              }
              break;       
     default:
             break;
    }
  }
 Tekstprintln(sptext);                                                
 InputString = "";
}

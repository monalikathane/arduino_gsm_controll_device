#include <LiquidCrystal.h>
#define ON 1
#define OFF 0
const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int incomingByte;      // a variable to read incoming ////Serial data into
unsigned long previousMillis = 0, previousMillis_D1 = 0, previousMillis_D2 = 0;  // will store last time LED was updated
// constants won't change:
const long interval = 3000;           // interval at which to blink (milliseconds)
const long interval_Display = 3000;           // interval at which to blink (milliseconds)
const int Device_1 = 9;
int potPin = A3; //input pin
int soil = 1;
char D1_FLAG = OFF;
int State = 0;
float Solar_volt, Battery_V;
String data;
void setup() {

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  // lcd rows and columns
  lcd.print("Humidity");
  // title of sorts
  Serial.begin(9600);
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Device_1, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  // Print a message to the LCD.
  lcd.print("Solar Pump");
  lcd.setCursor(0, 1);
  // Print a message to the LCD.
  lcd.print("CONTROL SYSTEM");
  delay(200);
  initialize();
  RecieveMessage();
  //Serial.println("initialize");
  sendsms(1);
}

void loop() {
  // map the values


  Display_Device();
  operate_device();
  int sensorValue_Solar = analogRead(A4);
  int sensorValue_bat = analogRead(A5);

  Solar_volt = (sensorValue_Solar * (5.0 / 1023.0)) * 9.6;
  Battery_V = (sensorValue_bat * (5.0 / 1023.0)) * 9.6;
}



void  Display_Device()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis_D1 >= interval_Display) {
    // save the last time you blinked the LED
    previousMillis_D1 = currentMillis;

    if (State == 0)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      // Print a message to the LCD.
      lcd.print("PUMP:");
      if ( D1_FLAG == ON ) lcd.print("ON ");
      else  lcd.print("OFF");
      State = 1;
    } else
    {
      State = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      // Print a message to the LCD.
      lcd.print("Solar V = ");
      lcd.print(Solar_volt);
      lcd.setCursor(0, 1);
      lcd.print("BV=");
      lcd.print(Battery_V);
      int soil = analogRead(potPin) ;
      soil = constrain(soil, 485, 1023);
      soil = map(soil, 485, 1023, 100, 0);
      //display final numbers
      lcd.print("SM:%");
      lcd.print(soil);

    }


  }
}

void operate_device()
{
  if (Serial.available() > 0)
  {
    data = Serial.readString();
    // Serial.println(data);
    lcd.clear();
    lcd.setCursor(0, 0 );

    lcd.print(data.substring(0, 15));
    lcd.setCursor(0, 1 );
    lcd.print(data.substring(15));
    int Secrete_char = data.indexOf('*');
    delay(2000);
    char val_op = data.charAt(Secrete_char + 1);
    if (Secrete_char != -1)
    {
      switch (val_op)
      {
        case '1': {
            int soil = analogRead(potPin) ;
            soil = constrain(soil, 485, 1023);
            soil = map(soil, 485, 1023, 100, 0);
            if (soil < 50)
            {
              digitalWrite(Device_1, HIGH);
              D1_FLAG = ON;
            }
          } break;
        case'2': {
            digitalWrite(Device_1, LOW);
            D1_FLAG = OFF;
          } break;
        default : 0;
      }
      sendsms(1);
    }
  }
}

void RecieveMessage()
{
  int ok_pos;

  Serial.println("AT+CMGF=1"); // AT Command to recieve a live SMS
  data = Serial.readString();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(data);
  ok_pos = data.indexOf("OK");
  if (ok_pos != -1)
  {
    lcd.setCursor(0, 1);
    lcd.print("Success");
  }
  delay(1000);
  ok_pos = -1;
  Serial.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
  data = Serial.readString();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(data);
  ok_pos = data.indexOf("OK");
  if (ok_pos != -1)
  {
    lcd.setCursor(0, 1);
    lcd.print("Success");
  }
  delay(1000);
}


void initialize()
{

  int reg = 0, SIM = 0;
  String subdata;
  delay(4000);
  Serial.println("AT+CFUN?");
  delay(100);
  data = Serial.readString();
  //////Serial.println(data);
  subdata = data.substring(18, 20);
  //////Serial.println("subdata = " + subdata);
  while (SIM == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    Serial.println("AT+CPIN?");
    delay(100);
    data = Serial.readString();
    ////Serial.println(data);
    int simRegstat = data.indexOf("READY");
    lcd.print(data.substring(0, 15));
    lcd.setCursor(0, 1 );
    lcd.print(data.substring(15));
    ////Serial.println(" INSERT SIM");
    delay(1000);
    ////Serial.println("subdata = " + subdata);

    if (simRegstat != -1) {
      lcd.print("Sim Ready");
      ////Serial.println(" sim Ready");
      SIM = 1;
      delay(2000);
    }
    else {

      lcd.print("INSERT SIM");
      lcd.setCursor(0, 1 );
      lcd.print("RESTART System");
      ////Serial.println(" INSERT SIM");
      delay(1000);

    }
  }


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Registering.");
  while (reg != 1)
  {
    Serial.println("AT+CREG?");
    delay(100);
    data = Serial.readString();
    ////Serial.println(data);
    int simNetstat = data.indexOf("1");
    subdata = data.substring(20, 21);
    ////Serial.println("subdata = " + subdata);
    if (simNetstat != -1)
      reg = 1;
    lcd.setCursor(0, 0);
    lcd.print("Registering.");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" SIM registered");
  delay(2000);
}



void sendsms(int state)
{
  int soil = analogRead(potPin) ;

  Serial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  Serial.print("AT+CMGS=");
  Serial.print('"');
  Serial.print("8459088130");//  GSM.println("ATD+917798162899;");
  //GSM.print("8788375337");
  Serial.print('"');
  Serial.println("\r"); // Replace x with mobile number
  delay(100);
  delay(1000);
  if (state == 1)  {
    Serial.print("PUMP: ");
    if ( D1_FLAG == ON ) Serial.print("ON ");
    else  Serial.print("OFF");
    soil = constrain(soil, 485, 1023);
    soil = map(soil, 485, 1023, 100, 0);
    //Send final numbers
    Serial.print(" SM:%");
    Serial.print(soil);
    if (soil > 50)
    {
    Serial.print("Soil contains enough moisture");
    }
  }
if (state == 2)  Serial.println("System Initialize "); // The SMS text you want to send
Serial.print("Solar volt =");
Serial.print(Solar_volt);
Serial.println("V");
Serial.print("Battery  volt =");
Serial.print(Battery_V);
Serial.println("V");
soil = constrain(soil, 485, 1023);
soil = map(soil, 485, 1023, 100, 0);
//Send final numbers
Serial.print(" SM:%");
Serial.print(soil);
delay(100);
Serial.println((char)26);// ASCII code of CTRL+Z
}

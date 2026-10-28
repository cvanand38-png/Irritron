

/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL3s0vR1D6t"
#define BLYNK_TEMPLATE_NAME         "Irritron"
#define BLYNK_AUTH_TOKEN            "3Uz9NNBKwp2PfoWuhBrylJmhgvNhlX2H"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "Vaishnaveeyam_2G_2GEXT";
char ssid[] = "Vaishnaveeyam_2";
char pass[] = "ABCDE12345";

BlynkTimer timer;
//My definitions
#define LED_PIN LED_BUILTIN
#define RELAY_PIN D2
#define ON_TIME 3600 // 3600 seconds(1 hr)
#define AUTO_MODE 1
#define MANUAL_MODE 0
#define NIGHT 0
#define DAY 1
#define NIGHT_THRESHOLD 100
#define DAY_THRESHOLD 300

char status[12];
char modeval[12];
char pumptime[12];
int on_time = 0;
bool b_status = false;
bool b_night = false;
int cur_state = DAY;
int prev_state = DAY;
int mode = AUTO_MODE;
bool bstarting = true;

//my functions

void ValveON()
{
  b_status = true;
  strcpy(status,"Valve ON");
  Blynk.virtualWrite(V1, status);
  //Activate relay
  digitalWrite(RELAY_PIN, HIGH); 
  digitalWrite(LED_PIN, LOW);
}
void ValveOFF()
{
  b_status = false;
  on_time = 0;
  strcpy(status,"Valve OFF");
  Blynk.virtualWrite(V1, status);
  //Deactivate relay
  digitalWrite(RELAY_PIN, LOW); 
  digitalWrite(LED_PIN, HIGH);
  //Blynk.virtualWrite(V0, 0);
}
// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
 // Serial.println("Button Pressed...");
  int value = param.asInt();

  if(mode == MANUAL_MODE)
  {
    if((value > 0) && (b_status == false))
    {
      ValveON();
      //Blynk.virtualWrite(V0, 1);
      Blynk.setProperty(V3, "isDisabled", true); // No mode change when Valve ON in manual control
    }
    else
    {
      ValveOFF();
      Blynk.setProperty(V3, "isDisabled", false); 
    }
  }
}
BLYNK_WRITE(V3)
{
  // Set incoming value from pin V0 to a variable
  mode = param.asInt();
  if(mode == AUTO_MODE)
  {
    strcpy(modeval,"Auto Mode");
    Blynk.setProperty(V0, "isDisabled", true); 
  }
  else if(mode == MANUAL_MODE)
  {
    strcpy(modeval,"Manual Mode");
    Blynk.setProperty(V0, "isDisabled", false); 
  }
  Blynk.virtualWrite(V2, modeval);
}

BLYNK_WRITE(V5)
{
  //Sync with mobile app
  if(mode == AUTO_MODE)
  {
    strcpy(modeval,"Auto Mode");
    Blynk.setProperty(V0, "isDisabled", true); 
    Blynk.virtualWrite(V3, 1);
  }
  else if(mode == MANUAL_MODE)
  {
    strcpy(modeval,"Manual Mode");
    Blynk.setProperty(V0, "isDisabled", false); 
    Blynk.virtualWrite(V3, 0);
  }
  Blynk.virtualWrite(V2, modeval);
  if(b_status == false)
    strcpy(status,"Valve OFF");
  else
    strcpy(status,"Valve ON");
  Blynk.virtualWrite(V1, status);
}
// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  int ldrValue = analogRead(A0);  // Read value from A0
  Blynk.virtualWrite(V4, ldrValue);
  if(ldrValue >= DAY_THRESHOLD) 
    cur_state = DAY;
  else if(ldrValue < NIGHT_THRESHOLD)
    cur_state = NIGHT;
  if(bstarting)
  {
    prev_state = cur_state;
    digitalWrite(RELAY_PIN, LOW); 
    digitalWrite(LED_PIN, HIGH);
    bstarting = false;
  }
  /*
  if(mode == AUTO_MODE)
  {
    strcpy(modeval,"Auto Mode");
    Blynk.virtualWrite(V3, 1);
    Blynk.setProperty(V0, "isDisabled", true); 
    Blynk.setProperty(V3, "isDisabled", false); 
  }
  else if(mode == MANUAL_MODE)
  {
    strcpy(modeval,"Manual Mode");
    Blynk.virtualWrite(V3, 0);
    Blynk.setProperty(V0, "isDisabled", false); 
  }
  Blynk.virtualWrite(V2, modeval);
  */
  /*
  //This code was used when connected to mains
  if( prev_state != cur_state)
  {
    if((prev_state == DAY) && (cur_state == NIGHT))
    { 
      //Turn ON valve during night so that watering will happen when pressure build up
      if( mode == AUTO_MODE)
        ValveON();
    }
    if((prev_state == NIGHT) && (cur_state == DAY))
    { 
      //Turn OFF valve when day breaks
      if( mode == AUTO_MODE)
        ValveOFF();
    }
    prev_state = cur_state;
  }
  */
  // start watering when mode changes from night to day and from day to night
  if( prev_state != cur_state)
  {
    if( mode == AUTO_MODE)
      ValveON();
    prev_state = cur_state;
  }
  if(b_status == true)
  {
    on_time++;
    if(mode == AUTO_MODE)
    {
      if(on_time > ON_TIME)
       ValveOFF();
    }
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every 60 seconds
  timer.setInterval(60000L, myTimerEvent);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

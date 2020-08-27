#define processing_out false
#define AHRS true         // Set to false for basic data read
#define SerialDebug true  // Set to true to get Serial output for debugging
#define LCD

#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "utility/MPU9250.h"

int timeElapsedMS = 0;
int screenTimeOutMS = 0;
bool isScreenOn = false;
int lastBatteryValue = 0;
int sampleInterval = 8;
unsigned long currentMill = 0;
unsigned long prevMill = 0;

MPU9250 IMU;

bool isConnected = false;
char ssid[] = "Prithvi";          // your network SSID (name)
char pass[] = "RhythmSynth";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
//const IPAddress outIp(192,168,87,118);        // remote IP of your computer
const IPAddress outIp1(192,168,43,203);          // Hotspot
const IPAddress outIp2(172,30,244,111);          // Eduroam
int selectedIP = 1;                             // 1 = Hotspot & 2 = Home Network
//const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int outPort = 9998;          // remote port to receive OSC
//const unsigned int outPort = 9997;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)

void setup() {
    M5.begin();
    Wire.begin();
    M5.Lcd.setTextSize(2);
    M5.Lcd.setBrightness(0);
    IMU.initMPU9250();
    M5.Lcd.setCursor(0,0);
    M5.Lcd.print("Not Connected - IDLE");    
}

void updateBatteryInfo()
{
  if(M5.Power.isChargeFull()) 
    { 
      M5.Lcd.setCursor(0,220); 
      M5.Lcd.print("Charge: FUL");
      M5.Power.setCharge(false);
    }

    else if(M5.Power.isCharging())
    { 
      M5.Lcd.setCursor(0,220); 
      M5.Lcd.print("Charge: WIP");
    }
    else
    {
      M5.Lcd.setCursor(0,220); 
      M5.Lcd.print("Charge: NOT");
    }

   M5.Lcd.setCursor(170,220); 
   int currentBattery = getBatteryLevel();
   if (currentBattery != lastBatteryValue)
   {
      M5.Speaker.beep();
   }
   M5.Lcd.print("BAT: " + String(getBatteryLevel()));
   lastBatteryValue = currentBattery;
}

int8_t getBatteryLevel() {
    Wire.beginTransmission(0x75);
    Wire.write(0x78);
    if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x75, 1)) {
        switch (Wire.read() & 0xF0) {
        case 0xE0: return 25;
        case 0xC0: return 50;
        case 0x80: return 75;
        case 0x00: return 100;
        default: return 0;
        }
    }
    return -1;
}

void checkConnectionStatus()
{
  if (isConnected && WiFi.status() != WL_CONNECTED)
  {
    M5.Lcd.print("Connection Lost");
    isConnected = false;
    WiFi.begin(ssid, pass);
  }
  if (!isConnected && WiFi.status() == WL_CONNECTED)
  {
    M5.Lcd.print("Reconnected");
    isConnected = true;
  }
}

void connectToPC()
{
  M5.Lcd.setCursor(0,0);
  M5.Lcd.print("                       ");
  WiFi.begin(ssid, pass);
    M5.Lcd.setCursor(0,0);
    M5.Lcd.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        M5.Lcd.print(".");
        delay(500);
    }
    isConnected = true;
    WiFi.setTxPower(WIFI_POWER_11dBm);
    M5.Lcd.setCursor(0,0);
    M5.Lcd.print("                 ");
    M5.Lcd.setCursor(0,20);
    M5.Lcd.print("Connected - " + String(outPort));
    Udp.begin(localPort);
}

void fetchIMUData()
{
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
    {
    IMU.readAccelData(IMU.accelCount);  // Read the x/y/z adc values
    IMU.getAres();
    IMU.ax = (float)IMU.accelCount[0]*IMU.aRes; // - accelBias[0];
    IMU.ay = (float)IMU.accelCount[1]*IMU.aRes; // - accelBias[1];
    IMU.az = (float)IMU.accelCount[2]*IMU.aRes; // - accelBias[2];

    IMU.readGyroData(IMU.gyroCount);  // Read the x/y/z adc values
    IMU.getGres();
    IMU.gx = (float)IMU.gyroCount[0]*IMU.gRes;
    IMU.gy = (float)IMU.gyroCount[1]*IMU.gRes;
    IMU.gz = (float)IMU.gyroCount[2]*IMU.gRes;
    
    IMU.updateTime();
    IMU.count = millis();
    IMU.sumCount = 0;
    IMU.sum = 0;
    }
}
void buildAndSendOSCMsg()
{
  if (isConnected)
    {
    //OSCMessage msg("/S1"); //OSC message compilation    
    OSCMessage msg("/S2"); //OSC message compilation    
    //OSCMessage msg("/S3"); //OSC message compilation    
    msg.add(IMU.ax);
    msg.add(IMU.ay);
    msg.add(IMU.az);
    
    msg.add(IMU.gx);
    msg.add(IMU.gy);
    msg.add(IMU.gz);
    msg.add((float)lastBatteryValue);
    switch (selectedIP)
    {
      case 1:
      Udp.beginPacket(outIp1, outPort);
      break;
      case 2:
      Udp.beginPacket(outIp2, outPort);
      break;
    }
    msg.send(Udp);
    Udp.endPacket();
    msg.empty(); // free space occupied by message
    }
}

void loop() {
  currentMill = millis();
  if (currentMill - prevMill > sampleInterval)
  {
    
  if(M5.BtnA.wasPressed()) { M5.Power.powerOFF(); }

  if(M5.BtnB.wasPressed())
  {
    if (!isConnected)
      {
        connectToPC();
      }
    else
      {
        if (selectedIP == 1)
        {
          selectedIP = 2;
          M5.Lcd.setCursor(0,40);
          M5.Lcd.print("                       ");
          M5.Lcd.setCursor(0,40);
          M5.Lcd.print("Connected - Home IP");
        }
        else
        {
          selectedIP = 1;
          M5.Lcd.setCursor(0,40);
          M5.Lcd.print("                       ");
          M5.Lcd.setCursor(0,40);
          M5.Lcd.print("Connected - Phone IP");
        }
      }
  }
  
  
  if(M5.BtnC.wasPressed()) 
  {  
    updateBatteryInfo();
    M5.Lcd.setBrightness(10);
    isScreenOn = true;
  }

  timeElapsedMS = (timeElapsedMS + 10) % 1000;
  if (timeElapsedMS == 0) { updateBatteryInfo(); }
  
  if (isScreenOn)
  {
    screenTimeOutMS += 10;
    if (screenTimeOutMS >= 1000)
    {
      M5.Lcd.setBrightness(0);
      isScreenOn = false;
      screenTimeOutMS = 0;
    }  
  }

  if (isConnected)
  {
    fetchIMUData();
    buildAndSendOSCMsg();
  }
    M5.update();
    prevMill = currentMill;
}
}

#include <WiFi.h>//library for wifi
#include <PubSubClient.h>//library for MQtt
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED 2
#define CROSS 13
#define CROSSIN 4
#define CROSSOUT 15
#define Crossing 34 //White Button
#define SchoolZone 35 //Yellow Button
#define HospitalZone 32 //Red Button
#define NoEntry 25 //Black Button
#define OneWay 26 //Blue Button
#define TakeDiversion 27 //Gray Button

void callback(char* subscribetopic, byte* payload, unsigned int payloadLength); 

//-------credentials of IBM Accounts------

#define ORG "m1r2sh"//IBM ORGANITION ID
#define DEVICE_TYPE "Roadsafety"//Device type mentioned in ibm watson IOT Platform
#define DEVICE_ID "safetydevice1"//Device ID mentioned in ibm watson IOT Platform
#define TOKEN "!_1ZsGYHI9TsD5kvOu"     //Token
String data3;
byte* payload;
unsigned int payloadLength;
int buttonState = 0,temp = 22,humid = 18;
#define SCREEN_ADDRESS 0x3C


//-------- Customise the above values --------
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";// Server Name
char publishTopic[] = "iot-2/evt/Data/fmt/json";// topic name and type of event perform and format in which data to be send
char subscribetopic[] = "iot-2/cmd/command/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";// authentication method
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;//client id


//-----------------------------------------
Adafruit_SSD1306 oled (128, 64, &Wire, -1);
WiFiClient wifiClient; // creating the instance for wificlient
PubSubClient client (server, 1883, callback ,wifiClient); //calling the predefined client id by passing parameter like server id,portand wificredential
void setup()// configureing the ESP32 
{
  Serial.begin(115200);
  oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  pinMode(LED,OUTPUT);
  pinMode(CROSSIN,OUTPUT);
  pinMode(CROSSOUT,OUTPUT);
  pinMode(CROSS, INPUT);
  pinMode(Crossing, INPUT);
  pinMode(SchoolZone, INPUT);
  pinMode(HospitalZone, INPUT);
  pinMode(NoEntry, INPUT);
  pinMode(OneWay, INPUT);
  pinMode(TakeDiversion, INPUT);

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(10, 10);
  oled.println("Welcome to Chennai");
  oled.setCursor(20, 20);
  oled.println("Speed Limit 30");
  oled.setCursor(40, 30);
  oled.println("Go Slow!");
  oled.display();
  
  delay(10);
  Serial.println();
  wificonnect();
  mqttconnect();
}

void loop()// Recursive Function
{
  changeText();

  roadCross();

  PublishData();
  ledBlink();
  delay(1000);
  if (!client.loop()) {
    mqttconnect();
  }
}



/*.....................................retrieving to Cloud...............................*/

void PublishData() {
  mqttconnect();//function call for connecting to ibm
  /*
     creating the String in in form JSon to update the data to ibm cloud
  */
  String payload = "{\"temperature\":";
  payload += temp;
  payload += "," "\"humidity\":";
  payload += humid;
  payload += "}";

  
  Serial.print("Sending payload: ");
  Serial.println(payload);

  
  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish ok");// if it sucessfully upload data on the cloud then it will print publish ok in Serial monitor or else it will print publish failed
  } else {
    Serial.println("Publish failed");
  }
  
}
void mqttconnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
      
     initManagedDevice();
     Serial.println();
  }
}
void wificonnect() //function defination for wificonnect
{
  Serial.println();
  Serial.print("Connecting to ");

  WiFi.begin("Wokwi-GUEST", "", 6);//passing the wifi credentials to establish the connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initManagedDevice() {
  if (client.subscribe(subscribetopic)) {
    Serial.println((subscribetopic));
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void ledBlink(){
  digitalWrite(LED,HIGH);
  delay(1000);
  digitalWrite(LED,LOW);
  delay(1000);
}

void roadCross(){
  buttonState = digitalRead(CROSS);
  
  if(buttonState == HIGH){
    digitalWrite(CROSSIN,LOW);
    digitalWrite(CROSSOUT, HIGH);
  }else{
    digitalWrite(CROSSOUT, LOW);
    digitalWrite(CROSSIN, HIGH);
    delay(3000);
  }
}

void changeText(){
  if(digitalRead(Crossing) == LOW){
    oled.clearDisplay();
    oled.setCursor(20, 25);
    oled.println("Crossing Ahead");
    oled.setCursor(40, 35);
    oled.println("Go Slow!");
    oled.display();
  }else if(digitalRead(SchoolZone) == LOW){
    oled.clearDisplay();
    oled.setCursor(30, 25);
    oled.println("School Zone");
    oled.setCursor(28, 35);
    oled.println("Do Not Honk!");
    oled.display();
  }else if(digitalRead(HospitalZone) == LOW){
    oled.clearDisplay();
    oled.setCursor(25, 25);
    oled.println("Hospital Zone");
    oled.setCursor(28, 35);
    oled.println("Do Not Honk!");
    oled.display();
  }else if(digitalRead(NoEntry) == LOW){
    oled.clearDisplay();
    oled.setCursor(40, 25);
    oled.println("No Entry");
    oled.setCursor(10, 35);
    oled.println("No Vehicles Allowed");
    oled.display();
  }else if(digitalRead(OneWay) == LOW){
    oled.clearDisplay();
    oled.setCursor(40, 25);
    oled.println("One Way");
    oled.setCursor(30, 35);
    oled.println("Single Lane");
    oled.display();
  }else if(digitalRead(TakeDiversion) == LOW){
    oled.clearDisplay();
    oled.setCursor(20, 25);
    oled.println("Take Diversion");
    oled.setCursor(10, 35);
    oled.println("Bad Road Condition");
    oled.display();
  }
}

void editText(String msg){
  if(msg == "Crossing"){
    oled.clearDisplay();
    oled.setCursor(20, 25);
    oled.println("Crossing Ahead");
    oled.setCursor(40, 35);
    oled.println("Go Slow!");
    oled.display();
  }else if(msg == "SchoolZone"){
    oled.clearDisplay();
    oled.setCursor(30, 25);
    oled.println("School Zone");
    oled.setCursor(28, 35);
    oled.println("Do Not Honk!");
    oled.display();
  }else if(msg == "HospitalZone"){
    oled.clearDisplay();
    oled.setCursor(25, 25);
    oled.println("Hospital Zone");
    oled.setCursor(28, 35);
    oled.println("Do Not Honk!");
    oled.display();
  }else if(msg == "NoEntry"){
    oled.clearDisplay();
    oled.setCursor(40, 25);
    oled.println("No Entry");
    oled.setCursor(10, 35);
    oled.println("No Vehicles Allowed");
    oled.display();
  }else if(msg == "OneWay"){
    oled.clearDisplay();
    oled.setCursor(40, 25);
    oled.println("One Way");
    oled.setCursor(30, 35);
    oled.println("Single Lane");
    oled.display();
  }else if(msg == "TakeDiversion"){
    oled.clearDisplay();
    oled.setCursor(20, 25);
    oled.println("Take Diversion");
    oled.setCursor(10, 35);
    oled.println("Bad Road Condition");
    oled.display();
  }
}

void callback(char* subscribetopic, byte* payload, unsigned int payloadLength) 
{
  
  Serial.print("callback invoked for topic: ");
  Serial.println(subscribetopic);
  for (int i = 0; i < payloadLength; i++) {
    //Serial.print((char)payload[i]);
    data3 += (char)payload[i];
  }
  editText(data3);
  
  Serial.println("data: "+ data3);
  data3="";
}
#include <WiFi.h>//library for wifi
#include <PubSubClient.h>//library for MQtt
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>//library for display

#define LED 2
#define RED 19 //Red light
#define YELLOW 18 //Yellow light
#define GREEN 5 //Green light
#define CROSS 13
#define CROSSIN 4
#define CROSSOUT 15
#define Crossing 34 //White Button
#define SchoolZone 35 //Yellow Button
#define HospitalZone 32 //Red Button
#define NoEntry 25 //Black Button
#define OneWay 26 //Blue Button
#define TakeDiversion 27 //Grey Button

void callback(char* subscribetopic, byte* payload, unsigned int payloadLength); 

//-------credentials of IBM Accounts------

#define ORG "m1r2sh"//IBM ORGANITION ID
#define DEVICE_TYPE "Roadsafety"//Device type mentioned in ibm watson IOT Platform
#define DEVICE_ID "safetydevice1"//Device ID mentioned in ibm watson IOT Platform
#define TOKEN "!_1ZsGYHI9TsD5kvOu"     //Token
String message;
int buttonState = 0,i=30;
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
  pinMode(RED,OUTPUT);
  pinMode(YELLOW,OUTPUT);
  pinMode(GREEN,OUTPUT);
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
  oled.println("Speed Limit 40");
  oled.setCursor(40, 30);
  oled.println("Go Slow!");
  oled.display();
  
  digitalWrite(GREEN, HIGH);
  digitalWrite(CROSSOUT, HIGH);

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
  String payload = "{\"Message\":\"Enter Command to Display\"}";

  
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
void wificonnect() //function definition for wificonnect
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

void ledBlink(){//function for led blinking system
  digitalWrite(LED,LOW);
  delay(1000);
  digitalWrite(LED,HIGH);
}

void countDown(){//traffic light countdown system
  for(i;i>0;i--){
    oled.clearDisplay();
    oled.setTextSize(3);
    oled.setCursor(48, 20);
    oled.println(i);
    oled.display();
    delay(1000);
  }
  oled.setTextSize(1);
  i=30;
}

void roadCross(){//manual crossing function
  buttonState = digitalRead(CROSS);
  
  if(buttonState == LOW){
    trafficOff();
    digitalWrite(CROSSOUT,LOW);
    digitalWrite(CROSSIN, HIGH);
    countDown();
    crossing();
    trafficOn();
    digitalWrite(CROSSIN, LOW);
    digitalWrite(CROSSOUT, HIGH);
  }
}

void trafficOn(){//traffic light set to go
  digitalWrite(RED,HIGH);
  delay(1000);
  digitalWrite(RED,LOW);
  digitalWrite(YELLOW,HIGH);
  delay(1500);
  digitalWrite(YELLOW,LOW);
  digitalWrite(GREEN,HIGH);
}

void trafficOff(){//traffic light set to stop
  digitalWrite(GREEN,HIGH);
  delay(1000);
  digitalWrite(GREEN,LOW);
  digitalWrite(YELLOW,HIGH);
  delay(1500);
  digitalWrite(YELLOW,LOW);
  digitalWrite(RED,HIGH);
}

void crossing(){//crossing display
  oled.clearDisplay();
  oled.setCursor(20, 25);
  oled.println("Crossing Ahead");
  oled.setCursor(40, 35);
  oled.println("Go Slow!");
  oled.display();
}

void schoolZone(){//school zone display
  oled.clearDisplay();
  oled.setCursor(30, 25);
  oled.println("School Zone");
  oled.setCursor(28, 35);
  oled.println("Do Not Honk!");
  oled.display();
}

void hospitalZone(){//hospital zone display
  oled.clearDisplay();
  oled.setCursor(25, 25);
  oled.println("Hospital Zone");
  oled.setCursor(28, 35);
  oled.println("Do Not Honk!");
  oled.display();
}

void noEntry(){//no entry display
  oled.clearDisplay();
  oled.setCursor(40, 25);
  oled.println("No Entry");
  oled.setCursor(10, 35);
  oled.println("No Vehicles Allowed");
  oled.display();
}

void oneWay(){//one way display
  oled.clearDisplay();
  oled.setCursor(40, 25);
  oled.println("One Way");
  oled.setCursor(30, 35);
  oled.println("Single Lane");
  oled.display();
}

void takeDiversion(){//take diversion display
  oled.clearDisplay();
  oled.setCursor(20, 25);
  oled.println("Take Diversion");
  oled.setCursor(10, 35);
  oled.println("Bad Road Condition");
  oled.display();
}

void roadWorkAhead(){//alert road work ahead
  oled.clearDisplay();
  oled.setCursor(40, 15);
  oled.println("CAUTION!");
  oled.setCursor(17, 25);
  oled.println("Road Work Ahead");
  oled.setCursor(15, 35);
  oled.println("Work On Progress");
  oled.display();
}

void badWeather(){//alert bad weather
  oled.clearDisplay();
  oled.setCursor(40, 15);
  oled.println("CAUTION!");
  oled.setCursor(30, 25);
  oled.println("Bad Weather");
  oled.setCursor(20, 35);
  oled.println("Low Visibility");
  oled.display();
}

void heavyTraffic(){//alert heavy traffic
  oled.clearDisplay();
  oled.setCursor(40, 15);
  oled.println("CAUTION!");
  oled.setCursor(25, 25);
  oled.println("Heavy Traffic");
  oled.setCursor(22, 35);
  oled.println("Take Diversion");
  oled.display();
}

void heavyRain(){//alert heavy rain
  oled.clearDisplay();
  oled.setCursor(40, 15);
  oled.println("CAUTION!");
  oled.setCursor(33, 25);
  oled.println("Heavy Rain");
  oled.setCursor(40, 35);
  oled.println("Go Slow!");
  oled.display();
}

void accidentZone(){//alert accident zone
  oled.clearDisplay();
  oled.setCursor(40, 15);
  oled.println("CAUTION!");
  oled.setCursor(25, 25);
  oled.println("Accident Zone");
  oled.setCursor(20, 35);
  oled.println("Speed Limit 30");
  oled.display();
}

void changeText(){//change display text
  if(digitalRead(Crossing) == LOW){
    crossing();
  }else if(digitalRead(SchoolZone) == LOW){
    schoolZone();
  }else if(digitalRead(HospitalZone) == LOW){
    hospitalZone();
  }else if(digitalRead(NoEntry) == LOW){
    noEntry();
  }else if(digitalRead(OneWay) == LOW){
    oneWay();
  }else if(digitalRead(TakeDiversion) == LOW){
    takeDiversion();
  }
}

void editText(String msg){//edit didplay text
  if(msg == "Crossing"){
    crossing();
  }else if(msg == "SchoolZone"){
    schoolZone();
  }else if(msg == "HospitalZone"){
    hospitalZone();
  }else if(msg == "NoEntry"){
    noEntry();
  }else if(msg == "OneWay"){
    oneWay();
  }else if(msg == "TakeDiversion"){
    takeDiversion();
  }else if(msg == "RoadCross"){
    trafficOff();
    digitalWrite(CROSSOUT,LOW);
    digitalWrite(CROSSIN, HIGH);
    countDown();
    crossing();
    trafficOn();
    digitalWrite(CROSSIN, LOW);
    digitalWrite(CROSSOUT, HIGH);
  }else if(msg == "RoadWorkAhead"){
    roadWorkAhead();
  }else if(msg == "BadWeather"){
    badWeather();
  }else if(msg == "HeavyTraffic"){
    heavyTraffic();
  }else if(msg == "HeavyRain"){
    heavyRain();
  }else if(msg == "AccidentZone"){
    accidentZone();
  }
}

//CallBack function
void callback(char* subscribetopic, byte* payload, unsigned int payloadLength) 
{
  Serial.print("callback invoked for topic: ");
  Serial.println(subscribetopic);
  for (int i = 0; i < payloadLength; i++) {
    message += (char)payload[i];
  }
  editText(message);
  Serial.println("data: "+ message);
  message="";
}
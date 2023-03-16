#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <Wire.h>



const char* ssid     = "Your Network Name";                 // SSID of local network
const char* password = "Your Password";                    // Password on network
String APIKEY = "API Key";                                 
String CityID = "CityID";                                 //Your City ID


WiFiClient client;
char servername[]="api.openweathermap.org";              // remote server we will connect to
String result;

int  counter = 60;                                      

String weatherDescription ="";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;

LiquidCrystal_I2C lcd(0x3F, 16, 2);    // Address of your i2c LCD back pack should be updated.

void setup() {
  Serial.begin(115200);
  int cursorPosition=0;
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.print("   Connecting");  
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  
             while (WiFi.status() != WL_CONNECTED) 
            {
            delay(500);
            lcd.setCursor(cursorPosition,2); 
            lcd.print(".");
            cursorPosition++;
            }
  lcd.clear();
  lcd.print("   Connected!");
  Serial.println("Connected");
  delay(1000);
}

void loop() {
    if(counter == 60)                                 //Get new data every 10 minutes
    {
      counter = 0;
      displayGettingData();
      delay(1000);
      getWeatherData();
    }else
    {
      counter++;
      displayWeather(weatherLocation,weatherDescription);
      delay(5000);
      displayConditions(Temperature,Humidity,Pressure);
      delay(5000);
    }
}

void getWeatherData()                                //client function to send/receive GET request data.
{
  if (client.connect(servername, 80))   
          {                                         //starts client connection, checks for connection
          client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
          client.println("Host: api.openweathermap.org");
          client.println("User-Agent: ArduinoWiFi/1.1");
          client.println("Connection: close");
          client.println();
          } 
  else {
         Serial.println("connection failed");        //error message if no client connect
          Serial.println();
       }

  while(client.connected() && !client.available()) 
  delay(1);                                          //waits for data
  while (client.connected() || client.available())    
       {                                             //connected or data available
         char c = client.read();                     //gets byte from ethernet buffer
         result = result+c;
       }

client.stop();                                      //stop client
result.replace('[', ' ');
result.replace(']', ' ');
Serial.println(result);
char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';
StaticJsonDocument<1024> doc;
DeserializationError error=deserializeJson(doc, jsonArray);

if (error)
  {
    Serial.print("deserializeJson() failed");
    Serial.println(error.c_str());
    return;
  }

String location = doc["name"];
String country = doc["sys"]["country"];
float temperature = doc["main"]["temp"];
float humidity = doc["main"]["humidity"];
String weather = doc["weather"]["main"];
String description = doc["weather"]["description"];
float pressure = doc["main"]["pressure"];
weatherDescription = description;
weatherLocation = location;
Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;

}

void displayWeather(String location,String description)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(location);
  lcd.print(", ");
  lcd.print(Country);
  lcd.setCursor(0,1);
  lcd.print(description);
}

void displayConditions(float Temperature,float Humidity, float Pressure)
{
 lcd.clear();                            //Printing Temperature
 lcd.print("T:"); 
 lcd.print(Temperature,1);
 lcd.print((char)223);
 lcd.print("C "); 
                                         
 lcd.print(" H:");                       //Printing Humidity
 lcd.print(Humidity,0);
 lcd.print(" %"); 
 
 lcd.setCursor(0,1);                     //Printing Pressure
 lcd.print("P: ");
 lcd.print(Pressure,1);
 lcd.print(" hPa");
}

void displayGettingData()
{
  lcd.clear();
  lcd.print("Getting data");
}

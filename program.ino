#include <Bridge.h>
#include <Temboo.h>
#include <Process.h>

// Contains Temboo account information
#include "TembooAccount.h" 

// Variables
int lightLevel;
float humidity;
float temperature;
float temperature_limit;
unsigned long time;

// Process to get the measurement time
Process date;

// Your Google Docs data
const String GOOGLE_USERNAME = "karthish06@gmail.com";
const String GOOGLE_PASSWORD = "wicwpednuxnbakqq";
const String SPREADSHEET_TITLE = "DHT";

// Include required libraries
#include "DHT.h"

// DHT11 sensor pins
#define DHTPIN 8 
#define DHTTYPE DHT11

// DHT instance
DHT dht(DHTPIN, DHTTYPE);



void setup() {
  
  {
    Serial.begin(115200);
    delay(4000);
    while(!Serial);
  }
  
  // Initialize DHT sensor
  dht.begin();
  
  // Start bridge
  Bridge.begin();
  
  // Start date process
  time = millis();
  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%D-%T");
    date.run();
  }

  // Set temperature limit
  temperature_limit = 25.0;

 
}


void loop() {
  
  // Measure the humidity & temperature
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
    
  // Measure light level
  int lightLevel = analogRead(A0);

 
  
  // Append data to Google Docs sheet
  runAppendRow(lightLevel, temperature, humidity);

  // Send email alert ?
  
        
  // Repeat every 10 minutes
  delay(1000);
  
}



// Function to add data to Google Docs
void runAppendRow(int lightLevel, float temperature, float humidity) {
  TembooChoreo AppendRowChoreo;

  // Invoke the Temboo client
  AppendRowChoreo.begin();

  // Set Temboo account credentials
  AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
  AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);
  
  // Identify the Choreo to run
  AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");

 // Set Choreo inputs
    AppendRowChoreo.addInput("Username", "karthish06@gmail.com");
    AppendRowChoreo.addInput("Password", "wicwpednuxnbakqq");

    AppendRowChoreo.addInput("SpreadsheetTitle", "DHT");
  
  // Restart the date process:
  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%D-%T");
    date.run();
   }
  
  // Get date
  String timeString = date.readString(); 
  
  // Format data
  String data = "";
  data = data + timeString + "," + String(temperature) + "," + String(humidity) + "," + String(lightLevel);

   AppendRowChoreo.addInput("RowData", data);
  
    // Run the Choreo; when results are available, print them to serial
    AppendRowChoreo.run();
    
    while(AppendRowChoreo.available()) {
      char c = AppendRowChoreo.read();
      Serial.print(c);
    }
    AppendRowChoreo.close();
  

  Serial.println("Waiting...");
  delay(3000); // wait 30 seconds between AppendRow calls
}


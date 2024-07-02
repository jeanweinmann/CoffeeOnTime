#include <NTPClient.h>
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "D-Link-2.4G";
const char* password = "Karli#2004";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  timeClient.begin();
  timeClient.setTimeOffset(7200);  // Set time offset (in seconds) from your timezone
  Serial.println("Connected to WiFi");
  makeRequest();
}

void loop() {
  // Your code here
}

void makeRequest() {
  HTTPClient http;

  // Replace with the actual URL including the query parameters
  const char* url = "https://developer.sepush.co.za/business/2.0/area?id=tshwane-14-hennopsparkext1&test=current";

  http.begin(url);

  // Replace with the actual token
  const char* token = "715405A3-7B374484-894CBE8C-CBC66506";

  http.addHeader("token", token);

  int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println(response);
    JSONVar myObject = JSON.parse(response);

    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    Serial.println("Loadshedding Information");
    Serial.print("Name: ");
    Serial.println(myObject["info"]["name"]);
    Serial.print("Region: ");
    Serial.println(myObject["info"]["region"]);

    Serial.print("Events:");
    JSONVar events = myObject["events"];
    Serial.println(events[0]["start"]);

    tm t;
    strptime(events[0]["start"], "%Y-%m-%dT%H:%M:%S%z", &t);
    time_t time1 = mktime(&t);
    Serial.println(time1);
    
    timeClient.update();
    Serial.print("Current time: ");
    Serial.println(timeClient.getEpochTime());

    Serial.println(difftime(timeClient.getEpochTime(), time1));


    Serial.print("Stages: ");
    JSONVar days = myObject["schedule"]["days"];

    for (int i = 0; i < days.length(); i++) {
      Serial.println("Day " + String(i));
      JSONVar stages = days[i]["stages"];
      for (int j = 0; j < stages.length(); j++) {
        Serial.println(stages[j]);
        Serial.print("");
      }
    }

  } else {
    Serial.print("Error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

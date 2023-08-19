#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6kmsOrkKd"
#define BLYNK_TEMPLATE_NAME "Fire Alert System"
#define BLYNK_AUTH_TOKEN "B4D2FhlYdnnylNg3KQImmHHd5_FijVd9"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// WiFi credentials.
//char ssid[] = "PandaHo2.4";
//char pass[] = "27753828";
char ssid[] = "GUS";
char pass[] = "PandaWiFi";

BlynkTimer timer;

#define FLAME 25
#define MQ2 32
#define WiFi_Status 2

DHT dht(27, DHT22);

WidgetLED FIRE_ON(V3);

void sendDataToSensor() {
  int fire = digitalRead(FLAME);
  Serial.println(fire);


   if (fire == 0) {
    FIRE_ON.on();
    Serial.println("FIRE DETECTED!");
    Blynk.logEvent("on_fire","FIRE DETECTED IN KITCHEN !");
  } else {
    FIRE_ON.off();
    Serial.println("NO FIRE DETECTED!");
  }

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Cannot read the data from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" C ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" % ");
  Serial.println();

  int sensorValue = analogRead(MQ2);

  Blynk.virtualWrite(V2, sensorValue);

  Serial.print("Gas Value: ");
  Serial.println(sensorValue);

  if (sensorValue >= 600) {
    Blynk.logEvent("gas","TOO MUCH GAS IN KITCHEN !");
  }
}

void setup() {
  // Debug console
  Serial.begin(9600);

  pinMode(WiFi_Status, OUTPUT);

  pinMode(FLAME, INPUT);
  Serial.println("Flame Sensor start");

  pinMode(MQ2, INPUT);
  Serial.println("MQ2 Sensor start");

  dht.begin();
  delay(2000);
  Serial.println("DHT Sensor start");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  Serial.println("Connect to WiFi");
  Serial.print("Local IP is: ");
  Serial.println(WiFi.localIP());

  timer.setInterval(1000L, sendDataToSensor);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(WiFi_Status, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(1000);            // wait for a second

    digitalWrite(WiFi_Status, LOW);  // turn the LED off by making the voltage LOW
    delay(1000);
  }

  Blynk.run();
  timer.run();
}

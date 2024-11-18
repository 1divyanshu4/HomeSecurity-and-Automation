//DHT11 And NodeMCU With Blynk

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3yJ85gRtq"
#define BLYNK_TEMPLATE_NAME "Home Security "
#define BLYNK_AUTH_TOKEN            "eiqLD15ezRPfJDSxkLBIwQNg4ut_Go98"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "eiqLD15ezRPfJDSxkLBIwQNg4ut_Go98";

char ssid[] = "Redmi";                             //Enter your WIFI name
char pass[] = "72580306";   


BlynkTimer timer;                      //Enter your WIFI password

// D0 = flame
// D1 = buzzer
// D2
// D3 = temp
// D4 = in4
// D5 = in3
// D6
// D7 = in2
// D8 = in1


#define DHTPIN D3   // D3
#define FLAME_PIN A0  // The ESP8266 pin connected to DO pin of the flame sensor
#define Switch_1 D8
#define Switch_2 D7
#define Switch_3 D5
#define Switch_4 D4
#define Buzzer D1


// Uncomment whatever type you're using!
#define DHTTYPE DHT11  // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.


void setup() {
  // Debug console
  Serial.begin(9600);

  pinMode(Switch_1, OUTPUT);
  pinMode(Switch_2, OUTPUT);
  pinMode(FLAME_PIN, INPUT);
  digitalWrite(Switch_1, 0);
  digitalWrite(Switch_2, 0);


  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  dht.begin();

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
}


void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();  // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V0, h);
}

void flameSensor() {
  int flameThreshold = 250;
  int flame_value = digitalRead(FLAME_PIN);
  Serial.println(flame_value);


  Blynk.virtualWrite(V2, flame_value);

  if (flame_value < flameThreshold) {
    digitalWrite(Buzzer, HIGH);
    Serial.println("No flame => No fire detected");
  } else {
    Serial.println("Flame present => Fire detected");
    digitalWrite(Buzzer, LOW);
  }
}



//Get the Switch1 button value
BLYNK_WRITE(V3) {
  int Switch1 = param.asInt();
  digitalWrite(Switch_1, Switch1);
}

//Get the Switch2 button value
BLYNK_WRITE(V4) {
  int Switch2 = param.asInt();
  digitalWrite(Switch_2, Switch2);
}

//Get the Switch3 button value
BLYNK_WRITE(V5) {
  int Switch3 = param.asInt();
  digitalWrite(Switch_3, Switch3);
}

//Get the Switch4 button value
BLYNK_WRITE(V6) {
  int Switch4 = param.asInt();
  digitalWrite(Switch_4, Switch4);
}

void loop() {
  
  Blynk.run();
  timer.run();
  sendSensor();
  flameSensor();

}

#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>


#define LED_BUILTIN 4
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASS"
#define MQTT_SERVER "192.168.0.102"
#define MQTT_PORT 1888
#define MQTT_TOPIC "test/espcam"
#define MQTT_CLIENT "ESP32"
#define TOPIC_UP "PICTURE"

//#define pirPin 16
//volatile int isrCounter = 0;
//unsigned long StartTime;
//unsigned long ElapsedTime;

//void IRAM_ATTR isr() {
//  isrCounter++;
//}

WiFiClient espClient;
PubSubClient client(espClient);


void camera_init() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = 5;
  config.pin_d1       = 18;
  config.pin_d2       = 19;
  config.pin_d3       = 21;
  config.pin_d4       = 36;
  config.pin_d5       = 39;
  config.pin_d6       = 34;
  config.pin_d7       = 35;
  config.pin_xclk     = 0;
  config.pin_pclk     = 22;
  config.pin_vsync    = 25;
  config.pin_href     = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn     = 32;
  config.pin_reset    = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size   = FRAMESIZE_VGA; // QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
  config.jpeg_quality = 10;           
  config.fb_count     = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void take_picture() {
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  if (MQTT_MAX_PACKET_SIZE == 128) {
    //SLOW MODE (increase MQTT_MAX_PACKET_SIZE)
    client.publish_P(TOPIC_UP, fb->buf, fb->len, false);
  }
  else {
    //FAST MODE (increase MQTT_MAX_PACKET_SIZE)
    client.publish(TOPIC_UP, fb->buf, fb->len, false);
  }
  Serial.println("CLIC");
  esp_camera_fb_return(fb);

void connectToWifi() {
  Serial.println("Conectandose al punto de acceso WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conexión establecida!");
  Serial.println(WiFi.localIP());
}
void connectToMqtt() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  
  Serial.println("Conectando con el servidor MQTT");
  client.setCallback(callback);
  
  if(client.connect(MQTT_CLIENT)){
    Serial.println("Conexión establecida con el servidor!");
    client.publish(MQTT_TOPIC, "Enviando imagen a continuacion!");
    delay(1000);
    Serial.println("Enviando Hola Mundo!");
  }
  else{
    Serial.println("No se ha conectado con el servidor :(");
    client.publish(MQTT_TOPIC, "Hola Mundo!");
  }
}

void setup() {
  Serial.begin(115200);
  digitalWrite(LED_BUILTIN, HIGH);
  camera_init();
  connectToWifi();
  delay(1000);
  connectToMqtt();
  take_picture();
  //Serial.begin(115200);
  //delay(1000); //Take some time to open up the Serial Monitor
  //pinMode (LED_BUILTIN, OUTPUT);//Specify that LED pin is output
  //pinMode(pirPin, INPUT_PULLUP);
  //attachInterrupt(pirPin, isr, RISING );
  digitalWrite(LED_BUILTIN, LOW);
  //pinMode (LED_BUILTIN, INPUT);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);
  Serial.println("Going to sleep now");
  delay(1000);
  esp_deep_sleep_start();
  //Serial.println("This will never be printed");
}

void loop() {
//  ElapsedTime = millis() - StartTime;
//if(ElapsedTime>2000 && isrCounter>0){
//  StartTime = millis();
//  Serial.printf("State:%d", isrCounter);
//  Serial.println("");
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(100);
//  digitalWrite(LED_BUILTIN, LOW);
//  isrCounter=0;
//  }
}

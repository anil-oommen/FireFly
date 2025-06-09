#ifndef WifiConnector_h
#define WifiConnector_h

#include "FS.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

/* Sometimes Connection Keeps Failing , turn on Debug & try different passwords */
#define WIFI_MANAGER_DEBUG false
#define SPIFFS_CONFIG_JSONFILE "/config.json"

/* Config Settings from the WifiManager @ Wifi Setup.*/
char config_mqtt_server[50] = "";
char config_mqtt_server_port[5] = "1883";
char config_mqtt_user[50] = "";
char config_mqtt_pswd[50] = "";

const char* initSetup_accessPointName  = FIREFLY_MCU_NAME;


#include <DoubleResetDetector.h>
// Number of seconds after reset during which a subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);



bool _shouldSaveConfigToFile = false;
void _saveConfigCallback () {
  Serial.println("INFO : [FIREFLY] Recieved Config from AP (Soft Access Point Mode)");
  _shouldSaveConfigToFile = true;
}

void _configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("INFO : [WIFIMGR] Entering ConfigMode: AP (Soft Access Point Mode)");
  Serial.print  ("INFO : [WIFIMGR]    > Connect to Soft Wifi AccessPoint: " );
  Serial.println(initSetup_accessPointName);
  Serial.print  ("INFO : [WIFIMGR]    > Configure at http://");
  Serial.println(WiFi.softAPIP() );
  
  drd.stop(); //Reset , so next boot does not go into SoftAP Config Mode.
}


bool loadConfigFromFile() {
  File configFile = SPIFFS.open(SPIFFS_CONFIG_JSONFILE, "r");
  if (!configFile) {
    Serial.println("ERRO : [CONFIG] Unable to load config File. ");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("ERRO : [CONFIG] Config file size overflow.");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("ERRO : [CONFIG] Unable to parse config json.");
    return false;
  }

  strcpy(config_mqtt_server, json["config_mqtt_server"]);
  strcpy(config_mqtt_server_port, json["config_mqtt_server_port"]);
  strcpy(config_mqtt_user, json["config_mqtt_user"]);
  strcpy(config_mqtt_pswd, json["config_mqtt_pswd"]);

  Serial.print("DEBUG: [CONFIG] Loaded from Json[");
  Serial.print(config_mqtt_server);Serial.print(", ");
  Serial.print(config_mqtt_user);Serial.print(", ");
  Serial.print(config_mqtt_pswd);Serial.print(" ");
  Serial.println("] ");
  
  return true;
}

bool saveConfigToFile() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["config_mqtt_server"] = config_mqtt_server;
  json["config_mqtt_server_port"] = config_mqtt_server_port;
  json["config_mqtt_user"] = config_mqtt_user;
  json["config_mqtt_pswd"] = config_mqtt_pswd;

  Serial.print("DEBUG: [CONFIG] Saving to Json[");
  Serial.print(config_mqtt_server);Serial.print(", ");
  Serial.print(config_mqtt_user);Serial.print(", ");
  Serial.print(config_mqtt_pswd);Serial.print(" ");
  Serial.println("] ");
  

  File configFile = SPIFFS.open(SPIFFS_CONFIG_JSONFILE, "w");
  if (!configFile) {
    Serial.println("ERRO : [CONFIG] Unable to save config file");
    return false;
  }
  json.printTo(configFile);
  return true;
}

void setupWifiConnection(){
  if (!SPIFFS.begin()) {
    Serial.println("ERRO : [CONFIG] Unable to mount FS. ");
    return;
  }

  loadConfigFromFile(); //Load any Previous Config.
  
  WiFiManager wifiManager;
  wifiManager.setAPCallback(_configModeCallback);
  wifiManager.setSaveConfigCallback(_saveConfigCallback); 
  wifiManager.setDebugOutput(WIFI_MANAGER_DEBUG);

  WiFiManagerParameter mqtt_serverWiFiConfig("mqtt_server", "192.168.1.200", config_mqtt_server, 50);
  WiFiManagerParameter mqtt_server_portWiFiConfig("mqtt_server_port", "1883", config_mqtt_server_port, 5);
  WiFiManagerParameter mqtt_userWiFiConfig("mqtt_user", "<username>", config_mqtt_user, 50);
  WiFiManagerParameter mqtt_pswdWiFiConfig("mqtt_pswd", "<password>", config_mqtt_pswd, 50);
  wifiManager.addParameter(&mqtt_serverWiFiConfig);
  wifiManager.addParameter(&mqtt_server_portWiFiConfig);
  wifiManager.addParameter(&mqtt_userWiFiConfig);
  wifiManager.addParameter(&mqtt_pswdWiFiConfig);
  
   if (drd.detectDoubleReset()) {
    Serial.println("INFO : [WIFIMGR] Hard Reset Detected. Forcing to ConfigMode: AP (Soft Access Point Mode)");
    wifiManager.startConfigPortal(initSetup_accessPointName, "");
   }else{
    Serial.println("INFO : [WIFIMGR] AutoConnect [ AP (Soft Access Point Mode) _or_ ST (Station Mode as WifiClient) ]" );
    wifiManager.autoConnect(initSetup_accessPointName, "");
   }
    //if(true) return;
   strcpy(config_mqtt_server, mqtt_serverWiFiConfig.getValue());
   strcpy(config_mqtt_server_port, mqtt_server_portWiFiConfig.getValue());
   strcpy(config_mqtt_user, mqtt_userWiFiConfig.getValue());
   strcpy(config_mqtt_pswd, mqtt_pswdWiFiConfig.getValue());

   //if (_shouldSaveConfigToFile) {  , Value not being set, callback not working. Skipping.
    saveConfigToFile();
  //}
  drd.stop();

  Serial.println("INFO : [WIFIMGR] Ready running ST (Station Mode as WifiClient) ]");
  Serial.print  ("INFO : [WIFIMGR]    > Connected to Wifi: ");
  Serial.println(WiFi.SSID() );
  Serial.print  ("INFO : [WIFIMGR]    > My IP: ");
  Serial.println(WiFi.localIP() );
}

#endif

#ifndef MqttConnector_h
#define MqttConnector_h
/*
 * ------------ IMPORTANT , NEEDED TO ENSURE MQTT PUBLISH DOES NOT FAIL --------------------------
 * 
 * MQTT Library Required. PubSubClient MQTT
 * https://pubsubclient.knolleary.net/
 * Important , MaxLimit on the 
 * https://github.com/knolleary/pubsubclient#limitations
 * https://github.com/knolleary/pubsubclient/issues/62 
 *  The maximum message size, including header, is 128 bytes by default. 
 *  This is configurable via MQTT_MAX_PACKET_SIZE in PubSubClient.h.
 *  
 *  below did not work even after changing PubSub...h
 *  #define MQTT_MAX_PACKET_SIZE 450 
 */
 
#include <PubSubClient.h>
#include <stdlib.h>



int         mqtt_server_port = 1883;  //TODO hardcoded for now, not copied from config_mqtt_server_port
const char* mqtt_client_id =                            "fireflymcu";
const char* mqtt_feedback_publish_topic =               "firefly/dctank/feedback";
const char* mqtt_control__recieve_topic =               "firefly/dctank/control";
const char* mqtt_control__recieve_retain =              "firefly/dctank/retain";
int         mqtt_subscribe_qos = 0;   //QoS 0 – at most once


typedef void (*funcMqttConnected)();
funcMqttConnected CallbackMqttConnected = NULL;

typedef void (*funcMqttNotConnected)();
funcMqttNotConnected CallbackMqttNotConnected = NULL;

typedef void (*funcJoystickCommand) (const int left,const int right);
funcJoystickCommand CallbackJoystickCommand = NULL;

typedef void (*funcDCSignal) (int a_direction, int a_pwmvalue, int b_direction, int b_pwmvalue);
funcDCSignal CallbackDCSignal = NULL;

typedef void (*funcStopMotor) ();
funcStopMotor CallbackStopMotor = NULL;


void _callbackMqttMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("DEBUG: [MQTT] Message Recieved[  < < < ]:");
  String strPayload = "";
  for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    strPayload +=receivedChar;
  }
  Serial.println(strPayload);
  StaticJsonBuffer<1000> JSONBuffer;   //Memory pool
  JsonObject& parsed = JSONBuffer.parseObject(payload); //Parse message
  if (!parsed.success()) {   //Check for errors in parsing
    Serial.println("ERROR: [RECV] JSON Parsing failed");
  }else{
    const char* command    = parsed["command"];
    if(!command){
      Serial.printf("ERROR: [RECV] no command \n"); 
    }else if(String(command) == "run"){
      JsonVariant joystick = parsed["joystick"];
      JsonVariant dcmotor = parsed["dcmotor"];
      if(joystick){
        const char* x    = parsed["joystick"]["x"];
        const char* y    = parsed["joystick"]["y"];
        if(x && y ){
          if(CallbackJoystickCommand != NULL){ 
            CallbackJoystickCommand(atoi(x),atoi(y));
          }
        }else{
          Serial.printf("ERROR: [RECV] Missing Parameter/s for 'joystick'.\n"); 
        }
      }else if(dcmotor){
        const char* a_direction     = parsed["dcmotor"]["a"]["direction"];
        const char* a_pwmvalue      = parsed["dcmotor"]["a"]["pwm"];
        const char* b_direction     = parsed["dcmotor"]["b"]["direction"];
        const char* b_pwmvalue      = parsed["dcmotor"]["b"]["pwm"];
        if(a_direction && a_pwmvalue && b_direction && b_pwmvalue){
          if(CallbackDCSignal != NULL){ 
            CallbackDCSignal(
              atoi(a_direction),atoi(a_pwmvalue),
              atoi(b_direction),atoi(b_pwmvalue)
            );
          }
        }else{
          Serial.printf("ERROR: [RECV] Missing Parameter/s for 'dcmotor' .\n"); 
        }
      }else{
        Serial.printf("ERROR: [RECV] Unsupported, requires joystick or dcmotor.\n"); 
      }
      
      /*JsonObject& joystick =  parsed["joystick"];
      const char* x    = joystick["x"];
      const char* y    = joystick["y"];
      callbackRunMotor(atoi(x),atoi(y)); */
    }else if(String(command) == "stop"){
      if(CallbackStopMotor != NULL){ 
        CallbackStopMotor();
      }
    }else{
      Serial.printf("ERROR: [RECV] Ignoring, Command not undestood %s.\n", command); 
    }
  }
  

}
WiFiClient wifiClient;
PubSubClient client(config_mqtt_server, mqtt_server_port, _callbackMqttMessage, wifiClient);
boolean mqttConnected =false;
EventTimer mqttReconnectOnFailTimer("MQTTReconnect#",1000 * 2, true, false); //every x seconds

boolean _isMQTTConnected(){
  mqttConnected = client.connected();
  if(mqttConnected){
    return true;
  }
  //Give a bit of breather when reconnecting.
  if(!mqttReconnectOnFailTimer.isDueForRun()) 
    return false;

  Serial.print("DEBUG: [MQTT] settings:[");
  Serial.print(config_mqtt_server);Serial.print(", ");
  Serial.print(config_mqtt_user);Serial.print(", ");
  Serial.print(config_mqtt_pswd);Serial.print(", ");
  Serial.println("] ");
  
  if (client.connect(mqtt_client_id,config_mqtt_user,config_mqtt_pswd)) {
    Serial.println("DEBUG: [MQTT] Connected to Broker"); 
    Serial.print("DEBUG: [MQTT] Subscribing to : ");
    Serial.println(mqtt_control__recieve_topic);
    Serial.print("DEBUG: [MQTT] Subscribing to : ");
    Serial.println(mqtt_control__recieve_retain);
    client.subscribe(mqtt_control__recieve_topic,mqtt_subscribe_qos);
    client.subscribe(mqtt_control__recieve_retain);
    mqttConnected = client.connected();
    if(mqttConnected){
      Serial.println("DEBUG: [MQTT] Connected.");
      if(CallbackMqttConnected != NULL){ 
        CallbackMqttConnected();
      }
    }else{
      Serial.println("DEBUG: [MQTT] Connection Failure.");
    }
  }
  else {
    Serial.println("DEBUG: [MQTT] Broker Connection Failed. (username, password)");
    if(CallbackMqttNotConnected != NULL){
      CallbackMqttNotConnected();
    }
    mqttConnected=false;
    //abort();
  }

}



boolean publishEvent(String event, String value, String message){
  if(!mqttConnected) return false;

  StaticJsonBuffer<500> jsonPubBuffer;
  JsonObject& root = jsonPubBuffer.createObject();

  root["client"] = mqtt_client_id;
  root["event"] = event;
  root["value"] = value ;
  root["message"] = message ;
  String jsonPrettyStr;
  root.prettyPrintTo(jsonPrettyStr);
  Serial.print("DEBUG: [MQTT] Message Publish [  > > > ]:");
  root.printTo(Serial);
  Serial.println();
  return client.publish(mqtt_feedback_publish_topic,jsonPrettyStr.c_str());
  return true;
}

void publishNotifyBootup(){
  publishEvent("Bootup",app_compile_version,app_compile_date);
}

/*
boolean isHiveConnected(){
  if(!_isMQTTConnected()) return false;
  return true;
}*/

void loopMqttConnector(){
  if(!_isMQTTConnected()) return ;
  client.loop();
}

void disconnectMqtt(){
  if(_isMQTTConnected()){
    client.disconnect();
    mqttReconnectOnFailTimer.enabled(false);
    Serial.println("DEBUG: [MQTT] Disconnected from Broker and turning off AutoReconnect");
  }
}

#endif

/* 
    This File contains all the config related to networking
*/

//Wifi related
#define smartWifiMode

char URL_innoair_post[] = "http://innoairiaqbox.viphk.91tunnel.com/";
#define OTA_currVers 1
#define OTA_server_url "http://innoairiaqbox.viphk.ngrok.org/"
char OTA_check_url[] = OTA_server_url"firmware/version.txt";
char OTA_update_url[] = OTA_server_url"firmware/iaq.bin";


//MQTT
#define mqttBroker "mqtt://broker.mqttdashboard.com"
#define mqtt_topic "/hkust_fanlab/wendy"

#define mqttBroker2 "mqtt://broker.emqx.io"
#define mqtt_topic2 "/hkust_fanlab/wendy"
//#define relayCtrl
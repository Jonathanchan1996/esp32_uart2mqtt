import random, csv, time, os, json
import numpy as np

from paho.mqtt import client as mqtt_client


#broker = 'broker.mqttdashboard.com'
broker = 'broker.emqx.io'
port = 1883
topic = "/hkust_fanlab/wendy/#"
# generate client ID with pub prefix randomly
client_id = f'python-mqtt-{random.randint(0, 100)}'
#username = 'hkust_ia'
#password = 'public'
rAry = np.array([1,1,47,47])

def write_csv(fileName,data):
    with open(fileName, 'a') as outfile:
        writer = csv.writer(outfile,  lineterminator='\n')
        writer.writerow(data)

def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    #client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        #print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
        if __name__ == '__main__':
            try:
                timestr = time.strftime("%Y%m%d")
                fileName= "mqtt_data/D_"+timestr+"_raw.csv"
                write_csv(fileName, [str(int(time.time())),msg.topic, str(msg.payload.decode()).replace('\r','').replace('\n','')])
                
                #PALM format, for specific project
                fileName= "mqtt_data/D_"+timestr+"_dat.csv"
                x = str(msg.payload.decode()).replace('\r','').replace('\n','')
                y=json.loads(x)
                print(y)
                rawData = y['val'].split(',')
                rawResist = [int(x,16) for x in rawData[2:-1]]
                heater = rawResist[4:8]
                heater = [3.3*x/4096.0 for x in heater]
                rawResist = rawResist[0:4]
                rawResist = [3.3*(vadc)/4096.0 for vadc in rawResist]
                rawResist = [np.min([v/(3.3-v), 5000]) for v in rawResist]
                rawResist = np.array(rawResist)
                rawResist = rawResist*rAry
                saveData = [y['ts'], rawData[0], rawData[1]]+rawResist.tolist()+heater
                write_csv(fileName, saveData)
            except:
                print('error read/write')

    client.subscribe(topic)
    client.on_message = on_message


def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()


if __name__ == '__main__':
    path = 'mqtt_data/'
    if not os.path.exists(path):
        os.mkdir(path)
    run()

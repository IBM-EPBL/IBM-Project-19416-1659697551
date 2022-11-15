import time
import sys
import ibmiotf.application
import ibmiotf.device
import random as r


#Provide your IBM Watson Device Credentials
organization = "m1r2sh"
deviceType = "Roadsafety"
deviceId = "safetydevice2"
authMethod = "token"
authToken = "t3USLaRSVT*BbaPGlA"

Message_list = ["Crossing","School Zone","Hospital Zone","No Entry","One Way","Take Diversion"]
Traffic_list = ["High","Moderate","Low"]


# Initialize GPIO

def myCommandCallback(cmd):
    print("Command received: %s" % cmd.data['command'])
    print(cmd)
        


try:
	deviceOptions = {"org": organization, "type": deviceType, "id": deviceId, "auth-method": authMethod, "auth-token": authToken}
	deviceCli = ibmiotf.device.Client(deviceOptions)
	#..............................................
	
except Exception as e:
	print("Caught exception connecting device: %s" % str(e))
	sys.exit()

# Connect and send a datapoint "hello" with value "world" into the cloud as an event of type "greeting" 10 times
deviceCli.connect()

while True:
        Message = r.choice(Message_list)
        Traffic = r.choice(Traffic_list)
        if Traffic == 'High':
            Message = "Go Slow!"
        #Get Sensor Data from DHT11
        temp=r.randint(0,100)
        humidity=r.randint(0,100)
        
        data = {"d":{ 'temp' : temp, 'humidity' : humidity, 'message' : Message, 'traffic' : Traffic}}
        #print data
        def myOnPublishCallback():
            print ("Published Temperature = %s C" % temp, "Humidity = %s %%" % humidity, "to IBM Watson")

        success = deviceCli.publishEvent("IoTSensor", "json", data, qos=0, on_publish=myOnPublishCallback)
        if not success:
            print("Not connected to IoTF")
        time.sleep(5)
        
        #deviceCli.commandCallback = myCommandCallback

# Disconnect the device and application from the cloud
deviceCli.disconnect()

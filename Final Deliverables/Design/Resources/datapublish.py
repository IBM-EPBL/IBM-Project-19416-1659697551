import time
import sys
import ibmiotf.application
import ibmiotf.device
import random as r
import weather


#Provide your IBM Watson Device Credentials
organization = "m1r2sh"
deviceType = "Roadsafety"
deviceId = "safetydevice2"
authMethod = "token"
authToken = "t3USLaRSVT*BbaPGlA"

#Data List
Message_list = ["Crossing","School Zone","Hospital Zone","No Entry","One Way","Take Diversion"]
Traffic_list = ["High","Moderate","Low"]
Notify_list = ["Heavy Traffic","Heavy Rain","Bad Weather","Road Work Ahead","Accident Zone"]

myLocation = "Chennai,IN"
APIKEY = "3833389c301e845d271b287e18bfba2f"


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
        #Random Data
        Message = r.choice(Message_list)
        Traffic = r.choice(Traffic_list)
        Alert = r.choice(Notify_list)

        #Get Weather from OpenWeatherMap
        weatherData = weather.get(myLocation, APIKEY)
        
        if Traffic == 'High':
            Message = "Go Slow!"
            Alert = "Heavy Traffic"
        elif weatherData["weather"] == "['rain']":
            Alert = "Heavy Rain"

        #JSON Data
        data = {"d":{ 'temp' : round(weatherData["temperature"], 2),
                      'humidity' : weatherData["humidity"],
                      'visibility' : weatherData["visibility"],
                      'location' : myLocation,
                      'message' : Message,
                      'traffic' : Traffic,
                      'alert' : Alert}
                }
        #print data
        def myOnPublishCallback(): 
            print ("Published Temperature = %s C" % round(weatherData["temperature"], 2), "Humidity = %s %%" % weatherData["humidity"], "to IBM Watson")

        success = deviceCli.publishEvent("IoTSensor", "json", data, qos=0, on_publish=myOnPublishCallback)
        if not success:
            print("Not connected to IoTF")
        time.sleep(5)

        #CallBack
        deviceCli.commandCallback = myCommandCallback

# Disconnect the device and application from the cloud
deviceCli.disconnect()

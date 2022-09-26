import random as r

temp = r.randrange(1,100)#38c is high
humi = r.randrange(1,100)#below 50 is high

print("Temperature:",temp)
print("Humidity:",humi)
if(temp > 38 and humi < 50):
    print("Temperature is high")
    print("Alarm system is ON!")
elif(temp < 38 and humi > 50):
    print("Temperature is low")
    print("Alarm system is OFF!")
else:
    print("Temperature cannot be determined")

import requests as reqs

def get(myLocation,APIKEY):
    apiURL = f"https://api.openweathermap.org/data/2.5/weather?q={myLocation}&appid={APIKEY}"
    responseJSON = (reqs.get(apiURL)).json()

    #JSON Object
    returnObject = {
        "temperature" : responseJSON['main']['temp'] - 273.15,
        "humidity" : responseJSON['main']['humidity'],
        "weather" : [responseJSON['weather'][_]['main'].lower() for _ in range(len(responseJSON['weather']))],
        "visibility" : responseJSON['visibility']/100
    }
    return(returnObject)
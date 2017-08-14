import threading
import os
import time
import serial
import httplib
import math

#GPS data
os.chdir('/home/pi/Desktop')

#Distance function
def Distance(lat1, long1,lat2,long2):

    degree_to_rad = float(math.pi / 180.0)

    d_lat = (lat2 - lat1) * degree_to_rad
    d_long = (long2 - long1) * degree_to_rad

    a = pow(math.sin(d_lat / 2), 2) + math.cos(lat1 * degree_to_rad) * math.cos(lat2 * degree_to_rad) * pow(math.sin(d_long / 2), 2)
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
    km = 6367 * c
    mi = 3956 * c

    return km

#global variables
global ser
global recieveBuffer
global recieveFlag
global webAddress
global data

#Serial Communication Settings
baud = 9600;
#port = 'Com4'
port ='/dev/ttyAMA0'

#default WebAddress and Data
webAddress = "gpsmaster.eu5.org"
webData = "/post.php?"

#Lat and Lng Coordinates, Initialied at Gate
lat=31.470997
lng=74.411116
lumsLat=31.470997
lumsLng=74.411116


ser = serial.Serial(port, baud)
if not ser.isOpen():
    ser.open()


recieveBuffer="NULL"
recieveFlag=0

def shutDown():
    os.system("sudo shutdown -h now")

def readSerial():
    global recieveBuffer
    global recieveFlag
    global ser
    while True:
        recieveBuffer = ser.readline()
        recieveFlag=1

# Function to Post Data, Return Y for success, N for failure
def httpGet(webAddress,webData):
    try:
        #print(webAddress)
        #print(webData)
        conn = httplib.HTTPConnection(webAddress)
        conn.request("GET",webData)
        res = conn.getresponse()
        #print res.status, res.reason
        conn.close()
    except:
        print("Exceptoion!!")
        return ['N','Failed']
    else:
        return ['Y',res]


def replayMaster(ser,recieved):
    global lat
    global lng
    recieved=recieved.replace("\n", "")
    recieved=recieved.replace("\r", "")
    tokenized = recieved.split(',');
    command = tokenized[0]
    if command=='AT':
        ser.write('<OK>')
    elif command=='POST':
        if lat!=0 and lng != 0: # and Distance(lumsLat,lumsLng,lat,lng)<50
            lat = float(tokenized[1])
            lng = float(tokenized[2])
            ans = httpGet(webAddress,"%slat=%f&lng=%f" % (webData, lat,lng))
            with open("logFile.txt","a") as fileStream:
                fileStream.write("%s,%f,%f\r\n" % (ans[0],lat,lng))
            fileStream.close()
            if ans[0]=='N':
                ser.write('<ERROR>')
            else:
                ser.write('<'+`ans[1].status`+'>')
        else:
            ser.write('<Invalid Coordinates>')
            print ("Invalid Coordinates")
    elif command=='CLOSE':
        ser.close()
    elif command=='HALT':
        ser.write('<Shutting Down>');
        shutDown()
    else:
        ser.write('<Unknown Command>')
        

serialReadThread = threading.Thread(target=readSerial)
serialReadThread.start()


ser.write('<OK>')
while True:
    #p#rint Distance(31.470997,74.411116,31.469723,74.512244)
    if recieveFlag:
        replayMaster(ser,recieveBuffer)
        print(recieveBuffer)
        recieveFlag=0
    print("waiting: |%f|,|%f|"%(lat,lng))
    time.sleep(0.4)

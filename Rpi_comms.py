import serial_communication_checksum
import auth_client
import sys
import learning
import numpy
import socket

class Rpi_comms:

    def __init__(self, ip_addr, port_num):
        ready = False
        #init serial comms
        self.Scomms = serial_communication_checksum.serial_communication()
        #init wireless comms
        self.Ml = learning.learning()
        model = self.Ml.machineTrain()
        data = numpy.empty((60, 12))
        count = 0
       
        self.Wcomms = auth_client.client(ip_addr, port_num)
        print('init Wcomms')
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (ip_addr, port_num)
        print(server_address)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(server_address)

        while ready == False:
            print('no handshake')
            ready = self.Scomms.handshake()
            
        #receivedData = self.Scomms.receiveData();
        while ready:
            try:
#                 receivedData = 'wavehands|7|7|7|7|'
#                 receivedData = [-15752,-1840,6484,571,1790,-97,-17156,-408,3104,4318,-1108,-2198]

#                 print(receivedData)
                receivedData = self.Scomms.receiveData()
                sensorData = receivedData.split('|')[0]
                current = receivedData.split('|')[1]
                voltage = receivedData.split('|')[2]
                power = receivedData.split('|')[3]
                cumpower = receivedData.split('|')[4]
                receivedData1 = "1,2,3,4,5,6,7,8,9,10,11,12"
                data[count] = [int(x) for x in sensorData.split(',')]
                count = count + 1
                if (count == 60) :
                    move = self.Ml.processData(data, model)
                    print(move)
                    count = 0
#                 self.Wcomms.sendData(receivedData)
                #receivedData1 = 'wavehands|7|7|7|7|'
                print(receivedData)
                msg = self.Wcomms.packData(move, current, voltage, power, cumpower)
                sock.sendall(msg)
            except Exception as e:
                print(e)
                
if len(sys.argv) != 3:
    print('Invalid number of arguments')
    print('python server.py [IP address] [Port]')
    sys.exit()
 
ip_addr = sys.argv[1]
port_num = int(sys.argv[2])
 
my_comms = Rpi_comms(ip_addr, port_num)

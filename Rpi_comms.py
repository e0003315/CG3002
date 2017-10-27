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
        self.Wcomms = auth_client.client(ip_addr, port_num)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (ip_addr, port_num)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(server_address)
       
        while ready == False:
            #print('no handshake')
            ready = self.Scomms.handshake()

        self.Ml = learning.learning()
        model = self.Ml.machineTrain()
        data = numpy.empty((60, 12))
        count = 0
        moveConcluded = [5]
        consecutiveCount = 0
        while ready:
            try:
                receivedData = self.Scomms.receiveData()
                sensorData = receivedData.split('|')[0]
                current = receivedData.split('|')[1]
                voltage = receivedData.split('|')[2]
                power = receivedData.split('|')[3]
                cumpower = receivedData.split('|')[4]
                print([int(x) for x in sensorData.split(',')])
                data[count] = [int(x) for x in sensorData.split(',')]
                count = count + 1
                if (count == 60) :
                    count = 0
                    consecutiveCount = (consecutiveCount + 1) % 5
                    move = self.Ml.processData(data, model)
                    moveConcluded[consecutiveCount] = move
                    # print(move)
                    if (x == moveConcluded[0] for x in moveConluded)	
                    	msg = self.Wcomms.packData(str(move), current, voltage, power, cumpower)
                    # print(msg)
                    	sock.sendall(msg)
                    # print('message sent')
                #print(receivedData)

            except Exception as e:
                print(e)
                
if len(sys.argv) != 3:
    print('Invalid number of arguments')
    print('python server.py [IP address] [Port]')
    sys.exit()
 
ip_addr = sys.argv[1]
port_num = int(sys.argv[2])
 
my_comms = Rpi_comms(ip_addr, port_num)

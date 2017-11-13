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
       
        file = 'takingReadings.csv'
        csv = open(file, "w")
        #while ready == False:
            #print('no handshake')
            #ready = self.Scomms.handshake()
        print("Entering training")
        self.Ml = learning.learning()
        model = self.Ml.machineTrain()
        data = numpy.zeros((30, 36))
        count = 0
        moveConcluded = [[],[],[]]
        consecutiveCount = 0
        finalflag =0 
        print("Entering handshake")
        while ready == False:
            ready = self.Scomms.handshake()

        print("Entering data receiving mode")
        while ready:
            try:
                receivedData = self.Scomms.receiveData()
                sensorData = receivedData.split('|')[0]
                csv.write(sensorData + '\n')
                current = receivedData.split('|')[1]
                voltage = receivedData.split('|')[2]
                power = receivedData.split('|')[3]
                cumpower = receivedData.split('|')[4]
               #print([int(x) for x in sensorData.split(',')])
                #data[count, 24:36] = [int(x) for x in sensorData.split(',')]
                if (count >= 0):
                    data[count%30, (count//30) * 12 : (count//30)*12 + 12] = [int(x) for x in sensorData.split(',')]
                count = count + 1
                if (count == 120) :
                    count = 0
                    move = self.Ml.processData(data, model)
                    moveConcluded[consecutiveCount] = move
                    consecutiveCount = (consecutiveCount + 1) % 3
                    #data[:,:12] = data[:,12:24]
                    #data[:,12:24] = data[:,24:36]
                    if (move == ["final"]):
                        finalflag = 1
                    print(move)
                    print(sensorData)
                    if (all((x != ["nomove"] and x == moveConcluded[0]) for x in moveConcluded)) :
                        count = -90
                        msg = self.Wcomms.packData(str(move), voltage, current, power, cumpower)
                        #print(msg)
                        sock.sendall(msg)
                        if (finalflag):
                            break
                        moveConcluded = [[],[],[]]
                    	# print('message sent')

            except Exception as e:
                print(e)
                
if len(sys.argv) != 3:
    print('Invalid number of arguments')
    print('python server.py [IP address] [Port]')
    sys.exit()
 
ip_addr = sys.argv[1]
port_num = int(sys.argv[2])
 
my_comms = Rpi_comms(ip_addr, port_num)

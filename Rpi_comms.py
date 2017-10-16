#import serial_communication
import auth_client
import sys
import learning
import numpy

class Rpi_comms:

    def __init__(self, ip_addr, port_num):

        #init serial comms
        #self.Scomms = serial_communication.serial_communications()
        #init wireless comms
#         self.Wcomms = auth_client.client(ip_addr, port_num)
        self.Ml = learning.learning()
        model = self.Ml.machineTrain()
        data = numpy.empty((80, 12))
        count = 0
       
        #receivedData = self.Scomms.receiveData();
        while True:
            try:
#                 receivedData = 'wavehands|7|7|7|7|'
                receivedData = [-15752,-1840,6484,571,1790,-97,-17156,-408,3104,4318,-1108,-2198]

#                 print(receivedData)
                data[count] = receivedData
                count = count + 1
                if (count == 80) :
                    move = self.Ml.processData(data, model)
                    print(move)
                    count = 0
#                 self.Wcomms.sendData(receivedData)
            except Exception as e:
                print(e)
                
if len(sys.argv) != 3:
    print('Invalid number of arguments')
    print('python server.py [IP address] [Port]')
    sys.exit()
 
ip_addr = sys.argv[1]
port_num = int(sys.argv[2])
 
my_comms = Rpi_comms(ip_addr, port_num)

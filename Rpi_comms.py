#import serial_communication
import auth_client
import sys

class Rpi_comms:

    def __init__(self, ip_addr, port_num):

        #init serial comms
        #self.Scomms = serial_communication.serial_communications()
        #init wireless comms
        self.Wcomms = auth_client.client(ip_addr, port_num)
       
        #receivedData = self.Scomms.receiveData();
        while True:
            try:
                receivedData = 'wavehands|7|7|7|7|'
                print(receivedData)
                self.Wcomms.sendData(receivedData)
            except Exception as e:
                print(e)
                
if len(sys.argv) != 3:
    print('Invalid number of arguments')
    print('python server.py [IP address] [Port]')
    sys.exit()

ip_addr = sys.argv[1]
port_num = int(sys.argv[2])

my_comms = Rpi_comms(ip_addr, port_num)

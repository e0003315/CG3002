import serial_communication_checksum
import auth_client
import sys
import socket

class Rpi_comms:

    def __init__(self, ip_addr, port_num):
        ready = False
        #init serial comms
        self.Scomms = serial_communication_checksum.serial_communication()
        #init wireless comms
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
                receivedData = self.Scomms.receiveData()
                receivedData1 = input('enter plaintext')
                #receivedData1 = 'wavehands|7|7|7|7|'
                print(receivedData)
                msg = self.Wcomms.packData(receivedData1)
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

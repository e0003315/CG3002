import serial
import time

flag = 0

ser = serial.Serial('/dev/ttyAMA0', 115200)

class serial_communication:
    def __init__(self):
        ready = self.handshake()
        while ready == False:
            print('no handshake')
            ready = self.handshake()
        while ready:
            global rcv
            data = self.readlineCR()
            if (data == '1'):
                print(rcv)
            if (data == '0'):
                print(rcv)
                
   
    def handshake(self):
        ser.write('0'.encode())
        ack1 = ser.read().decode('ascii')
        if ack1 == '1':
            ser.write('1'.encode())
            print('handshake pass')
            return True
        else:
            print("\r\n\Handshake not completed!")
            return False
        

    def readlineCR(self):
        global rcv
        rcv = ""
        checksum = 0
        while True:
            ch = ser.read().decode('ascii')
            if(ch == 'd'):
                return '0'
            if(ch == 's'):
                return '1'
            rcv += ch
    
my_serial_communication = serial_communication()

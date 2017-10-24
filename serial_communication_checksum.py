import serial
import time

ser = serial.Serial('/dev/ttyAMA0', 115200)

class serial_communication:
    def __init__(self):
     #   counter = 0
        global ready
     #   while ready == False:
     #       print('no handshake')
     #       ready = self.handshake()
     #   while ready:
     #       global rcv
     #       global los
     #       data = self.readlineCR()
     #       if (data == '0'):
     #           print(rcv)
     #           los = len(rcv)
     #           cs = self.checksum()
     #       if (data == '1'):
     #           print(rcv)
     #           if(los == int(rcv)):
     #               print('size is correct')
     #           else :
     #               print ('size is wrong')
     #           los = ""
     #       if (data == '2'):
     #           if(cs == int(rcv)):
     #               print('checksum is correct')
     #           else:
     #               print('checksum is wrong')
                
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
        

    def checksum(self):
        ch = '0'
        global rcv
        for i in range (0, len(rcv)):
            ch = chr(ord(ch) ^ ord(rcv[i]))
        print(ch) 
        return ch



    def readlineCR(self):
        global rcv
        rcv = ""
        while True:
            ch = ser.read().decode('ascii')
            if(ch == 'd' and len(rcv)!=0):
                return '0'
            if(ch == 's' and len(rcv)!=0):
                return '1'
            if (ch == 'c' and len(rcv)!=0):
                return '2'
            rcv += ch

    def receiveData(self):
        global rcv
        global los
        global readings
        while True:
            data = self.readlineCR()
            if (data == '0'):
                print(rcv)
                readings = rcv
                los = len(rcv)
                cs = self.checksum()
                flag = True
            if (data == '1'):
                print(rcv)
                if(los == int(rcv)): 
                    print('size is correct')
                else :
                    print ('size is wrong')
                    flag = False
                    los = ""
            if (data == '2'):
                #print(rcv)
                #print('calculated checksum is', ord(cs), 'received checksum is', ord(rcv))
                if(ord(cs) == int(rcv)):
                    print('checksum is correct')
                else:
                    print('checksum is wrong')
                    flag = False
                if(flag == True):
                    ser.write('1'.encode()); #ACK
                    print('ack')
                    return readings
                else:
                    ser.write('2'.encode()); #NACK
                    print('nack')
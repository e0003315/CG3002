from Crypto.Cipher import AES
from Crypto import Random
import socket
import sys
import hashlib
import base64

class client:
    PADDING_BYTE = " "
    def __init__(self, ip_addr, port_num):
        
        #Create a TCP/IP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (ip_addr, port_num)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(server_address)
        while True:
            try:
                plainText = input("Enter plain text: ")
                encodedMsg = self.encryptText(plainText)
                print('%s' %encodedMsg)
                sock.sendall(encodedMsg)
                print('send')
            except Exception as e:
                print(e)

    def encryptText(self, plainText):
        iv = Random.new().read(AES.block_size)
        key = "DDGrp2";
        secret_key = hashlib.sha256(key.encode()).digest()
        cipher = AES.new(secret_key, AES.MODE_CBC, iv)
        plainText = self.padding('#' + plainText)   
        encryptedText = cipher.encrypt(plainText)
        cipherText = base64.b64encode(iv + encryptedText)
        return cipherText;

    def padding(self, text):
        length = len(text)
        paddedText = text + (32-length%32)*self.PADDING_BYTE
        print('%s' %paddedText)
        return paddedText


if len(sys.argv) != 3:
    print('Invalid number of arguments')
    print('python server.py [IP address] [Port]')
    sys.exit()
    
ip_addr = sys.argv[1]
port_num = int(sys.argv[2])

my_client = client(ip_addr, port_num)

from Crypto.Cipher import AES
from Crypto import Random
import hashlib
import base64

class client:
    PADDING_BYTE = " "
    def __init__(self, ip_addr, port_num):
        global server_address  
        
    def encryptText(self, plainText):
        iv = Random.new().read(AES.block_size)
        key = "1234567890123456";
        secret_key = hashlib.sha256(key.encode()).digest()
        cipher = AES.new(key, AES.MODE_CBC, iv)
        plainText = self.padding('#' + plainText)   
        encryptedText = cipher.encrypt(plainText)
        cipherText = base64.b64encode(iv + encryptedText)
        return cipherText;

    def padding(self, text):
        length = len(text)
        paddedText = text + (32-length%32)*self.PADDING_BYTE
        print('%s' %paddedText)
        return paddedText

    def packData(self, action, current, voltage, power, cumpower):
        encodedMsg = self.encryptText(action +'|' + current + '|' + voltage + '|' + power + '|' + cumpower + '|')
        print('%s' %encodedMsg)
        return encodedMsg

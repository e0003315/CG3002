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
        print(plainText)
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

    def packData(self, action, voltage, current, power, cumpower):
        action1 = action[2:len(action)-2]
        encodedMsg = self.encryptText(action1 +'|' + voltage + 'V|' + current + 'mA|' + power + 'W|' + cumpower + 'W|')
        print('%s' %encodedMsg)
        return encodedMsg


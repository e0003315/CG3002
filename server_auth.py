from Crypto.Cipher import AES
from Crypto import Random
import base64
import sys
import os

class server_auth:
    PADDING_BYTE = " "
    def __init__(self):
        super(server_auth, self).__init__()

    def decryptText(self, cipherText, Key):
        decodedMSG = base64.b64decode(cipherText)
        iv = decodedMSG[:16]
        secret_key = Key; 
        cipher = AES.new(secret_key,AES.MODE_CBC,iv)
        decryptedText = cipher.decrypt(decodedMSG[16:]).strip()
        decryptedTextStr = decryptedText.decode('utf8')
        decryptedTextStr1 = decryptedTextStr[decryptedTextStr.find('#'):] 
        decryptedTextFinal = bytes(decryptedTextStr1[1:],'utf8').decode('utf8')
        action = decryptedTextFinal.split('|')[0]
        voltage = decryptedTextFinal.split('|')[1]
        current = decryptedTextFinal.split('|')[2]
        power = decryptedTextFinal.split('|')[3]
        cumpower = decryptedTextFinal.split('|')[4]
        return {'action': action, 'voltage': voltage, 'current': current, 'power': power, 'cumpower': cumpower}

    def encryptText(self, plainText):
        iv = Random.new().read(AES.block_size)
        secret_key = "1234567890123456";
        cipher = AES.new(secret_key, AES.MODE_CBC, iv)
        action = self.padding('#' + plainText.split('|')[0])
        voltage = self.padding(plainText.split('|')[1])
        current = self.padding(plainText.split('|')[2])
        power = self.padding(plainText.split('|')[3])
        cumpower = self.padding(plainText.split('|')[4])
        encryptedText = cipher.encrypt(action)+cipher.encrypt(voltage)+ cipher.encrypt(current)+cipher.encrypt(power)+cipher.encrypt(cumpower)
        cipherText = base64.b64encode(iv + encryptedText)
        return cipherText;

    def padding(self, text):
        length = len(text)
        paddedText = text + (15-length%15)*self.PADDING_BYTE + '|'
        print('%s' %paddedText)
        return paddedText




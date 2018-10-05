'''
Created on 28 Apr 2018

@author: danielg
'''
import sys
import serial
from serial.serialutil import SerialException
from win32con import DMPAPER_10X11

SYNCWORD = 0x55

class hciAgent():
    '''
    classdocs
    '''

    def __init__(self, Baudrate = 115200):
        self.hciComPort = None
        self.uartBaudrate = Baudrate
        self.initializeComPort()

    def initializeComPort(self):
        choiceOK = False
        while (choiceOK == False):
            try:        
                choice = input("please enter your HCI COM port number (q for quit)>> ")
                if (choice =='q'):
                    choiceOK = True
                elif (int(choice) < 0) or (int(choice) > 100):
                    print("invalid choise support port 0 till 100!")
                else:
                    try:
                        self.hciComPort = serial.Serial('COM'+choice, self.uartBaudrate, parity=serial.PARITY_NONE, rtscts = 0, timeout = 1, bytesize = serial.EIGHTBITS, stopbits = serial.STOPBITS_ONE)
                        choiceOK = True
                    except SerialException:
                        print("error couldn't open connection via", 'COM'+choice)
                        input("press any key to continue>>")
                    except Exception as e:
                        print("Received Exception:", e)
                        input("press any key to continue>>") 
            except:
                print("invalid choise!")
        #perform exit on q key
        if (choice == 'q'):
            sys.exit(1)
    
    def sendConfigshow(self, showID, maxPower, direction):
        HCIOpcode = 0x1
        HCILength = 0x3
        self.sendCommand([SYNCWORD, HCIOpcode, HCILength, showID, maxPower, direction]) 

    def sendStoreConfig(self, action):
        HCIOpcode = 0x2
        HCILength = 0x1
        self.sendCommand([SYNCWORD, HCIOpcode, HCILength, action])
        
    def sendCommand(self, cmd):
        #print(self.hciComPort)
        #print(cmd)
        #sizeWritten = self.hciComPort.write(cmd)
        #print("number of bytes written is", sizeWritten)
        #input("please enter any key to continue>>")
        self.hciComPort.write(cmd)
        
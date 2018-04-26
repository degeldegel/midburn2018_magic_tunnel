'''
Created on 26 Apr 2018

@author: DanielG
'''
import os
import sys
from PyQt5.QtWidgets import QApplication, QWidget, QMainWindow, QLabel
from PyQt5.QtGui import QPainter, QColor, QPen
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import Qt, QBasicTimer
import random
import numpy as npy

NUM_OF_SNAPSHOTS = 1200
NUM_OF_STRIPS = 20
NUM_OF_LEDS_IN_STRIP = 300
NUM_OF_RGB_BYTES = 3
GREEN = 0
RED = 1
BLUE = 2
 
class Led_emul(QMainWindow):
 
    def __init__(self):
        super().__init__()
        self.title = 'Led emulator - v0.1'
        self.left = 20
        self.top = 30
        self.width = 1840
        self.height = 1000
        # full led strip matrix with all snapshots as read from file 
        self.ledStripsMat = npy.zeros([NUM_OF_SNAPSHOTS, NUM_OF_STRIPS, NUM_OF_LEDS_IN_STRIP, NUM_OF_RGB_BYTES], dtype = int)
#         for snap_id in range(0, NUM_OF_SNAPSHOTS):
#             for strip_id in range(0, NUM_OF_STRIPS):
#                 for led_id in range(0, NUM_OF_LEDS_IN_STRIP):
#                     if (snap_id%3 == 0):
#                         self.ledStripsMat[snap_id][strip_id][led_id][0] = strip_id*10
#                     elif (snap_id%3 == 1):
#                         self.ledStripsMat[snap_id][strip_id][led_id][1] = strip_id*10
#                     else:
#                         self.ledStripsMat[snap_id][strip_id][led_id][2] = strip_id*10

        # snapshot times 
        self.snapshotTimes = npy.zeros([NUM_OF_SNAPSHOTS], dtype = int)
        #snapshot current index
        self.snapshotIdx = 0
        self.timer = QBasicTimer()
        self.currtime = 0;
        self.ledLayout = LedStripsWidget()
        self.initUI()
 
    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)
 
#        # Set window background color
#         self.setAutoFillBackground(True)
#         p = self.palette()
#         p.setColor(self.backgroundRole(), Qt.white)
#         self.setPalette(p)
 
        # Add Ledlayout widget and paint
        self.setCentralWidget(self.ledLayout)
        self.updateLedMatFromFile()
        print("finished LED update!")
#         self.ledLayout.updateLedStripMat(self.newLedStripMat)
 
        self.show()
        self.timer.start(1, self)
        
    def timerEvent(self, event):
        '''handles timer event'''
#        if event.timerId() == self.timer.timerId():
        self.currtime = self.currtime + 1
        if self.currtime == self.snapshotTimes[self.snapshotIdx]:
            print("Timer event:: snapshot idx is", self.snapshotIdx, "time is", self.currtime)
            self.ledLayout.updateLedStripMat(self.ledStripsMat[self.snapshotIdx])
            self.ledLayout.paintEvent(event)
            self.update()
            self.snapshotIdx = self.snapshotIdx + 1
            if (self.snapshotIdx == NUM_OF_SNAPSHOTS):
                self.timer.stop()
                self.snapshotIdx = 0
#        else:
#            super(Board, self).timerEvent(event) 
                
    def updateLedMatFromFile(self):
        snapIdx = -1
        stripIdx = 0
        file_name = os.path.join("..","..","midburn2018_magic_tunnel_emul","LED_Emul_output.txt")
        fh = open(file_name)
        for line in fh:
            if line.startswith("TIME:"):
                snapIdx = snapIdx + 1
                if snapIdx >= NUM_OF_SNAPSHOTS:
                    print (line)
                    print("Error accurred too many snapshots in file... exiting!")
                    exit(1)
                stripIdx = 0
                self.snapshotTimes[snapIdx] = int(line[6:])
            else:
                if stripIdx > NUM_OF_STRIPS:
                    print("Error accurred too many strips in file... exiting!")
                    exit(1)                    
                ledStr = line.split(",")
                for ledIdx in range(0, NUM_OF_LEDS_IN_STRIP):
                    colors = ledStr[ledIdx].split(" ")
                    self.ledStripsMat[snapIdx][stripIdx][ledIdx][GREEN] = int(colors[0])
                    self.ledStripsMat[snapIdx][stripIdx][ledIdx][RED]   = int(colors[1])
                    self.ledStripsMat[snapIdx][stripIdx][ledIdx][BLUE]  = int(colors[2])
                stripIdx = stripIdx + 1

class LedStripsWidget(QWidget):
    def __init__(self):
        super(LedStripsWidget, self).__init__()
        self.ledStripsMat = npy.zeros([NUM_OF_STRIPS, NUM_OF_LEDS_IN_STRIP, NUM_OF_RGB_BYTES], dtype = int)
    
    def paintEvent(self, event):
        qp = QPainter(self)    
        qp.setPen(Qt.black)
        #size = self.size()
       
        # Color Effect
        for strip_id in range(0, NUM_OF_STRIPS):
            for led_id in range(0, NUM_OF_LEDS_IN_STRIP):
                qp.setBrush(QColor(self.ledStripsMat[strip_id][led_id][0], self.ledStripsMat[strip_id][led_id][1], self.ledStripsMat[strip_id][led_id][2]))
                qp.drawRect(6*led_id, 10*strip_id, 6, 6)

    def updateLedStripMat(self, newLedStripsMat):
        self.ledStripsMat = npy.copy(newLedStripsMat)
    
if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = Led_emul()
    sys.exit(app.exec_())        
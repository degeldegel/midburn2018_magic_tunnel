'''
Created on 27 Apr 2018

@author: danielg
'''

#import sys
import os
#from project_hci_controller 
from hci_agent import hciAgent
#from sqlalchemy.sql.expression import false

# =======================
#     MENUS FUNCTIONS
# =======================

class ConUI():
    def __init__(self):
        self.uartHciAgent = hciAgent()
        # Menu definition
        self.menu_actions = {
            'main_menu': self.main_menu,
            '1': self.configShowMenu,
            '2': self.configSnakeShowMenu,
            'q': self.exit
        }
        
 
    # Main menu function - main loop of consule UI
    def main_menu(self):
        dontQuit = True 
        while (dontQuit == True):
            #os.system('clear')
            print("Welcome To the project HCI controller UI\n")
            print("Please choose the menu you want to start:")
            print("1. Configure show P&D")
            print("2. Configure snake show")
            print("3. save configuration to flash")
            print("q. Quit")
            choiceOK = False
            while (choiceOK == False):
                try:        
                    choice = input("please enter your choice>> ")
                    if (choice =='q'):
                        dontQuit = False
                        choiceOK = True
                    elif (int(choice) > 0) and (int(choice) < 4):
                        choiceOK = True
                    else:
                        print("invalid choise!")
                        
                except:
                    print("invalid choise!")
            self.exec_menu(choice)
        return
    
    # Execute menu
    def exec_menu(self, choice):
        #os.system('clear')
        ch = choice.lower()
        try:
            self.menu_actions[ch]()
        except Exception as e:
            print ("Your choise is not supported yet!\n")
            print ("exception was:", e)
            input("please enter any key to continue")
    
    # configure show HCI command IF
    def configShowMenu(self):
        print("1. Configure Power and direction")
        print("2. Configure Power only")
        print("b. Back")
        choiceOK = False
        while (choiceOK == False):
            try:        
                choice = input("please enter your choice>> ")
                if (choice =='b'):
                    choiceOK = True
                elif (int(choice) == 1):
                    choiceOK = True
                    self.configShow(False)
                elif (int(choice) == 2):
                    choiceOK = True
                    self.configShow(True)                    
                else:
                    print("invalid choise!")
            except:
                print("invalid choise!!")

    def configShow(self, powerOnly):
        print("Configure show HCI")
        print("==================\n")
        print("list of available shows:")
        print("------------------------")
        print("0. snake show")
        print("1. reserved show1")
        print("2. reserved show2")
        print("3. reserved show3")
        print("4. reserved show4")
        print("5. reserved show5")
        print("6. reserved show6")
        print("7. reserved show7")
        print("8. reserved show8")
        print("9. reserved show9")
        print("b. Back")
        choiceOK = False
        while (choiceOK == False):
            try:        
                showId = input("please enter your choice>> ")
                if (showId =='b'):
                    return
                elif (int(showId) < 0) or (int(showId) >9):
                    print("invalid choise!")
                else:
                    choiceOK = True
            except:
                print("invalid choise!")
        print()
        dontQuit = True
        while (dontQuit == True):
            choiceOK = False
            while (choiceOK == False):
                try:        
                    maxPower = input("please enter max power (0-100), press b for back>> ")
                    if (maxPower =='b'):
                        return
                    elif (int(maxPower) < 0) or (int(maxPower) > 100):
                        print("invalid choise!")
                    else:
                        choiceOK = True
                except:
                    print("invalid choise!")
            if (powerOnly == False):
                print("\nplease choose direction of show")
                print("0. Regular direction")
                print("1. reversed direction")
                print("2. don't change direction")
                print("b. Back")
                choiceOK = False
                while (choiceOK == False):
                    try:        
                        direction = input("please enter you choice>> ")
                        if (direction =='b'):
                            return
                        elif (int(direction) < 0) or (int(direction) > 2):
                            print("invalid choise!")
                        else:
                            choiceOK = True
                    except:
                        print("invalid choise!")
            else:
                direction = 2 #Dont change
            self.uartHciAgent.sendConfigshow(int(showId), int(maxPower), int(direction))
            choiceOK = False
            while (choiceOK == False):
                try:        
                    choice = input("configure again? [y/n]>> ")
                    if ((choice =='y') or (choice =='n')):
                        if (choice == 'n'):
                            dontQuit = False
                        choiceOK = True
                    else:
                        print("invalid choise!")
                except:
                    print("invalid choise!")
        return


    # Menu 2
    def configSnakeShowMenu(self):
        print ("Hello Menu 2 !\n")
        print ("9. Back")
        print ("0. Quit") 
        choice = input(" >>  ")
        self.exec_menu(choice)
        return
    
    def exit(self):
        return

# =======================
#      MAIN PROGRAM
# =======================

# Main Program
if __name__ == "__main__":
    consule = ConUI()
    consule.main_menu()
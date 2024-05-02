from tkinter import * #GUI library
from tkinter import messagebox
#import tkSimpleDialog
import random

# player properties
class player:
    def __init__(self, name, score):
        self.name=name
        self.score=score
# return player score for sorting
def player_score(self):
    return self.score
# game part
main_window = Tk()
main_window.title("Guess Size")
#main_window.geometry("900*500")
main_window.configure(background = "black")

# label
Label(main_window, text="Hello! Please choose game level!").pack()
#Label(main_window, text="Enter your guess:").grid(row=1, column=0)
        
# button click function (first level)
def board(): #ranking list
        third=Tk()
        third.title("Board")
        player1=player(str(player_name.get()), random.randint(0, 1000))
        player2=player(random.choice(('Joe','John','Jack')), random.randint(0, 1000))
        player3=player(random.choice(('Joe','John','Jack')), random.randint(0, 1000))
        player4=player(random.choice(('Joe','John','Jack')), random.randint(0, 1000))    
        playerlist=[player1, player2, player3, player4]
        sorted_playerlist=sorted(playerlist, key=player_score)
        n=4
        m=0
        for i in reversed(range(n)):
            Label(third, text=sorted_playerlist[i].name).grid(row=m, column=0)
            Label(third, text=sorted_playerlist[i].score).grid(row=m, column=1)
            m+=1  
def relax():
    second = Tk()
    main_window.destroy()
    second.title('Gameplay')
    Label(second, text="Please enter your name: ").grid(row=0, column=0)
    Label(second, text="Please enter your guess: ").grid(row=1, column=0)
    #text input; entry class data
    global player_name 
    player_name = Entry(second, width=50, borderwidth=5)
    player_name.grid(row=0, column=1) 
    answer = Entry(second, width=50, borderwidth=5)
    answer.grid(row=1, column=1)
    def relax_function():
        num = random.randint(0, 100)
        if int(answer.get())>num:
            output="Your name is: %s" %(str(player_name.get()))
            output += "Your answer: %d." %(int(answer.get()))
            output += "The answer: %d. The answer is smaller than your answer" %(num)
        elif num == int(answer.get()):
            output="Your name is: %s" %(str(player_name.get()))
            output += "Your answer: %d." %(int(answer.get()))
            output += "The answer: %d. You got it" %(num) 
        else:
            output="Your name is: %s" %(str(player_name.get()))
            output += "Your answer: %d." %(int(answer.get()))
            output += "The answer: %d. The answer is larger than your answer" %(num)
        messagebox.showinfo("Result", output)
    btn3 = Button(second, text="Next", command=relax_function).grid(row=2, column=0) 
    btn5 = Button(second, text="board", command=board).grid(row=3, column=0)
def hardcore():
    second = Tk()
    main_window.destroy()
    second.title('Gameplay')
    Label(second, text="Please enter your name: ").grid(row=0, column=0)
    Label(second, text="Please enter your guess: ").grid(row=1, column=0)
    #text input; entry class data
    global player_name
    player_name = Entry(second, width=50, borderwidth=5)
    player_name.grid(row=0, column=1) 
    answer = Entry(second, width=50, borderwidth=5)
    answer.grid(row=1, column=1)
    def hardcore_function():
        num = random.randint(0, 100)
        if int(answer.get())== num:
            output="Your name is: %s" %(str(player_name.get()))
            output += "Your answer: %d." %(int(answer.get()))
            output += "The answer: %d. You got it" %(num)
        else:
            output="Your name is: %s" %(str(player_name.get()))
            output += "Your answer: %d." %(int(answer.get()))
            output += "The answer: %d. GG" %(num)
        messagebox.showinfo("Result", output)
    btn4 = Button(second, text="Next", command=hardcore_function).grid(row=2, column=0)
    btn6 = Button(second, text="board", command=board).grid(row=3, column=0)
       
    #btn4 = Button(second, text="Next", command=board).pack()
# build buttons
#btn1 = Button(main_window, text="Next", command=click_next)
btn1 = Button(main_window, text="Relax", command=relax).pack()
btn2 = Button(main_window, text="Hardcore", command=hardcore).pack()
# button position
#btn1.grid(row=2, column=1)
main_window.mainloop()



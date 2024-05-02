from tkinter import *
import random

# player variable properties, name and score
class player:
    def __init__(self, name, score):
        self.name = name #players' name
        self.score = score #players' score
        
# return player score for sorting
def player_score(self):
    return self.score

# define start button at the beginning, print out gamerule, show difficult level buttons
def start():
    mainframe.destroy()
    global frame1
    frame1 = Frame(mainwindow, bg="white")
    frame1.place(relx=0.5, rely=0.5, anchor='center')
    rule = ("Rule: Both Easy & Hard require exact answer to win.\n Easy will provide hint while Hard level will not!\n\
        Game end if the correct answer is provided or score equal zero\n\
        Please select the difficulty: ")
    Label(frame1, text=rule, bg="white", font=('Arial', 14)).grid(row=0, column=1, columnspan=3, rowspan=3)
    Button(frame1, text="Easy", command=lambda: game_play("easy"), font=('Arial', 14)).grid(row=3, column=1) # easy button
    Button(frame1, text="Hard", command=lambda: game_play("hard"), font=('Arial', 14)).grid(row=3, column=3) # hard button

# window before leaderboard
def nameinput(result):
    frame2.destroy()
    global frame3
    frame3 = Frame(mainwindow, bg="white")
    frame3.place(relx=0.5, rely=0.5, anchor='center')
    if result == 'win':
        Label(frame3, text='Congratulations!', font=('sans-serif', 16)).grid(row=0, column=1)
    else:
        Label(frame3, text='Failed!', font=('sans-serif', 16)).grid(row=0, column=1)
    Label(frame3, text="Please enter your name: ").grid(row=1, column=1)
    player_name=Entry(frame3, width=50, border=5)
    player_name.grid(row=2, column=1)
    Button(frame3, text="Next", command=lambda:board(str(player_name.get()), score)).grid(row=3, column=1) # go to leaderboard

mainwindow=Tk() # window of all frames
mainwindow.title("Guess Size")
mainwindow.geometry('700x700')
mainwindow.config(background="white")
result=random.randint(0,100) # randomly generate a number between 0 to 100 as correct answer 
score = 10000 # player score; change back to 10000 at the end!!!
#sentence bank
sentences = ("Welcome to ChatPPT!\n An AI guesses number size software\n based on CloseAI algorithm system.") # introduction

# player list. For now we use random generate as extra players
def board(player_name, score):
    frame3.destroy()
    finalframe = Frame(mainwindow, bg="white")
    finalframe.place(relx=0.5, rely=0.5, anchor='center')
    player1=player(player_name, score)
    player2=player('Elon Mask', random.randint(0, 10000)) # dummy variable
    player3=player('Bill Gates', random.randint(0, 10000)) # dummy variable
    player4=player('Steve Jobs', random.randint(0, 10000)) # dummy variable
    playerlist=[player1, player2, player3, player4] # player array for sorting
    sorted_playerlist=sorted(playerlist, key=player_score)
    # print out the leaderboard
    Label(finalframe, text="Leader Board", bg='white', font=('sans-serif', 12, 'bold', 'underline')).grid(row=0, column=2)
    Label(finalframe, text="Name", bg='white', font=('sans-serif', 12)).grid(row=1, column=1)
    Label(finalframe, text="Score", bg='white', font=('sans-serif', 12)).grid(row=1, column=3)
    n=4
    m=3 # line between title and results
    for i in reversed(range(n)):
        Label(finalframe, text=sorted_playerlist[i].name, font=('sans-serif', 10)).grid(row=m, column=1)
        Label(finalframe, text=sorted_playerlist[i].score, font=('sans-serif', 10)).grid(row=m, column=3)
        m+=1

#trigger game mechanics
def game_play(choice):
    frame1.destroy()
    global frame2
    frame2 = Frame(mainwindow, bg="white")
    frame2.place(relx=0.5, rely=0.5, anchor='center')
    global player_guess
    player_guess = Entry(frame2, width=50, borderwidth=5) # player guess enter
    player_guess.grid(row=0, column=1)
    Button(frame2, text="Guess", command=lambda:check_guess(choice)).grid(row=1, column=1) # check answer

def check_guess(choice):
    global score
    global result
    global player_guess
    guess = int(player_guess.get())
    # if the wrong answer is provided, total score -200, easy prints out hints, hard print out nothing; score = 0, gameover!
    if choice == "easy":
        if score>0:
            if guess == result:
                msg=f'Your guess is {guess} You won the Game \U0001f606!'
                nameinput('win') # win! go to leaderborad    
            elif guess > result:
                score -= 200
                msg=f'Your guess is {guess} Your answer is too big {int(score/200)} chance(s) left'    
            else:
                score -= 200
                msg=f'Your guess is {guess} Your answer is too small {int(score/200)} chance(s) left' 
        else:
            msg='You Lost!'
            nameinput('lost')# lost! go to leaderborad
    else:
        if score>0:
            if guess == result:
                msg=f'Your guess is {guess}   You Won \U0001f606'
                nameinput('win') # win! go to leaderborad
            else:
                score -= 200
                msg=f'Your guess is {guess}  Nice Try \U0001f643 {int(score/200)} chance(s) left'
        else:
            msg='You Lost!'
            nameinput('lost') # lost! go to leaderborad
    Label(frame2, text=msg, bg='white', font=('sans-serif', 12)).grid(row=2, column=1)
    player_guess.delete(0, END) # clear the entry enter
    
mainframe = Frame(mainwindow, bg="white")
mainframe.place(relx=0.5, rely=0.5, anchor='center') # this is how the frame is located in the middle of the window
Label(mainframe, text=sentences, bg="white", font=('Arial', 14)).pack()
Button(mainframe, text="Start", command=start, font=('Arial', 14)).pack() # start button at the beginning
mainwindow.mainloop()
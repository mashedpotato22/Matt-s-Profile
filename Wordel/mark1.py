import turtle
import word

#wordlist = ['which', 'their', 'would', 'there', 'could', "other", 'about', 'great', 'these', 'after', 'first', 'never', 'where', 'those', 'shall', 'being']
answer=word.get_word() # answer randomly chose from wordlist
guess_correct=False # to detect if correct answer is provided
num_guess=6  # number of chances for player to guess
y=225
t=turtle.Turtle()
wn=turtle.Screen()
# turtle animation for colour boxes
def show_square(x,y,col,t):
    t.penup()
    t.goto(x,y)
    t.pendown()
    t.fillcolor(col)
    t.begin_fill()
    # code for square
    for i in range(4):
        t.speed(0)
        t.forward(50)
        t.right(90)
    t.end_fill()
# turtle input
def input_guess(prompt, wn):
    guess=wn.textinput("Wordle", prompt)
    if guess == None: return "     " # if no input at all
    elif len(guess) != 5: return guess[0:5].lower() # if input word length is more than five
    else: return guess.lower() # currect input
# check the input and call show_square to print out the colour box
def check_guess(guess, answer, y, t):
    count=0 # characters' index for word
    x = -250 # back to first box x-coordinate
    for i in guess:
        if i==answer[count]: show_square(x,y,"green",t)
        elif i in answer: show_square(x,y,"yellow",t)
        else: show_square(x,y,"red",t)
        count+=1
        x+=75 # move to next box position
    t.penup()
    t.goto(x,y-25)
    t.write(guess, font=("Verdana", 15, "normal"))
# show rule
t.penup()
t.goto(-175, 300)
t.write("Game colour code: 6 chances to get it right!", font=("Verdana", 12, "normal"))
t.goto(-175, 280)
t.write("Red box: Correct letter, Correct position", font=("Verdana", 10, "normal"))
t.goto(-175, 260)
t.write("Yellow box: Correct letter, Wrong position", font=("Verdana", 10, "normal"))
t.goto(-175, 240)
t.write("Red box: Letter is not in the answer", font=("Verdana", 10, "normal"))
#main function
while num_guess>0 and not guess_correct: 
    guess_prompt = "Please enter your guess "+str(num_guess)+" chances left"
    guess=input_guess(guess_prompt, wn)
    check_guess(guess, answer, y, t)
    y-=75
    if guess == answer:
        t.penup()
        t.goto(-225,-250)
        t.write("You got it, Congratulations! ", font=("Verdana", 25, "normal"))
        wn.exitonclick()
        #turtle.done()
        break
    num_guess-=1
else:
    t.penup()
    t.goto(-215,-300)
    t.write("The answer is "+answer+" \nBetter luck next time!", font=("Verdana", 25, "normal"))
    wn.exitonclick()
#turtle.done()


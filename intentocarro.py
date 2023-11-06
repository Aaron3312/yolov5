from tkinter import *
import socket


ESP_IP = "192.168.168.20"


ESP_PORT = 8266

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

def key_pressed(c):
    print("Tecla presionada: ", c.char)
    s.send(c.char.encode())
def key_released(c):
    print("Tecla liberada: ", c.char)
    s.send(c.char.encode())


root = Tk()
root.title("Control de Carro")

frame = Frame(root)

lbl_title = Label(frame, text="Control de Carro ROBOT")
lbl_title.grid(row=0, column=0, pady=10, padx=10)

frame.bind ("<KeyPress>", key_pressed)
frame.bind ("<KeyRelease>", key_released)

s.connect((ESP_IP, ESP_PORT))

frame.pack()
frame.focus_set()

root.mainloop()

# TO RUN THIS SHITTY CODE USE THIS IN PYTHON TERMINAL, and the other in visual terminal dont care
 #exec(open("E:\ProgramProyects\intentoFotoReconocimientoSIMG\intentocarro.py").read())
 # i dont care bout the other one its a shit
 # + need to make the other code of the arm to be able to run in this terminal cause this its a shit
 
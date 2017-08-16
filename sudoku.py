from tkinter import *
from ctypes import windll
import subprocess

windll.shcore.SetProcessDpiAwareness(1) #高DPI対応

root = Tk()
root.geometry('1000x1000')
root.title('sudoku')

def init(master, row, column):
	master.grid_rowconfigure(row % 3, weight=1)
	master.grid_columnconfigure(column % 3, weight=1)

	f = Frame(master, width=50, height=50)
	b = Button(f, justify='center', relief=FLAT, fg='blue', textvariable=var_list[row][column])

	f.grid_rowconfigure(0, weight=1)
	f.grid_columnconfigure(0, weight=1)
	f.grid_propagate(False)

	f.grid(row=row % 3, column=column % 3, padx=2, pady=2)
	b.grid(sticky='nsew')
	return b

def init2(master, column, text):
	master.grid_rowconfigure(0, weight=1)
	master.grid_columnconfigure(column, weight=1)

	f = Frame(master, width=50, height=50)
	b = Button(f, text = text)

	f.grid_rowconfigure(0, weight=1)
	f.grid_columnconfigure(0, weight=1)
	f.grid_propagate(False)

	f.grid(row=0, column=column)
	b.grid(sticky='nsew')
	return b

def init3(master, row, column):
	master.grid_rowconfigure(row, weight=1)
	master.grid_columnconfigure(column, weight=1)

	f = Frame(master, bg='black', padx=2, pady=2)
	f.grid(row=row, column=column)

	return f

# フレームの生成
f0 = Frame(root, bg='black', padx=4, pady=4)
f1 = Frame(root)

button_list = [init2(f1, i, '%d' % (i + 1)) for i in range(9)]
button_list.append(init2(f1, 9, 'del'))
var_list = [[StringVar() for column in range(9)] for row in range(9)]
frame_list = [[init3(f0, row, column) for column in range(3)] for row in range(3)]
board = [[init(frame_list[row // 3][column // 3], row, column) for column in range(9)] for row in range(9)]

def make_command(i):
	def callback():
		if button_list[i]["relief"] == RAISED:
			for b in button_list:
				b["relief"] = RAISED
			button_list[i]["relief"] = SUNKEN
		else:
			button_list[i]["relief"] = RAISED
	return callback

for i in range(10):
	button_list[i]["command"] = make_command(i)

def make_command2(row, column):
	def callback():
		pressed_num = -1
		for i in range(10):
			if button_list[i]["relief"] == SUNKEN:
				pressed_num = (i + 1) % 10
				break

		if pressed_num == -1:
			return
		elif pressed_num == 0:
			error_message.set('')
			var_list[row][column].set("")
		else:
			error_message.set('')
			board[row][column]['fg'] = 'blue'
			var_list[row][column].set(str(pressed_num))
	return callback

for row in range(9):
	for column in range(9):
		board[row][column]["command"] = make_command2(row, column)

# フレームの配置
f0.pack(padx=100, pady=100)
f1.pack(anchor=CENTER)

def solve():
	s = ""
	for vl in var_list:
		s += ' '.join([v.get() if v.get() != '' else '0' for v in vl])
		s += '\n'

	result = subprocess.run(["../sudoku/sudoku_solver"], input=s.encode('utf-8'), stdout=subprocess.PIPE)
	answer = result.stdout.decode('utf-8')
	if answer == "Error!!!\n":
		error_message.set(answer)
		return
	answer = [i.split() for i in answer.split('\n')]
	for i in range(9):
		for j in range(9):
			if var_list[i][j].get() == '':
				board[i][j]['fg'] = 'red'
			var_list[i][j].set(answer[i][j])

f2 = Frame(root)
f2.pack(pady=20)

b1 = Button(f2, text = 'solve', command = solve)
b1.pack(padx=10, side=LEFT)

def clearBoard():
	error_message.set('')
	for i in range(9):
		for j in range(9):
			var_list[i][j].set('')

b2 = Button(f2, text = 'clear', command = clearBoard)
b2.pack(padx=10, side=LEFT)

error_message = StringVar()
l1 = Label(fg='red', textvariable=error_message)
l1.pack(pady=50)

root.mainloop()

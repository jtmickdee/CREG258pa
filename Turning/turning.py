#!/usr/bin/env python
#2.5V and .25 A

from Tkinter import *
import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setup(23, GPIO.OUT)
pwm = GPIO.PWM(23, 100)
pwm.start(5)

class App:
	def __init__(self, master):
		frame = Frame(master)
		frame.pack()
		scale = Scale(frame, from_=0, to=180, orient=HORIZONTAL, command=self.update)
		scale.grid(row = 0)

	def update(self, angle):
		duty = float(angle)/ 10.0 + 2.5
		pwm.ChangeDutyCycle(duty)

root = Tk()
root.wm_title('Servo Control')
app = App(root)
root.geometry("200x50+0+0")
root.mainloop()
GPIO.cleanup()

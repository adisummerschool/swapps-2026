import iio
from queue import Queue
from threading import Thread, Event
from pynput import keyboard
from pynput.keyboard import Controller
import math
import tkinter as tk
import time

TIME = 0.1

def create_movement_gui(queue : Queue):
        def update_square(canvas, square, on_time):
                intensity = (255 - int(on_time * 255))
                color = f'#{intensity:02x}{intensity:02x}ff'
                canvas.itemconfig(square, fill=color)

        def update_gui():
                mov = queue.get()
                update_square(canvas, squares['left'], mov['left'])
                update_square(canvas, squares['right'], mov['right'])
                update_square(canvas, squares['front'], mov['front'])
                update_square(canvas, squares['back'], mov['back'])

                root.after(int(TIME * 50), update_gui)

        root = tk.Tk()
        canvas = tk.Canvas(root, width=400, height=400)
        canvas.pack()

        square_size = 100
        squares = {
                'left' : canvas.create_rectangle(50, 150, 50 + square_size, 150 + square_size, fill='white'),
                'right' : canvas.create_rectangle(250, 150, 250 + square_size, 150 + square_size, fill='white'),
                'front' : canvas.create_rectangle(150, 50, 150 + square_size, 50 + square_size, fill='white'),
                'back' : canvas.create_rectangle(150, 150, 150 + square_size, 150 + square_size, fill='white')
        }
        update_gui()

        root.mainloop()


def get_data(device):

        axis_data = {
                'x' : {'-' : int, '+' : int},
                'y' : {'-' : int, '+' : int},
                'z' : {'-' : int, '+' : int}
        }

        axes = ['x', 'y', 'z']

        channel_names = [f'voltage{i}' for i in range(6)]
        for i, channel_name in enumerate(channel_names):
                ch = device.find_channel(channel_name)
                if ch == None:
                        raise ValueError("colud not find channel")
                attr = ch.attrs['raw'].value

                polarity = '+' if i % 2 == 0 else '-'
                axis_data[axes[i // 2]][polarity] = int(attr)

        return axis_data

def get_roll_pitch(axis_data : dict):
        x = axis_data['x']['+'] - axis_data['x']['-']
        y = axis_data['y']['+'] - axis_data['y']['-']
        z = axis_data['z']['+'] - axis_data['z']['-']

        roll = math.atan2(y, z) * 180 / math.pi
        pitch = math.atan2(-x, math.sqrt(y ** 2 + z ** 2)) * 180 / math.pi

        return roll, pitch

def get_movement(rol, pitch):
        movement = {
                        'left' : 0,
                        'right' : 0,
                        'front' : 0,
                        'back' : 0
                }       

        #sa contina valori de la 0 la 1
        #valorile sa fie scalate intre 0 si 1024,
        #  tot ce e in afara va lua valorile maxime

        pitch = pitch / 45
        rol = rol / 45

        if pitch >= 0.2:
                if pitch > 45:
                        pitch = 45
                val_data = pitch * 1 / 45
                movement['left'] = max(min(1, pitch), 0.2)
        elif pitch <= -0.2:
                if pitch < -45:
                        pitch = -45
                val_data = - pitch * 1 / 45
                movement['right'] = max(min(1, -pitch), 0.2)

        if rol >= 0.2:
                movement['front'] = max(min(1, rol), 0.2)
        elif rol <= -0.2:
                movement['back'] = max(min(1, -rol), 0.2)

        return movement

def thread_keypress(key, on_time):
        # print(key, " ", on_time)
        kb_controller = Controller()
        kb_controller.press(key)
        time.sleep(on_time)
        kb_controller.release(key)

def start_iio(device):
        start = time.time()
        data = get_data(device)
        # print(data)
        roll, pitch = get_roll_pitch(data)
        movement = get_movement(roll, pitch)
        # print(movement)
        iio_timer = time.time() - start

        # print(iio_timer)
        # thread_keypress(key, on_time)

        if movement['front'] > 0.0:
                thread = Thread(target=thread_keypress, args=('w', movement['front'] * 2 * TIME, ))
                thread.daemon = True
                thread.start()
        if movement['back'] > 0.0:
                thread = Thread(target=thread_keypress, args=('s', movement['back'] * 2 * TIME, ))
                thread.daemon = True
                thread.start()
        if movement['left'] > 0.0:
                thread = Thread(target=thread_keypress, args=('a', movement['left'] * 2 * TIME, ))
                thread.daemon = True
                thread.start()
        if movement['right'] > 0.0:
                thread = Thread(target=thread_keypress, args=('d', movement['right'] * 2 * TIME, ))
                thread.daemon = True
                thread.start()

        time.sleep(abs(TIME - iio_timer))

        return movement

def thread_function(queue : Queue):
        device = init_iio()

        while not stop_event.is_set():
                if pause_event.is_set():
                        queue.put(start_iio(device))
                pause_event.wait()

def init_iio():
        ctx = iio.Context("ip:10.76.84.212")
        if ctx == None:
                print("Error create context")
                return None
        device = ctx.find_device("iio_adc_placa")
        if device == None:
                print("Error create device")
                return None        

        return device

def on_keypress(key):
        if key == keyboard.Key.ctrl_l:
                if pause_event.is_set():
                        pause_event.clear()
                        print("Paused")
                else: 
                        pause_event.set()
                        print("Resumed")
        elif key == keyboard.Key.esc:
                pause_event.set() #Grija la asta
                stop_event.set()
                print("Exit")
                return False
        return True


if __name__ == "__main__":

        pause_event = Event()
        pause_event.set()
        stop_event = Event()

        movement = {
                'left' : 0,
                'right' : 0,
                'front' : 0,
                'back' : 0
        }
        #Make a queue for shared the pointer in function 
        #not a copy of our movement dictionary
        movement_queue = Queue()
        movement_queue.put(movement)

        thread = Thread(target=create_movement_gui, args=(movement_queue, ))

        thread.daemon = True
        thread.start()

        
        thread = Thread(target=thread_function, args=(movement_queue, ))

        thread.daemon = True
        thread.start()

        #from pyinput import keybord
        with keyboard.Listener(on_press=on_keypress) as listener:
                listener.join()

        thread.join()






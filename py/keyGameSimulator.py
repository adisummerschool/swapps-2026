import iio
from queue import Queue
from threading import Thread, Event
from pynput import keyboard
from pynput.keyboard import Controller
import math
import tkinter as tk
import time

TIME = 0.1

def create_movement_gui(movement_queue: Queue):
        def update_square(canvas, square, on_time):
                intensity = 255 - int(on_time * 255)
                color = f'#{intensity:02x}{intensity:02x}ff'
                canvas.itemconfig(square, fill = color)

        def update_gui():
                mov = movement_queue.get()
                update_square(canvas, squares['left'], mov['left'])
                update_square(canvas, squares['right'], mov['right'])
                update_square(canvas, squares['front'], mov['front'])
                update_square(canvas, squares['back'], mov['back'])
                root.after(int(TIME * 50), update_gui)

        root = tk.Tk()
        canvas = tk.Canvas(root, width = 400, height = 400)
        canvas.pack()

        square_size = 100
        squares = {
                'left': canvas.create_rectangle(50, 150, 50 + square_size, 150 + square_size, fill = 'white'),
                'right': canvas.create_rectangle(250, 150, 250 + square_size, 150 + square_size, fill = 'white'),
                'back': canvas.create_rectangle(150, 250, 150 + square_size, 250 + square_size, fill = 'white'),
                'front': canvas.create_rectangle(150, 50, 150 + square_size, 50 + square_size, fill = 'white')
        }

        update_gui()

        root.mainloop()

def init_iio():
        ctx = iio.Context("ip:10.76.84.22")
        if ctx == None:
                print("The remote at 10.76.84.22 was not found. Terminating.")
                return
        
        device = ctx.find_device("ad5592r_s")
        if device == None:
                print("The device on the remote was not found. Terminating.")
                return
        return device

def get_data(device):
        axis_data = {
                'x': {'-': int, '+': int},
                'y': {'-': int, '+': int},
                'z': {'-': int, '+': int}
        }

        channels = [f'voltage{i}' for i in range(6)]
        axes = ['x', 'y', 'z']

        for i, chan_name in enumerate(channels):
                ch = device.find_channel(chan_name)
                if ch is None:
                        print("Counld not find channel ", i)
                attr = ch.attrs['raw'].value

                axis = axes[i // 2]
                polarity = '+' if i % 2 == 0 else '-'
                axis_data[axis][polarity] = int(attr)

        return axis_data

def get_roll_pitch(axis_data: dict):
        x = axis_data['x']['+'] - axis_data['x']['-']
        y = axis_data['y']['+'] - axis_data['y']['-']
        z = axis_data['z']['+'] - axis_data['z']['-']

        roll = math.atan2(y, z) * 180 / math.pi
        pitch = math.atan2(-x, math.sqrt(y ** 2 + z ** 2)) * 180 / math.pi

        return roll, pitch

def get_movement(roll, pitch):
        roll /= 45
        pitch /= 45
        mov = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
        if roll > 0.2:
                mov['right'] = min(1, roll)
        elif roll < -0.2:
                mov['left'] = min(1, -roll)

        if pitch > 0.2:
                mov['front'] = min(1, pitch)
        elif pitch < -0.2:
                mov['back'] = min(1, -pitch)
        return mov

def threaded_keypress(key, on_time):
        kb_controller = Controller()
        kb_controller.press(key)
        time.sleep(on_time)
        kb_controller.release(key)

def start_iio(device):
        start = time.time()
        data = get_data(device)
        roll, pitch = get_roll_pitch(data)
        movement = get_movement(roll, pitch)
        iio_timer = time.time() - start

        if movement['right'] > 0:
                thread = Thread(target = threaded_keypress, args = ('d', TIME * movement['right']))
                thread.daemon = True
                thread.start()
        if movement['left'] > 0:
                thread = Thread(target = threaded_keypress, args = ('a', TIME * movement['left']))
                thread.daemon = True
                thread.start()
        if movement['front'] > 0:
                thread = Thread(target = threaded_keypress, args = ('w', TIME * movement['front']))
                thread.daemon = True
                thread.start()
        if movement['back'] > 0:
                thread = Thread(target = threaded_keypress, args = ('s', TIME * movement['back']))
                thread.daemon = True
                thread.start()

        time.sleep(max(TIME - iio_timer, 0))

        return movement

def threaded_function(movement_queue: Queue):
        device = init_iio()

        while not stop_event.is_set():
                if pause_event.is_set():
                        movement_queue.put(start_iio(device))
                pause_event.wait()

def on_keypress(key):
        if key == keyboard.Key.ctrl_l:
                if pause_event.is_set():
                        pause_event.clear()
                        print("Paused")
                else:
                        pause_event.set()
                        print("Resumed")
        elif key == keyboard.Key.esc:
                pause_event.set()
                stop_event.set()
                print("Exit")
                return False
        return True
        
if __name__ == "__main__":
        pause_event = Event()
        stop_event = Event()
        pause_event.set()

        movement = {'left' : 0,
                    'right' : 0,
                    'front' : 0,
                    'back' : 0}
        movement_queue = Queue()
        movement_queue.put(movement) # trick to pass 'movement' bt reference and not by value to all threads

        thread1 = Thread(target = create_movement_gui, args = (movement_queue, ))
        thread1.daemon = True
        thread1.start()

        thread2 = Thread(target = threaded_function, args = (movement_queue, ))
        thread2.daemon = True
        thread2.start()

        with keyboard.Listener(on_press=on_keypress) as listener:
                listener.join()

        thread2.join()

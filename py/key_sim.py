import iio
from queue import Queue
from threading import Thread, Event, Lock
from pynput import keyboard
from pynput.keyboard import Controller
import math
import tkinter as tk
import time


TIME = 0.05

def create_movement_gui(queue: Queue):
        def update_square(canvas, square, on_time):
                intensity = 255 - int(on_time * 255)
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
        canvas = tk.Canvas(root, width = 400, height = 400)
        canvas.pack()

        square_size = 100
        squares = {
                'left' : canvas.create_rectangle(50, 150, 50 + square_size, 150 + square_size, fill='white'),
                'right' : canvas.create_rectangle(50 + 2 * square_size, 150, 50 + 3 * square_size, 150 + square_size, fill='white'),
                'front' : canvas.create_rectangle(150, 50, 150 + square_size, 50 + square_size, fill='white'),
                'back' : canvas.create_rectangle(150, 150, 150 + square_size, 150 + square_size, fill='white')
        }

        update_gui()
        root.mainloop()

def get_data(device):
        axis_data = {
                'x' : {'-': int, '+' : int},
                'y' : {'-': int, '+' : int},
                'z' : {'-': int, '+' : int}
        }
        axes = ['x', 'y', 'z']

        channel_names = [f'voltage{i}' for i in range(6)]

        for i, channel_names in enumerate(channel_names):
                chn = device.find_channel(channel_names, False)
                if chn is None:
                        raise ValueError("channel not found ")

                attr = chn.attrs["raw"].value

                axis = axes[i // 2]
                polarity = '+' if i % 2 == 0 else "-"
                axis_data[axis][polarity] = int(attr)

        return axis_data

def get_roll_data(axis: dict):
        x = axis['x']['+'] - axis['x']['-']
        y = axis['y']['+'] - axis['y']['-']
        z = axis['z']['+'] - axis['z']['-']

        roll = math.atan2(y, z) * 180 / math.pi
        pitch = math.atan2(-x, math.sqrt(y ** 2 + z ** 2)) *180 / math.pi

        return roll, pitch

def get_movement(roll, pitch):
        mov = {'left':0, 'right': 0, 'front': 0, 'back': 0}
        # 0 to 1 values
        # 45 degrees = 1
        roll /= 45
        pitch /= 45

        if roll >= 0.2:
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
        print(key, on_time)

def start_iio(device):
        start = time.time()
        data = get_data(device)
        roll, pitch = get_roll_data(data)
        movement = get_movement(roll, pitch)
        iio_timer = time.time() - start

        for direction, key in (('right', 'd'), ('left', 'a'), ('front', 'w'), ('back', 's')):
                value = movement[direction]
                if value > 0:
                        on_time = TIME * value
                        key_thread = Thread(target=threaded_keypress, args=(key, on_time))
                        key_thread.daemon = True
                        key_thread.start()

        time.sleep(max(0, TIME - iio_timer))

        return movement

def threaded_function(queue: Queue):
        device = init_iio()
        if device is None:
                print("failed to init iio, exiting thread")
                return

        while not stop_event.is_set():
                if pause_event.is_set():
                        queue.put(start_iio(device))
                pause_event.wait()

def init_iio():
        #device
        try:
                ctx = iio.Context("ip:10.76.84.14")
        except OSError:
                print("ctx not found")
                return None

        device = ctx.find_device("iio_ad5592r_s")
        if device is None:
                raise ValueError("Device not found")

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
                pause_event.set()
                stop_event.set()
                print("exit")
                return False
        return True


if __name__ == "__main__":
        pause_event = Event()
        pause_event.set()
        stop_event = Event()

        movement = {'left':0, 'right': 0, 'front': 0, 'back': 0}
        movement_queue = Queue()
        movement_queue.put(movement)

        thread = Thread(target= create_movement_gui, args=(movement_queue, ))
        thread.daemon = True
        thread.start()

        thread = Thread(target= threaded_function, args=(movement_queue, ))
        thread.daemon = True
        thread.start()

        # new thread ? 

        with keyboard.Listener(on_press=on_keypress) as listener:
                listener.join()
        
        thread.join()
import iio
from queue import Queue
from threading import Thread, Event
from pynput import keyboard
import math
import tkinter as tk
import time
from pynput.keyboard import Controller

TIME = 0.1

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

        root.after(int(TIME * 1000), update_gui)

    root = tk.Tk()
    canvas = tk.Canvas(root, width=400, height=400)
    canvas.pack()

    square_size = 100
    squares = {
        'left': canvas.create_rectangle(50, 150, 50 + square_size, 150 + square_size, fill='white'),
        'right': canvas.create_rectangle(50 + 2 * square_size, 150, 50 + 3 * square_size, 150 + square_size, fill='white'),
        'front': canvas.create_rectangle(150, 50, 150 + square_size, 50 + square_size, fill='white'),
        'back': canvas.create_rectangle(150, 150, 150 + square_size, 150 + square_size, fill='white')
    }

    update_gui()
    root.mainloop()

def get_data(device):
    axis_data = {
        'x': {'-': int, '+': int},
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int}
    }

    axes = ['x', 'y', 'z']

    # populate axis data from iio
    channel_names = [f'voltage{i}' for i in range(6)]

    for i, channel_name in enumerate(channel_names):
        ch = device.find_channel(channel_name)
        if ch is None:
            raise ValueError("Could not find channel")

        attr = ch.attrs['raw'].value

        axis = axes[i // 2]
        polarity = '+' if i % 2 == 0 else '-'
        axis_data[axis][polarity] = int(attr)

    return axis_data

def get_roll_pitch(data):
    x = data['x']['+'] - data['x']['-']
    y = data['y']['+'] - data['y']['-']
    z = data['z']['+'] - data['z']['-']

    roll = math.atan2(y, z) * 180 / math.pi
    pitch = math.atan2(-x, math.sqrt(y ** 2 + z ** 2)) * 180 / math.pi

    return roll, pitch

def get_movement(roll, pitch):
    mov = {'left': 0, 'right': 0, 'front': 0, 'back': 0}

    roll /= 45
    pitch /= 45

    if roll > 0.2:
        mov['right'] = round(min(1, roll), 3)
    elif roll < -0.2:
        mov['left'] = round(min(1, -roll), 3)

    if pitch > 0.2:
        mov['front'] = round(min(1, pitch), 3)
    elif pitch < -0.2:
        mov['back'] = round(min(1, -pitch), 3)

    return mov

def threaded_keypress(key, on_time):
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
    iio_timer = time.time() - start

    keys = {'left': 'a', 'right': 'd', 'front': 'w', 'back': 's'}

    for direction, on_time in movement.items():
        if on_time > 0:
            key_thread = Thread(target=threaded_keypress, args=(keys[direction], on_time * TIME))
            key_thread.daemon = True
            key_thread.start()

    time.sleep(abs(TIME - iio_timer))

    # print(movement)
    return movement

def  threaded_function(queue: Queue):
    device = init_iio()

    while not stop_event.is_set():
        if pause_event.is_set():
            queue.put(start_iio(device))
        pause_event.wait()

def init_iio():
    context = iio.Context("ip:10.76.84.4")
    device = context.find_device("ad5592r_s")
    if device is None:
        raise ValueError("Failed to get device")
    
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
        print("Exit")
        return False

    return True

if __name__ == "__main__":

    pause_event = Event()
    stop_event = Event()
    movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    # Using a queue to store movement commands to multithreaded processing
    movement_queue = Queue()
    movement_queue.put(movement)

    # This thread is for the GUI that will display the movement commands
    thread = Thread(target=create_movement_gui, args=(movement_queue, ))
    thread.daemon = True
    thread.start()

    # This thread is for processing the movement commands and reading ADC channels in the queue
    thread = Thread(target=threaded_function, args=(movement_queue, ))
    thread.daemon = True
    thread.start()

    # This thread is for listening to keypresses
    with keyboard.Listener(on_press=on_keypress) as listener:
        listener.join()

    # Wait for the threads to finish
    thread.join()

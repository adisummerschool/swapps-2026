import iio
import math

from queue import Queue
from threading import Thread
from threading import Event
from pynput import keyboard


def create_movement_gui(queue: Queue):
    pass

def get_data(device):
    axis_data = {
        'x':{'-': int, '+': int},
        'y':{'-': int, '+': int},
        'z':{'-': int, '+': int},  
    }
    #populate axis_data from iio
    axes = ['x','y','z']
    channel_names = [f'voltage{i}' for i in range(6)]
    for i,channel_name in enumerate(channel_names):
        ch = device.find_channel(channel_name)
        if ch is None:
            raise ValueError("could not find channel")

        attr = ch.attrs['raw'].value
        
        axis = axes[i // 2]
        polarity = '+' if i%2 == 0 else '-'
        axis_data[axis][polarity] = int(attr)

    return axis_data

def get_roll_pitch(axis: dict):
    x = axis['x']['+'] - axis['x']['-']
    y = axis['y']['+'] - axis['y']['-']
    z = axis['z']['+'] - axis['z']['-']

    roll = math.atan2(y, z) * 180 / math.pi
    pitch = math.atan2(-x, math.sqrt(y ** 2 + z ** 2)) * 180 / math.pi

    return roll, pitch

def get_movement(roll, pitch):
    mov = {'left': 0, 'right': 0, 'front': 0, 'back': 0}

    roll /= 45
    pitch /= 45

    if roll > 0.2:
        mov['right'] = round(min(1, roll),2)
    elif roll < -0.2:
        mov['left'] = round(min(1, -roll),2)

    if pitch > 0.2:
        mov['front'] = round(min(1, pitch),2)
    elif pitch < -0.2:
        mov['back'] = round(min(1, -pitch),2)

    return mov

def start_iio(device):
    data = get_data(device)
    #print(data)
    roll, pitch = get_roll_pitch(data)
    movement = get_movement(roll,pitch)

    print(movement)
    #return movement

def threaded_function(queue: Queue):
    device = init_iio()

    while not stop_event.is_set():
        if pause_event.is_set():
            queue.put(start_iio(device))
        pause_event.wait()

def init_iio():
    ctx = iio.Context("ip:10.76.84.15")
    device = ctx.find_device("ad5592r_s")
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

if __name__ == '__main__':

    pause_event = Event()
    stop_event = Event()

    movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    movement_queue = Queue()
    movement_queue.put(movement)

    thread = Thread(target=create_movement_gui, args=(movement_queue,))
    thread.daemon = True
    thread.start()

    thread = Thread(target=threaded_function, args=(movement_queue,))
    thread.daemon = True
    thread.start()

    with keyboard.Listener(on_press=on_keypress) as listener:
        listener.join()

    thread.join()
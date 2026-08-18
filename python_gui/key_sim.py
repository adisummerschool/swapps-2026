import iio
from queue import Queue
from threading import Thread   

def create_movment_gui(queue: Queue):
    pass

def get_data(device):
    channels = device.channels

    axis_data = {
        'x': {'-': int, '+': int},
        'y': {'-': int, '+': int},
        'z': {'-': int, '+': int},
    }

    axes=['x', 'y', 'z']

    channel_name={f"voltage{i}" for i in range(6)}

    for i, channel_name in enumerate(channel_name):
        ch = device.find_channel(channel_name)
        if ch is None:
            raise ValueError("Could not find channel")

        attr = ch.attrs['raw'].value

        axis = axes[i // 2]
        polarity = '+' if i % 2 == 0 else '-'
        axis_data[axis][polarity] = attr

    return axis_data


def start_iio(device):
    data = get_data(device)
    print(data)

    #roll, pitch = get_roll_pitch(data)
    #movement = get_movement(roll, pitch)

    #return movement

def threaded_function(queue: Queue):
    device = init_iio()

    #while True:
    queue.put(start_iio(device))


def init_iio():
    ctx = iio.Context("ip:10.76.84.34")
    device = ctx.find_device("ad5592r_s")
    if device is None:
        raise ValueError("Failed to find device")

    return device

if __name__ == '__main__':
    movement = {'left': 0, 'right': 0, 'front': 0, 'back': 0}
    movement_queue = Queue() 
    movement_queue.put(movement)

    thread = Thread(target=create_movment_gui, args=(movement_queue, ))
    thread.daemon = True
    thread.start()

    thread = Thread(target=threaded_function, args=(movement_queue, ))
    thread.daemon = True
    thread.start()

    thread.join() #asteapta pana cand se termina si ultimul thread

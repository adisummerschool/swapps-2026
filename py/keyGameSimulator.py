import iio
from queue import Queue
from threading import Thread

def create_movement_gui(movement_queue: Queue):
        pass

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
                axis_data[axis][polarity] = attr

        return axis_data

def start_iio(device):
        data = get_data(device)
        print(data)
        #[roll, pitch] = get_roll_pitch(data)
        #movement = get_movement(roll, pitch)
        return movement

def threaded_function(movement_queue: Queue):
        device = init_iio()

        #while True:
        movement_queue.put(start_iio(device))

if __name__ == "__main__":
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

        thread2.join()

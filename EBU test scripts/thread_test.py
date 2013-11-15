import threading

joystick = 0

class DataCaptureThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):
        print 'Second thread started'
        DataCapture()

def DataCapture():
    global joystick
    print 'Second thread running'
    while True:
        joystick += 1

print 'Program start'
t = DataCaptureThread()
t.start()

print 'first thread running'

import ctypes
import random
import time

_fb = ctypes.CDLL('./libetcfb.so')
#_fb.fb_setup.argtypes = ()
_fb.fb_fill.argtype = (ctypes.c_int)
#_fb.fb_flip.argtypes = ()

def fb_fill(color):
    global _fb
    _fb.fb_fill(ctypes.c_int(color))

def fb_setup():
    global _fb
    _fb.fb_setup()

def fb_flip():
    global _fb
    _fb.fb_flip()

def fb_rect():
    global _fb
    _fb.fb_rect()

fb_setup()


for i in range(0, 100) :
    fb_fill(random.randrange(0, 16000000))
    for i in range(0,100) : fb_rect()
    fb_flip()
    time.sleep(.02)

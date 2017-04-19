#!/usr/bin/python

import smbus
import math

import os
from time import sleep

import time, sys
from pygame import mixer

import RPi.GPIO as GPIO # always needed with RPi.GPIO  

''' Version info''' 

print "Version: 0.0.2"
print "Last updated: April 18, 2017"


# Power management registers
power_mgmt_1 = 0x6b
power_mgmt_2 = 0x6c

def read_byte(adr):
    return bus.read_byte_data(address, adr)

def read_word(adr):
    high = bus.read_byte_data(address, adr)
    low = bus.read_byte_data(address, adr+1)
    val = (high << 8) + low
    return val

def read_word_2c(adr):
    val = read_word(adr)
    if (val >= 0x8000):
        return -((65535 - val) + 1)
    else:
        return val

def dist(a,b):
    return math.sqrt((a*a)+(b*b))

def get_y_rotation(x,y,z):
    radians = math.atan2(x, dist(y,z))
    return -math.degrees(radians)

def get_x_rotation(x,y,z):
    radians = math.atan2(y, dist(x,z))
    return math.degrees(radians)

def withinRange( val1, val2, rangeVal ):
    if val2 <= val1 + rangeVal : 
        if val2 >= val1 - rangeVal : 
            return True 
    return False     

print "Setting up sounds"
mixer.init()
mixer.music.load('org.mp3')
isplaying = False 

print "Setting up accelerometer"

bus = smbus.SMBus(1) # or bus = smbus.SMBus(1) for Revision 2 boards
address = 0x68       # This is the address value read via the i2cdetect command

# Now wake the 6050 up as it starts in sleep mode
bus.write_byte_data(address, power_mgmt_1, 0)

last_rot_x = 0 
last_rot_y = 0 

cur_rot_x = 0 
cur_rot_y = 0 

timer_last_move = 0 ; 
timer_current = 0 ; 

setting_move_amount = 10 
setting_move_timer  = 10 

print "Setting up LEDs "

GPIO.cleanup()  
GPIO.setmode(GPIO.BCM)  # choose BCM or BOARD numbering schemes. I use BCM  
GPIO.setup(24, GPIO.OUT)
GPIO.output(24, GPIO.LOW)


print "Starting..."

while True:

    timer_current = time.time(); 

    accel_xout = read_word_2c(0x3b)
    accel_yout = read_word_2c(0x3d)
    accel_zout = read_word_2c(0x3f)

    accel_xout_scaled = accel_xout / 16384.0
    accel_yout_scaled = accel_yout / 16384.0
    accel_zout_scaled = accel_zout / 16384.0

    cur_rot_x = round( get_x_rotation(accel_xout_scaled, accel_yout_scaled, accel_zout_scaled), 0)
    cur_rot_y = round( get_y_rotation(accel_yout_scaled, accel_yout_scaled, accel_zout_scaled), 0)

    if withinRange(cur_rot_x, last_rot_x, setting_move_amount) == False:
        timer_last_move = time.time() + setting_move_timer; 
        print "XXXXX "
    
    if withinRange(cur_rot_y, last_rot_y, setting_move_amount) == False:
        timer_last_move = time.time() + setting_move_timer; 
        print "YYYYY "

    if timer_current < timer_last_move:
        print "Playing for ", round( timer_last_move - timer_current, 1), " more seconds"
        if( isplaying == False ):
            mixer.music.play(-1)
            isplaying = True 
        ''' Start speeding up the LEDs'''
        GPIO.output(24, GPIO.HIGH) 
            
    else:
        mixer.music.stop()       
        isplaying = False 
        ''' Slow down the LEDs'''
        GPIO.output(24, GPIO.LOW) 

    ''' print "x: ", cur_rot_x, ", y: ", cur_rot_y, ", timer: ", timer_last_move - timer_current '''

    last_rot_x = cur_rot_x
    last_rot_y = cur_rot_y
    
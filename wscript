#
# This file is the buildscript for Pebble SDK projects.
#
import os.path
try:
    from pebble_arm_waf import *
except:
    pass

def options(ctx):
    ctx.load('pebble_arm_waf')

def configure(ctx):
    ctx.load('pebble_arm_waf')

def build(ctx):
    ctx.load('pebble_arm_waf')
    ctx.pbl_program(source=ctx.path.ant_glob('src/c/**/*.c'),
                    target='pebble-app.elf')
    ctx.pbl_bundle(elf='pebble-app.elf')

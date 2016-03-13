#!/bin/bash

binary="firmware/bin.bin"

commands="speed 12000
si swd
device efm32hg322f64
connect
erase
loadbin ${binary},0x00000000
r
g
q"

echo "$commands" | JLinkExe

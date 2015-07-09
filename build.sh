#!/bin/sh
make && make flash && screen /dev/tty.wchusbserial14* 115200

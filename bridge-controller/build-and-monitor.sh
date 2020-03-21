#!/bin/bash

ENV="client"
TARGET="upload"

USB=`ls /dev/ttyUSB* | tr ' ' '\n' | sort  | head -1`

if [ "xx$1" = "xxserver" ] ; then 
    ENV="server"
    USB=`ls /dev/ttyUSB* | tr ' ' '\n' | sort  | tail -1` 
fi

pio run -e $ENV -t $TARGET --upload-port $USB && \
pio device monitor -b 230400 --echo --port $USB


#!/bin/sh

echo Konfiugruje BMD.
autoreconf && ./configure -C --enable-debug --prefix /opt/bmd $@ && make 

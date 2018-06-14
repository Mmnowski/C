#!/bin/bash
echo "Compiling files..."

binDirectory=bin

if [ ! -e $binDirectory ]; then
  mkdir $binDirectory
fi

gcc -o bin/client client.c -lX11
gcc -o bin/serwer serwer.c -lX11
echo "Done!"

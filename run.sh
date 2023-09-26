#!/bin/zsh
echo "Start Compiling..."
sleep 3
qmake -makefile
make all

echo "Compiling Done!"
echo "Cleaning Up..."
sleep 3
make clean
make mocclean

echo "Clean Up Done!"
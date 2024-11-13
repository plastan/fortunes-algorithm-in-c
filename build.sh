#!/bin/bash
echo " cleaning output ..."
rm a.out
echo "compiling ..."
gcc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -g
echo "executing ..."
./a.out

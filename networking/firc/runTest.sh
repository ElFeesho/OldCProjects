#!/bin/bash

#Q. Why is there a script to run the prog? Do you suck at coding?!
#A. No... just I don't think you'll be happy moving libfirc.a to /usr/lib

export LD_LIBRARY_PATH=./

#This script will simply set an environmental variable that bash uses
#to check where to find libs. (I think bash uses...)

./test

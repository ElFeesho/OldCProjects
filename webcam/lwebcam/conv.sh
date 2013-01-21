#!/bin/bash

for x in $(ls *.bmp); do convert $x $x.png; done
rm *.bmp
rename ".bmp.png" ".png" *.bmp.png
ffmpeg -i file%05d.png movie.mpg
rm *.png

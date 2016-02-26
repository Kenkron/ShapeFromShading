#!/bin/bash
FILES=color/*
for f in $FILES
do
    convert $f -resize 640x480\> small${f:0:18}.png
done

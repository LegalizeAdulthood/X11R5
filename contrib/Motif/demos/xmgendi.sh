#!/bin/sh
xmgendi "This is the label text." "Button 1" "Button 2" "Button 3"
case $? in
    1) echo "button 1 pressed";;
    2) echo "button 2 pressed";;
    3) echo "button 3 pressed";;
esac

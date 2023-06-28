#!/bin/sh

echo 'eaten?'
read ans

case $ans in
	y|Y|Y*|y*) echo "good"; echo "Eat more";
	n|N|N*|n*) echo "have some now";
	*) echo "what??";
esac


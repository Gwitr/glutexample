#!/bin/sh
gcc -O2 -Wall -Wextra -pedantic -o gltest gltest.c loadpng.c program.c texture.c quad.c -lm -lpng -lGL -lGLEW -lglut

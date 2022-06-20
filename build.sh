#!/bin/bash
flags=("-g" "-std=c11" "-Wall" "-funsigned-char")
gcc ${flags} src/*.c -o nashc

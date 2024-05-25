#!/bin/sh

find . -name "Makefile" -exec dos2unix {} \;
find . -name "*.h" -exec dos2unix {} \;
find . -name "*.c" -exec dos2unix {} \;
find . -name "*.cc" -exec dos2unix {} \;
find . -name "*.tab" -exec dos2unix {} \;
find files/ -exec dos2unix {} \;
find client/ -name "*.sh" -exec dos2unix {} \;
find files/ -name "*.sh" -exec dos2unix {} \;
find listener/ -name "*.sh" -exec dos2unix {} \;
find monitor/ -name "*.sh" -exec dos2unix {} \;
find queue/ -name "*.sh" -exec dos2unix {} \;
find router/ -name "*.sh" -exec dos2unix {} \;
find servers/ -name "*.sh" -exec dos2unix {} \;
find shared/ -name "*.sh" -exec dos2unix {} \;
find tools/ -name "*.sh" -exec dos2unix {} \;

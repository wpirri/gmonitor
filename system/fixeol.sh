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

find client/ -name "*.sh" -exec chmod 0755 {} \;
find files/ -name "*.sh" -exec chmod 0755 {} \;
find listener/ -name "*.sh" -exec chmod 0755 {} \;
find monitor/ -name "*.sh" -exec chmod 0755 {} \;
find queue/ -name "*.sh" -exec chmod 0755 {} \;
find router/ -name "*.sh" -exec chmod 0755 {} \;
find servers/ -name "*.sh" -exec chmod 0755 {} \;
find shared/ -name "*.sh" -exec chmod 0755 {} \;
find tools/ -name "*.sh" -exec chmod 0755 {} \;


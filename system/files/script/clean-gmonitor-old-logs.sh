#!/bin/sh

find /var/log/gmonitor -type f -mtime +7 -name '*.log' -execdir rm -f '{}' \;


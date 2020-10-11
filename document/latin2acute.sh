#!/bin/sh

process()
{
filename="${1}"
filename_bkp="${1}.tmp"

mv $filename $filename_bkp || exit 1
cat $filename_bkp | \
	sed 's/�/\&aacute;/g' | sed 's/�/\&eacute;/g' | sed 's/�/\&iacute;/g' | sed 's/�/\&oacute;/g' | sed 's/�/\&uacute;/g' | \
	sed 's/�/\&Aacute;/g' | sed 's/�/\&Eacute;/g' | sed 's/�/\&Iacute;/g' | sed 's/�/\&oacute;/g' | sed 's/�/\&Uacute;/g' | \
	sed 's/�/\&ntilde;/g' | sed 's/�/\&Ntilde;/g' \
> $filename || exit 1

rm $filename_bkp
}

for i in $*
do
	process $i
done


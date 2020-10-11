#!/bin/sh

process()
{
filename="${1}"
filename_bkp="${1}.tmp"

mv $filename $filename_bkp || exit 1
cat $filename_bkp | \
	sed 's/á/\&aacute;/g' | sed 's/é/\&eacute;/g' | sed 's/í/\&iacute;/g' | sed 's/ó/\&oacute;/g' | sed 's/ú/\&uacute;/g' | \
	sed 's/Á/\&Aacute;/g' | sed 's/É/\&Eacute;/g' | sed 's/Í/\&Iacute;/g' | sed 's/Ó/\&oacute;/g' | sed 's/Ú/\&Uacute;/g' | \
	sed 's/ñ/\&ntilde;/g' | sed 's/Ñ/\&Ntilde;/g' \
> $filename || exit 1

rm $filename_bkp
}

for i in $*
do
	process $i
done


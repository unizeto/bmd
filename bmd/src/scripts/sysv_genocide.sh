#!/bin/bash
for((z=0; z<5; z++)); do
for dupa in `ipcs | grep 0x | cut -d' ' -f2`; do
	echo "Removing id $dupa"
	if ipcrm -m $dupa || ipcrm -s $dupa || ipcrm -q $dupa; then echo "Cant remove $dupa"; fi
done
done

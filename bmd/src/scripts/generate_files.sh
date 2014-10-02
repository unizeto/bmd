#!/bin/bash

if (( $#!=4 )) && (( $#!=5 )); then
	echo "Usage:"
	echo "$0 source_file destination_dir pattern nr_of_files";
	echo "or:"
	echo "$0 /dev/urandom destination_dir pattern nr_of_files filesize";
	echo "Argument description:"
	echo "	source_file - use ur head, dude!"
	echo "	destination_dir - where to place the files"
	echo "	pattern - format: prefix/suffix - eg: plik/.txt";
	echo "	nr_of_files - u know what i mean, don't u?";
	echo "	filesize - if u want to truncate the file or usr /dev/urandom or /dev/null as source";
	echo "Usage examples:"
	echo "$0 ~/.bash_history /tmp/dupa dupa/.txt 1000"
	echo "$0 /dev/urandom /tmp/dupa dupa/.txt 10 10M"
	exit -1;
fi

if [[ ! -e $1 ]]; then 
	echo "File $1 not found.";
	exit -1;
fi

if [[ ! -e $1 ]]; then 
	echo "File $1 not found.";
	exit -1;
fi

if [[ ! -r $1 ]]; then 
	echo "File $1 not readable.";
	exit -1;
fi

if [[ ! -d $2 ]]; then 
	mkdir -p "$2";
fi

if [[ ! -d $2 ]]; then 
	echo "Cannot create directory $2";
fi
fprefix=`echo $3 | cut -d'/' -f1`;
fsuffix=`echo $3 | cut -d'/' -f2`;

for(( i=0; i<=$4; i++ )); do 
	echo "$2/$filename";
	filename=`printf "$fprefix%0u$fsuffix" $i`;
	if (( $#==4 )); then
		cp "$1" "$2/$filename";
	else
		dd if="$1" of="$2/$filename" count=1 bs="$5"
	fi
done


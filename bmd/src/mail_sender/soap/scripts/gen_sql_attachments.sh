#!/bin/bash

if [ "$#" -ne 3 ]
then
	printf "\n\nERROR : Niepoprawne wywolanie skryptu !!!\n\n"
	printf "Wymagane 3 parametry :\n"
	printf "1st sciezka do katalogu z zalacznikami (KONIECZNIE bez '/' na koncu)\n"
	printf "2nd wartosc kolumny mailing_email_template.id dla szablonu, do ktorego ma byc przyporzadkowany zalacznik jpeg\n"
	printf "3rd nazwa pliku wynikowego (skryptu sql), ktory zostanie stworzony w current working directory\n"
	printf "przyklad: ./gen_sql_attachments.sh ./images 3 wynik.sql\n"
	exit
fi

if [ -e "./$3" ]; then
	printf "\n\nERROR : Plik ./$3 juz istnieje.\n\n"
	exit
fi

ORIGINAL_ATTACHMENTS_LIST=`ls $1`
for iter in $ORIGINAL_ATTACHMENTS_LIST
do
	echo "Encoding with base64 $1/$iter file ..."
	base64 --wrap=0 $1/$iter > $1/$iter.b64
done

printf "\nBEGIN TRANSACTION;\n\n" >> ./$3

for iter in $ORIGINAL_ATTACHMENTS_LIST
do
	echo "Adding insert statement for $1/$iter file ..."
	printf "INSERT INTO mailing_email_attachments (created, templateid, filename, mimetype, disposition, content) VALUES (now(), '$2', '$iter', 'image/jpeg', 'inline', '" >> ./$3
	cat $1/$iter.b64 >> ./$3
	printf "');\n\n" >> ./$3
done

printf "\nCOMMIT TRANSACTION;\n\n" >> ./$3

rm $1/*.b64

printf "\n\nDone ... check ./$3 file for results.\n\n"

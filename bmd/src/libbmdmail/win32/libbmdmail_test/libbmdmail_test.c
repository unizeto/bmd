#include <bmd\libbmdmail\libbmdmail.h>

int main()
{
	int status = 0;

	mail_send_info_t *mi		= NULL;	//do wyslania maila
	status = set_mail_send_info(&mi, "smtp.unet.pl:25",
		"akarczmarczyk@unet.pl", NULL, NULL, 
"FROM: <akarczmarczyk@unet.pl>\r\n\
TO: <akarczmarczyk@unet.pl>\r\n\
SUBJECT: Ala ma kota.\r\n\
\r\n\
Ala ma kota raz dwa trzy.\r\n"
		, 0, NULL, NULL, "akarczmarczyk@unet.pl");
	status = bmd_send_mail(mi);
	return 0;
}
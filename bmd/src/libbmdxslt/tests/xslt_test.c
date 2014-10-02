#include <bmd/libbmdxslt/libbmdxslt.h>

/* /usr/lib/libxslt.so \	*/

int main(int argc, char **argv)
{
int status 			= 0;
int i				= 0;

char *xsl		= NULL;
char *xml		= NULL;
char *html		= NULL;

	if (argc <= 1)
	{
		printf("\n*************** xml to html converter by PKL *******************\n");
		printf("*                                                              *\n");
		printf("*      -xsl        xsl file                                    *\n");
		printf("*      -xml        xml file                                    *\n");
		printf("*      -html       html result file                            *\n");
		printf("*                                                              *\n");
		printf("****************************************************************\n\n");
		return -1;
	}


 	for (i = 1; i < argc; i++)
	{
		if (strcmp("-xsl",argv[i])==0)
		{
			asprintf(&xsl, "%s", argv[i+1]);
		}
		if (strcmp("-xml",argv[i])==0)
		{
			asprintf(&xml, "%s", argv[i+1]);
		}
		if (strcmp("-html",argv[i])==0)
		{
			asprintf(&html, "%s", argv[i+1]);
		}
	}

	printf("xsl: %s\n",xsl);
	printf("xml: %s\n",xml);
	printf("html: %s\n",html);

	status=convertXmlToHtml(xml,xsl,html);
	if (status<0)
	{
		printf("\n error in converting %s to %s   %i\n\n",xml,html,status);
		return -1;
	}

	free(xsl); xsl=NULL;
	free(xml); xml=NULL;
	free(html); html=NULL;


	return 0;

}

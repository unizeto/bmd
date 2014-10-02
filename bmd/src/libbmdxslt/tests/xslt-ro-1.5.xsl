<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:f="http://www.unizeto.pl/szablon/formularz/1" xmlns:ds="http://www.w3.org/2000/09/xmldsig#" xmlns:ndap="http://www.mswia.gov.pl/standardy/ndap">
<xsl:template match="/">
        <html>
            <head>
                <title>EUP: <xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tytul/ndap:oryginalny"/> </title>
						<style>
		    
			body{
				 font-family: Tahoma, Arial, Helvetica, sans-serif;
				 font-size: 9px;
				 line-height: 140%;
			}
		   
		   .form-input {
				 font-family: Tahoma, Arial, Helvetica, sans-serif;
				 font-size: 11px;
				 width: 250px;
				 color: #666666;
				 border: 1px solid #e2e2e2;
				 margin: 2px 0px 2px 0; 
			}

		   .form-textarea {
				 font-family: Tahoma, Arial, Helvetica, sans-serif;
				 font-size: 11px;
				 width: 700px;
				 height: 200px;
				 color: #666666;
				 border: 1px solid #e2e2e2;
				 margin: 2px 0px 2px 0; 
			}
			
			.ramka {
				 font-family: Tahoma, Arial, Helvetica, sans-serif;
				 font-size: 9px;
				 width: 700px;
				 border: 1px solid #e2e2e2;
			}	

			.header {
				 font-family: Tahoma, Arial, Helvetica, sans-serif;
				 font-size: 20px;
				 text-align: center;
				 font-weight: bold;
			}
			
			input.button{
				width: 150px;
				height: 20px;
				color:#e00000;
				font-family: Tahoma, Arial, Helvetica, sans-serif;
				font-size:11px;
			}
			
			td.input {
			    border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250px;
			}
			
		</style>
        </head>

<body leftmargin="0" topmargin="20" marginwidth="0" marginheight="20">
<table width="700" border="0" celpadding="0" cellspacing="0" align="center">
  <tr align="left" class="ramka">
         <td>
		    <table width="350" border="0" celpadding="0" cellspacing="0" align="left">
				<tr>
					<td width="100" height="20">Miejscowość:</td>
					<td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/f:tresc/f:miejscowosc"/></td>					
				</tr>
				<tr>

					<td width="100">Data złożenia:</td>
					<td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:data/ndap:czas"/></td>					
				</tr>			
			</table>
		</td>	
  </tr>
  <tr>
  		<td>
		<br/><br/>
		</td>

  </tr>
  <tr>
      <td>
		   <table width="700" border="0" celpadding="0" cellspacing="0">
				<tr>
					<td width="350">
		<table width="350" border="0" celpadding="0" cellspacing="0" align="left">
		    <tr>
                <td width="100"></td>

                <td width="250" height="20" style="border-style: solid solid;
				border-width: 0px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><b>Dane składającego wniosek</b></td>
	        </tr>
            <tr>
                <td width="100">Imię:</td>
                <td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:imie"/></td>
	        </tr>
			<tr>
                 <td width="100">Nazwisko:</td>

                 <td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:nazwisko"/></td>
	        </tr>
			<tr>
                 <td width="100">Pesel:</td>
                 <td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:id[@typ='pesel']"/></td>
	        </tr>
			<tr>
                 <td width="100">Kod pocztowy:</td>

                 <td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:adres/ndap:kod"/></td>
	        </tr>	
			<tr>
                 <td width="100">Miejscowość:</td>
                 <td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:adres/ndap:miejscowosc"/></td>
	        </tr>
			<tr>
                 <td width="100">Ulica:</td>

                 <td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:adres/ndap:ulica"/></td>
	        </tr>
			<tr>
                 <td width="100">Budynek:</td>
                <td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:adres/ndap:budynek"/></td>
	        </tr>
			<tr>
                 <td width="100">Lokal:</td>

                 <td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:adres/ndap:lokal"/></td>
	        </tr>
			<tr>
                 <td width="100">E-mail:</td>
                 <td width="250" height="20" style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 250;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tworca/ndap:podmiot/ndap:osoba/ndap:kontakt[@typ='email']"/></td>

	        </tr>
	    </table>
					</td>
					<td width="350" align="right"><p class="zero">
					<b>
                    <xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:odbiorca/ndap:podmiot/ndap:instytucja/ndap:nazwa"/><br/>
                    <xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:odbiorca/ndap:podmiot/ndap:instytucja/ndap:adres/ndap:ulica"/>&#160;
                    <xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:odbiorca/ndap:podmiot/ndap:instytucja/ndap:adres/ndap:budynek"/><br/>
                    <xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:odbiorca/ndap:podmiot/ndap:instytucja/ndap:adres/ndap:miejscowosc"/>
                    </b></p>
					</td>
				</tr>
		   </table>
     </td>
  </tr>
  <tr>
      <td align="center"><br/><br/>
            <span style="				 font-family: Tahoma, Arial, Helvetica, sans-serif;
				 font-size: 18px;
				 text-align: center;		
				 font-weight: bold;"><xsl:value-of select="//f:pismo/f:dane/ndap:dokument/ndap:tytul/ndap:oryginalny"/></span>
         <br/><br/>

     </td>
  </tr>
  <tr>
      <td>
			 Opis sprawy:
      </td>
 </tr>

  <tr>
      <td>
          <table width="700" cellpadding="0" cellspacing="0">
             <tr>
                 <td style="border-style: solid solid;
				border-width: 1px;
				border-color: #e2e2e2;
				padding: 0px 4px 0px 4px;
				width: 700;
				text-align: justify;
				">
					<xsl:call-template name="break">
						<xsl:with-param name="text" select="//f:pismo/f:dane/f:tresc/f:tekst" />
					</xsl:call-template>
				</td>
             </tr>
         </table>
      </td>
 </tr>

   <tr>
  		<td>
		<br/><br/>
		</td>
  </tr>
   <tr align="left">
         <td>
		    <table width="700" border="0" celpadding="0" cellspacing="0">
				<tr>

					<td width="350" align="left"></td>
					<td width="350" align="right"></td>
				</tr>
			</table>
		</td>	
  </tr>
</table>
</body>
</html>
</xsl:template>

<xsl:template name="break">
	<xsl:param name="text" select="."/>
	<xsl:choose>
	<xsl:when test="contains($text, '&#xa;')">
		<xsl:value-of select="substring-before($text, '&#xa;')"/>
		<br/>
		<xsl:call-template name="break">
			<xsl:with-param name="text" select="substring-after($text,'&#xa;')"/>
		</xsl:call-template>
	</xsl:when>
	<xsl:otherwise>
		<xsl:value-of select="$text"/>
	</xsl:otherwise>
	</xsl:choose>
</xsl:template>

</xsl:stylesheet>

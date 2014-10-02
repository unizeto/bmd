--
-- DB2SQL database
--
-- create database awizo automatic storage yes using codeset UTF-8 territory PL

CREATE TABLE mailing_message_senders (
    id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE) primary key,
    name varchar(20),
    email varchar(40),
    created timestamp with DEFAULT CURRENT TIMESTAMP NOT NULL,
    deleted timestamp,
    smtpaddr varchar(40),
    smtpport varchar(5),
    smtpuser varchar(20),
    smtppassword varchar(20),
    pfxfile varchar(2500),
    pfxpin varchar(20),
    allowimageabsent integer,
    maximagesize varchar(8),
    csvdir varchar(64),
    csvseparator varchar(4),
    awizoaction integer,
    smsapiurl varchar(40),
    smsapierror varchar(20)
);

CREATE TABLE mailing_email_template (
    id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE) primary key,
    created timestamp with DEFAULT CURRENT TIMESTAMP NOT NULL,
    deleted timestamp,
    messagesenderid integer NOT NULL references mailing_message_senders(id),
    name varchar(20),
    lang varchar(3) default 'PL',
    version varchar(10),
    description varchar(128),
    xsltohtml varchar(3200),
    xsltotext varchar(3200),
    sign integer DEFAULT 1
);

CREATE TABLE mailing_email_attachments (
    id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE) primary key,
    created timestamp with DEFAULT CURRENT TIMESTAMP NOT NULL,
    deleted timestamp,
    templateid integer NOT NULL references mailing_email_template(id),
    filename varchar(255),
    mimetype varchar(127),
    disposition varchar(127),
    content varchar(32000)
);

CREATE TABLE raports (
    id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE) primary key,
    msgid varchar(256),
    msg_sender varchar(256),
    msg_recipient varchar(256),
    composition_status integer,
    composition_time timestamp with DEFAULT CURRENT TIMESTAMP NOT NULL,
    template_name varchar(256),
    template_id integer,
    msg_type integer,
    sending_status integer,
    sending_time timestamp,
    attempt_of_sending_awizo integer DEFAULT 0,
    awizo_info varchar(256)
);

CREATE INDEX min_key_index
  ON mailing_email_template(messagesenderid);
CREATE INDEX full_key_index
  ON mailing_email_template(messagesenderid, name, lang, version);
  
CREATE TABLE config
(
  id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE) primary key,
  mailqueue integer,
  maxcachesize integer,
  maxmailqueue integer,
  sendingtime integer
);

CREATE TABLE buffer
(
  id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE) primary key,
  message long varchar,
  created timestamp with DEFAULT CURRENT TIMESTAMP NOT NULL,                                                                                              
  deleted timestamp,
  date_time timestamp,
  status integer
);

CREATE TABLE awizo_servers
(
  id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE) primary key,
  host varchar(128),
  port varchar(6)
);

INSERT INTO awizo_servers(host, port) values ('127.0.0.1', '2051');

INSERT INTO config(mailqueue, maxcachesize, maxmailqueue, sendingtime) VALUES (0, 10, 10, 120);

/*sender system*/
INSERT INTO mailing_message_senders(name, email, created, smtpaddr, smtpport, smtpuser, smtppassword, pfxfile, 
pfxpin, maximagesize, allowimageabsent, awizoaction, smsapiurl, smsapierror) 
VALUES ('system', 'pawel.maciejewski@spmtdbd03', CURRENT DATE, '127.0.0.1', '25', 'pawel.maciejewski', 'ives098q', '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',
'12345678', '123', 1, 1, '', 'ERROR');

/*szablony systemowe*/
INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Rejestracja Biura', 'PL', '1.0', 'Rejestracja Biura', '<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body>
    <h3>Witaj <xsl:value-of select="//METADATA/NAME"/>&#160;<xsl:value-of select="//METADATA/SURNAME"/>,</h3>
    <p>
    	<b>SOE</b> to nowoczesne i proste spojrzenie na fakturę elektroniczną, które zakłada, że używanie faktur 
    	elektronicznych może być przyjazne, nieskomplikowane i intuicyjne.
    </p>
    <p>
    	Dziękujemy za zaufanie i rejestrację w portalu <b>Systemu Obsługi e-Faktur</b>. W systemie będziesz
		mógł między innymi:
		<ul style="">
			<li>odbierać wystawione dla Twojego klienta faktury elektroniczne;</li>
			<li>odebrać korekty do faktur elektronicznych oraz zaakceptować ich treść;</li>
			<li>dokonać weryfikacji podpisu elektronicznego umieszczonego pod fakturami;</li>
			<li>udostępniać faktury wybranym organom kontroli skarbowej i podatkowej.</li>
		</ul>
    </p>
    <p>
    Aby zalogować się do systemu przejdź na stronę <u><xsl:value-of select="//METADATA/LOGINDOMAIN"/></u>. Dane
	uwierzytelniające do systemu to:
	<br/><br/>
	Nazwa użytkownika: <xsl:value-of select="//METADATA/LOGIN"/><br/>
	Hasło użytkownika: <xsl:value-of select="//METADATA/PASSWORD"/>
    </p>
    <p>
    	Po pierwszym zalogowaniu się będziesz mógł dokonać zmiany hasła.
    </p>
    <p>
    	Pozdrawiamy,<br/>
		Zespół Systemu Obsługi e-Faktur <br/>
		Unizeto Technologies SA<br/>
    </p>
    <p style="font-size: 9px;">
    	Wiadomość wysłana przez Unizeto Technologies SA, z siedzibą i adresem w Szczecinie, ul. Królowej Korony Polskiej 21, 70-486 Szczecin,
		wpisana do rejestru przedsiębiorców Krajowego Rejestru Sądowego pod numerem KRS 0000233499, prowadzonego przez Sąd Rejonowy w
		Szczecinie, XVII Wydział Gospodarczy Krajowego Rejestru Sądowego, N IP 852-000-64-44, REGON 810404880, której kapitał zakładowy
		wynosi: 5 600 000,00 PLN.
    </p>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
', '<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">Dziekujemy za rejestracje w SOE. Twoj login: <xsl:value-of select="//METADATA/LOGIN"/>; haslo: <xsl:value-of select="//METADATA/PASSWORD"/></xsl:template>
</xsl:stylesheet>', 0);

INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Raport udostepnien', 'PL', '1.0', 'E-mail wysyłany do wystawcy po automatycznym udostępnieniu faktur', '<xsl:stylesheet version = "1.0" encoding="UTF-8" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
<xsl:output method="xml" version="4.0" encoding="UTF-8" omit-xml-declaration="yes"  doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" indent="yes"/>

<xsl:template match="DOCUMENT">
<html xml:lang="en" lang="en">
<head>
  <meta http-equiv="content-type" content="application/xhtml+xml; charset=UTF-8" />
  <meta name="author" content="Unizeto Technologies SA" />
  <title>Raport udostępnień</title>
</head>
 <body>
 	<h4>Raport automatycznego udostępniania faktur</h4>
 	<p>Rozpoczęty: <xsl:value-of select="//START" /></p>
 	<p>Zakończony: <xsl:value-of select="//STOP" /></p>
 	<p>Ilość faktur do udostępnienia: <xsl:value-of select="//TOTAL" /></p>
 	<p>Udostępnienia zakończone powodzeniem: <xsl:value-of select="//SHARED" /></p>
 	<xsl:if test="count(//ERRORS/ITEM)>0">
 		<br/><br/>
 		<p>Wystąpiły następujące błędy:</p>
 		<table>
 			<tr>
 				<th>L.p.</th>
 				<th>Id faktury</th>
 				<th>Numer faktury</th>
 				<th>Adres e-mail</th>
 				<th>Komunikat</th>
 			</tr>
 		<xsl:for-each select="//ERRORS/ITEM">
 			<tr>
 				<td><xsl:value-of select="LP" /></td>
 				<td><xsl:value-of select="INVOICEID" /></td>
 				<td><xsl:value-of select="SINSEQ" /></td>
 				<td><xsl:value-of select="EMAIL" /></td>
 				<td><xsl:value-of select="MESSAGE" /></td>
 			</tr>
 		</xsl:for-each>
 		</table> 
 	</xsl:if>
 </body>
</html>
</xsl:template>
</xsl:stylesheet>', '', 0);

INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Rejestracja Wystawcy', 'PL', '1.0', 'Rejestracja Wystawcy', '<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body>
    <h3>Witaj <xsl:value-of select="//METADATA/NAME"/>&#160;<xsl:value-of select="//METADATA/SURNAME"/>,</h3>
    <p>
    	<b>SOE</b> to nowoczesne i proste spojrzenie na fakturę elektroniczną, które zakłada, że używanie faktur 
    	elektronicznych może być przyjazne, nieskomplikowane i intuicyjne.
    </p>
    <p>
    	Dziękujemy za zaufanie i rejestrację w portalu <b>Systemu Obsługi e-Faktur</b>. W systemie będziesz
		mógł między innymi:
		<ul style="">
			<li>udostępniać faktury elektroniczne dla Twoich klientów;</li>
			<li>wystawiać korekty do faktur elektronicznych;</li>
			<li>przeglądać noty korygujące do faktur wystawione przez Twoich klientów;</li>
			<li>zaimportować i przechowywać faktury wystawione przez innych wystawców;</li>
			<li>dokonać weryfikacji podpisu elektronicznego umieszczonego pod fakturami;</li>
			<li>udostępniać faktury swojemu biuru rachunkowemu;</li>
			<li>udostępniać faktury wybranym organom kontroli skarbowej i podatkowej.</li>
		</ul>
    </p>
    <p>
    Aby zalogować się do systemu przejdź na stronę <u><xsl:value-of select="//METADATA/LOGINDOMAIN"/></u>. Dane
	uwierzytelniające do systemu to:
	<br/><br/>
	Nazwa użytkownika: <xsl:value-of select="//METADATA/LOGIN"/><br/>
	<!--Hasło użytkownika: <xsl:value-of select="//METADATA/PASSWORD"/> -->
    </p>
    <p>
    	Po pierwszym zalogowaniu się będziesz mógł dokonać zmiany hasła.
    </p>
    <p>
    	Pozdrawiamy,<br/>
		Zespół Systemu Obsługi e-Faktur <br/>
		Unizeto Technologies SA<br/>
    </p>
    <p style="font-size: 9px;">
    	Wiadomość wysłana przez Unizeto Technologies SA, z siedzibą i adresem w Szczecinie, ul. Królowej Korony Polskiej 21, 70-486 Szczecin,
		wpisana do rejestru przedsiębiorców Krajowego Rejestru Sądowego pod numerem KRS 0000233499, prowadzonego przez Sąd Rejonowy w
		Szczecinie, XVII Wydział Gospodarczy Krajowego Rejestru Sądowego, N IP 852-000-64-44, REGON 810404880, której kapitał zakładowy
		wynosi: 5 600 000,00 PLN.
    </p>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
', '', 0);

INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Elektroniczne awizo zbiorcze', 'PL', '1.0', 'Elektroniczne awizo zbiorcze', '<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body>
    <p>
    	Uprzejmie informujemy, iż <b><xsl:value-of select="//METADATA/STOP" /></b> zostały 
    	przez użytkownika <xsl:value-of select="//METADATA/SENDER_NAME"/> <xsl:value-of select="//METADATA/SENDER_SURNAME"/> (<xsl:value-of select="//METADATA/SENDER_EMAIL"/>)
    	udostępnione dla: <xsl:value-of select="//METADATA/FNAME"/> następujące elektroniczne Faktury:
    	
    	<table>
 			<tr>
 				<th>L.p.</th>
 				<th>Data wystawienia</th>
 				<th>Numer faktury</th>
 				<th>Termin płatności</th>
 				<th>Kwota brutto</th>
 			</tr>
    	<xsl:for-each select="//METADATA/FILES/ITEM">
 			<tr>
 				<td><xsl:value-of select="LP" /></td>
 				<td><xsl:value-of select="STATEDATE" /></td>
 				<td><xsl:value-of select="SINSEQ" /></td>
 				<td><xsl:value-of select="PAYDUEDT" /></td>
 				<td><xsl:value-of select="TOTINVDUE" /></td>
 			</tr>
    	</xsl:for-each>
 		</table> 
    </p>
    <p>
    	Udostępnione faktury znajdują się w portalu Systemu Obsługi e-Faktur na stronie
		<u><xsl:value-of select="//METADATA/LOGINDOMAIN"/></u> po uprzednim zalogowaniu.
    </p>
    <p>
		Dziękujemy za korzystanie z naszych usług.
    </p>
    <p>
    	Pozdrawiamy,<br/>
		Zespół Systemu Obsługi e-Faktur <br/>
		Unizeto Technologies SA<br/>
    </p>
    <p style="font-size: 9px;">
    	Wiadomość wysłana przez Unizeto Technologies SA, z siedzibą i adresem w Szczecinie, ul. Królowej Korony Polskiej 21, 70-486 Szczecin,
		wpisana do rejestru przedsiębiorców Krajowego Rejestru Sądowego pod numerem KRS 0000233499, prowadzonego przez Sąd Rejonowy w
		Szczecinie, XVII Wydział Gospodarczy Krajowego Rejestru Sądowego, N IP 852-000-64-44, REGON 810404880, której kapitał zakładowy
		wynosi: 5 600 000,00 PLN.
    </p>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
', NULL, 0);

INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Awaryjny Kod Logowania', 'PL', '1.0', 'Awaryjny Kod Logowania', '<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body style="font-family: Tahoma, Verdana, sans-serif; font-size: 12px;">
    <h3>Witaj <xsl:value-of select="//METADATA/NAME"/>&#160;<xsl:value-of select="//METADATA/SURNAME"/>,</h3>
    <p>
    	<b>SOE</b> to nowoczesne i proste spojrzenie na fakturę elektroniczną, które zakłada, że używanie faktur 
    	elektronicznych może być przyjazne, nieskomplikowane i intuicyjne.
    </p>
    <p>
    	Niniejszy kod pozwoli Ci na jednorazowe zalogowanie się do systemu bez użycia Twojego hasła. Kod jest aktywny
		jedynie przez godzinę od momentu wygenerowania. Aby awaryjnie zalogować się do systemu przejdź na stronę logowania
		i podaj poniższe dane uwierzytelniające do systemu:
	 </p>
	 <p>
		<strong>Strona logowania:</strong>&#160;<a><xsl:attribute name="href"><xsl:value-of select="//METADATA/REMINDDOMAIN"/></xsl:attribute><xsl:value-of select="//METADATA/REMINDDOMAIN"/></a><br/>
		<strong>Email / Login:</strong>&#160;<span style="font-family: monospace;"><xsl:value-of select="//METADATA/LOGIN"/></span><br/>
		<strong>Kod jednorazowy:</strong>&#160;<span style="font-family: monospace;"><xsl:value-of select="//METADATA/REMINDCODE"/></span>
    </p>
    <p>
    	Po zalogowaniu będzie możliwa <em>zmiana hasła</em>.
    </p>
    <p>
    	Pozdrawiamy,<br/>
		Zespół Systemu Obsługi e-Faktur <br/>
		Unizeto Technologies SA<br/>
    </p>
    <p style="font-size: 9px;">
    	Wiadomość wysłana przez Unizeto Technologies SA, z siedzibą i adresem w Szczecinie, ul. Królowej Korony Polskiej 21, 70-486 Szczecin,
		wpisana do rejestru przedsiębiorców Krajowego Rejestru Sądowego pod numerem KRS 0000233499, prowadzonego przez Sąd Rejonowy w
		Szczecinie, XVII Wydział Gospodarczy Krajowego Rejestru Sądowego, N IP 852-000-64-44, REGON 810404880, której kapitał zakładowy
		wynosi: 5 600 000,00 PLN.
    </p>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
', '<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">Twoj awaryjny kod logowania do SOE to: <xsl:value-of select="//METADATA/REMINDCODE"/></xsl:template>

</xsl:stylesheet>
', 0);

INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Powiadomienie o decyzji w sprawie zmiany danych', 'PL', '1.0', 'Powiadomienie o decyzji w sprawie zmiany danych', '<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body style="font-family: Tahoma, Verdana, sans-serif; font-size: 12px;">
    <h3>Witaj <xsl:value-of select="//METADATA/NAME"/>&#160;<xsl:value-of select="//METADATA/SURNAME"/>,</h3>
    <p>
    	Informujemy, że wniosek o zmianę danych osobowych został
    	<xsl:choose>
    		<xsl:when test="//METADATA/DECISION = ''1''"> 
				 zaakceptowany przez Administratora i żądane dane zostały zmienione.
			</xsl:when>
			<xsl:otherwise>
				odrzucony przez Administratora. Dane osobowe nie uległy zmianie.
			</xsl:otherwise>
		</xsl:choose>
    </p>
    <p>
    	Pozdrawiamy,<br/>
		Zespół Systemu Obsługi e-Faktur <br/>
		Unizeto Technologies SA<br/>
    </p>
    <p style="font-size: 9px;">
    	Wiadomość wysłana przez Unizeto Technologies SA, z siedzibą i adresem w Szczecinie, ul. Królowej Korony Polskiej 21, 70-486 Szczecin,
		wpisana do rejestru przedsiębiorców Krajowego Rejestru Sądowego pod numerem KRS 0000233499, prowadzonego przez Sąd Rejonowy w
		Szczecinie, XVII Wydział Gospodarczy Krajowego Rejestru Sądowego, N IP 852-000-64-44, REGON 810404880, której kapitał zakładowy
		wynosi: 5 600 000,00 PLN.
    </p>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
', '<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">Wniosek o zmianę danych osobowych został <xsl:choose><xsl:when test="//METADATA/DECISION = ''1''">zaakceptowany, dane zostały zmienione.</xsl:when><xsl:otherwise>odrzucony, dane nie uległy zmianie.</xsl:otherwise></xsl:choose></xsl:template>
</xsl:stylesheet>
', 0);

INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Powiadomienie o odrzuceniu biura', 'PL', '1.0', 'Powiadomienie o odrzuceniu wniosku o rejestrację biura', '<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body style="font-family: Tahoma, Verdana, sans-serif; font-size: 12px;">
    <h3>Witaj <xsl:value-of select="//METADATA/NAME"/>&#160;<xsl:value-of select="//METADATA/SURNAME"/>,</h3>
    <p>
    	Informujemy, że wniosek o rejestrację Biura Rachunkowego <xsl:value-of select="//METADATA/COMPANY"/> został odrzucony przez Administratora.<br/>
    	<xsl:if test="//METADATA/DECISION != ''''">
    	Uzasadnienie: <xsl:value-of select="//METADATA/DECISION"/>
    	</xsl:if>    	 
    </p>
    <p>
    	Pozdrawiamy,<br/>
		Zespół Systemu Obsługi e-Faktur <br/>
		Unizeto Technologies SA<br/>
    </p>
    <p style="font-size: 9px;">
    	Wiadomość wysłana przez Unizeto Technologies SA, z siedzibą i adresem w Szczecinie, ul. Królowej Korony Polskiej 21, 70-486 Szczecin,
		wpisana do rejestru przedsiębiorców Krajowego Rejestru Sądowego pod numerem KRS 0000233499, prowadzonego przez Sąd Rejonowy w
		Szczecinie, XVII Wydział Gospodarczy Krajowego Rejestru Sądowego, N IP 852-000-64-44, REGON 810404880, której kapitał zakładowy
		wynosi: 5 600 000,00 PLN.
    </p>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
', '<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">Wniosek o rejestracje Biura Rachunkowego zostal odrzucony.</xsl:template>
</xsl:stylesheet>
', 0);

INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Zaproszenie Biura', 'PL', '1.0', 'Zaproszenie Biura', '<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body>
    <p>
    	Ten e-mail został wysłany z Systemu Obsługi e-Faktur<br/> 
    	przez użytkownika <xsl:value-of select="//METADATA/SENDER_NAME"/>&#160;<xsl:value-of select="//METADATA/SENDER_SURNAME"/> (<xsl:value-of select="//METADATA/SENDER_EMAIL"/>),<br/>
    	który zaprasza do rejestracji konta Biura Rachunkowego.
    </p>
    <p>
    	Aby założyć konto w systemie e-Faktur jako Biuro Rachunkowe, <br/>
    	wypełnij wniosek o rejestrację, znajdujący się pod następującym adresem:<br/>
		<a href="{//METADATA/URL}" target="_blank">Rejestracja Biura</a><br/> 
		Konto zostanie aktywowane po akceptacji wniosku przez Administratora.
    </p>
    <p>
		Zapraszamy do korzystania z naszych usług.
    </p>
    <p>
    	Pozdrawiamy,<br/>
		Zespół Systemu Obsługi e-Faktur <br/>
		Unizeto Technologies SA<br/>
    </p>
    <p style="font-size: 9px;">
    	Wiadomość wysłana przez Unizeto Technologies SA, z siedzibą i adresem w Szczecinie, ul. Królowej Korony Polskiej 21, 70-486 Szczecin,
		wpisana do rejestru przedsiębiorców Krajowego Rejestru Sądowego pod numerem KRS 0000233499, prowadzonego przez Sąd Rejonowy w
		Szczecinie, XVII Wydział Gospodarczy Krajowego Rejestru Sądowego, N IP 852-000-64-44, REGON 810404880, której kapitał zakładowy
		wynosi: 5 600 000,00 PLN.
    </p>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>', NULL, 0);

INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Wycofanie wspolpracy z Biurem', 'PL', '1.0', 'Wycofanie wspolpracy z Biurem', '<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body>
  	<h3>Witaj <xsl:value-of select="//METADATA/NAME"/>&#160;<xsl:value-of select="//METADATA/SURNAME"/>,</h3>
    <p>
    	Informujemy, że użytkownik <xsl:value-of select="//METADATA/SENDER_NAME"/>&#160;<xsl:value-of select="//METADATA/SENDER_SURNAME"/> (<xsl:value-of select="//METADATA/SENDER_EMAIL"/>)<br/>
    	wycofał e-Faktury z Państwa konta i nie będzie udostępniał kolejnych e-Faktur do Biura <xsl:value-of select="//METADATA/COMPANY"/>. 
    </p>
    <p>
    	Pozdrawiamy,<br/>
		Zespół Systemu Obsługi e-Faktur <br/>
		Unizeto Technologies SA<br/>
    </p>
    <p style="font-size: 9px;">
    	Wiadomość wysłana przez Unizeto Technologies SA, z siedzibą i adresem w Szczecinie, ul. Królowej Korony Polskiej 21, 70-486 Szczecin,
		wpisana do rejestru przedsiębiorców Krajowego Rejestru Sądowego pod numerem KRS 0000233499, prowadzonego przez Sąd Rejonowy w
		Szczecinie, XVII Wydział Gospodarczy Krajowego Rejestru Sądowego, N IP 852-000-64-44, REGON 810404880, której kapitał zakładowy
		wynosi: 5 600 000,00 PLN.
    </p>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>', NULL, 0);

INSERT INTO mailing_email_template(created, messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign) 
VALUES (CURRENT DATE, 1, 'Raport wycofan', 'PL', '1.0', 'Raport wycofań udostępnień faktur', '<?xml version="1.0"?>
<xsl:stylesheet version = "1.0" encoding="UTF-8" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
<xsl:output method="xml" version="4.0" encoding="UTF-8" omit-xml-declaration="yes"  doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN" doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" indent="yes"/>

<xsl:template match="DOCUMENT">
<html xml:lang="en" lang="en">
<head>
  <meta http-equiv="content-type" content="application/xhtml+xml; charset=UTF-8" />
  <meta name="author" content="Unizeto Technologies SA" />
  <title>Raport wycofań</title>
</head>
 <body>
 	<h4>Raport wycofania faktur dla: <xsl:value-of select="//FNAME" /></h4>
 	<p>Rozpoczęty: <xsl:value-of select="//START" /></p>
 	<p>Zakończony: <xsl:value-of select="//STOP" /></p>
 	<p>Ilość faktur do wycofania: <xsl:value-of select="//TOTAL" /></p>
 	<p>Ilość wycofanych faktur: <xsl:value-of select="//SHARED" /></p>
 </body>
</html>
</xsl:template>
</xsl:stylesheet>', NULL, 0);

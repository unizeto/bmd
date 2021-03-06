CREATE TABLE KODY_PODSYSTEM (
		KODY_PODSYSTEM_ID INTEGER NOT NULL GENERATED ALWAYS AS IDENTITY ( START WITH 1 INCREMENT BY 1 MINVALUE 1 MAXVALUE 2147483647 NO CYCLE CACHE 20 NO ORDER ), 
		KOD VARCHAR(3) NOT NULL, 
		NAZWA VARCHAR(255) NOT NULL
	)
DATA CAPTURE NONE 
IN USERSPACE1
COMPRESS NO;

ALTER TABLE KODY_PODSYSTEM ADD CONSTRAINT SQL140825182947400 PRIMARY KEY (KODY_PODSYSTEM_ID);

ALTER TABLE KODY_PODSYSTEM ADD CONSTRAINT SQL140825182947550 UNIQUE (KOD);

INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('AIP','Aplikacja Innych Podmiotów');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('APL',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('ASR',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('AUA','Aplikacja Usługodawców i Aptek');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('HDM','Hurtownia Danych - Monitorowanie');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('IKP','Internetowe Konto Pacjenta');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('PPD',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('PPP',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('RDM',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SAA','System Adminstracji - Administracja');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SAU','System Administracji - Audyt');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SAZ','System Administracji - Zabezpieczenia i Prywatność');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SGO','System Gromadzenia Danych Medycznych - Osobista Dokumentacja Medyczna');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SGR','System Gromadzenia Danych Medycznych - Recepty');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SGS','System Gromadzenia Danych Medycznych - Skierowania i Zwolnienia');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SGZ','System Gromadzenia Danych Medycznych - Zdarzenia Medyczne');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SUS',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SRR',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SRS',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SWD',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('SWN','System Wykrywania Nadużyć');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('WRL',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('WRU',' ');
INSERT INTO KODY_PODSYSTEM(KOD, NAZWA) VALUES('ZDP',' ');


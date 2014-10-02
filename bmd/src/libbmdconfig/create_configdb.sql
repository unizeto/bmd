create database konfiguracja WITH ENCODING='UTF8';
\c konfiguracja
create table sections (id_sec SERIAL NOT NULL,
 section varchar UNIQUE NOT NULL,
 primary key(id_sec) );
create table options (id SERIAL NOT NULL,
 id_sec integer references sections(id_sec) ON DELETE CASCADE NOT NULL,
 option varchar NOT NULL,
 value varchar NOT NULL,
 primary key(id) ); 
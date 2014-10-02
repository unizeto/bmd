create database testconf WITH ENCODING='UTF8';
\c testconf
create table sections (id_sec SERIAL NOT NULL,
 section varchar UNIQUE NOT NULL,
 primary key(id_sec) );
create table options (id SERIAL NOT NULL,
 id_sec integer references sections(id_sec) ON DELETE CASCADE NOT NULL,
 option varchar NOT NULL,
 value varchar NOT NULL,
 primary key(id) ); 
insert into sections (section) values ('sekcja0');
insert into sections (section) values ('sekcja1');
insert into options (id_sec, option, value) values (1, 'sek0_opcja0', 'blabla');
insert into options (id_sec, option, value) values (1, 'sek0_opcja1', 'plik.txt');
insert into options (id_sec, option, value) values (1, 'sek0_opcja4', 'plik');
insert into options (id_sec, option, value) values (1, 'sek0_opcja5', 'inny');
insert into options (id_sec, option, value) values (1, 'sek0_opcja6', 'nie plik');
insert into options (id_sec, option, value) values (2, 'sek1_opcja0', 'terefere');
insert into options (id_sec, option, value) values (2, 'sek1_opcja1', 'plik.txt');
insert into options (id_sec, option, value) values (2, 'sek1_opcja4', 'plik');
insert into options (id_sec, option, value) values (2, 'sek1_opcja5', 'something else');
insert into options (id_sec, option, value) values (2, 'sek1_opcja6', 'whatever');
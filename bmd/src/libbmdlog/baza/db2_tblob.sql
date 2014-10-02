
-- niniejszy skrypt nalezy wykonac, jesli oprogramowanie wykorzystywac bedzie mechanizm zapisu BLOBow jako BYTEA a nie pg_large_objects

--DROP TABLE tblob CASCADE;
CREATE TABLE tblob (
	id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE),
	fk_co_raster_id bigint NOT NULL,
	bobj blob
)
;
COMMENT ON TABLE tblob
    IS 'Tabela zawierajaca fragmenty plikow lub calosci (w zaleznosci od przelacznika #define BYTEA_NON_STREAMED w libbmddb/postgres/pq_libbmddb.h'
;

----------------------------------------------------------------------------------------------------------------------------
-- ponizsze zmiany dostosowuja strukture bazy do zmiany z pg_large_object na BYTEA
----------------------------------------------------------------------------------------------------------------------------
ALTER TABLE hash ALTER COLUMN sig_value SET DATA TYPE bigint;
----------------------------------------------------------------------------------------------------------------------------
-- powyzsze zmiany dostosowuja strukture bazy do zmiany z pg_large_object na BYTEA
----------------------------------------------------------------------------------------------------------------------------

CREATE TABLE 	tblob_0 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=0 ), 
		bobj blob);
CREATE TABLE 	tblob_1 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=1 ), 
		bobj blob);
CREATE TABLE 	tblob_2 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=2 ), 
		bobj blob);
CREATE TABLE 	tblob_3 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=3 ), 
		bobj blob);
CREATE TABLE 	tblob_4 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=4 ), 
		bobj blob);
CREATE TABLE 	tblob_5 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=5 ), 
		bobj blob);
CREATE TABLE 	tblob_6 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=6 ), 
		bobj blob);
CREATE TABLE 	tblob_7 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=7 ), 
		bobj blob);
CREATE TABLE 	tblob_8 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=8 ), 
		bobj blob);
CREATE TABLE 	tblob_9 (id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE), 
		fk_co_raster_id bigint NOT NULL,
		CHECK ( fk_co_raster_id >= 1 AND fk_co_raster_id <= 9223372036854775807 AND mod(fk_co_raster_id,10)=9 ), 
		bobj blob);

CREATE INDEX tblob_0_index ON tblob_0(fk_co_raster_id);
CREATE INDEX tblob_1_index ON tblob_1(fk_co_raster_id);
CREATE INDEX tblob_2_index ON tblob_2(fk_co_raster_id);
CREATE INDEX tblob_3_index ON tblob_3(fk_co_raster_id);
CREATE INDEX tblob_4_index ON tblob_4(fk_co_raster_id);
CREATE INDEX tblob_5_index ON tblob_5(fk_co_raster_id);
CREATE INDEX tblob_6_index ON tblob_6(fk_co_raster_id);
CREATE INDEX tblob_7_index ON tblob_7(fk_co_raster_id);
CREATE INDEX tblob_8_index ON tblob_8(fk_co_raster_id);
CREATE INDEX tblob_9_index ON tblob_9(fk_co_raster_id);


                
CREATE OR REPLACE FUNCTION ins_tblob(rstr_id BIGINT, b_obj BLOB) RETURNS INTEGER AS $$
	DECLARE
    part INTEGER;
BEGIN
	part = mod(rstr_id,10);
	
	IF (rstr_id > 9223372036854775807) THEN
	
		RAISE EXCEPTION 'Błąd funkcji ins_tblob(). Warunek (rstr_id > 9223372036854775807) dla rastra %.', rstr_id;
		
	ELSIF (rstr_id >= 1) THEN
	
		IF (part = 0) THEN
			INSERT INTO tblob_0(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSIF (part = 1) THEN
			INSERT INTO tblob_1(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSIF (part = 2) THEN
			INSERT INTO tblob_2(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSIF (part = 3) THEN
			INSERT INTO tblob_3(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSIF (part = 4) THEN
			INSERT INTO tblob_4(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSIF (part = 5) THEN	
			INSERT INTO tblob_5(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSIF (part = 6) THEN
			INSERT INTO tblob_6(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSIF (part = 7) THEN
			INSERT INTO tblob_7(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSIF (part = 8) THEN
			INSERT INTO tblob_8(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSIF (part = 9) THEN
			INSERT INTO tblob_9(fk_co_raster_id,bobj) VALUES (rstr_id, b_obj);
		ELSE
			RAISE EXCEPTION 'Błąd funkcji ins_tblob(). Nieprawidlowe dzialanie modulo.';
		END IF;
	
	ELSE
		RAISE EXCEPTION 'Błąd funkcji ins_tblob() dla rastra %.', rstr_id;
	END IF;
	RETURN part;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION sel_tblob(rstr_id BIGINT) RETURNS SETOF RECORD AS $$
	DECLARE
	b_obj RECORD;
    part INTEGER;
BEGIN
	part = mod(rstr_id,10);
	
	IF (rstr_id > 9223372036854775807) THEN
	
		RAISE EXCEPTION 'Błąd funkcji sel_tblob(). Warunek (rstr_id > 9223372036854775807) dla rastra %.', rstr_id;
	
	ELSIF (rstr_id >= 1) THEN
	
		IF (part = 0) THEN
			SELECT bobj INTO b_obj FROM tblob_0 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSIF (part = 1) THEN
			SELECT bobj INTO b_obj FROM tblob_1 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSIF (part = 2) THEN
			SELECT bobj INTO b_obj FROM tblob_2 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSIF (part = 3) THEN
			SELECT bobj INTO b_obj FROM tblob_3 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSIF (part = 4) THEN
			SELECT bobj INTO b_obj FROM tblob_4 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSIF (part = 5) THEN	
			SELECT bobj INTO b_obj FROM tblob_5 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSIF (part = 6) THEN
			SELECT bobj INTO b_obj FROM tblob_6 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSIF (part = 7) THEN
			SELECT bobj INTO b_obj FROM tblob_7 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSIF (part = 8) THEN
			SELECT bobj INTO b_obj FROM tblob_8 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSIF (part = 9) THEN
			SELECT bobj INTO b_obj FROM tblob_9 WHERE fk_co_raster_id = rstr_id LIMIT 1;
		ELSE
			RAISE EXCEPTION 'Błąd funkcji sel_tblob(). Nierpawidlowe dzialanie modulo';
		END IF;
	
	ELSE
		RAISE EXCEPTION 'Błąd funkcji sel_tblob() dla rastra %.', rstr_id;
	END IF;
	
	IF FOUND THEN
		RETURN NEXT b_obj;
	END IF;
	RETURN ;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION ins_tblob_trig() RETURNS trigger AS $ins_tblob_trig$
	BEGIN
		PERFORM ins_tblob(NEW.fk_co_raster_id, NEW.bobj);
		RETURN NULL;
	END;
$ins_tblob_trig$ LANGUAGE plpgsql;


CREATE TRIGGER t_tblob_ins
 BEFORE INSERT
 ON tblob
 FOR EACH ROW
 EXECUTE PROCEDURE ins_tblob_trig();


CREATE SEQUENCE co_raster_id
 INCREMENT BY 1
 MINVALUE 1
 NOMAXVALUE --MAXVALUE 9223372036854775807
 START 1
 CACHE 1;


COMMIT TRANSACTION;

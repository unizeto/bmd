
/* Tabela uzywana przy testach OIDów */

CREATE TABLE crypto_objects (
	id			INTEGER	PRIMARY KEY,
	raster			oid,		-- oid (postgresowy) pliku zlozony do BMD jako large object
);
CREATE SEQUENCE crypto_objects_id_seq INCREMENT 1 MINVALUE 1;




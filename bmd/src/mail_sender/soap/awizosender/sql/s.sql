CREATE TABLE buffer
(
  id INT NOT NULL GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1, NO CACHE) primary key,
  message long varchar,
  created timestamp with DEFAULT CURRENT TIMESTAMP NOT NULL,                                                                                              
  deleted timestamp,
  date_time timestamp,
  status integer
);


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


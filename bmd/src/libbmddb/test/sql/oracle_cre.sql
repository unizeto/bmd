

PROMPT Creating Table 'OR_LARGEOBJECT'
CREATE TABLE OR_LARGEOBJECT
 (ID NUMBER(9) NOT NULL
 ,DATA BLOB
 )
/

PROMPT Creating Sequence 'OR_LARGEOBJECT_ID_SEQ'
CREATE SEQUENCE OR_LARGEOBJECT_ID_SEQ
 NOMAXVALUE
 NOMINVALUE
 NOCYCLE
/



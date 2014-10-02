
BEGIN;

CREATE SEQUENCE log_id_seq INCREMENT 1 MINVALUE 1;

CREATE TABLE log (
    id integer NOT NULL,
    remote_host character varying NOT NULL,
    remote_port character varying NOT NULL,
    date_time_begin timestamp without time zone,
    remote_pid character varying NOT NULL,
    src_file character varying,
    src_line character varying,
    src_function character varying,
    log_level character varying NOT NULL,
    log_owner character varying NOT NULL,
    log_referto character varying,
    service character varying NOT NULL,
    date_time_end timestamp without time zone NOT NULL,
    operation_type character varying NOT NULL,
    log_string character varying,
    log_reason character varying,
    log_solution character varying,
    document_size character varying,
    document_filename character varying,
    event_code character varying,
    log_owner_event_visible character varying,
    log_referto_event_visible character varying,
    user_data1 character varying,
    user_data2 character varying,
    user_data3 character varying,
    user_data4 character varying,
    user_data5 character varying,
    db_date_time_commit timestamp with time zone DEFAULT now() NOT NULL,
    verification_status integer,
    verification_date_time character varying,
    hash_value character varying
);



--
-- TOC entry 1513 (class 1259 OID 182024)
-- Dependencies: 3
-- Name: assoc; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE assoc (
    id integer NOT NULL,
    log_src integer,
    hash_src integer
);


--
-- TOC entry 1515 (class 1259 OID 182051)
-- Dependencies: 3
-- Name: events_table_behavior; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE events_table_behavior (
    id integer NOT NULL,
    events_info character varying(256) NOT NULL,
    q_fatal integer,
    v_fatal integer,
    q_deny integer,
    v_deny integer,
    q_done integer,
    v_done integer,
    q_try integer,
    v_try integer,
    q_debug integer,
    v_debug integer
);


--
-- TOC entry 1514 (class 1259 OID 182049)
-- Dependencies: 3
-- Name: events_table_behavior_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE events_table_behavior_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 1864 (class 0 OID 0)
-- Dependencies: 1514
-- Name: events_table_behavior_id_seq; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('events_table_behavior_id_seq', 1, false);


--
-- TOC entry 1512 (class 1259 OID 182013)
-- Dependencies: 3
-- Name: hash; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE hash (
    id integer NOT NULL,
    hash_value character varying NOT NULL,
    verification_status integer,
    verification_date_time character varying,
    sig_value oid
);


--
-- TOC entry 1511 (class 1259 OID 182011)
-- Dependencies: 3
-- Name: hash_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE hash_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 1865 (class 0 OID 0)
-- Dependencies: 1511
-- Name: hash_id_seq; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('hash_id_seq', 6516, true);






--
-- TOC entry 1866 (class 0 OID 0)
-- Dependencies: 1509
-- Name: log_id_seq; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('log_id_seq', 5370, true);


--
-- TOC entry 1517 (class 1259 OID 215607)
-- Dependencies: 1799 3
-- Name: report; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE report (
    id integer NOT NULL,
    name character varying(255),
    description text,
    label character varying(100),
    query text,
    kind character varying(50),
    css text,
    created timestamp without time zone DEFAULT now() NOT NULL,
    deleted timestamp without time zone,
    fieldnames character varying(500)
);


--
-- TOC entry 1516 (class 1259 OID 215605)
-- Dependencies: 3 1517
-- Name: report_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE report_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 1867 (class 0 OID 0)
-- Dependencies: 1516
-- Name: report_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE report_id_seq OWNED BY report.id;


--
-- TOC entry 1868 (class 0 OID 0)
-- Dependencies: 1516
-- Name: report_id_seq; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('report_id_seq', 20, true);


--
-- TOC entry 1519 (class 1259 OID 215655)
-- Dependencies: 1800 3
-- Name: reportparam; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE reportparam (
    id integer NOT NULL,
    reportid integer NOT NULL,
    label character varying(100) NOT NULL,
    description character varying(500),
    dict character varying(50),
    type character varying(50),
    created timestamp without time zone DEFAULT now() NOT NULL,
    deleted timestamp without time zone,
    number integer
);


--
-- TOC entry 1518 (class 1259 OID 215653)
-- Dependencies: 1519 3
-- Name: reportparam_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE reportparam_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 1869 (class 0 OID 0)
-- Dependencies: 1518
-- Name: reportparam_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE reportparam_id_seq OWNED BY reportparam.id;


--
-- TOC entry 1870 (class 0 OID 0)
-- Dependencies: 1518
-- Name: reportparam_id_seq; Type: SEQUENCE SET; Schema: public; Owner: -
--

SELECT pg_catalog.setval('reportparam_id_seq', 25, true);


--
-- TOC entry 1798 (class 2604 OID 215610)
-- Dependencies: 1517 1516 1517
-- Name: id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE report ALTER COLUMN id SET DEFAULT nextval('report_id_seq'::regclass);


--
-- TOC entry 1801 (class 2604 OID 215680)
-- Dependencies: 1519 1518 1519
-- Name: id; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE reportparam ALTER COLUMN id SET DEFAULT nextval('reportparam_id_seq'::regclass);


--
-- TOC entry 1855 (class 0 OID 182051)
-- Dependencies: 1515
-- Data for Name: events_table_behavior; Type: TABLE DATA; Schema: public; Owner: -
--

INSERT INTO events_table_behavior values(0,'Błędy połączenia sieciowego',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(1,'Błąd zbindowania gniazda',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(2,'Błąd rozpoczęcia działania serwera',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(3,'Błąd uruchomienia pluginu',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(4,'Błąd zalogowania',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(5,'Błąd obsłużenia żądania',1,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(6,'Dowolne błędy zwracane przez funkcje',0,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(7,'Błąd obsługi datagramu',0,1,0,0,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(8,'Brak dostępu na poziomie logowania',0,0,1,1,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(9,'Niepoprawne dane',0,0,0,1,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(10,'Błąd weryfikacji ścieżki certyfikatów',0,0,0,1,0,0,0,0,0,0);
INSERT INTO events_table_behavior values(11,'Poprawne zalogowanie',0,0,0,0,1,1,0,0,0,0);
INSERT INTO events_table_behavior values(12,'Poprawne obsłużenie żądania',0,0,0,0,1,1,0,0,0,0);
INSERT INTO events_table_behavior values(13,'Poprawne wykonanie działania pluginu',0,0,0,0,1,1,0,0,0,0);
INSERT INTO events_table_behavior values(14,'Poprawne rozpoczęcie działania serwera',0,0,0,0,1,1,0,0,0,0);
INSERT INTO events_table_behavior values(15,'Poprawne obsłużenie datagramu',0,0,0,0,0,1,0,0,0,0);
INSERT INTO events_table_behavior values(16,'Rozpoczęcie działania serwera',0,0,0,0,0,0,1,1,0,0);
INSERT INTO events_table_behavior values(17,'Próba rozpoczęcia działania pluginu',0,0,0,0,0,0,1,1,0,0);
INSERT INTO events_table_behavior values(18,'Próba obsłużenia żądania',0,0,0,0,0,0,1,1,0,0);
INSERT INTO events_table_behavior values(19, 'Próba zalogowania do BMD',0,0,0,0,0,0,1,1,0,0);
INSERT INTO events_table_behavior values(20,'Próba obsłużenia datagramu',0,0,0,0,0,0,0,1,0,0);
INSERT INTO events_table_behavior values(21,'Informowanie o rozpoczęciu działania funkcji',0,0,0,0,0,0,0,0,1,1);
INSERT INTO events_table_behavior values(22,'Zapytanie wysyłane do bazy danych',0,0,0,0,0,0,0,0,1,1);


--
-- TOC entry 1856 (class 0 OID 215607)
-- Dependencies: 1517
-- Data for Name: report; Type: TABLE DATA; Schema: public; Owner: -
--

INSERT INTO report (id, name, description, label, query, kind, css, created, deleted, fieldnames) VALUES (10, 'Częstość odwiedzania poszczególnych podstron', 'Ten raport przedstawia zestawienie wejść na poszczególne podstrony witryny IKP', 'Odwiedziny stron', 'SELECT 
  regexp_replace(log_reason,''/[0-9]+/?'', ''''),
  count(*)
FROM
  log
WHERE 
  date_time_begin::date >= ''$1''
  AND date_time_begin::date <= ''$2''
GROUP BY
  regexp_replace(log_reason,''/[0-9]+/?'', '''')
ORDER BY
  count(*) DESC;', 'kołowy', NULL, '2011-03-04 13:05:30.699204', NULL, 'Adres strony,Ilość wyświetleń');
INSERT INTO report (id, name, description, label, query, kind, css, created, deleted, fieldnames) VALUES (9, 'Zestawienie użytkowników zalogowanych i wylogowanych z systemu', 'Ten raport obrazuje logowanie użytkowników do systemu i ich wylogowywanie w bieżącym miesiącu.', 'Logowanie/wylogowywanie', 'SELECT
  (SELECT
      count(*)
    FROM
      log
    WHERE
      date_time_begin::date >= ''$1''
      AND date_time_begin::date <= ''$2''
      AND event_code = ''505'') AS zalogowani,
  (SELECT
      count(*)
    FROM
      log
    WHERE
      date_time_begin::date >= ''$1''
      AND date_time_begin::date <= ''$2''
      AND event_code = ''506'') AS wylogowani;', 'koĹ‚owy', NULL, '2011-03-04 12:40:17.309385', NULL, 'Użytkownicy zalogowani,Użytkownicy wylogowani');
INSERT INTO report (id, name, description, label, query, kind, css, created, deleted, fieldnames) VALUES (1, 'Logowanie użytkowników do systemu', 'Ten raport obrazuje logowanie użytkownikĂłw do systemu każdego dnia', 'Logowanie', 'SELECT
  date_time_begin::date,
  count(*)
FROM
  log
WHERE
  date_time_begin >= ''2011-02-01''
  AND event_code = ''505''
GROUP BY 
  date_time_begin::date
ORDER BY
  date_time_begin::date', 'słupkowy', NULL, '2011-03-02 15:56:20.790663', NULL, 'Data,Liczba zalogowań danego dnia');


--
-- TOC entry 1857 (class 0 OID 215655)
-- Dependencies: 1519
-- Data for Name: reportparam; Type: TABLE DATA; Schema: public; Owner: -
--

INSERT INTO reportparam (id, reportid, label, description, dict, type, created, deleted, number) VALUES (13, 9, 'Data od', NULL, '', 'date', '2011-03-08 10:41:15.227328', NULL, 1);
INSERT INTO reportparam (id, reportid, label, description, dict, type, created, deleted, number) VALUES (14, 9, 'Data do', NULL, '', 'date', '2011-03-08 10:43:19.469905', NULL, 2);
INSERT INTO reportparam (id, reportid, label, description, dict, type, created, deleted, number) VALUES (15, 10, 'Data od', NULL, '', 'date', '2011-03-08 10:44:51.397915', NULL, 1);
INSERT INTO reportparam (id, reportid, label, description, dict, type, created, deleted, number) VALUES (16, 10, 'Data do', NULL, '', 'date', '2011-03-08 10:44:58.517744', NULL, 2);


--
-- TOC entry 1859 (class 0 OID 0)
-- Dependencies: 1858
-- Data for Name: BLOB COMMENTS; Type: BLOB COMMENTS; Schema: -; Owner: -
--


SET search_path = public, pg_catalog;

--
-- TOC entry 1836 (class 2606 OID 182030)
-- Dependencies: 1513 1513
-- Name: assoc_hash_src_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY assoc
    ADD CONSTRAINT assoc_hash_src_key UNIQUE (hash_src);


--
-- TOC entry 1840 (class 2606 OID 182028)
-- Dependencies: 1513 1513
-- Name: assoc_log_src_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY assoc
    ADD CONSTRAINT assoc_log_src_key UNIQUE (log_src);


--
-- TOC entry 1842 (class 2606 OID 182055)
-- Dependencies: 1515 1515
-- Name: events_table_behavior_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY events_table_behavior
    ADD CONSTRAINT events_table_behavior_pkey PRIMARY KEY (id);


--
-- TOC entry 1831 (class 2606 OID 182020)
-- Dependencies: 1512 1512
-- Name: hash_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY hash
    ADD CONSTRAINT hash_pkey PRIMARY KEY (id);


--
-- TOC entry 1813 (class 2606 OID 181985)
-- Dependencies: 1510 1510
-- Name: log_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY log
    ADD CONSTRAINT log_pkey PRIMARY KEY (id);


--
-- TOC entry 1845 (class 2606 OID 215612)
-- Dependencies: 1517 1517
-- Name: report_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY report
    ADD CONSTRAINT report_pkey PRIMARY KEY (id);


--
-- TOC entry 1847 (class 2606 OID 215660)
-- Dependencies: 1519 1519
-- Name: reportparameters_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY reportparam
    ADD CONSTRAINT reportparameters_pkey PRIMARY KEY (id);


--
-- TOC entry 1834 (class 1259 OID 182048)
-- Dependencies: 1513
-- Name: assoc_hash_src_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE UNIQUE INDEX assoc_hash_src_idx ON assoc USING btree (hash_src);


--
-- TOC entry 1837 (class 1259 OID 182046)
-- Dependencies: 1513
-- Name: assoc_id_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX assoc_id_idx ON assoc USING btree (id);


--
-- TOC entry 1838 (class 1259 OID 182047)
-- Dependencies: 1513
-- Name: assoc_log_src_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE UNIQUE INDEX assoc_log_src_idx ON assoc USING btree (log_src);


--
-- TOC entry 1802 (class 1259 OID 181990)
-- Dependencies: 1510
-- Name: date_time_begin_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX date_time_begin_idx ON log USING btree (date_time_begin);


--
-- TOC entry 1803 (class 1259 OID 182002)
-- Dependencies: 1510
-- Name: date_time_end_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX date_time_end_idx ON log USING btree (date_time_end);


--
-- TOC entry 1804 (class 1259 OID 181987)
-- Dependencies: 1510
-- Name: db_date_time_commit_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE UNIQUE INDEX db_date_time_commit_idx ON log USING btree (db_date_time_commit);


--
-- TOC entry 1805 (class 1259 OID 182008)
-- Dependencies: 1510
-- Name: document_filename_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX document_filename_idx ON log USING btree (document_filename);


--
-- TOC entry 1806 (class 1259 OID 182007)
-- Dependencies: 1510
-- Name: document_size_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX document_size_idx ON log USING btree (document_size);


--
-- TOC entry 1807 (class 1259 OID 181998)
-- Dependencies: 1510
-- Name: event_code_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX event_code_idx ON log USING btree (event_code);


--
-- TOC entry 1843 (class 1259 OID 182056)
-- Dependencies: 1515
-- Name: evnets_table_behavior_id_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX evnets_table_behavior_id_idx ON events_table_behavior USING btree (id);


--
-- TOC entry 1829 (class 1259 OID 182021)
-- Dependencies: 1512
-- Name: hash_id_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE UNIQUE INDEX hash_id_idx ON hash USING btree (id);


--
-- TOC entry 1832 (class 1259 OID 182023)
-- Dependencies: 1512
-- Name: hash_verification_date_time_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX hash_verification_date_time_idx ON hash USING btree (verification_date_time);


--
-- TOC entry 1833 (class 1259 OID 182022)
-- Dependencies: 1512
-- Name: hash_verification_status_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX hash_verification_status_idx ON hash USING btree (verification_status);


--
-- TOC entry 1808 (class 1259 OID 181986)
-- Dependencies: 1510
-- Name: log_id_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE UNIQUE INDEX log_id_idx ON log USING btree (id);


--
-- TOC entry 1809 (class 1259 OID 181995)
-- Dependencies: 1510
-- Name: log_level_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_level_idx ON log USING btree (log_level);


--
-- TOC entry 1810 (class 1259 OID 181999)
-- Dependencies: 1510
-- Name: log_owner_event_visible_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_owner_event_visible_idx ON log USING btree (log_owner_event_visible);


--
-- TOC entry 1811 (class 1259 OID 181996)
-- Dependencies: 1510
-- Name: log_owner_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_owner_idx ON log USING btree (log_owner);


--
-- TOC entry 1814 (class 1259 OID 182005)
-- Dependencies: 1510
-- Name: log_reason_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_reason_idx ON log USING btree (log_reason);


--
-- TOC entry 1815 (class 1259 OID 182000)
-- Dependencies: 1510
-- Name: log_referto_event_visible; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_referto_event_visible ON log USING btree (log_referto_event_visible);


--
-- TOC entry 1816 (class 1259 OID 181997)
-- Dependencies: 1510
-- Name: log_referto_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_referto_idx ON log USING btree (log_referto);


--
-- TOC entry 1817 (class 1259 OID 182006)
-- Dependencies: 1510
-- Name: log_solution_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_solution_idx ON log USING btree (log_solution);


--
-- TOC entry 1818 (class 1259 OID 182004)
-- Dependencies: 1510
-- Name: log_string_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_string_idx ON log USING btree (log_string);


--
-- TOC entry 1819 (class 1259 OID 182010)
-- Dependencies: 1510
-- Name: log_verification_date_time_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_verification_date_time_idx ON log USING btree (verification_date_time);


--
-- TOC entry 1820 (class 1259 OID 182009)
-- Dependencies: 1510
-- Name: log_verification_status_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX log_verification_status_idx ON log USING btree (verification_status);


--
-- TOC entry 1821 (class 1259 OID 182003)
-- Dependencies: 1510
-- Name: operation_type_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX operation_type_idx ON log USING btree (operation_type);


--
-- TOC entry 1822 (class 1259 OID 181988)
-- Dependencies: 1510
-- Name: remote_host_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX remote_host_idx ON log USING btree (remote_host);


--
-- TOC entry 1823 (class 1259 OID 181991)
-- Dependencies: 1510
-- Name: remote_pid_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX remote_pid_idx ON log USING btree (remote_pid);


--
-- TOC entry 1824 (class 1259 OID 181989)
-- Dependencies: 1510
-- Name: remote_port_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX remote_port_idx ON log USING btree (remote_port);


--
-- TOC entry 1825 (class 1259 OID 182001)
-- Dependencies: 1510
-- Name: service_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX service_idx ON log USING btree (service);


--
-- TOC entry 1826 (class 1259 OID 181992)
-- Dependencies: 1510
-- Name: src_file_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX src_file_idx ON log USING btree (src_file);


--
-- TOC entry 1827 (class 1259 OID 181994)
-- Dependencies: 1510
-- Name: src_function_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX src_function_idx ON log USING btree (src_function);


--
-- TOC entry 1828 (class 1259 OID 181993)
-- Dependencies: 1510
-- Name: src_line_idx; Type: INDEX; Schema: public; Owner: -
--

CREATE INDEX src_line_idx ON log USING btree (src_line);


--
-- TOC entry 1850 (class 2606 OID 182041)
-- Dependencies: 1513 1830 1512
-- Name: assoc_hash_src_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY assoc
    ADD CONSTRAINT assoc_hash_src_fkey FOREIGN KEY (hash_src) REFERENCES hash(id);


--
-- TOC entry 1848 (class 2606 OID 182031)
-- Dependencies: 1512 1830 1513
-- Name: assoc_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY assoc
    ADD CONSTRAINT assoc_id_fkey FOREIGN KEY (id) REFERENCES hash(id);


--
-- TOC entry 1849 (class 2606 OID 182036)
-- Dependencies: 1510 1513 1812
-- Name: assoc_log_src_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY assoc
    ADD CONSTRAINT assoc_log_src_fkey FOREIGN KEY (log_src) REFERENCES log(id);


--
-- TOC entry 1851 (class 2606 OID 215661)
-- Dependencies: 1519 1844 1517
-- Name: reportparameters_reportid_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY reportparam
    ADD CONSTRAINT reportparameters_reportid_fkey FOREIGN KEY (reportid) REFERENCES report(id);


-- Completed on 2011-03-10 14:23:57

--
-- PostgreSQL database dump complete
--

COMMIT;

#include <bmd/libbmdutils/libbmdutils.h>


/*
============================ parse_errors_file ==============================
*/

START_TEST(libbmdutils_bmd_new)
{
    struct test_struct_t {
	char b;
    } * a;

    fail_unless(a=bmd_new(sizeof(struct test_struct_t),2345678), NULL);

    bmd_vfy(a,2345678);

    a->b = 123;
    bmd_vfy(a,2345678);
    bmd_free(a);
    fail_if (a!=NULL);

    fail_unless(a=bmd_new(sizeof(struct test_struct_t),2345678), NULL);
    a->b = 123;
    bmd_vfy(a,2345678);
}
END_TEST

START_TEST(libbmdutils_parse_error_file_null_args)
{
    fail_if(parse_errors_file(NULL, NULL, NULL) == KLIENT_CONF_OK,
	    "Funkcja parse_errors_file() przyjęła argumenty o wartości NULL!");
}
END_TEST


START_TEST(libbmdutils_parse_error_file_nxfile)
{
	error_info *ei;
	struct mallinfo old,new;
	int err_num;

	old = mallinfo();
    fail_unless(
		parse_errors_file("/tmp/non-existing-file", &ei, &err_num) ==
			KLIENT_CONF_FILE_ERROR_MEMORY,
	    "Funkcja parse_errors_file() otworzyła nie istniejący plik!");
	new=mallinfo();
	fail_if (CHECK_LEAKS(old,new) == 0,NULL);
}
END_TEST

START_TEST(libbmdutils_parse_error_file_shadow)
{
	error_info *ei;
	int err_num;
    fail_unless(
		parse_errors_file("/etc/shadow", &ei, &err_num) ==
			KLIENT_CONF_FILE_ERROR_MEMORY,
	    "Funkcja parse_errors_file() otworzyła nie istniejący plik!");
}
END_TEST

START_TEST(libbmdutils_kontrolki_NULL)
{
    line_to_BMD_attr(0,0,0,0,0,0);
	file_content_to_BMD_attr_values (0,0,0,0,0);
	fail_unless (parse_file(0,0,0) == KLIENT_CONF_OK, NULL);
}
END_TEST

START_TEST(libbmdutils_kontrolki_parse_file_nxfile)
{
	BMD_attr ** attrs;
	int * attr_no;
	fail_unless(parse_file("/not/existing/file/in/nx/dir",&attrs,attr_no) ==
		KLIENT_CONF_FILE_ERROR_MEMORY, NULL);
}
END_TEST

START_TEST(libbmdutils_kontrolki_parse_file_passwd)
{
	BMD_attr ** attrs=NULL;
	int attr_no;
	fail_unless(parse_file("/etc/passwd",&attrs,&attr_no) != KLIENT_CONF_OK, NULL);
}
END_TEST

START_TEST(libbmdutils_kontrolki_duplicate_oid)
{
	OID_attr_t oid={}, oid2={};
	fail_if (duplicate_oid(NULL, NULL)==0, NULL);

	fail_if (duplicate_oid(NULL, &oid)==0, NULL);
	fail_if (duplicate_oid(&oid, NULL)==0, NULL);

	oid.oid_table = NULL;
	oid.oid_table_size = 10;
	fail_if (duplicate_oid(&oid, &oid2)==0, NULL);

	oid.oid_table = (long*) malloc(10*sizeof(unsigned long int));
	oid.oid_table_size = 0;
	fail_if (duplicate_oid(&oid, &oid2)==0, NULL);

	oid.oid_table = (long*) malloc(100);
	oid.oid_table_size = 10;
	fail_if (duplicate_oid(&oid, &oid2)!=0, NULL);

	fail_if (oid.oid_table_size != oid2.oid_table_size, NULL);

	/* @todo kopiujemy za dużo danych więc się wywraca; uzgodnić rozmiar danych
		w kontrolki_utils.c i poprawić test */
	fail_if (memcmp(oid.oid_table, oid2.oid_table,
		oid.oid_table_size*sizeof(unsigned long int))!=0,NULL);
}
END_TEST


Suite * libbmdutils_test_suite(void)
{
    Suite *s = suite_create("libbmdutils");
    TCase *testcase = tcase_create("libbmdutils kontrolki_utils");

    /* Przypisujemy przypadek testowy do zbioru testów */
    suite_add_tcase(s, testcase);
    /* Dodajemy funkcje inicjalizujaca */

    /* dodajemy testy do przypadku testowego */
    tcase_add_test(testcase, libbmdutils_bmd_new);
    tcase_add_test(testcase, libbmdutils_parse_error_file_null_args);
    tcase_add_test(testcase, libbmdutils_parse_error_file_nxfile);
    tcase_add_test(testcase, libbmdutils_parse_error_file_shadow);
    tcase_add_test(testcase, libbmdutils_kontrolki_NULL);
    tcase_add_test(testcase, libbmdutils_kontrolki_parse_file_nxfile);
    tcase_add_test(testcase, libbmdutils_kontrolki_duplicate_oid);
	return s;
}

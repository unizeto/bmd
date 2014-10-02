#include <bmd/libbmdnet/libbmdnet.h>

/* 
================================ BMDNET -> OPENSSL_BIO METHOD ==================================
*/
/** Connection handler */

static bmdnet_handler_t libbmdnet_test_hdl;
/* 
Single function tests
*/

/* 
Full protocol tests 
*/

START_TEST(bmdnet_connect_wp_pl_80)
{
/* This test case checks whole connection */
    const char tosend[] = "GET / HTTP/1.1\nHost: www.wp.pl\n\n";
    char received[65535];

    fail_if(libbmdnet_test_hdl == 0, NULL);

    fail_if(bmdnet_connect(libbmdnet_test_hdl, "www.wp.pl", 80) != 0,
	    NULL);

    fail_if(bmdnet_send
	    (libbmdnet_test_hdl, (void *) tosend,
	     strlen(tosend)) != strlen(tosend), NULL);

    fail_if(bmdnet_recv(libbmdnet_test_hdl, received, 65534, 1) <= 0,
	    NULL);

    /* check if we received valid HTTP response eg. response contains 200 OK status */
    received[65534] = 0;
    fail_if(strstr(received, "200 OK\r\n") == NULL,
	    "received response is not a valid HTTP response");
}
END_TEST 


START_TEST(bmdnet_10xconnect_wp_pl_80)
{
/* This test case checks whole connection */
    const char tosend[] = "GET / HTTP/1.1\nHost: www.wp.pl\n\n";
    char received[65535];
    long i;

    fail_if(libbmdnet_test_hdl == 0, NULL);

    for (i=0;i<10;i++) {
        fail_if(bmdnet_connect(libbmdnet_test_hdl, "www.wp.pl", 80) != 0,
	    NULL);

        fail_if(bmdnet_send
	    (libbmdnet_test_hdl, (void *) tosend,
	     strlen(tosend)) != strlen(tosend), NULL);

        fail_if(bmdnet_recv(libbmdnet_test_hdl, received, 65534, 1) <= 0,
	    NULL);

    /* check if we received valid HTTP response eg. response contains 200 OK status */
        received[65534] = 0;
	fail_if(strstr(received, "200 OK\r\n") == NULL,
	    "received response is not a valid HTTP response");
	bmdnet_close(&libbmdnet_test_hdl);
    }
}
END_TEST 

START_TEST(bmdnet_recv_min_wp_pl_80)
{
/* This test case checks whole connection */
    const char tosend[] = "GET / HTTP/1.1\nHost: www.wp.pl\n\n";
    char received[65535];
	long err; 
	
    fail_if(libbmdnet_test_hdl == 0, NULL);

    fail_if(bmdnet_connect(libbmdnet_test_hdl, "www.wp.pl", 80) != 0,
	    NULL);

    fail_if(bmdnet_send
	    (libbmdnet_test_hdl, (void *) tosend,
	     strlen(tosend)) != strlen(tosend), NULL);

	
    fail_if(bmdnet_recv_min(libbmdnet_test_hdl, received, 5, 65534) <=
	    0, NULL);

	fail_if(strncmp("HTTP/",received,5),NULL);
	
	err=bmdnet_recv_min(libbmdnet_test_hdl, received, 50000, 65534);
    fail_if(err < 0,NULL);

	err=bmdnet_recv_min(libbmdnet_test_hdl, received, 50000, 65534);
    fail_unless(err < 0,NULL);

}
END_TEST 

START_TEST(bmdnet_connect_wp_pl_81)
{
    fail_if(libbmdnet_test_hdl == 0, NULL);

    fail_if(bmdnet_connect(libbmdnet_test_hdl, "www.wp.pl", 81) == 0,
	    "connected to usually closed port 81 of www.wp.pl");
}
END_TEST 

START_TEST(bmdnet_connect_nxdomain_com_pl_de_80)
{
    fail_if(libbmdnet_test_hdl == 0, NULL);

    fail_if(bmdnet_connect
	    (libbmdnet_test_hdl, "www.nxdomain.com.pl.de", 80) == 0,
	    "connected to not existing host www.nxdomain.com.pl.de");
}
END_TEST

/* Initialization functions */
void libbmdnet_test_openssl_bio_setup()
{
    assert(bmdnet_init() == 0);
    libbmdnet_test_hdl = bmdnet_create(BMD_NET_OPENSSL_BIO_METHOD, 0);
}



void libbmdnet_test_openssl_bio_teardown()
{
    assert(bmdnet_close(&libbmdnet_test_hdl) == 0);
    assert(bmdnet_destroy() == 0);
}

void libbmdnet_test_plaintcp_setup()
{
    assert(bmdnet_init() == 0);
    libbmdnet_test_hdl = bmdnet_create(BMD_NET_PLAINTCP_METHOD, 0);
}

/* equivalent to openssl_bio_teardown */
void libbmdnet_test_plaintcp_teardown()
{
    libbmdnet_test_openssl_bio_teardown();
}


Suite *libbmdnet_test_suite(void)
{
    Suite *s = suite_create("libbmdnet");
    TCase *testcase_bio = tcase_create("libbmdnet_OPENSSL-BIO");
    TCase *testcase_tcp = tcase_create("libbmdnet_PLAINTCP");

    /* Przypisujemy przypadek testowy do zbioru testów */
    suite_add_tcase(s, testcase_bio);
    /* Dodajemy funkcję inicjalizującą */
    tcase_add_checked_fixture(testcase_bio,
			      libbmdnet_test_openssl_bio_setup,
			      libbmdnet_test_openssl_bio_teardown);

    /* dodajemy testy do przypadku testowego */
    tcase_add_test(testcase_bio, bmdnet_connect_wp_pl_80);
    tcase_add_test(testcase_bio, bmdnet_connect_wp_pl_81);
    tcase_add_test(testcase_bio, bmdnet_connect_nxdomain_com_pl_de_80);
    tcase_add_test(testcase_bio, bmdnet_recv_min_wp_pl_80);	
    tcase_set_timeout(testcase_bio,10);
    


/* Testy TCP roznia sie tylko funkcja inicjalizujaca */
    /* Przypisujemy przypadek testowy do zbioru testów */
    suite_add_tcase(s, testcase_tcp);
    /* Dodajemy funkcję inicjalizującą */
    tcase_add_checked_fixture(testcase_tcp, libbmdnet_test_plaintcp_setup,
			      libbmdnet_test_plaintcp_teardown);

    /* dodajemy testy do przypadku testowego */
    tcase_add_test(testcase_tcp, bmdnet_connect_wp_pl_80);
    tcase_add_test(testcase_tcp, bmdnet_connect_wp_pl_81);
    tcase_add_test(testcase_tcp, bmdnet_connect_nxdomain_com_pl_de_80);
    tcase_add_test(testcase_tcp, bmdnet_recv_min_wp_pl_80);
    tcase_add_test(testcase_tcp, bmdnet_10xconnect_wp_pl_80);
    tcase_set_timeout(testcase_tcp,10);

    return s;
}

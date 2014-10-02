#include <libbmdxpath.h>
int bmdxpath_init_test();
int bmdxpath_destroy_test();
int bmdxpath_register_ns_test(char *prefix, char *uri);
int bmdxpath_register_ns_list_test(char *nslist, char insep, char outsep);
int bmdxpath_find_test();
int bmdxpath_count_result_nodes_test();
int bmdxpath_get_nodes_test();
int bmdxpath_free_nodes_test();
int bmdxpath_get_strings_test();
int bmdxpath_get_string_test();
int bmdxpath_sprintf_test();
int bmdxpath_doc2genbuf_test();
int bmdxpath_doc2buffer_test();

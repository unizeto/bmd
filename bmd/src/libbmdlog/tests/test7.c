#include <time.h>
#include <string.h>
#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdlog/tree_build/tree_node.h>
#include <bmd/libbmdlog/tree_build/tree_build.h>
#include <bmd/libbmdcms/libbmdcms.h>

extern long _GLOBAL_debug_level;

int main(int argc, char* argv[])
{

  _GLOBAL_debug_level = 2;


  ATD                         hashList;
  bmd_crypt_ctx_t*  signContext = NULL;
  void*             dbhandle    = NULL;
  int               intRet      =    0;


  intRet = INIT( &hashList );

  bmd_init();

  bmd_set_ctx_file( "tklimek.pfx", "tklimek:6", 9, &signContext );

  if ( intRet < 0 ) exit(1);

  intRet = bmd_db_connect2("127.0.0.1", "5432", "logi", "tklimek", "", &dbhandle);

  if ( intRet < 0) exit(1);

  intRet = ReadLogFromDatabase( dbhandle, &hashList );

  if (intRet < 0 ) exit(1);

  printf("\n ILOSC : %d \n", hashList.ulCount);

/*  intRet = EvaluateTreeL0( dbhandle, &hashList, atoi(argv[1]), BUILD_ALL );

  intRet = EvaluateOtherTreeLevels( dbhandle, &hashList, signContext, 2 );

  printf("%d\n", intRet);*/

  bmd_ctx_destroy( &signContext );
  bmd_end();

  intRet = bmd_db_disconnect(&dbhandle);

  return 0;

}



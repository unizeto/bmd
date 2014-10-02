#ifndef __TREE_NODE_H__
#define __TREE_NODE_H__


  /* ----------------------------------- */

     typedef enum 
      { 
       logNode, hashNode, oldestHashNode 
      } nodeType_t;

  /* ----------------------------------- */

     typedef struct
      {

         /* ----------------------------- */ 
     
            char*                     id;
    	    char*             hash_value;
	    nodeType_t        nodeStatus;
  
         /* ----------------------------- */ 

      }treeNode_t;


  /* -------------------------------------------------------------------------------- */
  /* API */
  
     int CreateTreeNode( treeNode_t** const, const char* const, const char* const, \
                         const nodeType_t );
     void DestroyTreeNode( void* const );

  /* -------------------------------------------------------------------------------- */


#endif


/*****************************************
 *			                 *
 * Implementacja obszaru pamięci         *
 * dzielonej na zakolejkowane maile      *
 *			                 *
 * Data : 07-10-2009                     *
 * Autor : Tomasz Klimek                 *
 *			                 *
 *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/libbmderr/libbmderr.h>
#include <sys/mman.h>
#include <bmd/libbmdutils/libbmdutils.h>


/**
 * Funkcja tworzy segment pamięci dzielonej jako
 * odwzorowanie pliku.
 * 
 * @param[in] fName - nazwa pliku odwzorowania
 * @param[in] shared - struktura pamięci dzielonej
 * @param[in,out] shPtr  - wskaźnik do obszaru pamięci dzielonej
 *
 * @retval BMD_ERR_PARAM1         - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2         - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY         - błąd alokacji pamięci
 * @retval BMD_ERR_OP_FAILED	  - błąd inicjowania semafora/pamięci dzielonej
 * @retval BMD_OK                 - zakończenie pomyślne
 *
*/

long createShRegion(char const * const fName, shRegion_t const * const shared, shRegion_t** const shPtr ) {

  /* --------------- */

       int fD = 0;   /* uchwyt do pliku */

  /* --------------- */


  if (fName == NULL) {
    PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
    return BMD_ERR_PARAM1;
  }

  if (shared == NULL) {
    PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
    return BMD_ERR_PARAM2;
  }

  if (shPtr == NULL) {
    PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
    return BMD_ERR_PARAM2;
  }

  fD = open( fName, O_RDWR | O_CREAT, 0666);
  unlink(fName);

  if (fD == -1){
    PRINT_ERROR("Błąd utworzenia pliku odwzorowania. Error = %d\n", BMD_ERR_OP_FAILED);
    return BMD_ERR_OP_FAILED;
  }

  write(fD, shared, sizeof(shRegion_t));

 /* *************************************************************/
 /* Utworzenie segmentu pamięci dzielonej z mapowaniem do pliku */
 /* *************************************************************/
  *shPtr = mmap(NULL, sizeof(shRegion_t), PROT_READ | PROT_WRITE, MAP_SHARED, fD, 0);
  close(fD);


  if (*shPtr == NULL){
    PRINT_ERROR("Błąd alokacji obszaru pamięci dzielonej. Error = %d\n", BMD_ERR_MEMORY);
    return BMD_ERR_MEMORY;
  }

 /* ****************************** */
 /* Zainicjowanie semafora klienta */
 /* ****************************** */

  if (sem_init( &((*shPtr)->clientMutex), 1, 0) == -1) {
    PRINT_ERROR("Błąd inicjowania semafora dla klienta. Error = %d\n", BMD_ERR_OP_FAILED);
    return BMD_ERR_OP_FAILED;
  }

 /* ********************************** */
 /*  Zainicjowanie semafora dla akcji  */
 /* ********************************** */

  if (sem_init( &((*shPtr)->awizoActionMutex), 1, 1) == -1) {
    PRINT_ERROR("Błąd inicjowania semafora dla akcji. Error = %d\n", BMD_ERR_OP_FAILED);
    return BMD_ERR_OP_FAILED;
  }

 return BMD_OK;

}


/**
 * Funkcja zwalnia wcześniej zainicjalizowany segment 
 * pamięci dzielonej
 * 
 * @param[in] shared - struktura pamięci dzielonej
 * @param[in,out] shPtr  - wskaźnik do obszaru pamięci dzielonej
 *
 * @retval BMD_ERR_PARAM1         - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2         - niepoprawny drugi parametr
 * @retval BMD_OK                 - zakończenie pomyślne
 *
*/


long destroyShRegion(shRegion_t const * const shared, shRegion_t* const shPtr ) {


  if (shared == NULL) {
    PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
    return BMD_ERR_PARAM1;
  }

  if (shPtr == NULL) {
    PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
    return BMD_ERR_PARAM2;
  }

  munmap( shPtr, sizeof(shRegion_t));

 return BMD_OK;

}



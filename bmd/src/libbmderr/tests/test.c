#include <bmd/libbmderr/err_stack.h>

err_t f1(void);
err_t f2(void);
err_t f3(void);

/************************/
/* TESTY STOSU BLEDOW	*/
/************************/
err_t f1(void)
{
err_t err = 0;
	/* ... */

	/* Załózmy że ta funkcja wywoluje f2 w ktorej wystapi blad */
	err = f2();
	if(BMD_ERR(err)){
		BMD_SETERR(err, -111);
		return err;
	}
	return err;
}

err_t f2(void)
{
err_t err = 0;
	/* Jakis kod funkcji f2 - bez bledow */
	/* ... */

	/* I tu nagle wystepuje blad ale w funkcji f3*/
	err = f3();
	if(BMD_ERR(err)){	/* Obsluga bledu */
		BMD_SETERR(err, -111);
		return err;
	}
	return err;
}

err_t f3(void)
{
err_t err = 0;
	/* Standardowy kod */
	/* ... */

	/* Jakis blad */
	if(1){	/* obsluga bledu */
		BMD_SETERR(err, -222);
		BMD_SETIERR(err, LP, "LIBBMDDB");
		BMD_SETIERR(err, AD, "To jest blad otwarcia pliku /dev/urandom");
		BMD_SETIERR(err, AS, "Zainstaluj patcha do systemu operacyjnego");
		return err;
	}
}

int main(int argc, char *argv[])
{
int status = 0;
err_t err = 0;
	/************************************************/
	/* Test stosu bledow				*/
	/************************************************/
	/* W main obowiazkowo wykonuje 2 operacje	*/
	/* 1. zrzucam stos oraz 			*/
	/* 2. czyszcze go				*/
	/************************************************/
	err = f1();
	if(BMD_ERR(err)){
		printf("Wystapily bledy. Sprawdzam ścieżke wywołań:\n");
		BMD_BTERR(err);
		BMD_FREEERR(err);
	}
	BMD_FREEERR(err); /* cos ala CryptEnd */
	return status;
}

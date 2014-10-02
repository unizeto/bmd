#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmdaa2/db/file/AC_db_file.h>

extern long _GLOBAL_debug_level;

int main(int argc, char *argv[])
{
int err = 0;
ConfigData_t *ConfigData = NULL;

	_GLOBAL_debug_level = 2;

	err = ConfigData_read("conf/uniac.cnf", &ConfigData);
	if(err<0){
		printf("Bład wczytania opcji konfiguracyjnych. Kod bledu = %i\n", err);
		return err;
	}
	ConfigData_validate(ConfigData);
	//ConfigData_print(ConfigData);

	AttributesSequence_t *AttributesSequence = NULL;
	AA_ReadAttributesFromFile("conf/attributes.db", "03 4A 1C", "C=<PL> O=<Unizeto Sp. z o.o.> CN=<Certum Level I>", ConfigData, &AttributesSequence);

	AC_AttributesSequence_free_ptr(&AttributesSequence);
	ConfigData_free(&ConfigData);

	return err;
}

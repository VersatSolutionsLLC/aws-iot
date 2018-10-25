/*
 * configDefinition.c
 *
 *  Created on: Aug 14, 2018
 *      Author: Admin
 */

#include "../Configuration/config.h"

#include "legato.h"

#include "interfaces.h"
#include "../Configuration/minIni.h"

char TEMP[512];
static le_mutex_Ref_t cfMutex;
# define LOCK()      le_mutex_Lock(cfMutex)
# define UNLOCK()    le_mutex_Unlock(cfMutex)
enum _fileAccessType fileAccessType = LINUX;

int config_OpenFile() {

	int isOpen = 0;
	FILE *fptr;
	le_fs_AccessMode_t accType = LE_FS_RDONLY;
	le_fs_FileRef_t fileRef;

	switch (fileAccessType) {
	case LINUX:
		LE_DEBUG(
				"Trying to open file.\nFile name = %s\nFile access type = LINUX",
				CONFIG_FILE_NAME);
		fptr = fopen(CONFIG_FILE_NAME, "r");
		if (fptr != NULL) {
			LE_DEBUG("File '%s' is opened", CONFIG_FILE_NAME);
			fclose(fptr);
			isOpen = 1;
		} else {
			LE_ERROR("Failed to open file using '%s' in LINUX method",
					CONFIG_FILE_NAME);
		}
		break;
	case LEGATO_FILE_SYSTEM:
		LE_DEBUG(
				"Trying to open file.\nFile name = %s\nFile access type = LEGATO_FILE_SYSTEM",
				CONFIG_FILE_NAME);
		le_result_t resType = le_fs_Open(CONFIG_FILE_NAME, accType, &fileRef);
		if (LE_OK == resType) {
			LE_DEBUG("File '%s' is opened", CONFIG_FILE_NAME);
			isOpen = 1;
		} else {
			LE_ERROR(
					"failed to open file %s using LEGATO_FILE_SYSTEM. Error type %s",
					CONFIG_FILE_NAME, LE_RESULT_TXT(resType));
		}
		break;
	default:
		LE_ERROR(
				"Invalid option %s is chosen to open file using LEGATO_FILE_SYSTEM",
				CONFIG_FILE_NAME);
	}
	return isOpen;
}

int config_GetString(const char* section, const char* key,
		const char* defaultValue, char* keyValue, size_t size) {

	LOCK();
	int n = ini_gets(section, key, defaultValue, TEMP, sizearray(TEMP),
	CONFIG_FILE_NAME);
	UNLOCK();
	if (0 == n) {

		strcpy(keyValue, defaultValue);
		return false;
	}
	strcpy(keyValue, TEMP);
	return true;
}

int config_GetInteger(const char* section, const char* key, int defaultValue,
		int* keyValue) {
	LOCK();
	int n = ini_getl(section, key, INVALID_VALUE_FOR_LONG, CONFIG_FILE_NAME);
	UNLOCK();
	if ( INVALID_VALUE_FOR_LONG == n) {
		n = defaultValue;
		return false;
	}
	*keyValue = n;
	return true;
}

int config_GetFloat(const char* section, const char* key, double defaultValue,
		double* keyValue) {
	LOCK();
	float n = ini_getf(section, key, INVALID_VALUE_FOR_FLOAT, CONFIG_FILE_NAME);
	UNLOCK();
	if (INVALID_VALUE_FOR_FLOAT == n) {
		n = defaultValue;
		return false;
	}
	*keyValue = n;
	return true;
}

int config_GetBoolean(const char* section, const char* key, bool defaultValue,
		bool* keyValue) {
	LOCK();

	if (0 != defaultValue && 1 != defaultValue) {
		LE_ERROR(
				"Invalid boolean value '%d' is specified as default value for '%s' key",
				defaultValue, key);
		return false;
	}
	int n = ini_getbool(section, key, INVALID_VALUE_FOR_BOOLEAN,
	CONFIG_FILE_NAME);
	UNLOCK();
	LE_INFO("In getBool: n = %d", n);
	if (INVALID_VALUE_FOR_BOOLEAN == n) {
		*keyValue = defaultValue;
		return false;
	}
	*keyValue = n;
	return true;
}

int config_PutString(const char* section, const char* key, const char* value) {
	// Write key with value under specific section
	LOCK();
	int n = ini_puts(section, key, value, CONFIG_FILE_NAME);
	UNLOCK();

	int isSuccessfulKeyEntry = 0;
	if (0 == n) {
		LE_ERROR(
				"Failed to add entry ( %s = %s ) under '%s' section of the configuration file '%s'",
				key, value, section, CONFIG_FILE_NAME);
	} else {

		// Verify successful write operation
		char buffer[50];
		LOCK();
		int m = ini_gets(section, key,"", buffer, sizearray(buffer),
		CONFIG_FILE_NAME);
		UNLOCK();

		if ((0 == m) && ( 0 != strcmp(value,""))) {
			LE_ERROR(
					"Key addition failed.Entry (%s = %s) is not found in '%s section of the configuration file(%s)",
					key, value, section, CONFIG_FILE_NAME);
		} else {
			LE_DEBUG(
					"Successfully added entry (%s = %s) in '%s' section of the configuration file '%s'",
					key, value, section, CONFIG_FILE_NAME);
			isSuccessfulKeyEntry = 1;
		}
	}
	if (isSuccessfulKeyEntry) {
		return true;
	} else {
		return false;
	}
}

COMPONENT_INIT {
	cfMutex = le_mutex_CreateNonRecursive("configFileMutex");
	int isOpen = config_OpenFile();
	if (0 == isOpen) {
		LE_ERROR("Unable to open configuration file %s", CONFIG_FILE_NAME);
	}
}


/*
 * configDefinition.c
 *
 *  Created on: Aug 14, 2018
 *      Author: Admin
 */

#include "legato.h"
#include "minIni.h"
#include "configDefinition.h"
#include "interfaces.h"

char TEMP[100];
static le_mutex_Ref_t cfMutex;
# define LOCK()      le_mutex_Lock(cfMutex)
# define UNLOCK()    le_mutex_Unlock(cfMutex)

void config_OpenFile() {

	// File Read operation through LINUX Method
	FILE *fptr;
	LE_INFO("(LINUX METHOD) open file: %s",CONFIG_FILE_NAME);
	fptr = fopen(CONFIG_FILE_NAME,"r");
	if(fptr == NULL) {
		LE_ERROR("(LINUX METHOD ): Failed to open file ");
	} else {
		LE_DEBUG("(LINUX METHOD ): File is opened");
		fclose(fptr);
	}

	// File Read operation through LEGATO Method
	le_fs_AccessMode_t accType = LE_FS_RDONLY ;
	le_fs_FileRef_t fileRef;

	LE_INFO("(LEGATO METHOD) open file: %s",CONFIG_FILE_NAME);
	le_result_t resType = le_fs_Open(CONFIG_FILE_NAME,accType,&fileRef);
	if (LE_OK != resType)
	{
		LE_ERROR("(LEGATO METHOD ): Failed to open file");
	} else {
		LE_DEBUG("(LEGATO METHOD ): File is opened");
	}
}

void config_sGetValue (const char* section,const char* key,const char* defaultValue,char* keyValue, size_t size) {

	LOCK();
	int n = ini_gets(section,key,defaultValue,TEMP, sizearray(TEMP),CONFIG_FILE_NAME);
	UNLOCK();
	if ( 0 == n ) {
		LE_FATAL("No entry corresponding to '%s' is found under (%s) section of the configuration file(%s)",key,section,CONFIG_FILE_NAME);
	} else if ( 0 == strcmp(TEMP,"NULL") ) {
		LE_FATAL("No value can be retrieved against '%s' under (%s) section of the configuration file(%s)",key,section,CONFIG_FILE_NAME);
	} else {
		LE_DEBUG("%s = %s",key,TEMP);
	}
	strcpy(keyValue,TEMP);
}

void config_iGetValue (const char* section,const char* key,int defaultValue,int* keyValue) {
	LOCK();
	int n = ini_getl(section,key,defaultValue,CONFIG_FILE_NAME);
	UNLOCK();
	if ( 0 == n ) {
		LE_DEBUG("No entry corresponding to '%s' is found under (%s) section of the configuration file(%s)",key,section,CONFIG_FILE_NAME);
	} else {
		LE_DEBUG("%s = %d",key,n);
	}
	*keyValue = n;
}

void config_fGetValue (const char* section,const char* key,double defaultValue,double* keyValue) {
	LOCK();
	float n = ini_getf(section,key,defaultValue,CONFIG_FILE_NAME);
	UNLOCK();
	if ( 0 == n ) {
		LE_DEBUG("No entry corresponding to '%s' is found under (%s) section of the configuration file(%s)",key,section,CONFIG_FILE_NAME);
	} else {
		LE_DEBUG("%s = %f",key,n);
	}
	*keyValue = n;
}

void config_boolGetValue (const char* section,const char* key,int defaultValue,int* keyValue) {
	LOCK();
	int n = ini_getbool(section,key,defaultValue,CONFIG_FILE_NAME);
	UNLOCK();
	if ( defaultValue == n ) {
		LE_DEBUG("No value can be retrieved against '%s' under (%s) section of the configuration file(%s).",key,section,CONFIG_FILE_NAME);
		LE_DEBUG("Reading default value %d for %s",defaultValue,key);
	}
	LE_DEBUG("%s = %d",key,n);
	*keyValue = n;
}

void config_sPutValue (const char* section,const char* key,const char* value) {
	// Write key with value under specific section
	LOCK();
	int n = ini_puts(section,key,value,CONFIG_FILE_NAME);
	UNLOCK();
	if ( 0 == n ) {
		LE_FATAL("Failed to add entry ( %s = %s ) under '%s' section of the configuration file '%s'",key,value,section,CONFIG_FILE_NAME);
	}  else {

		// Verify successful write operation
		char buffer[50];
		LOCK();
		int m = ini_gets(section,key,NULL,buffer, sizearray(buffer),CONFIG_FILE_NAME);
		UNLOCK();
		LE_DEBUG("%s = %s",key,TEMP);
		if ( 0 == m ) {
			LE_FATAL("No entry corresponding to '%s' is found under (%s) section of the configuration file(%s)",key,section,CONFIG_FILE_NAME);
		} else if ( 0 == strcmp(buffer,"NULL") ) {
			LE_FATAL("No value can be retrieved against '%s' under (%s) section of the configuration file(%s)",key,section,CONFIG_FILE_NAME);
		} else {
			LE_DEBUG("Successfully add entry (%s = %s) in %s section of the configuration file %s",key,value,section,CONFIG_FILE_NAME);
		}
	}
}

void config_test(char* params, size_t size){
	params = "ABCD";
}

COMPONENT_INIT
{
	cfMutex = le_mutex_CreateNonRecursive("configFileMutex");
	if (0) {
    LE_INFO("Hello, world.");
    config_OpenFile();
    char *section = "config";

    char aws_host_name[50];
    char aws_cert_path[50];
    char aws_client_id[50];
    char aws_thing_name[50];
    char aws_root_CA_filename[50];
    char aws_certificate_filename[50];
    char aws_private_key_filename[50];
    int aws_port_number = 0;

    config_sGetValue(section,"HostName","NULL",aws_host_name,100);
    config_sGetValue(section,"CertPath","NULL",aws_cert_path,100);
    config_iGetValue(section,"PortNumber",440,&aws_port_number);
    config_sGetValue(section,"ClientId","NULL",aws_client_id,100);
    config_sGetValue(section,"ThingName","NULL",aws_thing_name,100);
    config_sGetValue(section,"RootCAFileName","NULL",aws_root_CA_filename,100);
    config_sGetValue(section,"CertificateFileName","NULL",aws_certificate_filename,100);
    config_sGetValue(section,"PrivateKeyFileName","NULL",aws_private_key_filename,100);

    LE_DEBUG("****************************************************");
    LE_DEBUG(" Host Name = %s",aws_host_name);
    LE_DEBUG(" Cert Path = %s",aws_cert_path);
    LE_DEBUG(" Client Id = %s",aws_client_id);
    LE_DEBUG(" Thing Name = %s",aws_thing_name);
    LE_DEBUG(" Root CA = %s",aws_root_CA_filename);
    LE_DEBUG(" Certificate File = %s",aws_certificate_filename);
    LE_DEBUG(" Pvt Key = %s",aws_private_key_filename);
    LE_DEBUG(" Port Number = %d",aws_port_number);
    LE_DEBUG("****************************************************");

    double fl_x;
    config_fGetValue("others","Percentage",0.0,&fl_x);

    int bool_x;
    config_boolGetValue("others","IsOpen",-1,&bool_x);

    LE_DEBUG("****************************************************");
    LE_DEBUG(" Percentage 	(Float entry) 	= %f",fl_x);
    LE_DEBUG(" IsOpen 		(Bool entry) 	= %d",bool_x);
    LE_DEBUG("****************************************************");
	}

}


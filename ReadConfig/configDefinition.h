/*
 * configDefinition.h
 *
 *  Created on: Aug 14, 2018
 *      Author: Admin
 */

#ifndef CONFIGDEFINITION_H_
#define CONFIGDEFINITION_H_


# define DEFAULT_THING_NAME 			"MangoH"
# define DEFAULT_CLIENT_ID 				"MangoH"
# define sizearray(a)  					(sizeof(a) / sizeof((a)[0]))
# define MAX_BUFFER 50
# define CONFIG_FILE_NAME 				"/usr/tests/config.ini"

# define AWSIOT_SECTION 				"config"

# define KEY_HOST_NAME 					"HostName"
# define KEY_CERT_PATH 					"CertPath"
# define KEY_CLIENT_ID 					"ClientId"
# define KEY_THING_NAME 				"ThingName"
# define KEY_ROOT_CA_FILE_NAME 			"RootCAFileName"
# define KEY_CERTIFICATE_FILE_NAME 		"CertificateFileName"
# define KEY_PRIVATE_KEY_FILE_NAME 		"PrivateKeyFileName"
# define KEY_PORT_NUMBER 				"PortNumber"


LE_SHARED void config_OpenFile();
/*LE_SHARED void config_sGetValue (char*,char*,char*,char*);
LE_SHARED void config_iGetValue (char*,char*,int,int*);
LE_SHARED void config_fGetValue (char*,char*,float,float*);
LE_SHARED void config_boolGetValue (char*,char*,int,int*);*/



#endif /* CONFIGDEFINITION_H_ */

/*
 * aws_iot_read_config_header.h
 *
 *  Created on: Aug 27, 2018
 *      Author: Admin
 */

#ifndef AWS_IOT_CONFIG_DEFN_H_
#define AWS_IOT_CONFIG_DEFN_H_

# define CONFIG_SECTION 					"aws-iot"

# define CONFIG_KEY_HOST_NAME 				"Host"
# define CONFIG_KEY_CLIENT_ID 				"ClientId"
# define CONFIG_KEY_THING_NAME 				"ThingName"
# define CONFIG_KEY_ROOT_CA_FILE_NAME 		"RootCAFile"
# define CONFIG_KEY_CERTIFICATE_FILE_NAME 	"DeviceCertFile"
# define CONFIG_KEY_PRIVATE_KEY_FILE_NAME 	"DevicePrivateKeyFile"
# define CONFIG_KEY_PORT_NUMBER				"Port"

# define CONFIG_DEFAULT_PORT_NUMBER 		443
# define CONFIG_KEY_CELLULAR_PREFERRED 		"CellularDataService"
# define CONFIG_DEFAULT_CELLULAR_PREFERRED  true

#endif /* AWS_IOT_CONFIG_DEFN_H_ */

/*
 * configDefinition.h
 *
 *  Created on: Aug 14, 2018
 *      Author: Admin
 */

#ifndef CONFIGDEFINITION_H_
#define CONFIGDEFINITION_H_

# define sizearray(a)  					(sizeof(a) / sizeof((a)[0]))
# define MAX_BUFFER 50
# define CONFIG_FILE_NAME 				"/etc/aws/config.ini"



# define INVALID_VALUE_FOR_LONG 		pow(2, (sizeof(long) * CHAR_BIT) -1) - 1
# define INVALID_VALUE_FOR_FLOAT 		pow(2, (sizeof(float) * CHAR_BIT) -1) - 1
# define INVALID_VALUE_FOR_BOOLEAN      -1

enum _fileAccessType {
	LINUX, LEGATO_FILE_SYSTEM
} fileAccessType;

#endif /* CONFIGDEFINITION_H_ */

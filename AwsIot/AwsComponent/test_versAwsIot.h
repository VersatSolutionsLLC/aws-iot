/*
 * test_AwsIot.h
 *
 *  Created on: Aug 2, 2018
 *      Author: Admin
 */
#include "aws_iot_mqtt_client.h"
#ifndef TEST_AWSIOT_H_
#define TEST_AWSIOT_H_

ClientState _getConnectionState();
void test();
void test_subscribe_publish_hibernate();
void test_on_hibernate_subscribe_publish();
void test_subscribe_on_hibernate();
void test_unsubscribe_on_hibernate();
void test_publish_on_hibernate();
void test_publish_only();

/**
 * @brief Debug level logging macro.
 *
 * Macro to expose function, line number as well as desired log message.
 */
#ifdef ENABLE_IOT_TEST
#define IOT_TEST() test();
#else
#define IOT_TEST()
#endif /* ENABLE_IOT_TEST */

#endif /* TEST_AWSIOT_H_ */

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
int aws_Connect();
int aws_Publish(const char*,int,int,const char*,int);
int aws_Subscribe(const char*,int,int);
int aws_UnSubscribe(const char*,int);
int aws_disconnect();
void test();
void test_connect_subscribe_publish_unsubscribe_disconnect();
void test_concurrency_of_publish_subscribe();
void test_unsubscribe_on_subscribe();
void test_repeated_unsubscribe_on_subscribe();
void test_unsubscribe_without_subscribe();
void test_unsubscribe_for_unordered_subscribe();
void test_concurrency_of_subscribe_and_unsubscribe();

void test_multi_connect();
void test_multi_disconnect();
void test_publish_without_connect();
void test_subscribe_without_connect();
void test_repeated_publish_for_same_topic();
void test_repeated_publish_for_different_topic();
void test_repeated_subscribe_on_same_topic();
void test_repeated_subscribe_on_different_topic();
void test_publish_with_null_topic();
void test_publish_with_invalid_topicLength();
void test_publish_with_null_payload();
void test_publish_with_invalid_payloadLength();
void test_publish_with_invalid_QOS_value();
void test_Reset();
void _reset();
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

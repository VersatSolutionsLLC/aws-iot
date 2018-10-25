/*
 * Test_AwsIot.c
 *
 *  Created on: Aug 2, 2018
 *      Author: Admin
 */

#include "legato.h"
#include "interfaces.h"
#include "test_versAwsIot.h"

/**================================================================================
 * Task  			: Main test function that invokes others test function
 *
 * Objective		: This function is called from AwsIot.c to test exposed aws_iot
 * 					  functions to test the functionality of connect, disconnect
 * 					  publish, subscribe, unsubscribe operations
 * Return 			: None
 * ================================================================================
 */
void test() {
//	test_subscribe_publish_hibernate();
//	test_on_hibernate_subscribe_publish();
//	test_subscribe_on_hibernate();
//	test_unsubscribe_on_hibernate();
//	test_publish_on_hibernate();
	test_publish_only();
}
void _onSubscribeCallback(char *ptrPayload, void *reportPayloadPtr) {
	LE_INFO("\n========== PAYLOAD DATA =============\n");
	LE_INFO("\n\t\t %s", ptrPayload);
	LE_INFO("\n===========================================================\n");
}

void test_publish_only() {
//	char buf1[1024];
//	int i=0;
//	char ch = 'a';
//	for(i = 0 ; i<= 498; i++ ) {
//		buf1[i] = ch++;
//		if ( ch == 'z') {
//			ch = 'a';
//		}
//	}
//	buf1[i] = '\0';

	//char *buf1 = "{\"radio\":{\"IMEI\":352913090104454,\"IMSI\":404310395247608,\"RATS\":\"LTE\",\"TEMP\":36,\"RSSI\":-58,\"RSRP\":-910,\"RSRQ\":-140,\"SNR\":-10,\"BLER\":-1},\"scell\":{\"CELLID\":2859780,\"LAC\":-1,\"TAC\":27},\"ncell\":{},\"gps\":{\"P1\":\"V1\",\"P2\":\"V2\"}}";
	char buf1[1024];
	strcpy(buf1,"{\"radio\":{\"IMEI\":352913090104454,\"IMSI\":404310395247608,\"RATS\":\"LTE\",\"TEMP\":35,\"RSSI\":-58,\"RSRP\":-940,\"RSRQ\":-110,\"SNR\":24,\"BLER\":-1},{\"scell\":{\"CELLID\":208146947,\"LAC\":-1,\"TAC\":27},\"ncell\":{\"CellNo\":0,\"CID\":161,\"LAC\":65535,\"RXLevel\":-56},\"gps\":{\"LAT\":\"0.1\",\"LON\":\"0.2\",\"ALT\":\"0.3\",\"ALTWGS84\":\"0.4\",\"HACC\":\"0.5\",\"VACC\":\"0.9\"}}");
	LE_INFO("=============================Payload===================================");
	LE_INFO("%s",buf1);
	LE_INFO("strlen(buf1) = %d, sizeof(buf1) = %d",strlen(buf1),sizeof(buf1));
	LE_INFO("=======================================================================");

	int rc = aws_Publish("VersatIOT", strlen("versatIOT"), 1, buf1, strlen(buf1));
	LE_DEBUG("Publish 2 Status.. (%d) payload length = %d", rc,strlen(buf1));
	LE_ASSERT(0 == rc);
}
void test_subscribe_publish_hibernate() {
	LE_DEBUG("=================== TEST: test_subscribe_publish_hibernate =======================");
	int rc;
//	aws_AddSubscribeEventHandler(
//			(aws_SubscribeEventHandlerFunc_t) _onSubscribeCallback, NULL);
//
//	rc = aws_Subscribe("hello", strlen("hello"), 1);
//	LE_DEBUG("Subscribe Status.. (%d)", rc);
//	LE_ASSERT(0 == rc);
//
//	rc = aws_Publish("hello", strlen("hello"), 1, (void*) "HELLO",
//			strlen("HELLO"));
//	LE_DEBUG("Publish 1 Status.. (%d)", rc);
//	LE_ASSERT(0 == rc);

	rc = aws_Publish("hello", strlen("hello"), 1, "HELLO", strlen("HELLO"));
	LE_DEBUG("Publish 2 Status.. (%d)", rc);
	LE_ASSERT(0 == rc);
//
//	rc = aws_Unsubscribe("hello", strlen("hello"));
//	LE_DEBUG("Unsubscribe result (%d) ", rc);
//	LE_ASSERT(0 == rc);
//
//	aws_Hibernate();
//	LE_ASSERT(aws_IsInHibernation() == true);
	LE_DEBUG("=================== TEST: test_subscribe_publish_hibernate: PASS =======================");
}
//void test_on_hibernate_subscribe_publish() {
//	LE_DEBUG("=================== TEST: test_on_hibernate_subscribe_publish =======================");
//	aws_Hibernate();
//	LE_ASSERT(aws_IsInHibernation() == true);
//	int rc;
//	aws_AddSubscribeEventHandler(
//			(aws_SubscribeEventHandlerFunc_t) _onSubscribeCallback, NULL);
//
//	rc = aws_Subscribe("hello", strlen("hello"), 1);
//	LE_DEBUG("Subscribe Status.. (%d)", rc);
//	LE_ASSERT(0 == rc);
//
//	rc = aws_Publish("hello", strlen("hello"), 1, (void*) "HELLO",
//			strlen("HELLO"));
//	LE_DEBUG("Publish 1 Status.. (%d)", rc);
//	LE_ASSERT(0 == rc);
//
//	rc = aws_Publish("hello", strlen("hello"), 1, "HELLO", strlen("HELLO"));
//	LE_DEBUG("Publish 2 Status.. (%d)", rc);
//	LE_ASSERT(0 == rc);
//
//	rc = aws_Unsubscribe("hello", strlen("hello"));
//	LE_DEBUG("Unsubscribe result (%d) ", rc);
//	LE_ASSERT(0 == rc);
//	LE_DEBUG("=================== TEST: test_on_hibernate_subscribe_publish: PASS =======================");
//}
//void test_subscribe_on_hibernate() {
//	LE_DEBUG("=================== TEST: test_subscribe_on_hibernate =======================");
//	int rc;
//	aws_Hibernate();
//	LE_ASSERT(true == aws_IsInHibernation());
//
//	aws_AddSubscribeEventHandler(
//				(aws_SubscribeEventHandlerFunc_t) _onSubscribeCallback, NULL);
//
//	rc = aws_Subscribe("hello", strlen("hello"), 1);
//	LE_ASSERT(0 == rc);
//
//	LE_DEBUG("Checking Hibernation state");
//	LE_ASSERT(true == aws_IsInHibernation());
//	LE_DEBUG("=================== TEST: test_subscribe_on_hibernate: PASS =======================");
//
//}
void test_unsubscribe_on_hibernate() {
	LE_DEBUG("=================== TEST: test_unsubscribe_on_hibernate =======================");
	int rc;
	aws_Hibernate();
	LE_ASSERT(true == aws_IsInHibernation());

	rc = aws_Unsubscribe("hello", strlen("hello"));
	LE_DEBUG("aws_Unsubscribe : rc = %d",rc);

	LE_DEBUG("Checking Hibernation state");
	LE_ASSERT(true == aws_IsInHibernation());
	LE_DEBUG("=================== TEST: test_unsubscribe_on_hibernate: PASS =======================");
}
void test_publish_on_hibernate() {
	LE_DEBUG("=================== TEST: test_publish_on_hibernate =======================");
	int rc;
	aws_Hibernate();
	LE_ASSERT(true == aws_IsInHibernation());

	rc = aws_Publish("hello", strlen("hello"), 1, "HELLO", strlen("HELLO"));
	LE_ASSERT(0 == rc);

	LE_DEBUG("Checking Hibernation state");
	LE_ASSERT(false == aws_IsInHibernation());
	LE_DEBUG("=================== TEST: test_publish_on_hibernate: PASS =======================");
}

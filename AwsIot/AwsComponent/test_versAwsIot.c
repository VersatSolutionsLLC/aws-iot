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
	test_subscribe_publish_hibernate();
//	test_on_hibernate_subscribe_publish();
//	test_subscribe_on_hibernate();
//	test_unsubscribe_on_hibernate();
//	test_publish_on_hibernate();
}
void _onSubscribeCallback(char *ptrPayload, void *reportPayloadPtr) {
	LE_INFO("\n========== PAYLOAD DATA =============\n");
	LE_INFO("\n\t\t %s", ptrPayload);
	LE_INFO("\n===========================================================\n");
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
void test_on_hibernate_subscribe_publish() {
	LE_DEBUG("=================== TEST: test_on_hibernate_subscribe_publish =======================");
	aws_Hibernate();
	LE_ASSERT(aws_IsInHibernation() == true);
	int rc;
	aws_AddSubscribeEventHandler(
			(aws_SubscribeEventHandlerFunc_t) _onSubscribeCallback, NULL);

	rc = aws_Subscribe("hello", strlen("hello"), 1);
	LE_DEBUG("Subscribe Status.. (%d)", rc);
	LE_ASSERT(0 == rc);

	rc = aws_Publish("hello", strlen("hello"), 1, (void*) "HELLO",
			strlen("HELLO"));
	LE_DEBUG("Publish 1 Status.. (%d)", rc);
	LE_ASSERT(0 == rc);

	rc = aws_Publish("hello", strlen("hello"), 1, "HELLO", strlen("HELLO"));
	LE_DEBUG("Publish 2 Status.. (%d)", rc);
	LE_ASSERT(0 == rc);

	rc = aws_Unsubscribe("hello", strlen("hello"));
	LE_DEBUG("Unsubscribe result (%d) ", rc);
	LE_ASSERT(0 == rc);
	LE_DEBUG("=================== TEST: test_on_hibernate_subscribe_publish: PASS =======================");
}
void test_subscribe_on_hibernate() {
	LE_DEBUG("=================== TEST: test_subscribe_on_hibernate =======================");
	int rc;
	aws_Hibernate();
	LE_ASSERT(true == aws_IsInHibernation());

	aws_AddSubscribeEventHandler(
				(aws_SubscribeEventHandlerFunc_t) _onSubscribeCallback, NULL);

	rc = aws_Subscribe("hello", strlen("hello"), 1);
	LE_ASSERT(0 == rc);

	LE_DEBUG("Checking Hibernation state");
	LE_ASSERT(true == aws_IsInHibernation());
	LE_DEBUG("=================== TEST: test_subscribe_on_hibernate: PASS =======================");

}
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

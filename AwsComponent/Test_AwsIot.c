/*
 * Test_AwsIot.c
 *
 *  Created on: Aug 2, 2018
 *      Author: Admin
 */

#include "legato.h"
#include "test_AwsIot.h"

void* _publishThread(void*);
void* _subscribeThread(void*);
void* _multiSubscribeThread(void*);
void* _multiUnsubscribeThread(void*);
void _stopTestThread(le_thread_Ref_t);

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
	
    test_multi_connect();
//	test_multi_disconnect();
//	test_publish_without_connect();
//	test_subscribe_without_connect();
//	test_repeated_publish_for_same_topic();
//	test_repeated_publish_for_different_topic();
//	test_publish_with_null_topic();
//	test_publish_with_invalid_topicLength(); // Publish failed when length is 500
//	test_publish_with_null_payload(); //Topic can be published with null payload
//	test_publish_with_invalid_payloadLength(); // Publish failed when length is 500
//	test_publish_with_invalid_QOS_value(); // Topic can be published with invalid QOS
//	test_repeated_subscribe_on_same_topic();
//	test_repeated_subscribe_on_different_topic();


//	test_connect_subscribe_publish_unsubscribe_disconnect();
//	test_unsubscribe_on_subscribe();
//	test_repeated_unsubscribe_on_subscribe();
//	test_unsubscribe_without_subscribe();
//	test_unsubscribe_for_unordered_subscribe();
//	test_concurrency_of_publish_subscribe();
//	test_concurrency_of_subscribe_and_unsubscribe();
}

void _reset() {
	int retCode;
	int loopExit = 0;
	ClientState state;
	LE_DEBUG("test cleaning up...");
	while(1) {
		state = _getConnectionState();
		if ( state >= 3 && state <= 9 ) {
			LE_DEBUG("Network is connected. Disconnecting now..");
			retCode = aws_disconnect();
			LE_ASSERT(0 == retCode);
			loopExit = 1;
		} else if ( 2 == state ) {
			LE_DEBUG("Client is in 'connecting' state.Wait for a while");
			sleep(5);
		} else if( 10 == state ) {
			LE_DEBUG("Client is in 'disconnecting' state.Wait for a while");
			sleep(5);
		} else if( state >= 11 && state <= 13) {
			LE_DEBUG("Client is already disconnected.Skipping clean up operation");
			loopExit = 1;
		} else {
			LE_DEBUG("Connection is not initialized yet.Skipping clean up operation");
			loopExit = 1;
		}

		if (1 == loopExit ) {
			break;
		}
	}
}

void test_Reset() {
	int retCode = aws_Connect();
	LE_INFO("retcode = %d",retCode);
	retCode = aws_disconnect();
	//LE_ASSERT(0 == retCode);
	_reset();
}

void test_multi_connect() {
	_reset();
	LE_INFO("\n================= Test: test_mqtt_multi_connect ===============\n");

	//Connect to remote client
	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	//Trying to connect again to remote client
	retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	LE_INFO("\n============= test_mqtt_multi_connect:  PASS ============\n");
}
void test_multi_disconnect() {
	_reset();
	LE_INFO("\n================= Test: test_multi_disconnect ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	retCode = aws_disconnect();
	LE_ASSERT(0 == retCode);

	retCode = aws_disconnect();
	LE_ASSERT(0 != retCode);
	LE_INFO("\n================= test_multi_disconnect: PASS ===============\n");
}

void test_publish_without_connect() {

	_reset();
	LE_INFO("\n================= Test: test_publish_without_connect ===============\n");

	char 	*sTopic 		= "Temperature";
	char 	*sPayload 		= "28 degree celsius";
	int 	nTopicLen 		= strlen(sTopic);
	int 	nPayloadLen 	= strlen(sPayload);
	int 	nQos 			= 1;

	int retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 != retCode);

	LE_INFO("\n================= test_publish_without_connect: PASS ===============\n");
}

void test_subscribe_without_connect() {

	_reset();
	LE_INFO("\n================= Test: test_subscribe_without_connect ===============\n");

	char 	*sTopic 		= "Temperature";
	int 	nTopicLen 		= strlen(sTopic);
	int 	nQos 			= 1;

	int retCode = aws_Subscribe(sTopic,nTopicLen,nQos);
	LE_ASSERT(0 != retCode);
	LE_INFO("\n================= test_subscribe_without_connect: PASS ===============\n");
}

void test_repeated_publish_for_different_topic() {

	_reset();
	LE_INFO("\n================= Test: test_repeated_publish_for_different_topic ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	sTopic[100] 		= "Temperature";
	char 	sPayload[100] 		= "28 degree celsius";
	int 	nTopicLen 			= strlen(sTopic);
	int 	nPayloadLen 		= strlen(sPayload);
	int 	nQos 				= 1;

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 == retCode);

	strcpy(sTopic,"Altitude");
	strcpy(sPayload,"9.14 m");
	nTopicLen 	= strlen(sTopic);
	nPayloadLen = strlen(sPayload);

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 == retCode);

	LE_INFO("\n================= test_repeated_publish_for_different_topic: PASS ===============\n");
}
void test_repeated_publish_for_same_topic() {

	_reset();
	LE_INFO("\n================= Test: test_repeated_publish_for_same_topic ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	sTopic[100] 		= "Temperature";
	char 	sPayload[100] 		= "28 degree celsius";
	int 	nTopicLen 			= strlen(sTopic);
	int 	nPayloadLen 		= strlen(sPayload);
	int 	nQos 				= 1;

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 == retCode);

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 == retCode);

	LE_INFO("\n================= test_repeated_publish_for_same_topic: PASS ===============\n");
}

void test_publish_with_null_topic() {

	_reset();
	LE_INFO("\n================= Test: test_publish_with_null_topic ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	*sTopic 			= "";
	char 	*sPayload 			= "28 degree celsius";
	int 	nTopicLen 			= 0;
	int 	nPayloadLen 		= strlen(sPayload);
	int 	nQos 				= 1;

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 != retCode);
	LE_INFO("\n================= test_publish_with_null_topic: PASS ===============\n");
}
void test_publish_with_invalid_topicLength() {

	_reset();
	LE_INFO("\n================= Test: test_publish_with_invalid_topicLength ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	*sTopic 			= "Temperature";
	char 	*sPayload 			= "28 degree celsius";
	int 	nTopicLen 			= 500;
	int 	nPayloadLen 		= strlen(sPayload);
	int 	nQos 				= 1;

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 != retCode);
	LE_INFO("\n================= test_publish_with_invalid_topicLength: PASS ===============\n");
}
void test_publish_with_null_payload() {

	_reset();
	LE_INFO("\n================= Test: test_publish_with_null_payload ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	*sTopic 			= "Temperature";
	char 	*sPayload 			= "";
	int 	nTopicLen 			= strlen(sTopic);
	int 	nPayloadLen 		= 0;
	int 	nQos 				= 1;

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 == retCode);
	LE_INFO("\n================= test_publish_with_null_payload: PASS ===============\n");
}
void test_publish_with_invalid_payloadLength() {

	_reset();
	LE_INFO("\n================= Test: test_publish_with_invalid_payloadLength ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	*sTopic 			= "Temperature";
	char 	*sPayload 			= "28 degree celsius";
	int 	nTopicLen 			= strlen(sTopic);
	int 	nPayloadLen 		= 500;
	int 	nQos 				= 1;

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 != retCode);
	LE_INFO("\n================= test_publish_with_invalid_payloadLength: PASS ===============\n");
}
void test_publish_with_invalid_QOS_value() {

	_reset();
	LE_INFO("\n================= Test: test_publish_with_invalid_QOS_value ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	*sTopic 			= "Temperature";
	char 	*sPayload 			= "28 degree celsius";
	int 	nTopicLen 			= strlen(sTopic);
	int 	nPayloadLen 		= strlen(sPayload);
	int 	nQos 				= -1;

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 != retCode);
	LE_INFO("\n================= test_publish_with_invalid_QOS_value: PASS ===============\n");
}
void test_repeated_subscribe_on_same_topic() {

	_reset();
	LE_INFO("\n================= Test: test_repeated_subscribe_on_same_topic ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT( 0 == retCode );

	char 	*sTopic 		= "Temperature";
	int 	nTopicLen 		= strlen(sTopic);
	int 	nQos 			= 1;

	retCode = aws_Subscribe(sTopic,nTopicLen,nQos);
	LE_ASSERT(0 == retCode);

	retCode = aws_Subscribe(sTopic,nTopicLen,nQos);
	LE_ASSERT(0 == retCode);
	LE_INFO("\n================= test_repeated_subscribe_on_same_topic: PASS ===============\n");

	retCode = aws_UnSubscribe(sTopic,nTopicLen);
	LE_ASSERT(0 == retCode);
}

void test_repeated_subscribe_on_different_topic() {

	_reset();
	LE_INFO("\n================= Test: test_repeated_subscribe_on_different_topic ===============\n");

	int retCode = aws_Connect();
	LE_ASSERT( 0 == retCode );

	char 	*sTopic 		= "Temperature";
	int 	nTopicLen 		= strlen(sTopic);
	int 	nQos 			= 1;

	retCode = aws_Subscribe(sTopic,nTopicLen,nQos);
	LE_ASSERT(0 == retCode);

	retCode = aws_Subscribe("Altitude",8,nQos);
	LE_ASSERT(0 == retCode);

	retCode = aws_Subscribe("GPS",3,nQos);
	LE_ASSERT(0 == retCode);

	LE_INFO("\n================= test_repeated_subscribe_on_different_topic: PASS ===============\n");

	retCode = aws_UnSubscribe("GPS",3);
	LE_ASSERT(0 == retCode);

	retCode = aws_UnSubscribe("Altitude",8);
	LE_ASSERT(0 == retCode);

	retCode = aws_UnSubscribe("Temperature",11); // Error occurred.
	LE_ASSERT(0 == retCode);


}

/**================================================================================
 * Task  			: Test ordered execution of exposed aws_iot functions
 *
 * Objective		: Invokes functions connect, subscribe, publish, unsubscribe
 * 					  ,disconnect to test sequential execution
 *
 * Return 			: None
 * ================================================================================
 */
void test_connect_subscribe_publish_unsubscribe_disconnect() {

	_reset();
	LE_INFO("\n================= Test: test_connect_subscribe_publish_unsubscribe_disconnect ===============\n");
	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	*sTopic 		= "Temperature";
	char 	*sPayload 		= "28 degree celsius";
	int 	nTopicLen 		= strlen(sTopic);
	int 	nPayloadLen 	= strlen(sPayload);
	int 	nQos 			= 1;

	retCode = aws_Subscribe(sTopic,nTopicLen,nQos);
	LE_ASSERT(0 == retCode);

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 == retCode);

	retCode = aws_Publish(sTopic,nTopicLen,nQos,"PUB - 2",7);
	LE_ASSERT(0 == retCode);

	retCode = aws_UnSubscribe(sTopic,nTopicLen);
	LE_ASSERT(0 == retCode);

	retCode = aws_disconnect();
	LE_ASSERT(0 == retCode);

	LE_INFO("\n================= test_connect_subscribe_publish_unsubscribe_disconnect: PASS ===============\n");
}

/**================================================================================
 * Task  			: Test behavior of publish subscribe in concurrent environment
 * Objective		: Create thread to check concurrent execution of aws_iot
 * 					  functions
 *
 * Return 			: None
 * ================================================================================
 */
void test_concurrency_of_publish_subscribe() {

	_reset();
	LE_INFO("================ TEST: test_concurrency_of_publish_subscribe ===============");

	// sub thread: connect and subscribe
	le_thread_Ref_t subThRef = le_thread_Create("t_subsribe_Thread",_subscribeThread,NULL);
	le_thread_SetJoinable(subThRef);
	le_thread_Start(subThRef);

	// pub thread: connect and publish
	le_thread_Ref_t pubThRef = le_thread_Create("t_publish_Thread",_publishThread,NULL);
	le_thread_SetJoinable(pubThRef);
	le_thread_Start(pubThRef);

	// Wait for completion of pub and sub thread
	_stopTestThread(pubThRef);
	_stopTestThread(subThRef);

	// disconnect
	int retCode = aws_disconnect();
	LE_ASSERT(0 == retCode);

	LE_INFO("================ test_concurrency_of_publish_subscribe: PASS ===============");

}

/**================================================================================
 * Task  			: Connect and publish
 * Objective		: Thread function used to execute connect and publish
 * 					  operation in a different thread to check behavior
 * 					  in a concurrent environment
 *
 * Return 			: void pointer
 * ================================================================================
 */
void* _publishThread(void *p) {
	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	*sTopic 		= "Temperature";
	char 	sPayload[256];
	char 	sBuffer[256];

	int 	nTopicLen 		= strlen(sTopic);
	int 	nQos 			= 1;
	int 	nPubCount 		= 1;
	int 	nPayloadLen;

	while ( nPubCount <= 10 ) {
		sprintf(sBuffer,"Data for pub - %d",nPubCount++);
		strcpy(sPayload,sBuffer);
		nPayloadLen = strlen(sPayload);

		retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
		if (0 != retCode) {

			// In case of publish failure terminate thread
			//stopPubThread = true;
			le_thread_Exit(NULL);
			LE_DEBUG("Publish thread is exited");
			break;
		}
	}
	return NULL;
}

/**================================================================================
 * Task  			: Connect and subscribe
 * Objective		: Thread function used to execute connect and subscribe
 * 					  operation in a different thread to check behavior
 * 					  in a concurrent environment
 *
 * Return 			: void pointer
 * ================================================================================
 */
void* _subscribeThread(void *p) {

	LE_DEBUG("Attempting to connect....");
	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char 	*sTopic 		= "Temperature";
	int 	nTopicLen 		= strlen(sTopic);
	int 	nQos 			= 1;
	int 	nWaitTime		= 10;

	retCode = aws_Subscribe(sTopic,nTopicLen,nQos);
	if (0 != retCode ) {
			le_thread_Exit(NULL);
			LE_DEBUG("Subscribe thread exited.");
			//stopSubThread = true; // true signifies thread is stopped
	} else {
		LE_DEBUG("Wait for %d seconds before unsubscribing topic %s",nWaitTime,sTopic);
		sleep(nWaitTime);

		retCode = aws_UnSubscribe(sTopic,nTopicLen);
		if (0 != retCode ) {
			le_thread_Exit(NULL);
			LE_DEBUG("Subscribe thread exited.");
			//stopSubThread = true; // true signifies thread is stopped
		}
	}
	return NULL;
}

/**================================================================================
 * Task  			: Terminates thread using thread reference
 * Objective		: Used to terminate thread after execution
 *
 * Return 			: None
 * ================================================================================
 */
void _stopTestThread(le_thread_Ref_t thRef) {
	if (thRef != le_thread_GetCurrent()) {
		void* threadResult;
		le_thread_Join(thRef, &threadResult);
		LE_DEBUG("Test thread is joined to main thread");
	}
}

void* _multiSubscribeThread(void *p) {
	char 	*sTopic 		= "Temperature";
	int 	nQos 			= 1;
	int 	nWaitTime		= 5;
	int 	nSubCount		= 1;
	char    sBuffer[256];
	int 	retCode;

	while ( nSubCount <= 5 ) {
		sprintf(sBuffer,"%s-%d",sTopic,nSubCount++);
		retCode = aws_Subscribe(sBuffer,strlen(sBuffer),nQos);
		if (0 != retCode ) {
			LE_DEBUG("Error(%d) subscribing topic.",retCode);
		}
	}

	LE_DEBUG("Wait %d secs before leaving subscribe thread",nWaitTime);
	sleep(nWaitTime);
	return NULL;
}
void* _multiUnsubscribeThread(void *p) {
	char 	*sTopic 		= "Temperature";
	int 	nWaitTime		= 5;
	int 	nUnsubCount		= 1;
	char    sBuffer[256];
	int 	retCode;

	while ( nUnsubCount <= 5 ) {
		sprintf(sBuffer,"%s-%d",sTopic,nUnsubCount++);
		retCode = aws_UnSubscribe(sBuffer,strlen(sBuffer));
		strcpy(sBuffer,"");
		if (0 != retCode ) {
			LE_DEBUG("Error(%d) unsubscribing topic.",retCode);
		}
	}

	LE_DEBUG("Wait %d secs before leaving subscribe thread",nWaitTime);
	sleep(nWaitTime);
	return NULL;

}
void test_unsubscribe_on_subscribe() {

	_reset();
	LE_INFO("================ TEST: test_unsubscribe_on_subscribe ===============");
	int retCode = aws_Connect();
	char 	*sTopic 		= "Temperature";
	int 	nTopicLen 		= strlen(sTopic);
	int 	nQos 			= 1;

	retCode = aws_Subscribe(sTopic,nTopicLen,nQos);
	LE_ASSERT( 0 == retCode );

	retCode = aws_UnSubscribe(sTopic,nTopicLen);
	LE_ASSERT( 0 == retCode );
	LE_INFO("================ test_unsubscribe_on_subscribe: PASS ===============");
}
void test_repeated_unsubscribe_on_subscribe() {

	_reset();
	LE_INFO("================ TEST: test_repeated_unsubscribe_on_subscribe ===============");
	int retCode = aws_Connect();
	char 	*sTopic 		= "Temperature";
	int 	nTopicLen 		= strlen(sTopic);
	int 	nQos 			= 1;

	retCode = aws_Subscribe(sTopic,nTopicLen,nQos);
	LE_ASSERT( 0 == retCode );

	retCode = aws_UnSubscribe(sTopic,nTopicLen);
	LE_ASSERT( 0 == retCode );

	retCode = aws_UnSubscribe(sTopic,nTopicLen);
	LE_DEBUG("retCode = %d",retCode);
	LE_ASSERT( 0 != retCode );
	LE_INFO("================ test_repeated_unsubscribe_on_subscribe: PASS ===============");
}
void test_unsubscribe_without_subscribe() {
	_reset();
	LE_INFO("================ TEST: test_unsubscribe_without_subscribe ===============");
	int retCode = aws_Connect();
	char 	*sTopic 		= "Temperature";
	int 	nTopicLen 		= strlen(sTopic);

	retCode = aws_UnSubscribe(sTopic,nTopicLen);
	LE_DEBUG("retCode = %d",retCode);
	LE_ASSERT( 0 != retCode );
	LE_INFO("================ test_unsubscribe_without_subscribe: PASS ===============");
}
void test_unsubscribe_for_unordered_subscribe() {
	_reset();
	LE_INFO("================ TEST: test_unsubscribe_for_unordered_subscribe ===============");
	int retCode = aws_Connect();
	LE_ASSERT( 0 == retCode );

	retCode = aws_Subscribe("Temperature",11,1);
	LE_ASSERT( 0 == retCode );

	retCode = aws_Subscribe("Light",5,1);
	LE_ASSERT( 0 == retCode );

	retCode = aws_Subscribe("GPS",3,1);
	LE_ASSERT( 0 == retCode );

	LE_DEBUG("Test: Trying to unsubscribe topic 'GPS'");
	retCode = aws_UnSubscribe("GPS",3);
	LE_ASSERT( 0 == retCode );

	LE_DEBUG("Test: Trying to unsubscribe topic 'Temperature'");
	retCode = aws_UnSubscribe("Temperature",11);
	LE_ASSERT( 0 == retCode );

	LE_DEBUG("Test: Trying to unsubscribe topic 'Light'");
	retCode = aws_UnSubscribe("Light",5);
	LE_ASSERT( 0 == retCode );

	LE_INFO("================ test_unsubscribe_for_unordered_subscribe: PASS ===============");
}
void test_concurrency_of_subscribe_and_unsubscribe() {
	_reset();
	LE_INFO("================ TEST: test_concurrent_subscribe_and_unsubscribe ===============");
	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	// sub thread: subscribe more than one time
	le_thread_Ref_t subThRef = le_thread_Create("t_Multi_Subs_Thread",_multiSubscribeThread,NULL);
	le_thread_SetJoinable(subThRef);
	le_thread_Start(subThRef);

	// unsubscribe thread: unsubscribe more than one time
	le_thread_Ref_t unsubThRef = le_thread_Create("t_Multi_Unsubs_Thread",_multiUnsubscribeThread,NULL);
	le_thread_SetJoinable(unsubThRef);
	le_thread_Start(unsubThRef);

	// Wait for completion of subscribe and unsubscribe
	LE_DEBUG("Terminating thread for multi_subscribe");
	_stopTestThread(subThRef);
	LE_DEBUG("Terminating thread for multi_unsubscribe");
	_stopTestThread(unsubThRef);

	// disconnect
	retCode = aws_disconnect();
	LE_ASSERT(0 == retCode);

	LE_INFO("================ test_concurrent_subscribe_and_unsubscribe: PASS ===============");

}

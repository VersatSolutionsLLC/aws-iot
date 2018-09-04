#include "legato.h"

#include "../ConfigManager/config.h"
#include "interfaces.h"

# define TEST_SECTION	"agent_1"
# define KEY_TOPIC_NAME "Topic"
# define KEY_QOS		"QOS"
# define DEFAULT_TOPIC_NAME ""
# define DEFAULT_QOS_VALUE 1

void MyHandlerFunc(char *ptrPayload, void *reportPayloadPtr) {
	LE_INFO("\n========== (subscribe_component) PAYLOAD DATA =============\n");
	LE_INFO("\n\t\t %s", ptrPayload);
	LE_INFO("\n===========================================================\n");
}

void _subscribe() {
	int retCode = aws_Connect();
	LE_ASSERT(0 == retCode);

	char sTopic[50];
	int nQos = 0;

	config_GetString(TEST_SECTION, KEY_TOPIC_NAME,DEFAULT_TOPIC_NAME, sTopic, 50);
	config_GetInteger(TEST_SECTION, KEY_QOS,DEFAULT_QOS_VALUE, &nQos);

	int nTopicLen = strlen(sTopic);

	retCode = aws_Subscribe(sTopic, nTopicLen, nQos);
	LE_ASSERT(0 == retCode);

	aws_AddSubscribeEventHandler(
			(aws_SubscribeEventHandlerFunc_t) MyHandlerFunc, NULL);

	LE_ASSERT(0 == retCode);

	LE_INFO("Waiting for 5 secs before unsubscribing topic");
	sleep(5);

	retCode = aws_UnSubscribe(sTopic, nTopicLen);
	LE_ASSERT(0 == retCode);
	LE_INFO("====================END===================================");
}

void _connected(char* infName, bool isConnected, void* contextPtr) {
	if (!isConnected) {
		LE_WARN("Device not connect to Internet!");
		return;
	}

	if (isConnected) {
		_subscribe();
	}

}
COMPONENT_INIT {
	if (radio_IsConnected()) {
		_connected("manual", true, NULL);
	}
	radio_AddDataConnectionStateHandler(
			(radio_DataConnectionStateHandlerFunc_t) _connected, NULL);

	radio_Connect();

}

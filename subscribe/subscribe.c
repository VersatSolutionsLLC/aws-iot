#include "legato.h"
#include "interfaces.h"

void MyHandlerFunc (char *ptrPayload, void *reportPayloadPtr )
{
	LE_INFO("\n========== (subscribe_component) PAYLOAD DATA =============\n");
	LE_INFO("\n\t\t %s",ptrPayload);
	LE_INFO("\n===========================================================\n");
}
COMPONENT_INIT
{
	int retCode = aws_Connect();
	LE_ASSERT( 0 == retCode );

	char 	*sTopic 			= "Temperature";
	char 	sPayload[100] 		= "28 degree celsius";
	int 	nTopicLen 			= strlen(sTopic);
	int 	nPayloadLen 		= strlen(sPayload);
	int 	nQos 				= 1;

	retCode = aws_Subscribe(sTopic,nTopicLen,nQos);
	LE_ASSERT(0 == retCode);

	aws_AddSubscribeEventHandler((aws_SubscribeEventHandlerFunc_t)MyHandlerFunc, NULL);

	retCode = aws_Publish(sTopic,nTopicLen,nQos,sPayload,nPayloadLen);
	LE_ASSERT(0 == retCode);

	LE_INFO("Waiting for 5 secs before unsubscribing topic");
	sleep(5);

	retCode = aws_UnSubscribe(sTopic,nTopicLen);
	LE_ASSERT(0 == retCode);
	LE_INFO("====================END===================================");
}

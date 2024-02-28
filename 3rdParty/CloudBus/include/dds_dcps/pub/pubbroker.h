#ifndef _CLOUD_BUS_PUB_BROKER_H_
#define _CLOUD_BUS_PUB_BROKER_H_

#include "../core/dcps_core.h"
#include <map>
#include <string>

#if HERMES_OS == HERMES_OS_WINDOWS_NT
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

BEGIN_DDS_NAMESPACE

class PubBroker
{
public:
	PubBroker();

	~PubBroker();

	ReturnCode_t pub_data(Long domainId,const NodeId& nodeId, const char* topicName, const char* data, int len);

	ReturnCode_t pub_topic(Long domainId, const char* topicName);


private:
	SOCKET									m_socket;
	typedef std::map<std::string, size_t>	SeqMap;
	SeqMap									m_seqMap;	
};

END_DDS_NAMESPACE

#endif
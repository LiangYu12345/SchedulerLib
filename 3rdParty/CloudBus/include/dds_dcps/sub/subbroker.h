#ifndef _CLOUD_BUS_PUB_BROKER_H_
#define _CLOUD_BUS_PUB_BROKER_H_

#include "../core/dcps_core.h"
#include <WinSock2.h>
#include <map>
#include <string>
#include <list>

using std::list;
using std::string;
using std::map;

#pragma comment(lib,"ws2_32.lib")

BEGIN_DDS_NAMESPACE

class SubscriberImpl;

typedef std::pair<long, string>			TopicPair;
typedef list<TopicPair>					TopicList;
typedef map<ULONG, TopicList>			MultiGroupMap;
typedef map<string, DataSeq>			TopicSeqMap;
typedef map<NodeId, TopicSeqMap>		NodeTopicSeqMap;
typedef map<Long, NodeTopicSeqMap>		DataSeqMap;

class SubBroker
{
public:
	SubBroker(SubscriberImpl*);
	~SubBroker();

	bool			start();

	bool			stop();

	bool			isStarted();

	bool			isStopped();

	void			onRecv(char* data, size_t len);

	void			sub_topic(Long domainId, const char* topicName);

	void			cancel_sub(Long domainId, const char* topicName);

	bool			joinMulitGroup(ULONG& mcastAddr, ULONG& interfaceAddr);

	bool			leaveMultiGroup(ULONG& mcastAddr, ULONG& interfaceAddr);

	SOCKET			getSocket();

protected:
	bool			addTopic(Long domainId, const char* topicName);

	bool			delTopic(Long domainId, const char* topicName);

	bool			isTopicExist(Long domainId, const char* topicName);

	bool			isGroupEmpty(ULONG mcastGroup);

	bool			isGroupExist(ULONG mcastGroup);


private:
	SOCKET									m_socket;
	SubscriberImpl*							m_pSub;
	bool									m_bStopped;
	bool									m_bStarted;
	DataSeqMap								m_dataSeqMap;
	MultiGroupMap							m_groupMap;
};

END_DDS_NAMESPACE

#endif
#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "RdtReceiver.h"
class TCPreceiver :public RdtReceiver {
private:
	int expectedseqnum;            //�ڴ����յ���һ�������
	Packet lastAckPkt;             //
public:
	TCPreceiver();
	virtual ~TCPreceiver();
public:
	void receive(Packet &packet);
};
#endif

#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "RdtReceiver.h"
class GBNreceiver :public RdtReceiver {
private:
	int expectedseqnum;            //�ڴ����յ���һ�������
	Packet lastAckPkt;             //
public:
	GBNreceiver();
	virtual ~GBNreceiver();
public:
	void receive(Packet &packet);
};
#endif

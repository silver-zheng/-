#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "RdtReceiver.h"
class GBNreceiver :public RdtReceiver {
private:
	int expectedseqnum;            //期待接收的下一分组序号
	Packet lastAckPkt;             //
public:
	GBNreceiver();
	virtual ~GBNreceiver();
public:
	void receive(Packet &packet);
};
#endif

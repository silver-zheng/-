#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H
#include "RdtReceiver.h"
#include "SRsender.h"
class SRreceiver :public RdtReceiver {
private:
	int receivebase;
	//int expectedseqnum;            //期待接收的下一分组序号
	Packet *recpkt = new Packet[PKTSIZE];//滑动窗口
	Packet lastAckPkt;
public:
	SRreceiver();
	virtual ~SRreceiver();
public:
	void receive(Packet &packet);
};
#endif


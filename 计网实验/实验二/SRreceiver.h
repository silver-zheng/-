#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H
#include "RdtReceiver.h"
#include "SRsender.h"
class SRreceiver :public RdtReceiver {
private:
	int receivebase;
	//int expectedseqnum;            //�ڴ����յ���һ�������
	Packet *recpkt = new Packet[PKTSIZE];//��������
	Packet lastAckPkt;
public:
	SRreceiver();
	virtual ~SRreceiver();
public:
	void receive(Packet &packet);
};
#endif


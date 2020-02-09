#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H
#include "RdtSender.h"
#define N 4
#define PKTSIZE 8
class SRsender :public RdtSender {
private:
	int sendbase;                      //�����
	int nextseqnum;                //��һ���
	bool waitingState;             //������������֮��Ϊ��
	int pktsize = PKTSIZE;
	Packet *sndpkt = new Packet[pktsize];//��������
public:
	bool send(Message &message);
	void receive(Packet & ackPkt);
	void timeoutHandler(int seqNum);
	bool getWaitingState();
	SRsender();
	virtual ~SRsender();
};



#endif

#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H
#include "RdtSender.h"
#define N 4
#define PKTSIZE 8
class SRsender :public RdtSender {
private:
	int sendbase;                      //基序号
	int nextseqnum;                //下一序号
	bool waitingState;             //滑动窗口满了之后为真
	int pktsize = PKTSIZE;
	Packet *sndpkt = new Packet[pktsize];//滑动窗口
public:
	bool send(Message &message);
	void receive(Packet & ackPkt);
	void timeoutHandler(int seqNum);
	bool getWaitingState();
	SRsender();
	virtual ~SRsender();
};



#endif

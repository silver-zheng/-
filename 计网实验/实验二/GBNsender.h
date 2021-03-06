#ifndef GBN_RDT_SENDER_H
#define GBN_RDT_SENDER_H
#include "RdtSender.h"
#define N 4
#define PKTSIZE 8
class GBNsender :public RdtSender {
private:
	int base;                      //基序号
	int nextseqnum;                //下一序号
	bool waitingState;             //滑动窗口满了之后为真
	int windowsize = PKTSIZE;
	Packet *sndpkt = new Packet[windowsize];//滑动窗口
public:
	bool send(Message &message);
	void receive(Packet & ackPkt);
	void timeoutHandler(int seqNum);
	bool getWaitingState();
	GBNsender();
	virtual ~GBNsender();
};



#endif

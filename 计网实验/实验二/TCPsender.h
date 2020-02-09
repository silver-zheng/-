#ifndef GBN_RDT_SENDER_H
#define GBN_RDT_SENDER_H
#include "RdtSender.h"
#define N 4
#define PKTSIZE 8
class TCPsender :public RdtSender {
private:
	int base;                      //�����
	int nextseqnum;                //��һ���
	int y;                         //�ظ�ACK����
	bool waitingState;             //������������֮��Ϊ��
	int windowsize = PKTSIZE ;
	Packet *sndpkt = new Packet[windowsize];//��������
public:
	bool send(Message &message);
	void receive(Packet & ackPkt);
	void timeoutHandler(int seqNum);
	bool getWaitingState();
	TCPsender();
	virtual ~TCPsender();
};



#endif

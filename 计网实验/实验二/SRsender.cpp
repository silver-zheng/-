#include "stdafx.h"
#include "Global.h"
#include "SRSender.h"

bool make_sure[PKTSIZE] = {0,0,0,0,0,0,0,0};//用于标记分组是否被接收

SRsender::SRsender() :sendbase(0), nextseqnum(0), waitingState(false) {}

SRsender::~SRsender() {}

bool SRsender::getWaitingState() {
	if (nextseqnum == (sendbase - 1 + N) % PKTSIZE) {
		waitingState = true;
	}
	else waitingState = false;
	return waitingState;
}

bool SRsender::send(Message &message) {
	if (getWaitingState())//发从窗口满了，不将应用层的数据交给Rdt
		return false;
	make_sure[nextseqnum] = false;//刚刚发送的分组标记为未接收
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
	sndpkt[nextseqnum].acknum = -1;//忽略该字段
	sndpkt[nextseqnum].seqnum = nextseqnum;
	sndpkt[nextseqnum].checksum = 0;
	sndpkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndpkt[nextseqnum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, sndpkt[nextseqnum].seqnum);
	pUtils->printPacket("发送方发送报文", sndpkt[nextseqnum]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);
	nextseqnum = (nextseqnum + 1) % pktsize;



	printf("此时窗口的包为：\n");
	for (int i = sendbase; i != nextseqnum; i = (i + 1) % PKTSIZE)
		pUtils->printPacket("", sndpkt[i]);

	return true;
}

void SRsender::receive(Packet &ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum == ackPkt.checksum) {
		pUtils->printPacket("发送方正确收到确认", ackPkt);
		if (make_sure[ackPkt.acknum] == false) {//如果还未确认，则标记为确认
			make_sure[ackPkt.acknum] = true;
			pns->stopTimer(SENDER, ackPkt.acknum);
			while (make_sure[sendbase] && sendbase != nextseqnum) {
				//puts("111\n");
				sendbase = (sendbase + 1) % pktsize;
			}
			
			//printf("%d \n", waitingState);
		}
		printf("完成接收后：sendbase=%d   nextseqnum=%d\n", sendbase, nextseqnum); 
		//printf("%d  %d\n", sendbase, nextseqnum);
		
		if (sendbase != nextseqnum) {
			printf("此时窗口的包为：\n");
			for (int i = sendbase; i != nextseqnum; i = (i + 1) % PKTSIZE)
				pUtils->printPacket("", sndpkt[i]);
		}
		else printf("此时窗口为空!\n");
	}
}

void SRsender::timeoutHandler(int seqNum) {
	pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", sndpkt[seqNum]);
	pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	pns->sendToNetworkLayer(RECEIVER, sndpkt[seqNum]);			//重新发送数据包
}
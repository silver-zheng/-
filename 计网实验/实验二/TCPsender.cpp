#include "stdafx.h"
#include "Global.h"
#include "TCPSender.h"



TCPsender::TCPsender():base(0),nextseqnum(0),y(0),waitingState(false){}

TCPsender::~TCPsender(){}

bool TCPsender::getWaitingState() {
	if (nextseqnum == (base - 1 + N) % PKTSIZE ) {
		waitingState = true;
	}
	else waitingState = false;
	return waitingState;
}

bool TCPsender::send(Message &message) {
	if (getWaitingState())//发从窗口满了，不将应用层的数据交给Rdt
		return false;
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
	sndpkt[nextseqnum].seqnum = nextseqnum;
	sndpkt[nextseqnum].acknum = -1;
	sndpkt[nextseqnum].checksum = 0;
	sndpkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndpkt[nextseqnum]);
	pUtils->printPacket("发送方发送报文", sndpkt[nextseqnum]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);
	if (base == nextseqnum) {
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	}
	nextseqnum = (nextseqnum + 1) % PKTSIZE;


	printf("此时窗口的包为：\n");
	for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
		pUtils->printPacket("", sndpkt[i]);
	return true;
}

void TCPsender::receive(Packet &ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	//正确收到分组，窗口往右滑动
	if (checkSum == ackPkt.checksum) {
		/*int num = ackPkt.acknum - base + 1;//窗口应该右移的数目
		for (int i = 0; i < num;i++) {
			for (int j = base; j < nextseqnum; j++)
				memcpy(sndpkt[base + i].payload, sndpkt[base + i + 1].payload, sizeof(sndpkt[base].payload));
		}*/
		if (base + N <= PKTSIZE && (ackPkt.acknum <= base + N - 1  && ackPkt.acknum >= base)
			||base + N > PKTSIZE && (ackPkt.acknum <= (base + N - 1) % PKTSIZE || ackPkt.acknum >= base)) {
			base = (ackPkt.acknum + 1) % PKTSIZE;
			y = 0;
		}
		else y++;
		printf("\n------y=%d-----", y);
		if (y == 3) {
			y = 0;
			pns->stopTimer(SENDER, 0);
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
			printf("快速重传开始，重传的分组为：\n");
			pUtils->printPacket("", sndpkt[base]);
			pns->sendToNetworkLayer(RECEIVER, sndpkt[base]);
		}
		else if (y == 0) {
			pUtils->printPacket("发送方正确收到确认", ackPkt);
			pns->stopTimer(SENDER, 0);
			if (base != nextseqnum)//如果窗口非空，继续计时
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);

			printf("此时窗口的包为：\n");
			for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
				pUtils->printPacket("", sndpkt[i]);
		}
		else
		pUtils->printPacket("发送方没有正确收到确认", ackPkt);
	}
}

void TCPsender::timeoutHandler(int seqNum) {
	pUtils->printPacket("发送方定时器时间到，重发序号为base的报文:", sndpkt[base]);
	pns->stopTimer(SENDER, 0);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	//重传序号是base的分组
	pUtils->printPacket(",", sndpkt[base]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[base]);
}

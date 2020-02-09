#include "stdafx.h"
#include "Global.h"
#include "GBNSender.h"


int SEQNUM = 0;//用于上层数据分组按序分配序号

GBNsender::GBNsender():base(0),nextseqnum(0),waitingState(false){}

GBNsender::~GBNsender(){}

bool GBNsender::getWaitingState() {
	if (nextseqnum == (base - 1 + N) % windowsize) {
		waitingState = true;
	}
	else waitingState = false;
	return waitingState;
}

bool GBNsender::send(Message &message) {
	if (getWaitingState())//发从窗口满了，不将应用层的数据交给Rdt
		return false;
	if (nextseqnum < base + N) {
		memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
		sndpkt[nextseqnum].seqnum = nextseqnum;
		sndpkt[nextseqnum].acknum = -1;
		sndpkt[nextseqnum].checksum = 0;
		//SEQNUM++;
		sndpkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndpkt[nextseqnum]);
		pUtils->printPacket("发送方发送报文", sndpkt[nextseqnum]);
		pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);
		if (base == nextseqnum) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
		nextseqnum = (nextseqnum + 1) % windowsize;

		printf("此时窗口的包为：\n");
		for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
			pUtils->printPacket("", sndpkt[i]);
		return true;
	}
}

void GBNsender::receive(Packet &ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	//正确收到分组，窗口往右滑动
	if (checkSum == ackPkt.checksum) {
		/*int num = ackPkt.acknum - base + 1;//窗口应该右移的数目
		for (int i = 0; i < num;i++) {
			for (int j = base; j < nextseqnum; j++)
				memcpy(sndpkt[base + i].payload, sndpkt[base + i + 1].payload, sizeof(sndpkt[base].payload));
		}*/
		base = (ackPkt.acknum + 1) % windowsize;
		//waitingState = false;//此时窗口有位置存放应用层来的数据
		pUtils->printPacket("发送方正确收到确认", ackPkt);
		pns->stopTimer(SENDER, 0);
		if(base != nextseqnum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		printf("完成接收后：base=%d   nextseqnum=%d\n", base, nextseqnum);
		//printf("%d  %d\n", sendbase, nextseqnum);

		if (base != nextseqnum) {
			printf("此时窗口的包为：\n");
			for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
				pUtils->printPacket("", sndpkt[i]);
		}
		else printf("此时窗口为空!\n");
	}
}

void GBNsender::timeoutHandler(int seqNum) {
	printf("发送方定时器时间到，重发窗口全部报文!\n");
	printf("此时窗口的包为：\n");
	for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
		pUtils->printPacket("", sndpkt[i]);
	puts("开始重发:\n");
	pns->stopTimer(SENDER, 0);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	for (int i = base; i != nextseqnum; i = (i + 1) % windowsize) {
		//重传序号大于base的全部分组
		pUtils->printPacket(",", sndpkt[i]);
		pns->sendToNetworkLayer(RECEIVER, sndpkt[i]);
	}
}

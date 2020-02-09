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
	if (getWaitingState())//���Ӵ������ˣ�����Ӧ�ò�����ݽ���Rdt
		return false;
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
	sndpkt[nextseqnum].seqnum = nextseqnum;
	sndpkt[nextseqnum].acknum = -1;
	sndpkt[nextseqnum].checksum = 0;
	sndpkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndpkt[nextseqnum]);
	pUtils->printPacket("���ͷ����ͱ���", sndpkt[nextseqnum]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);
	if (base == nextseqnum) {
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	}
	nextseqnum = (nextseqnum + 1) % PKTSIZE;


	printf("��ʱ���ڵİ�Ϊ��\n");
	for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
		pUtils->printPacket("", sndpkt[i]);
	return true;
}

void TCPsender::receive(Packet &ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	//��ȷ�յ����飬�������һ���
	if (checkSum == ackPkt.checksum) {
		/*int num = ackPkt.acknum - base + 1;//����Ӧ�����Ƶ���Ŀ
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
			printf("�����ش���ʼ���ش��ķ���Ϊ��\n");
			pUtils->printPacket("", sndpkt[base]);
			pns->sendToNetworkLayer(RECEIVER, sndpkt[base]);
		}
		else if (y == 0) {
			pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
			pns->stopTimer(SENDER, 0);
			if (base != nextseqnum)//������ڷǿգ�������ʱ
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);

			printf("��ʱ���ڵİ�Ϊ��\n");
			for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
				pUtils->printPacket("", sndpkt[i]);
		}
		else
		pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
	}
}

void TCPsender::timeoutHandler(int seqNum) {
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط����Ϊbase�ı���:", sndpkt[base]);
	pns->stopTimer(SENDER, 0);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	//�ش������base�ķ���
	pUtils->printPacket(",", sndpkt[base]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[base]);
}

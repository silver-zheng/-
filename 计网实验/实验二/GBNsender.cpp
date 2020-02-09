#include "stdafx.h"
#include "Global.h"
#include "GBNSender.h"


int SEQNUM = 0;//�����ϲ����ݷ��鰴��������

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
	if (getWaitingState())//���Ӵ������ˣ�����Ӧ�ò�����ݽ���Rdt
		return false;
	if (nextseqnum < base + N) {
		memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
		sndpkt[nextseqnum].seqnum = nextseqnum;
		sndpkt[nextseqnum].acknum = -1;
		sndpkt[nextseqnum].checksum = 0;
		//SEQNUM++;
		sndpkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndpkt[nextseqnum]);
		pUtils->printPacket("���ͷ����ͱ���", sndpkt[nextseqnum]);
		pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);
		if (base == nextseqnum) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
		nextseqnum = (nextseqnum + 1) % windowsize;

		printf("��ʱ���ڵİ�Ϊ��\n");
		for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
			pUtils->printPacket("", sndpkt[i]);
		return true;
	}
}

void GBNsender::receive(Packet &ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	//��ȷ�յ����飬�������һ���
	if (checkSum == ackPkt.checksum) {
		/*int num = ackPkt.acknum - base + 1;//����Ӧ�����Ƶ���Ŀ
		for (int i = 0; i < num;i++) {
			for (int j = base; j < nextseqnum; j++)
				memcpy(sndpkt[base + i].payload, sndpkt[base + i + 1].payload, sizeof(sndpkt[base].payload));
		}*/
		base = (ackPkt.acknum + 1) % windowsize;
		//waitingState = false;//��ʱ������λ�ô��Ӧ�ò���������
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		pns->stopTimer(SENDER, 0);
		if(base != nextseqnum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		printf("��ɽ��պ�base=%d   nextseqnum=%d\n", base, nextseqnum);
		//printf("%d  %d\n", sendbase, nextseqnum);

		if (base != nextseqnum) {
			printf("��ʱ���ڵİ�Ϊ��\n");
			for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
				pUtils->printPacket("", sndpkt[i]);
		}
		else printf("��ʱ����Ϊ��!\n");
	}
}

void GBNsender::timeoutHandler(int seqNum) {
	printf("���ͷ���ʱ��ʱ�䵽���ط�����ȫ������!\n");
	printf("��ʱ���ڵİ�Ϊ��\n");
	for (int i = base; i != nextseqnum; i = (i + 1) % PKTSIZE)
		pUtils->printPacket("", sndpkt[i]);
	puts("��ʼ�ط�:\n");
	pns->stopTimer(SENDER, 0);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	for (int i = base; i != nextseqnum; i = (i + 1) % windowsize) {
		//�ش���Ŵ���base��ȫ������
		pUtils->printPacket(",", sndpkt[i]);
		pns->sendToNetworkLayer(RECEIVER, sndpkt[i]);
	}
}

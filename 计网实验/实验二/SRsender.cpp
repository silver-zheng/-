#include "stdafx.h"
#include "Global.h"
#include "SRSender.h"

bool make_sure[PKTSIZE] = {0,0,0,0,0,0,0,0};//���ڱ�Ƿ����Ƿ񱻽���

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
	if (getWaitingState())//���Ӵ������ˣ�����Ӧ�ò�����ݽ���Rdt
		return false;
	make_sure[nextseqnum] = false;//�ոշ��͵ķ�����Ϊδ����
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
	sndpkt[nextseqnum].acknum = -1;//���Ը��ֶ�
	sndpkt[nextseqnum].seqnum = nextseqnum;
	sndpkt[nextseqnum].checksum = 0;
	sndpkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndpkt[nextseqnum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, sndpkt[nextseqnum].seqnum);
	pUtils->printPacket("���ͷ����ͱ���", sndpkt[nextseqnum]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);
	nextseqnum = (nextseqnum + 1) % pktsize;



	printf("��ʱ���ڵİ�Ϊ��\n");
	for (int i = sendbase; i != nextseqnum; i = (i + 1) % PKTSIZE)
		pUtils->printPacket("", sndpkt[i]);

	return true;
}

void SRsender::receive(Packet &ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum == ackPkt.checksum) {
		pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
		if (make_sure[ackPkt.acknum] == false) {//�����δȷ�ϣ�����Ϊȷ��
			make_sure[ackPkt.acknum] = true;
			pns->stopTimer(SENDER, ackPkt.acknum);
			while (make_sure[sendbase] && sendbase != nextseqnum) {
				//puts("111\n");
				sendbase = (sendbase + 1) % pktsize;
			}
			
			//printf("%d \n", waitingState);
		}
		printf("��ɽ��պ�sendbase=%d   nextseqnum=%d\n", sendbase, nextseqnum); 
		//printf("%d  %d\n", sendbase, nextseqnum);
		
		if (sendbase != nextseqnum) {
			printf("��ʱ���ڵİ�Ϊ��\n");
			for (int i = sendbase; i != nextseqnum; i = (i + 1) % PKTSIZE)
				pUtils->printPacket("", sndpkt[i]);
		}
		else printf("��ʱ����Ϊ��!\n");
	}
}

void SRsender::timeoutHandler(int seqNum) {
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط��ϴη��͵ı���", sndpkt[seqNum]);
	pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
	pns->sendToNetworkLayer(RECEIVER, sndpkt[seqNum]);			//���·������ݰ�
}
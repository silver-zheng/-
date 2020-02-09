#include "stdafx.h"
#include "Global.h"
#include "SRsender.h"
#include "SRreceiver.h"

bool buff_sure[PKTSIZE] = { 0,0,0,0,0,0,0,0 };

SRreceiver::SRreceiver() :receivebase(0) {
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

SRreceiver::~SRreceiver(){}

void SRreceiver::receive(Packet &packet) {
	int checkSum = pUtils->calculateCheckSum(packet);
	if (checkSum == packet.checksum) {
		//ֻҪ�յ�����ȷ�ķ���
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
		if (receivebase + N <= PKTSIZE && (packet.seqnum <= receivebase + N - 1 && packet.seqnum >= receivebase)
			|| receivebase + N > PKTSIZE && (packet.seqnum <= (receivebase + N - 1) % PKTSIZE || packet.seqnum >= receivebase)) {
			pUtils->printPacket("�����ڴ�����", packet);
			lastAckPkt.acknum = packet.seqnum;
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);
			buff_sure[packet.seqnum] = true;
			recpkt[packet.seqnum] = packet;
			while (buff_sure[receivebase]) {
				Message msg;
				memcpy(msg.data, recpkt[receivebase].payload, sizeof(packet.payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				buff_sure[receivebase] = false;
				receivebase = (receivebase + 1) % PKTSIZE;
			}
		}
		else  {
			pUtils->printPacket("�����ڴ�����", packet);
			lastAckPkt.acknum = packet.seqnum;
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);
		}
	}
	else pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
}


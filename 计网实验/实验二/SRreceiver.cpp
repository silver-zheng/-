#include "stdafx.h"
#include "Global.h"
#include "SRsender.h"
#include "SRreceiver.h"

bool buff_sure[PKTSIZE] = { 0,0,0,0,0,0,0,0 };

SRreceiver::SRreceiver() :receivebase(0) {
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

SRreceiver::~SRreceiver(){}

void SRreceiver::receive(Packet &packet) {
	int checkSum = pUtils->calculateCheckSum(packet);
	if (checkSum == packet.checksum) {
		//只要收到了正确的分组
		pUtils->printPacket("接收方正确收到发送方的报文", packet);
		if (receivebase + N <= PKTSIZE && (packet.seqnum <= receivebase + N - 1 && packet.seqnum >= receivebase)
			|| receivebase + N > PKTSIZE && (packet.seqnum <= (receivebase + N - 1) % PKTSIZE || packet.seqnum >= receivebase)) {
			pUtils->printPacket("报文在窗口内", packet);
			lastAckPkt.acknum = packet.seqnum;
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("接收方发送确认报文", lastAckPkt);
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
			pUtils->printPacket("报文在窗口外", packet);
			lastAckPkt.acknum = packet.seqnum;
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("接收方发送确认报文", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);
		}
	}
	else pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
}


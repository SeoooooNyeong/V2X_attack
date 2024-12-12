#include "veins/modules/application/traci/TraCIReplaymo11p.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include <cstring>
#include <sstream>
#include <thread>
#include <chrono>

using namespace veins;

Define_Module(veins::TraCIReplaymo11p);

void TraCIReplaymo11p::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;

        packetsSent = 0;
        packetsReceived = 0;
        totalDelay = 0.0;
        totalBytesReceived = 0.0;
        collisions = 0;
        simulationStartTime = simTime().dbl();

        attackFlag = false;

        // Schedule the first message for sending attack messages
        scheduleAt(simTime() + 1, new cMessage("startAttack"));
    }
}

void TraCIReplaymo11p::finish() {
    simulationEndTime = simTime().dbl();

    double pdr = packetsReceived / static_cast<double>(packetsSent);
    double avgEndToEndDelay = (packetsReceived > 0) ? totalDelay / packetsReceived : 0.0;

    recordScalar("Packet Delivery Ratio (PDR)", pdr);
    recordScalar("Average End-to-End Delay", avgEndToEndDelay);
}

TraCIReplaymo11p::~TraCIReplaymo11p() {
    stopAttack();
    for (auto& msg : capturedMessages) {
        delete msg;
    }
}

void TraCIReplaymo11p::onWSA(DemoServiceAdvertisment* wsa) {
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

// void TraCIReplaymo11p::onWSM(BaseFrame1609_4* frame) {
//     TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
//     findHost()->getDisplayString().setTagArg("i", 1, "green");

//     double delay = simTime().dbl() - wsm->getTimestamp().dbl();
//     totalDelay += delay;
//     packetsReceived++;
//     totalBytesReceived += wsm->getByteLength();

//     captureMessage(wsm);
//     EV << "Captured message: " << wsm->getDemoData() << endl;

//     // RSU에 캡처한 메시지 재전송
//     scheduleAt(simTime() + 1, new cMessage("startAttack"));
// }

void TraCIReplaymo11p::onWSM(BaseFrame1609_4* frame) {
    EV << "onWSM called" << endl;
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
    findHost()->getDisplayString().setTagArg("i", 1, "green");

    double delay = simTime().dbl() - wsm->getTimestamp().dbl();
    totalDelay += delay;
    packetsReceived++;
    totalBytesReceived += wsm->getByteLength();

    // 모든 메시지를 캡처합니다
    captureMessage(wsm);
    EV << "Received message: " << wsm->getDemoData() << endl;

    scheduleAt(simTime() + 1, new cMessage("startAttack"));
}


void TraCIReplaymo11p::captureMessage(TraCIDemo11pMessage* wsm) {
    capturedMessages.push_back(wsm->dup()); // 메시지 복사하여 저장
}

void TraCIReplaymo11p::replayCapturedMessages() {
    EV << "Replaying captured messages...\n";
    for (auto& msg : capturedMessages) {
        EV << "Sending message to RSU...\n";
        sendToRSU(msg); // RSU로 재전송
    }
    capturedMessages.clear(); // 재전송 후 캡처된 메시지 목록 초기화
}

void TraCIReplaymo11p::sendToRSU(TraCIDemo11pMessage* msg) {
    cModule* rsu = getSimulation()->getModuleByPath("RSUExampleScenario.rsu[0]"); // 경로를 조정하세요
    if (rsu) {
        msg->setSenderAddress(myId); // 메시지 송신자 주소 설정
        EV << "Sending message: " << msg->getDemoData() << endl;
        sendDirect(msg, rsu, "veinsradioIn"); // RSU에 메시지 전송
        EV << "Node " << getParentModule()->getIndex() << " replayed message to RSU: " << msg->getDemoData() << "\n";
        packetsSent++;
    } else {
        EV_ERROR << "RSU not found in the simulation.\n";
    }
}

void TraCIReplaymo11p::handleSelfMsg(cMessage* msg) {
    if (strcmp(msg->getName(), "startAttack") == 0) {
        replayCapturedMessages(); // 캡처한 메시지 재전송
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void TraCIReplaymo11p::handlePositionUpdate(cObject* obj) {
    DemoBaseApplLayer::handlePositionUpdate(obj);
    lastDroveAt = simTime(); // 마지막 주행 시간 업데이트
}

void TraCIReplaymo11p::startAttack() {
    attackFlag = true;
}

void TraCIReplaymo11p::stopAttack() {
    attackFlag = false;
}

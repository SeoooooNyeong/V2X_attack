#include "veins/modules/application/traci/TraCIDoSmo11p.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include <cstring> // for strcmp
#include <sstream> // for stringstream
#include <thread>  // for std::thread
#include <chrono>  // for std::chrono

using namespace veins;

Define_Module(veins::TraCIDoSmo11p);

void TraCIDoSmo11p::initialize(int stage) {
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
        scheduleAt(simTime() + 100, new cMessage("startAttack"));
    }
}

void TraCIDoSmo11p::finish() {
    simulationEndTime = simTime().dbl();

    // Calculate and record metrics
    double pdr = packetsReceived / static_cast<double>(packetsSent);
    double avgEndToEndDelay = (packetsReceived > 0) ? totalDelay / packetsReceived : 0.0;
    double throughput = totalBytesReceived / (simulationEndTime - simulationStartTime);
    double networkLoad = packetsSent / (simulationEndTime - simulationStartTime);
    double collisionRate = collisions / (simulationEndTime - simulationStartTime);

    recordScalar("Packet Delivery Ratio (PDR)", pdr);
    recordScalar("Average End-to-End Delay", avgEndToEndDelay);
    recordScalar("Throughput", throughput);
    recordScalar("Network Load", networkLoad);
    recordScalar("Collision Rate", collisionRate);
}

TraCIDoSmo11p::~TraCIDoSmo11p() {
    stopAttack();
}

void TraCIDoSmo11p::onWSA(DemoServiceAdvertisment* wsa) {
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void TraCIDoSmo11p::onWSM(BaseFrame1609_4* frame) {
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
    findHost()->getDisplayString().setTagArg("i", 1, "green");

    double delay = simTime().dbl() - wsm->getTimestamp().dbl();
    totalDelay += delay;
    packetsReceived++;
    totalBytesReceived += wsm->getByteLength();

    if (strcmp(wsm->getDemoData(), "startAttack") == 0) {
        startAttack();
    }

    if (mobility->getRoadId()[0] != ':') {
        traciVehicle->changeRoute(wsm->getDemoData(), 9999);
    }

    if (!sentMessage) {
        sentMessage = true;
        // Schedule a repeat of the received traffic update
        wsm->setSenderAddress(myId);
        wsm->setSerial(3);
        scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
    }
}

void TraCIDoSmo11p::handleSelfMsg(cMessage* msg) {
    if (strcmp(msg->getName(), "startAttack") == 0) {
        sendToRSU();
        scheduleAt(simTime() + 0.01, new cMessage("startAttack")); // Reschedule to keep sending every 0.01 seconds
    }
    else if (strcmp(msg->getName(), "Normal") == 0){
        sendToRSU();
        scheduleAt(simTime() + 0.01, new cMessage("Normal"));
    }
    else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void TraCIDoSmo11p::sendToRSU() {
    cModule* rsu = getSimulation()->getModuleByPath("RSUExampleScenario.rsu[0]"); // Adjust the path if needed
    if (rsu) {
        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        wsm->setTimestamp(simTime());

        // Using stringstream for message content
        std::ostringstream messageStream;
        messageStream << "Attack Message from Node " << getParentModule()->getIndex();
        wsm->setDemoData(messageStream.str().c_str());

        // Send the message to RSU
        sendDirect(wsm, rsu, "veinsradioIn");
        EV << "Node " << getParentModule()->getIndex() << " sent attack message to RSU\n";

        packetsSent++;
    } else {
        EV_ERROR << "RSU not found in the simulation.\n";
    }
}

void TraCIDoSmo11p::handlePositionUpdate(cObject* obj) {
    DemoBaseApplLayer::handlePositionUpdate(obj);

    if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10 && !sentMessage) {
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
            wsm->setDemoData("Traffic Information Service");
            scheduleAt(simTime() + 1, wsm);
        }
    } else {
        lastDroveAt = simTime();
    }
}

void TraCIDoSmo11p::startAttack() {
    attackFlag = true;
    // Starting a thread to send messages to the RSU
    attackThread = std::thread([this]() {
        while (attackFlag && simTime() < 200) {
            // Create a message for RSU
            cMessage* msg = new cMessage("sendRSUMessage");
            scheduleAt(simTime() + 0.01, msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Sleep to prevent busy-waiting
        }
    });
}

void TraCIDoSmo11p::stopAttack() {
    attackFlag = false;
    if (attackThread.joinable()) {
        attackThread.join();
    }
}

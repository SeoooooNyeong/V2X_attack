//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 


#include "veins/modules/application/traci/traCIDoSmt11p.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include <cstring> // for strcmp
#include <sstream> // Include this for stringstream

using namespace veins;

Define_Module(veins::traCIDoSmt11p);

void traCIDoSmt11p::initialize(int stage) {
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

        // Schedule the first broadcast message to be sent by node 5
        if (getParentModule()->getIndex() == 5) {
            scheduleAt(simTime() + 1, new cMessage("sendBroadcastMessage"));
        }
    }
}

void traCIDoSmt11p::finish() {
    simulationEndTime = simTime().dbl();

    // Calculate metrics
    double pdr = packetsReceived / static_cast<double>(packetsSent);
    double avgEndToEndDelay = totalDelay / packetsReceived;
    double throughput = totalBytesReceived / (simulationEndTime - simulationStartTime);
    double networkLoad = packetsSent / (simulationEndTime - simulationStartTime);
    double collisionRate = collisions / (simulationEndTime - simulationStartTime);

    // Record metrics using OMNeT++ statistics
    recordScalar("Packet Delivery Ratio (PDR)", pdr);
    recordScalar("Average End-to-End Delay", avgEndToEndDelay);
    recordScalar("Throughput", throughput);
    recordScalar("Network Load", networkLoad);
    recordScalar("Collision Rate", collisionRate);

    stopAttack(); // Ensure the attack thread is stopped
}

traCIDoSmt11p::~traCIDoSmt11p() {
    stopAttack();
}

void traCIDoSmt11p::onWSA(DemoServiceAdvertisment* wsa) {
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void traCIDoSmt11p::onWSM(BaseFrame1609_4* frame) {
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
    findHost()->getDisplayString().setTagArg("i", 1, "green");

    EV << "Node " << getParentModule()->getIndex() << " received message: " << wsm->getDemoData() << " from node: " << wsm->getSenderAddress() << "\n";

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
        wsm->setSenderAddress(myId);
        wsm->setSerial(3);
        scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
    }
}


void traCIDoSmt11p::handleSelfMsg(cMessage* msg) {
    if (strcmp(msg->getName(), "sendBroadcastMessage") == 0) {
        if (getParentModule()->getIndex() == 5) {
            cModule* parentModule = getParentModule()->getParentModule();
            int numNodes = 0;

            while (true) {
                cModule* node = parentModule->getSubmodule("node", numNodes);
                if (node == nullptr) {
                    break;
                }
                numNodes++;
            }

            for (int i = 0; i < numNodes; ++i) {
                if (i != getParentModule()->getIndex()) {
                    cModule* node = parentModule->getSubmodule("node", i);
                    if (node) {
                        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
                        wsm->setDemoData("startAttack");
                        wsm->setTimestamp(simTime());
                        wsm->setSenderAddress(getParentModule()->getIndex());

                        sendDirect(wsm, node, "veinsradioIn");
                        EV << "Broadcast message sent from node " << getParentModule()->getIndex() << " to node " << node->getIndex() << "\n";
                        packetsSent++;
                    }
                }
            }
        } else {
            delete msg;
        }
    } else if (strcmp(msg->getName(), "sendRSUMessage") == 0) {
        sendToRSU();
        delete msg;
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}
void traCIDoSmt11p::sendToRSU() {
    cModule* rsu = getSimulation()->getModuleByPath("RSUExampleScenario.rsu[0]");
    if (rsu) {
        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        wsm->setTimestamp(simTime());

        std::ostringstream messageStream;
        messageStream << "Attack Message from Node " << getParentModule()->getIndex();
        wsm->setDemoData(messageStream.str().c_str());

        sendDirect(wsm->dup(), rsu, "veinsradioIn");
        EV << "Node " << getParentModule()->getIndex() << " sent attack message to RSU\n";

        packetsSent++;
        delete wsm;
    } else {
        EV_ERROR << "RSU not found in the simulation.\n";
    }
}


void traCIDoSmt11p::handlePositionUpdate(cObject* obj) {
    DemoBaseApplLayer::handlePositionUpdate(obj);

    if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false) {
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

void traCIDoSmt11p::startAttack() {
    attackFlag = true;
    attackThread = std::thread([this]() {
        while (attackFlag && simTime() < 200) {
            cMessage* msg = new cMessage("sendRSUMessage");
            scheduleAt(simTime() + 0.01, msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}

void traCIDoSmt11p::stopAttack() {
    attackFlag = false;
    if (attackThread.joinable()) {
        attackThread.join();
    }
}

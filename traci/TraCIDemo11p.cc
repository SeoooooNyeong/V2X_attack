//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <veins/modules/application/traci/TraCIDemo11p.h>
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Define_Module(veins::TraCIDemo11p);

void TraCIDemo11p::initialize(int stage)
{
   DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
    }
/*
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;

        // Schedule the first message to be sent by node 5
        if (getParentModule()->getIndex() == 5) {
            scheduleAt(simTime() + 1, new cMessage("sendRSUMessage"));
        }
    } */
}

void TraCIDemo11p::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    findHost()->getDisplayString().setTagArg("i", 1, "green");

    if (mobility->getRoadId()[0] != ':') traciVehicle->changeRoute(wsm->getDemoData(), 9999);
    if (!sentMessage) {
        sentMessage = true;
        // repeat the received traffic update once in 2 seconds plus some random delay
        wsm->setSenderAddress(myId);
        wsm->setSerial(3);
        scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
    }
}

void TraCIDemo11p::sendToRSU() {
    cModule* rsu = getSimulation()->getModuleByPath("RSUExampleScenario.rsu[0]"); // Adjust the path if needed
    if (rsu) {
        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        wsm->setTimestamp(simTime());

        // Using stringstream for message content
        std::ostringstream messageStream;
        wsm->setDemoData(messageStream.str().c_str());

        // Send the message to RSU
        sendDirect(wsm, rsu, "veinsradioIn");

    } else {
        EV_ERROR << "RSU not found in the simulation.\n";
    }
}

void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{
   if (TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(msg)) {
        // send this message on the service channel until the counter is 3 or higher.
        // this code only runs when channel switching is enabled
        sendDown(wsm->dup());
        wsm->setSerial(wsm->getSerial() + 1);
        sendToRSU();
        if (wsm->getSerial() >= 3) {
            // stop service advertisements
            stopService();
            delete (wsm);
        }
        else {
            scheduleAt(simTime() + 1, wsm);
        }
    }
    else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
// another behaviour

 /*   if (TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(msg)) {
        // Check if the node or car number is 5
        if (getParentModule()->getIndex() == 5) {
            // Perform actions specific to node or car number 5
            // send this message on the service channel until the counter is 3 or higher.
            // this code only runs when channel switching is enabled
            sendDown(wsm->dup());
            wsm->setSerial(wsm->getSerial() + 1);
            if (wsm->getSerial() >= 3) {
                // stop service advertisements
                stopService();
                delete (wsm);
            } else {
                scheduleAt(simTime() + 1, wsm);
            }
        } else {
            // Handle other nodes or cars
            // Optionally, include similar logic or other handling for different nodes
            sendDown(wsm->dup());
            wsm->setSerial(wsm->getSerial() + 1);
            if (wsm->getSerial() >= 3) {
                stopService();
                delete (wsm);
            } else {
                scheduleAt(simTime() + 1, wsm);
            }
        }
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    } */
    // Another functions
    /*
    if (TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(msg)) {
        // Check if the node or car number is 5
        if (getParentModule()->getIndex() == 5) {
            // Perform actions specific to node or car number 5
            // Send message to RSU
            cModule* rsu = getSimulation()->getModuleByPath("RSUExampleScenario.rsu[0]"); // Adjust the path if needed
            if (rsu) {
                sendDirect(wsm->dup(), rsu, "veinsradioIn"); // Send the message directly to the RSU application layer
            } else {
                EV_ERROR << "RSU not found in the simulation.\n";
            }
            delete wsm;
        } else {
            // Handle other nodes or cars
            // send this message on the service channel until the counter is 3 or higher.
            // this code only runs when channel switching is enabled
            sendDown(wsm->dup());
            wsm->setSerial(wsm->getSerial() + 1);
            if (wsm->getSerial() >= 3) {
                // stop service advertisements
                stopService();
                delete wsm;
            } else {
                scheduleAt(simTime() + 1, wsm);
            }
        }
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
    */
   // another

   /*
    if (strcmp(msg->getName(), "sendRSUMessage") == 0) {
        // Only node 5 sends the message to RSU
        if (getParentModule()->getIndex() == 5) {
            cModule* rsu = getSimulation()->getModuleByPath("RSUExampleScenario.rsu[0]"); // Adjust the path if needed
            if (rsu) {
                TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
                sendDirect(wsm->dup(), rsu, "veinsradioIn"); // Send the message directly to the RSU application layer
                EV << "Message sent from node 5 to RSU\n";
                delete wsm;
            } else {
                EV_ERROR << "RSU not found in the simulation.\n";
            }

            // Schedule the next message
            if (simTime() < 200) {
                scheduleAt(simTime() + 1, msg);
            } else {
                delete msg;
            }
        } else {
            delete msg;
        }
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
*/
}

void TraCIDemo11p::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    // stopped for for at least 10s?
    if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false) {
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
            populateWSM(wsm);
            wsm->setDemoData(mobility->getRoadId().c_str());

            // host is standing still due to crash
            if (dataOnSch) {
                startService(Channel::sch2, 42, "Traffic Information Service");
                // started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
            }
            else {
                // send right away on CCH, because channel switching is disabled
                sendDown(wsm);
            }
        }
    }
    else {
        lastDroveAt = simTime();
    }
}

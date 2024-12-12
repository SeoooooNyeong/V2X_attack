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

#include <veins/modules/application/traci/DoSAttacker.h>
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Define_Module(veins::DoSAttacker);

void DoSAttacker::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
        sendInterval = SimTime(0.1);
      //  targetAddress = LAddress::L2Type(par("targetNodeAddress").longValue());
        targetAddress = LAddress::L2Type(par("targetNodeAddress").intValue());
        // Schedule the first DDoS message
        scheduleAt(simTime() + sendInterval, new cMessage("sendDDoSMessage"));
    }
}

void DoSAttacker::onWSA(DemoServiceAdvertisment* wsa)
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

void DoSAttacker::handleSelfMsg(cMessage* msg)
{
    if (strcmp(msg->getName(), "sendDDoSMessage") == 0) {
        sendDoSMessage();
        scheduleAt(simTime() + sendInterval, msg); // Reschedule message
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void DoSAttacker::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);
    wsm->setDemoData("DoS attack message");
    wsm->setRecipientAddress(targetAddress);
    sendDown(wsm);
}

void DoSAttacker::handlePositionUpdate(cObject* obj)
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

void DoSAttacker::sendDoSMessage()
{
    TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
    populateWSM(wsm);
    wsm->setDemoData("DoS attack message");
    wsm->setRecipientAddress(targetAddress);
    sendDown(wsm);
}

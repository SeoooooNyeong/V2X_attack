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

#include "DDoSAttacker.h"

#include <omnetpp.h>

using namespace omnetpp;

class DDoSAttacker : public cSimpleModule
{
  private:
    cMessage *sendMessageEvent;
    int attackInterval;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void sendAttackMessage();

  public:
    DDoSAttacker();
    virtual ~DDoSAttacker();
};

Define_Module(DDoSAttacker);

DDoSAttacker::DDoSAttacker()
{
    sendMessageEvent = nullptr;
}

DDoSAttacker::~DDoSAttacker()
{
    cancelAndDelete(sendMessageEvent);
}

void DDoSAttacker::initialize()
{
    attackInterval = par("attackInterval");
    sendMessageEvent = new cMessage("sendAttackMessage");
    scheduleAt(simTime() + attackInterval, sendMessageEvent);
}

void DDoSAttacker::handleMessage(cMessage *msg)
{
    if (msg == sendMessageEvent)
    {
        sendAttackMessage();
        scheduleAt(simTime() + attackInterval, sendMessageEvent);
    }
    else
    {
        delete msg; // Discard any incoming messages
    }
}

void DDoSAttacker::sendAttackMessage()
{
    for (int i = 0; i < gateSize("out"); i++) {
        cMessage *attackMsg = new cMessage("DDoSAttack");
        send(attackMsg, "out", i);
    }
}

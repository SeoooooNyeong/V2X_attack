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

#include "MaliciousCar.h"
Define_Module(MaliciousCar);

void MaliciousCar::initialize()
{
    attackInterval = par("attackInterval");
    scheduleAt(simTime() + attackInterval, new cMessage("attackTimer"));
}

void MaliciousCar::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        sendFloodMessage();
        scheduleAt(simTime() + attackInterval, msg);
    } else {
        // Regular message handling
    }
}

void MaliciousCar::sendFloodMessage()
{
    cMessage *floodMsg = new cMessage("floodMsg");
    floodMsg->setTimestamp();
    send(floodMsg, "radioIn");
}

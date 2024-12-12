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

#include "RSUMessage.h"

Define_Module(RSUMessage);

void RSUMessage::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        messageInterval = par("messageInterval").doubleValue();
        messageContent = par("messageContent").stdstringValue();
        scheduleMessage = new cMessage("sendMessage");
        scheduleAt(simTime() + messageInterval, scheduleMessage);
    }
}

void RSUMessage::handleSelfMsg(cMessage *msg) {
    if (msg == scheduleMessage) {
        sendMessageToVehicles();
        scheduleAt(simTime() + messageInterval, scheduleMessage);
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void RSUMessage::sendMessageToVehicles() {
    WaveShortMessage* wsm = new WaveShortMessage();
    wsm->setWsmData(messageContent.c_str());
    wsm->setSenderAddress(myId);
    wsm->setRecipientAddress(LAddress::L2BROADCAST);
    wsm->setSerial(0);

    // Broadcast the message
    sendDown(wsm);
}

void RSUMessage::handleLowerMsg(cMessage *msg) {
    // Handle messages received from vehicles (if needed)
    delete msg;
}

void RSUMessage::finish() {
    // Clean up resources
    cancelAndDelete(scheduleMessage);
}

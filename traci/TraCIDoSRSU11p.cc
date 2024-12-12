//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
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

#include "veins/modules/application/traci/TraCIDoSRSU11p.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Define_Module(veins::TraCIDoSRSU11p);

TraCIDoSRSU11p::TraCIDoSRSU11p()
{
    lastWSMTime = simTime();
    wsmCount = 0;
    receivedWSMs = 0;
    droppedWSMs = 0;
}

void TraCIDoSRSU11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        rateLimitInterval = 1.0;
        maxWSMsPerInterval = 10;
    }
}

void TraCIDoSRSU11p::finish()
{
    // Record statistics
    recordScalar("Received WSMs", receivedWSMs);
    recordScalar("Dropped WSMs", droppedWSMs);
}

void TraCIDoSRSU11p::onWSA(DemoServiceAdvertisment* wsa)
{
    if (!validateWSA(wsa)) {
        EV_ERROR << "Invalid WSA received. Discarding." << endl;
        return;
    }

    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void TraCIDoSRSU11p::onWSM(BaseFrame1609_4* frame)
{
    if (simTime() - lastWSMTime > rateLimitInterval) {
        wsmCount = 0;
        lastWSMTime = simTime();
    }
    if (wsmCount >= maxWSMsPerInterval) {
        EV_WARN << "Rate limit exceeded for WSMs. Discarding message." << endl;
        droppedWSMs++;
        return;
    }

    TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(frame);
    if (!wsm) {
        EV_ERROR << "Unknown message received: " << frame->getClassName() << ". Discarding." << endl;
        droppedWSMs++;
        return;
    }

    if (!validateWSM(wsm)) {
        EV_ERROR << "Invalid WSM received. Discarding." << endl;
        droppedWSMs++;
        return;
    }

    receivedWSMs++;
    wsmCount++;
    sendDelayedDown(wsm->dup(), 2 + uniform(0.01, 0.2));
}

bool TraCIDoSRSU11p::validateWSA(DemoServiceAdvertisment* wsa)
{
    if (wsa->getPsid() <= 0) return false;
    if (wsa->getTargetChannel() < 0 || wsa->getTargetChannel() > 7) return false;
    return true;
}

bool TraCIDoSRSU11p::validateWSM(TraCIDemo11pMessage* wsm)
{
    if (wsm->getSenderAddress() == -1) return false;
    if (wsm->getBitLength() <= 0) return false;
    return true;
}

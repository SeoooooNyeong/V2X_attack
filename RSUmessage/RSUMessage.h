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


#ifndef RSUMESSAGEAPP_H
#define RSUMESSAGEAPP_H

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/messages/DemoSafetyMessage_m.h"

class RSUMessageApp : public DemoBaseApplLayer {
  protected:
    simtime_t messageInterval;
    std::string messageContent;
    cMessage *scheduleMessage;

    virtual void initialize(int stage) override;
    virtual void handleSelfMsg(cMessage *msg) override;
    virtual void handleLowerMsg(cMessage *msg) override;
    virtual void sendMessageToVehicles();
    virtual void finish() override;
};

#endif // RSUMESSAGEAPP_H

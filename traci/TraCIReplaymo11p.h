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

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include <thread>
#include <atomic>

namespace veins {

class VEINS_API TraCIReplaymo11p : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;
    ~TraCIReplaymo11p();

protected:
    int packetsSent;
    int packetsReceived;
    double totalDelay;
    double totalBytesReceived;
    double simulationStartTime;
    double simulationEndTime;
    int collisions;

    std::vector<TraCIDemo11pMessage*> capturedMessages; // 罹≪쿂�맂 硫붿떆吏�瑜� ���옣�븷 踰≫꽣

    bool sentMessage;
    simtime_t lastDroveAt;
    int currentSubscribedServiceId;

    std::thread attackThread;
    std::atomic<bool> attackFlag;

    void onWSA(DemoServiceAdvertisment* wsa) override;
    void onWSM(BaseFrame1609_4* frame) override;
    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;

    void recordCollision();
    
    void captureMessage(TraCIDemo11pMessage* msg); // Function to capture messages
    void replayCapturedMessages(); // Function to replay captured messages
    void sendToRSU(TraCIDemo11pMessage* msg);

    void startAttack(); // Function to start the attack
    void stopAttack();  // Function to stop the attack
};

} // namespace veins

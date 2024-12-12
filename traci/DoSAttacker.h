#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace veins {

class VEINS_API DoSAttacker : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;

protected:
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;
    simtime_t sendInterval;
    LAddress::L2Type targetAddress;

protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    virtual void sendDoSMessage();
    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
};

} // namespace veins

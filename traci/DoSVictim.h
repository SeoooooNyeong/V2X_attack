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
#include "DoSReportGenerator.h"
#include <unordered_map>
namespace veins {

class VEINS_API DoSVictim : public DemoBaseApplLayer {


protected:
    virtual void initialize(int stage) override;
    virtual void handleSelfMsg(cMessage *msg) override;
    virtual void handleLowerMsg(cMessage *msg) override;
    virtual void handlePositionUpdate(cObject *obj) override;

private:
    void applyRateLimiting(int senderId);
    void applyAnomalyDetection(int senderId);

    simtime_t rateLimitInterval;
    int messageLimit;
    simtime_t anomalyDetectionInterval;
    int anomalyThreshold;
    std::unordered_map<int, int> messageCount;
    std::unordered_map<int, int> messageCountAnomaly;
    DoSReportGenerator* reportGenerator;
};
}
/* DDOSVICTIM_H_ */

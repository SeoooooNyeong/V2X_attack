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
#include <omnetpp.h>
#include <unordered_map>
#include <fstream>

namespace veins {

class VEINS_API DoSReportGenerator : public cSimpleModule {

//protected:
public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    void logData();
    void generateReport();

    std::ofstream reportFile;
    std::unordered_map<int, int> messageStats;
    int totalMessagesSentByAttacker;
    int totalMessagesReceivedByVictim;
    int rateLimitExceededCount;
    int anomaliesDetected;
    int messagesDroppedDueToMitigation;
    double totalLatency;
    int totalMessagesProcessed;
};
}

/* SRC_VEINS_MODULES_APPLICATION_TRACI_DOSREPORTGENERATOR_H_ */

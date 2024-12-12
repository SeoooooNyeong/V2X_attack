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

#include <veins/modules/application/traci/DoSReportGenerator.h>
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Define_Module(veins::DoSReportGenerator);

void DoSReportGenerator::initialize() {
    reportFile.open("results/DDoSReport.txt", std::ios::out);
    if (!reportFile.is_open()) {
        EV << "Failed to open report file!" << endl;
    }

    totalMessagesSentByAttacker = 0;
    totalMessagesReceivedByVictim = 0;
    rateLimitExceededCount = 0;
    anomaliesDetected = 0;
    messagesDroppedDueToMitigation = 0;
    totalLatency = 0.0;
    totalMessagesProcessed = 0;
}

void DoSReportGenerator::handleMessage(cMessage *msg) {
    // No handling needed as this module just logs data
}

void DoSReportGenerator::finish() {
    generateReport();
    reportFile.close();
}

void DoSReportGenerator::logData() {
    // Logging data periodically if needed
}

void DoSReportGenerator::generateReport() {
    reportFile << "Total Messages Sent by Attacker: " << totalMessagesSentByAttacker << "\n";
    reportFile << "Total Messages Received by Victim: " << totalMessagesReceivedByVictim << "\n";
    reportFile << "Rate Limit Exceeded Count: " << rateLimitExceededCount << "\n";
    reportFile << "Anomalies Detected: " << anomaliesDetected << "\n";
    reportFile << "Messages Dropped Due to Mitigation: " << messagesDroppedDueToMitigation << "\n";
    reportFile << "Total Latency: " << totalLatency << "\n";
    reportFile << "Total Messages Processed: " << totalMessagesProcessed << "\n";
}

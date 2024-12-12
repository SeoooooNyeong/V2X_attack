#include <veins/modules/application/traci/DoSVictim.h>
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Define_Module(veins::DoSVictim);

void DoSVictim::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
        rateLimitInterval = SimTime(1); // 1 second interval
        messageLimit = 10; // Max 10 messages per second from a single node

        anomalyDetectionInterval = SimTime(5); // 5 second interval
        anomalyThreshold = 50; // Threshold for detecting anomalies

        scheduleAt(simTime() + rateLimitInterval, new cMessage("rateLimitCheck"));
        scheduleAt(simTime() + anomalyDetectionInterval, new cMessage("anomalyDetectionCheck"));
        reportGenerator = check_and_cast<DoSReportGenerator*>(getParentModule()->getSubmodule("reportGenerator"));
    }
}

void DoSVictim::handleLowerMsg(cMessage *msg) {
    int senderId = msg->getSenderModuleId();

    applyRateLimiting(senderId);
    applyAnomalyDetection(senderId);

    reportGenerator->totalMessagesReceivedByVictim++;
    reportGenerator->totalLatency += (simTime() - msg->getCreationTime()).dbl(); // Convert SimTime to double
    reportGenerator->totalMessagesProcessed++;

    EV << "Received message: " << msg->getName() << endl;
    delete msg;
}

void DoSVictim::applyRateLimiting(int senderId) {
    messageCount[senderId]++;
    if (messageCount[senderId] > messageLimit) {
        EV << "Rate limit exceeded for node: " << senderId << endl;
        reportGenerator->rateLimitExceededCount++;
        reportGenerator->messagesDroppedDueToMitigation++;
    }
}

void DoSVictim::applyAnomalyDetection(int senderId) {
    messageCountAnomaly[senderId]++;
    if (messageCountAnomaly[senderId] > anomalyThreshold) {
        EV << "Anomaly detected from node: " << senderId << endl;
        reportGenerator->anomaliesDetected++;
        reportGenerator->messagesDroppedDueToMitigation++;
    }
}

void DoSVictim::handleSelfMsg(cMessage *msg) {
    if (strcmp(msg->getName(), "rateLimitCheck") == 0) {
        messageCount.clear();
        scheduleAt(simTime() + rateLimitInterval, msg);
    } else if (strcmp(msg->getName(), "anomalyDetectionCheck") == 0) {
        messageCountAnomaly.clear();
        scheduleAt(simTime() + anomalyDetectionInterval, msg);
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void DoSVictim::handlePositionUpdate(cObject *obj) {
    DemoBaseApplLayer::handlePositionUpdate(obj);
}

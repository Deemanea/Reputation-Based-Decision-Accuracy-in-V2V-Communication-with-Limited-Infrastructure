#ifndef VEINS_MODULES_APPLICATION_REPUTATIONCOMMUNICATION_REPUTATIONCOMMUNICATIONAPP_H_
#define VEINS_MODULES_APPLICATION_REPUTATIONCOMMUNICATION_REPUTATIONCOMMUNICATIONAPP_H_

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/ReputationCommunication/ReputationCommunicationAppMessage_m.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"


#include <string>
#include <vector>

namespace veins {

struct NodeState {
    int NodeId;              // Vehicle's ID
    double NodeScore;        // Reputation score
    bool Attacker;           // Flag indicating if the vehicle is an attacker
    bool validCertificate;   // Whether the certificate is valid
    double likelihoodMalicious; // Likelihood of being malicious
    double likelihoodHonest; // Likelihood of being honest
};



class VEINS_API ReputationCommunicationApp : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;

protected:
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;
    int MNodeId;
    bool HasAccident;
    bool RouteChangeRequested;
    double MScore;
    double Threshold_Score;
    double AttackStart;
    double AttackDuration;
    bool isHonestVehicle(int nodeId);  // Function to check if a vehicle is honest
       void forwardMessage(ReputationCommunicationAppMessage* wsm); // Function to forward messages
       void changeRoute(std::string accidentRoute); // Function to change the vehicle's route
    cMessage* AttackTimer;
    std::list<NodeState> MyNodeList;
    std::list<NodeState> MyScoreList;


    // Declare Attacker variable
    bool Attacker;

    // Declare accident position
    Coord accidentPosition;

    // Mobility components
    veins::TraCIMobility* mobility;
    veins::TraCICommandInterface* traci;
    veins::TraCICommandInterface::Vehicle* traciVehicle;

    static bool globalHasAccident;
    // Helper functions for signing and verification

    bool verifyCertificate(const std::string& certificate);  // Modified to remove signature parameter

    std::string extractCertData(const std::string& certificate);  // New function declaration
    std::string extractCertSignature(const std::string& certificate);  // New function declaration
    double GetReputationScoreofNode(int nodeId);

    // Statistics
    int messagesSent;
    int messagesReceived;
    int validMessagesReceived;
    int invalidMessagesReceived;
    int truePositiveDecisions; // Updated based on new requirements
    int falsePositiveDecisions; // For false positive tracking
    int invalidReputationMessages;
    int invalidCertificateMessages;
    int forwardedMessages;
    double responseTime; // for measuring total response time
    int correctDecisions; // To calculate decision accuracy

    // New Metrics and Variables Added:

    // Message Propagation Delay (new)
    double messagePropagationDelay;  // To track individual message delays

    // Decision Metrics
    int trueNegativeDecisions;   // True negative messages
    int falseNegativeDecisions;  // False negative messages

    // Calculated metrics (Precision, Recall, F1 Score, etc.)
    double precision;
    double recall;
    double specificity;
    double f1Score;

    // Reputation-specific variables
    double MRV;
    int highRepCount;
    int lowRepCount;

    // Reputation file handling
    std::string ReputationFile;
    void GenerateFile();
    void ReadReputationFile();
    void SetMyReputationState();
    void UpdateReputationKnowledge(int nodeId, double score);
    void logAttackDetails(const std::string& action);
    void updateDisplayString(const std::string& info);

    // Key generation functions
    void generateKeys();
    void generateCACertificate();
    void generateVehicleCertificates(NodeState& node);
    void storeKeysAndCertificates(NodeState& node);

    // Message handling functions
    void sendDENMMessage();
    void simulateSybilAttack();


    double GetMaliciousLikelihoodOfNode(int nodeId);

    // Decay-related parameters
    double decayRate;        // Decay rate for reputation
    double decayInterval;    // Time interval for decay

    // Message handling functions
    void applyReputationDecay();   // Apply reputation decay over time
    bool checkLocalCertificateValidity(int nodeId);

protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
    bool isSybilAttackMessage(int nodeId);

};

} // namespace veins

#endif // VEINS_MODULES_APPLICATION_REPUTATIONCOMMUNICATION_REPUTATIONCOMMUNICATIONAPP_H_

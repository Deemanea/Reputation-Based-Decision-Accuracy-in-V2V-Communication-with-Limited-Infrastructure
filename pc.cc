#include "veins/modules/application/ReputationCommunication/ReputationCommunicationApp.h"
#include "veins/modules/application/ReputationCommunication/ReputationCommunicationAppMessage_m.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"

#include <sstream>
#include <fstream>
#include <string>

using namespace veins;

Define_Module(veins::ReputationCommunicationApp);

void ReputationCommunicationApp::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        sentMessage = false;
        ReputationFile = par("ReputationFile").stringValue();
        MNodeId = findHost()->getIndex();
        Threshold_Score = par("Threshold_Score").doubleValue();
        HasAccident = par("HasAccident").boolValue();
        RouteChangeRequested = false;

        MRV = par("MRV").doubleValue();
        highRepCount = 0;
        lowRepCount = 0;

        messagesSent = 0;
        messagesReceived = 0;
        validMessagesReceived = 0;
        invalidMessagesReceived = 0;
        forwardedMessages = 0;
        responseTime = 0.0;
        correctDecisions = 0;

        truePositiveDecisions = 0;
        falsePositiveDecisions = 0;
        trueNegativeDecisions = 0;
        falseNegativeDecisions = 0;

        precision = 0.0;
        recall = 0.0;
        specificity = 0.0;
        f1Score = 0.0;

        NodeState myNode;
        myNode.NodeId = MNodeId;

        ReadReputationFile();
        SetMyReputationState();

       // if (MNodeId == 1 || MNodeId == 5 || MNodeId == 8) {
            scheduleAt(simTime(), new cMessage("SendDENMMessage"));
      //  }

        AttackStart = par("AttackStart").doubleValue();
        AttackDuration = par("AttackDuration").doubleValue();

        if (Attacker) {
            scheduleAt(simTime() + AttackStart, new cMessage("SybilAttackStart"));
        }

        mobility = TraCIMobilityAccess().get(findHost());
        traci = mobility->getCommandInterface();
        EV << "Initialization completed for Node " << MNodeId << endl;
    }

}

void ReputationCommunicationApp::finish()
{
    recordScalar("MessagesSent", messagesSent);
    recordScalar("MessagesReceived", messagesReceived);
    recordScalar("ValidMessagesReceived", validMessagesReceived);
    recordScalar("InvalidMessagesReceived", invalidMessagesReceived);
   // recordScalar("FinalReputationScore", MScore);
    recordScalar("ForwardedMessages", forwardedMessages);

    if (messagesReceived > 0) {
        double avgPropagationDelay = responseTime / messagesReceived;
        recordScalar("AveragePropagationDelay", avgPropagationDelay);
        EV << "Average Message Propagation Delay: " << avgPropagationDelay << " seconds" << endl;
    }

    double TP = truePositiveDecisions;
    double FP = falsePositiveDecisions;
    double TN = trueNegativeDecisions;
    double FN = falseNegativeDecisions;

    if (TP + FP > 0) precision = TP / (TP + FP);
    if (TP + FN > 0) recall = TP / (TP + FN);
    if (TN + FP > 0) specificity = TN / (TN + FP);
    if (precision + recall > 0) f1Score = 2 * (precision * recall) / (precision + recall);

    recordScalar("TruePositive", TP);
    recordScalar("FalsePositive", FP);
    recordScalar("TrueNegative", TN);
    recordScalar("FalseNegative", FN);

    recordScalar("Precision", precision);
    recordScalar("Recall (Sensitivity)", recall);
    recordScalar("Specificity", specificity);
    recordScalar("F1 Score", f1Score);

    EV << "Precision: " << precision << endl;
    EV << "Recall (Sensitivity): " << recall << endl;
    EV << "Specificity: " << specificity << endl;
    EV << "F1 Score: " << f1Score << endl;

    if (messagesReceived > 0) {
        recordScalar("AverageResponseTime", responseTime / messagesReceived);
        double accuracy = (double)correctDecisions / messagesReceived;
        recordScalar("DecisionAccuracy", accuracy);
    }

    recordScalar("TruePositive", correctDecisions);
    recordScalar("FalseNegative", messagesReceived - correctDecisions);
    recordScalar("HighReputationMessages", highRepCount);
    recordScalar("LowReputationMessages", lowRepCount);

    EV << "Node " << MNodeId << " finished execution." << endl;
}

void ReputationCommunicationApp::SetMyReputationState()
{
    for (const auto& elem : MyScoreList) {
        if (elem.NodeId == MNodeId) {
            Attacker = elem.Attacker;
            MScore = elem.NodeScore;
            EV << "Set reputation state for Node " << MNodeId << ": Attacker=" << Attacker << ", Score=" << MScore << endl;
            return;
        }
    }
}




void ReputationCommunicationApp::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
        EV << "Node " << MNodeId << " subscribed to service " << wsa->getPsid() << " on channel " << wsa->getTargetChannel() << endl;
    }
}

void ReputationCommunicationApp::onWSM(BaseFrame1609_4* frame)
{
    EV << "onWSM called" << endl;
    simtime_t messageReceivedTime = simTime();

    ReputationCommunicationAppMessage* wsm = check_and_cast<ReputationCommunicationAppMessage*>(frame);
    LAddress::L2Type senderAddr = wsm->getSenderAddress();
    int nodeId = senderAddr;
    messagesReceived++;

    if (isSybilAttackMessage(nodeId)) {
        EV << "Message from Node " << nodeId << " is identified as a Sybil attack message." << endl;
        invalidMessagesReceived++;
        return;
    }

    simtime_t propagationDelay = simTime() - wsm->getTimestamp();
        EV << "Message propagation delay: " << propagationDelay << " seconds." << endl;

        recordScalar("AveragePropagationDelay", 1);


        Coord currentPosition = mobility->getCurrentDirection();  // Get the current position of the vehicle using TraCIMobility
            double distance = currentPosition.distance(curPosition);  // Calculate the distance between sender and receiver
            EV << "Distance from sender: " << distance << " meters." << endl;
            recordScalar("DistanceFromSender", distance);  // Record distance from sender


   // double senderScore = GetReputationScoreofNode(nodeId);
    bool isCertificateValid = checkLocalCertificateValidity(nodeId);

   // EV << "Sender " << nodeId << " has a reputation score of: " << senderScore << endl;
      EV << "Sender " << nodeId << " certificate is " << (isCertificateValid ? "valid" : "invalid") << endl;

      if (isCertificateValid) {
          // Check if the sender is honest or malicious
          bool isHonest = isHonestVehicle(nodeId);

          if (isHonest) {
              // True positive: Honest vehicle with correct accident data
              truePositiveDecisions++;
              correctDecisions++;
              EV << "True positive: Accepting message from honest vehicle " << nodeId << endl;
              recordScalar("TruePositive", 1);
              //UpdateReputationKnowledge(nodeId, senderScore);

              // Forward the message
              forwardMessage(wsm);
              forwardedMessages++;


              std::string accidentRoute = wsm->getDemoData();  // Get the actual route from the message
              EV << "Accident information received for route: " << accidentRoute << endl;

              if (mobility->getRoadId() == accidentRoute) {
                  changeRoute(accidentRoute);
                  EV << "Route changed to avoid accident on route: " << accidentRoute << endl;
                  recordScalar("RouteChangeRequested", 1);
              } else {
                  EV << "Current route does not match the accident route: " << accidentRoute << endl;
              }



          } else {
              // False positive: Malicious vehicle with incorrect accident data
              falsePositiveDecisions++;
              EV << "False Positive: Accepting message from malicious vehicle " << nodeId << endl;
              recordScalar("FalsePositive", 1);
              //UpdateReputationKnowledge(nodeId, senderScore);


          }

      } else {
          bool isHonest = isHonestVehicle(nodeId);
            if (!isCertificateValid) {
                if (isHonest) {
                    // False Positive: Message rejected from an honest vehicle
                    falseNegativeDecisions++;
                    EV << " False Negative: Message from honest node " << nodeId << " rejected due to low reputation or invalid certificate." << endl;
                    recordScalar("FalseNegative", 1);
                } else {
                    // True Negative: Message rejected from a malicious vehicle
                    correctDecisions++;
                    trueNegativeDecisions++;
                    EV << "True negative: Message from malicious node " << nodeId << " rejected due to low reputation or invalid certificate." << endl;
                    recordScalar("TrueNegative", 1);
                }
            }

            invalidMessagesReceived++;
        }

            // Measure message processing time
        simtime_t messageValidationTime = simTime() - messageReceivedTime;
        responseTime += messageValidationTime.dbl();
        EV << "Message validation and processing time: " << messageValidationTime << " seconds." << endl;

        EV << "Vehicle " << MNodeId << " received message from Vehicle " << nodeId << " at " << simTime() << endl;
               EV << "Received message data: " << wsm->getDemoData() << endl;

               //EV << "Verifying certificate: " << wsm->getCertificate() << endl;
              // EV << "Verifying Sender's reputation score: " << senderScore << endl;
               EV << "Verifying Sender Address: " << wsm->getSenderAddress() << endl;
               std::string senderLocation = mobility->getRoadId();
               EV << "Sender Location: " << senderLocation << endl;

}

            // Function to forward the message
    void ReputationCommunicationApp::forwardMessage(ReputationCommunicationAppMessage* wsm) {
        if (!sentMessage) {
            EV << "Message from Node " << MNodeId << " is valid. Forwarding the message." << endl;

            sentMessage = true;
            wsm->setSerial(2);
            wsm->setSenderAddress(MNodeId);
            wsm->setTimestamp(simTime());
            scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
            forwardedMessages++;
            validMessagesReceived++;
            EV << "Message forwarded by Node " << MNodeId << endl;
        } else {
                                        EV << "Message forwarding skipped as message already sent by Node " << MNodeId << endl;

        }
    }

    // Function to change route

        void ReputationCommunicationApp::changeRoute(std::string accidentRoute) {
            try {
                std::map<std::string, std::string> routeMap = {
                    {"lane:-28822164#6_0", "detourRoute1"},  // Route #3, with a corresponding detour
                    {"anotherAccidentRoute", "detourRoute2"},  // Another accident route with its detour
                };

                if (routeMap.find(accidentRoute) != routeMap.end()) {
                    std::string detourRoute = routeMap[accidentRoute];  // Get the corresponding detour route
                    traciVehicle->changeRoute(detourRoute.c_str(), 9999);  // Change to the detour route
                    EV << "Route changed to avoid accident on route: " << accidentRoute << ". New route: " << detourRoute << endl;
                    recordScalar("RouteChangeRequested", 1);
                } else {
                    EV << "Accident route " << accidentRoute << " not recognized. No route change applied." << endl;
                }
            } catch (const std::exception& e) {
                EV << "Error changing route: " << e.what() << endl;
            }
        }


    // Function to check if the vehicle is honest based on the loaded reputation and likelihoods
    bool ReputationCommunicationApp::isHonestVehicle(int nodeId) {
        for (const auto& elem : MyScoreList) {
            if (elem.NodeId == nodeId) {
                EV << "Checking honesty for Node " << nodeId << endl;
                if (!elem.status) {  // If status is false, it's honest
                    EV << "Node " << nodeId << " is considered Honest ." << endl;
                    return true;
                } else {
                    EV << "Node " << nodeId << "is considered MALICIOUS ." << endl;
                    return false;
                }
            }
        }

        // Default to considering a vehicle malicious if it's not found
        return false;
    }


bool ReputationCommunicationApp::checkLocalCertificateValidity(int nodeId)
{
    for (const auto& node : MyScoreList) {
        if (node.NodeId == nodeId) {
            EV << "Certificate validity check for Node " << nodeId << ": " << (node.validCertificate ? "Valid" : "Invalid") << endl;
             return node.validCertificate;  // Return certificate validity from the reputation file
         }
     }
     EV << "Certificate not found for Node " << nodeId << ". Assuming invalid." << endl;
     return false;  // Default to invalid if the node is not found
 }



void ReputationCommunicationApp::handleSelfMsg(cMessage* msg)
{
    if (ReputationCommunicationAppMessage* wsm = dynamic_cast<ReputationCommunicationAppMessage*>(msg)) {
        sendDown(wsm->dup());
                        wsm->setSerial(wsm->getSerial() + 1);
                        if (wsm->getSerial() >= 3) {
                            stopService();
                            delete wsm;
                        } else {
                            scheduleAt(simTime() + 1, wsm);
                        }
                        EV << "Self message handled by Node " << MNodeId << endl;
                    } else if (strcmp(msg->getName(), "SybilAttackStart") == 0) {
                        EV << "Starting Sybil Attack at " << simTime() << endl;
                        simulateSybilAttack();
                        scheduleAt(simTime() + AttackDuration, new cMessage("SybilAttackEnd"));
                    } else if (strcmp(msg->getName(), "SybilAttackEnd") == 0) {
                        EV << "Ending Sybil Attack at " << simTime() << endl;
                    } else if (strcmp(msg->getName(), "SendDENMMessage") == 0) {
                        sendDENMMessage();
                        scheduleAt(simTime() + 60, new cMessage("SendDENMMessage"));
                    } else {
                        DemoBaseApplLayer::handleSelfMsg(msg);
                    }
                }

                bool ReputationCommunicationApp::isSybilAttackMessage(int nodeId)
                {
                    if (nodeId >= 100) {
                        EV << "Node " << nodeId << " is part of a Sybil attack." << endl;
                        return true;
                    }
                    return false;
                }

                void ReputationCommunicationApp::sendDENMMessage()
                {
                    EV << "Vehicle " << MNodeId << "  preparing to send DENM message" << endl;

                  ReputationCommunicationAppMessage* wsm = new ReputationCommunicationAppMessage();
                   populateWSM(wsm);
                    //wsm->setDemoData("Accident ahead, change route.");

                   std::string accidentRoute = mobility->getRoadId();
                       if (isHonestVehicle(MNodeId)) {
                           wsm->setDemoData(accidentRoute.c_str());   // Set the actual road ID as the data
                       } else {
                           wsm->setDemoData("No Accident Don't Change the Route!");
                       }

                    wsm->setSenderAddress(MNodeId);
                    wsm->setTimestamp(simTime());

                    double myScore = GetReputationScoreofNode(MNodeId);
                    wsm->setReputationValue(myScore);

                    EV << "DENM message created by Vehicle " << MNodeId << endl;

                    if (dataOnSch) {
                        startService(Channel::sch2, 42, "Traffic Information Service");
                        scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
                    } else {
                        sendDown(wsm);
                    }

                    sentMessage = true;
                    messagesSent++;
                    EV << "DENM message sent from Vehicle " << MNodeId << ": " << wsm->getDemoData() << endl;
                }
                void ReputationCommunicationApp::handlePositionUpdate(cObject* obj)
                {
                    DemoBaseApplLayer::handlePositionUpdate(obj);

                    if (mobility->getSpeed() < 1 && simTime() - lastDroveAt >= 20 && HasAccident) {
                        findHost()->getDisplayString().setTagArg("i", 1, "yellow");
                        sendDENMMessage();
                        EV << "Position update: Accident detected by Node " << MNodeId << endl;
                    } else {
                        lastDroveAt = simTime();
                    }
                }
                void ReputationCommunicationApp::simulateSybilAttack()
                {
                    for (int i = 0; i < 5; ++i) {
                        ReputationCommunicationAppMessage* wsm = new ReputationCommunicationAppMessage();
                        populateWSM(wsm);
                        wsm->setDemoData(" No Accident. The route is clear.");
                        wsm->setSenderAddress(MNodeId + 500 + i);  // Adjusted for Sybil attack
                        wsm->setTimestamp(simTime());

                        if (dataOnSch) {
                            startService(Channel::sch2, 42, "Traffic Information Service");
                            scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
                        } else {
                            sendDown(wsm);
                        }


                        messagesSent++;
                        EV << "Sybil attack message sent from Vehicle " << MNodeId << ": " << wsm->getDemoData() << endl;
                    }
                }



                void ReputationCommunicationApp::ReadReputationFile()
                {
                    std::fstream my_file;
                    std::string line;
                    my_file.open(ReputationFile, std::ios::in);
                    if (!my_file.is_open()) {
                        std::cout << "File not created";
                    } else {
                        while (getline(my_file, line)) {
                            std::stringstream ss(line);
                            int Id;
                            bool stat;
                            double score;
                            bool cert_valid;


                            ss >> Id >> score >> stat >> cert_valid>> stat;
                            NodeState AStat;
                            AStat.NodeId = Id;
                            AStat.NodeScore = score;
                            AStat.Attacker = (stat == 1);  // Attacker for Sybil Attack (unchanged)
                            AStat.validCertificate = cert_valid;
                            AStat.status = (stat == 1);    // New field: true if malicious, false if honest
 // New field: true if malicious, false if honest
       // Higher likelihood of being honest with higher score

                            MyScoreList.push_back(AStat);
                        }
                        my_file.close();
                        EV << "Reputation file read successfully." << endl;
                    }
                }

                void ReputationCommunicationApp::UpdateReputationKnowledge(int nodeId, double score)
                {
                    for (auto& elem : MyScoreList) {
                        if (elem.NodeId == nodeId) {
                            elem.NodeScore = score;
                            EV << "Updated reputation score to " << score << " for Node " << nodeId << endl;
                            return;
                        }
                    }
                    NodeState newNode;
                    newNode.NodeId = nodeId;
                    newNode.NodeScore = score;
                    newNode.Attacker = false;
                    MyScoreList.push_back(newNode);
                    EV << "Added new Node " << nodeId << " with reputation score " << score << endl;
                }

                double ReputationCommunicationApp::GetReputationScoreofNode(int nodeId)
                {
                    for (const auto& elem : MyScoreList) {
                        if (elem.NodeId == nodeId) {
                            EV << "Found reputation score " << elem.NodeScore << " for Node " << nodeId << endl;
                            return elem.NodeScore;
                        }
                    }
                    EV << "Node ID " << nodeId << " not found in reputation list." << endl;
                    return -1.0;
                }


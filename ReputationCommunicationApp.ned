package org.car2x.veins.modules.application.ReputationCommunication;

import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple ReputationCommunicationApp extends DemoBaseApplLayer
{
    parameters:
        @class(veins::ReputationCommunicationApp);
        @display("i=block/app2");
        string ReputationFile = default("test.txt"); // TXT File to store Reputation Info(ID,SCORE)
        double Threshold_Score = default(0.5);
        double AttackStart = default(0s) @unit(s); // When the attack starts
        double AttackDuration = default(0s) @unit(s); // The duration of the attack
        bool HasAccident = default(false); // Tell the application layer if it will have an accident or not
        int maxMessages = default(10); // Maximum number of messages to consider for redundancy score normalization
        double MRV = default(0.7); // Initial Message Reputation Value
}

network ScenarioWithReputationTrustS1
{
    submodules:
        reputationCommunicationApp: ReputationCommunicationApp {
            @display("p=100,100");
        }
}

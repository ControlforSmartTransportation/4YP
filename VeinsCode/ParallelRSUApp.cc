//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <veins/modules/application/traci/ParallelRSUApp.h>
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Define_Module(veins::ParallelRSUApp);

void ParallelRSUApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void ParallelRSUApp::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    // this rsu repeats the received traffic update in 1 second plus some random delay
    //sendDelayedDown(wsm->dup(), 1 + uniform(0.01, 0.2));

    if ( strcmp(wsm->getDemoData(), "a") == 0) { //if incoming road id = road called "a"


        EV << "WSM from road a received" << std::endl;
        counter_a++;


        if (counter_a == 2 && sentMessage == false) { //send message to vehicles on road a saying to slow down
            EV << "Three vehicles detected on road a, time to slow them down" << std::endl;
            findHost()->getDisplayString().setTagArg("i", 1, "red"); //turns icon red in simulation

            sentMessage = true; //should stop multiple messages sending


            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
            populateWSM(wsm);

            const char* && slow_down = "slow down on top road";
            //wsm->setDemoData(findHost()->getClassName());
            wsm->setDemoData(slow_down);
            //EV << "contents of demoData is " << wsm->getDemoData() << std::endl;
            sendDelayedDown(wsm,  uniform(0.001, 0.05));

        }
        else {
                findHost()->getDisplayString().setTagArg("i", 1, "white");
        }
    }
}

void ParallelRSUApp::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind()) {
    case SEND_BEACON_EVT: {
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        populateWSM(bsm);
        sendDown(bsm);
        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);


        //reset counter every time a beacon is sent
        //EV << "counter is being reset" << std::endl;
        counter_a = 0;
        sentMessage = false;
        //EV << "Counter value is " << counter << std::endl;


        break;
    }
    case SEND_WSA_EVT: {
        DemoServiceAdvertisment* wsa = new DemoServiceAdvertisment();
        populateWSM(wsa);
        sendDown(wsa);
        scheduleAt(simTime() + wsaInterval, sendWSAEvt);
        EV << "I shouldn't be seeing this message perhaps maybe?" << std::endl;
        break;
    }
    default: {
        if (msg) EV_WARN << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
        break;
    }
    }
}



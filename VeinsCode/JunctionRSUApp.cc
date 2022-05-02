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

#include <veins/modules/application/traci/JunctionRSUApp.h>
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Define_Module(veins::JunctionRSUApp);

void JunctionRSUApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void JunctionRSUApp::onWSM(BaseFrame1609_4* frame)
{

    //comment everything out and slowly add back in
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);


    if ( strcmp(wsm->getDemoData(), "right_in") == 0) {

        EV << "WSM from right in-road received" << std::endl;
        counter_right++;
        EV << "Right counter is " << counter_right << std::endl;
    }
        /**

    } else if ( strcmp(wsm->getDemoData(), "bottom_in") == 0) { //if incoming road id = different in roads

        EV << "WSM from bottom in-road received" << std::endl;
        counter_bottom++;
        EV << "Bottom counter is " << counter_bottom << std::endl;

    } else if ( strcmp(wsm->getDemoData(), "left_in") == 0) {

        EV << "WSM from left in-road received" << std::endl;
        counter_left++;
        EV << "Left counter is " << counter_left << std::endl;

    }

    */
        /**

        if (counter_a == 3 && sentMessage == false) { //send message to vehicles on road a saying to slow down
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
    */
}

void JunctionRSUApp::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind()) {
    case SEND_BEACON_EVT: {
        DemoSafetyMessage* bsm = new DemoSafetyMessage();
        populateWSM(bsm);
        sendDown(bsm);
        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);


        //reset counter every time a beacon is sent
        EV << "counters are being reset" << std::endl;
        counter_left = 0;
        counter_bottom = 0;
        counter_right = 0;
        //sentMessage = false;
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



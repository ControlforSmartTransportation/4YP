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

#include <veins/modules/application/traci/SmartJunctionRSUApp_stop_modified.h>
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include <algorithm>
#include <initializer_list>
using namespace veins;

Define_Module(veins::SmartJunctionRSUApp_stop_modified);

void SmartJunctionRSUApp_stop_modified::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void SmartJunctionRSUApp_stop_modified::onWSM(BaseFrame1609_4* frame)
{

    //comment everything out and slowly add back in
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);


    if ( strcmp(wsm->getDemoData(), "left_in") == 0) {

        //EV << "WSM from left in-road received" << std::endl;
        counter_left++;
        EV << "Left counter is " << counter_left << std::endl;


    } else if ( strcmp(wsm->getDemoData(), "bottom_in") == 0) { //if incoming road id = different in roads

        //EV << "WSM from bottom in-road received" << std::endl;
        counter_bottom++;
        EV << "Bottom counter is " << counter_bottom << std::endl;

    } else if ( strcmp(wsm->getDemoData(), "right_in") == 0) {

        //EV << "WSM from right in-road received" << std::endl;
        counter_right++;
        EV << "Right counter is " << counter_right << std::endl;

   }
    else if ( strcmp(wsm->getDemoData(), "top_in") == 0) {

            //EV << "WSM from right in-road received" << std::endl;
            counter_top++;
            EV << "Top counter is " << counter_top << std::endl;

       }

    counter_max = std::max({counter_left,counter_bottom});
    counter_min = std::min({counter_left,counter_bottom});
    EV << "Max counter is " << counter_max << std::endl;
    EV << "Min counter is " << counter_min << std::endl;

    if (counter_max == 5 && sentMessage == false) { //send message to vehicles on most free road saying to slow down

        sentMessage = true; //should stop multiple messages sending
        EV << "Congestion in junction" << std::endl;
        findHost()->getDisplayString().setTagArg("i", 1, "red"); //turns icon red in simulation

        if (counter_bottom == counter_max) {
            message = "all roads apart from bottom road stop";

        } else if (counter_left == counter_max){
            message = "all roads apart from left road stop";

       // } else if (counter_right == counter_max) {
       //     message = "all roads apart from right road stop";

      //  } else if (counter_top == counter_max) {
     //       message = "all roads apart from top road stop";
        }


        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        populateWSM(wsm);
        wsm->setDemoData(message);
        EV << "contents of RSU command is " << wsm->getDemoData() << std::endl;
        sendDelayedDown(wsm,  uniform(0.001, 0.05));

    }
    else {
            findHost()->getDisplayString().setTagArg("i", 1, "white");
    }


}

void SmartJunctionRSUApp_stop_modified::handleSelfMsg(cMessage* msg)
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
        counter_top = 0;
        counter_max = 0;
        counter_min = 0;
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



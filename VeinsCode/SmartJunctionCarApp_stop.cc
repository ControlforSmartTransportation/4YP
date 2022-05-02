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

#include <veins/modules/application/traci/SmartJunctionCarApp_stop.h>
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include <iostream>
#include <typeinfo>
#include <stdlib.h>
using namespace veins;
using namespace std;
Define_Module(veins::SmartJunctionCarApp_stop);




void SmartJunctionCarApp_stop::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        //EV << "Initializing " << par("appName").stringValue() << std::endl;

        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        randomn = rand() %10;
        respond = true;
        EV << "car number is " << randomn << std::endl;

        //change this bit to change percentage of autonomy in simulation
        if (true == false) {
            EV << "vehicle will not respond to messages "  << std::endl;
            respond = false;
        } else {
            respond = true;
            EV << "vehicle will respond to messages " << std::endl;
        }


    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void SmartJunctionCarApp_stop::finish()
{
    DemoBaseApplLayer::finish();
    // statistics recording goes here
}

void SmartJunctionCarApp_stop::onBSM(DemoSafetyMessage* bsm)
{


    traciVehicle->setSpeed(-1);
    // Your application has received a beacon message from another car or RSU
    //EV<< "Road ID is called " << mobility->getRoadId().c_str() <<  std::endl;    //print out road ID of each vehicle as they receive the BSM
    if (mobility->getRoadId() == "left_in" || mobility->getRoadId() == "bottom_in" || mobility->getRoadId() == "right_in" || mobility->getRoadId() == "top_in") {

        findHost()->getDisplayString().setTagArg("i", 1, "red");
        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        populateWSM(wsm);
        wsm->setDemoData(mobility->getRoadId().c_str());
        EV<< "Sending WSM to RSU now" <<  std::endl;
        sendDelayedDown(wsm,  uniform(0.001, 0.05));

    } else {
        findHost()->getDisplayString().setTagArg("i", 1, "white");
        lastDroveAt = simTime();
    }


    //want to make vehicles slow down on one road if big queue on other road for 30s
    /**
    if ((mobility->getRoadId() == "bottom_in_main" || mobility->getRoadId() == "bottom_in" )){
        traciVehicle->slowDown(0, 30);
        EV << "I am not speed"  << std::endl;
        findHost()->getDisplayString().setTagArg("i", 1, "red");
        slowCheck_bottom = true;
    }
    else {
        findHost()->getDisplayString().setTagArg("i", 1, "white");
    }
    */
}

void SmartJunctionCarApp_stop::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    if ((stopCheck == false) && (respond == true)) {
        if (strcmp(wsm->getDemoData(),"all roads apart from bottom road stop") == 0 && mobility->getRoadId() == "left_in" ) {
            //EV << "I am bottom road so will slow "  << std::endl;
            // traciVehicle->setMaxSpeed(traciVehicle->getMaxSpeed()/2);
            traciVehicle->setSpeed(0);
            findHost()->getDisplayString().setTagArg("i", 1, "blue");
            stopCheck = true;

        } else if (strcmp(wsm->getDemoData(),"all roads apart from left road stop") == 0 && mobility->getRoadId() == "bottom_in" ) {
            //EV << "I am left road so will slow"  << std::endl;
           // traciVehicle->setMaxSpeed(traciVehicle->getMaxSpeed()/2);
            traciVehicle->setSpeed(0);
            findHost()->getDisplayString().setTagArg("i", 1, "blue");
            stopCheck = true;

        } else if (strcmp(wsm->getDemoData(),"all roads apart from right road stop") == 0 && mobility->getRoadId() == "right_in" ) {
            //EV << "I am right road so will slow"  << std::endl;
           // traciVehicle->setMaxSpeed(traciVehicle->getMaxSpeed()/2);
            traciVehicle->setSpeed(0);
            findHost()->getDisplayString().setTagArg("i", 1, "blue");
            stopCheck = true;
        } else if (strcmp(wsm->getDemoData(),"all roads apart from top road stop") == 0 && mobility->getRoadId() == "top_in" ) {
           // EV << "I am top road so will slow"  << std::endl;
           // traciVehicle->setMaxSpeed(traciVehicle->getMaxSpeed()/2);
            traciVehicle->setSpeed(0);
            findHost()->getDisplayString().setTagArg("i", 1, "blue");
           // stopCheck = true;
        }
    } else {
    //    traciVehicle->setSpeed(-1);
        //stopCheck = false;
    }



//if vehicle is on road ID 'a', slow it down by 20%
//if (mobility->getRoadId()[0] == 'a') {

//what happens if remove the square brackets
//if (mobility->getRoadId() == "a") {
 //   traciVehicle->setSpeed(4);
//traciVehicle->setSpeed(mobility->getSpeed() * 0.8);
//};





}

void SmartJunctionCarApp_stop::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void SmartJunctionCarApp_stop::handleSelfMsg(cMessage* msg)
{
    DemoBaseApplLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
}

void SmartJunctionCarApp_stop::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    if (mobility->getRoadId() == "left_out" || mobility->getRoadId() == "right_out" || mobility->getRoadId() == "top_out" || mobility->getRoadId() == "bottom_out" )  {
            traciVehicle->setSpeed(-1);
            slowCheck_left = false;
            slowCheck_right = false;
            slowCheck_bottom = false;

            findHost()->getDisplayString().setTagArg("i", 1, "white");
    }
/**
        // no longer on the road on which they were told to slow?
    if (slowCheck_left && mobility->getRoadId() != "left_in" && mobility->getRoadId() != "left_main_in")  {
        traciVehicle->setSpeed(-1);
        slowCheck_left = false;
        findHost()->getDisplayString().setTagArg("i", 1, "white");
    } else if (slowCheck_right && mobility->getRoadId() != "right_in" && mobility->getRoadId() != "right_main_in")  {
        traciVehicle->setSpeed(-1);
        slowCheck_right = false;
        findHost()->getDisplayString().setTagArg("i", 1, "white");
    } else if (slowCheck_bottom && mobility->getRoadId() != "bottom_in" && mobility->getRoadId() != "bottom_main_in")  {
        traciVehicle->setSpeed(-1);
        slowCheck_bottom = false;
        findHost()->getDisplayString().setTagArg("i", 1, "white");
        EV << "I am speed"  << std::endl;
    }


*/

}

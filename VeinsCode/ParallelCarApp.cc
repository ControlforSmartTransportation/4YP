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

#include <veins/modules/application/traci/ParallelCarApp.h>
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include <iostream>
#include <typeinfo>
using namespace veins;
using namespace std;
Define_Module(veins::ParallelCarApp);




void ParallelCarApp::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        //EV << "Initializing " << par("appName").stringValue() << std::endl;

        sentMessage = false;
        slowCheck_a = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;

    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void ParallelCarApp::finish()
{
    DemoBaseApplLayer::finish();
    // statistics recording goes here
}

void ParallelCarApp::onBSM(DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU

    //EV<< "Road ID is called " << mobility->getRoadId().c_str() <<  std::endl;    //print out road ID of each vehicle as they receive the BSM

    if (mobility->getRoadId() == "a") {    //condition for if vehicle is on a certain road


        findHost()->getDisplayString().setTagArg("i", 1, "red"); //turns icon red in simulation

        TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
        populateWSM(wsm);
        wsm->setDemoData(mobility->getRoadId().c_str());
       // EV<< "Sending WSM to RSU now" <<  std::endl;
        sendDelayedDown(wsm,  uniform(0.001, 0.05));

    }
    else {
        findHost()->getDisplayString().setTagArg("i", 1, "white");
        lastDroveAt = simTime();
    }


    //if (mobility->getSpeed() < 8) { condition for if vehicle is below a certain speed

    //EV<< "I am a car and I have received a BSM, my ID is " << mobility->getId() <<  std::endl;
    //if vehicle is on road ID 'a', slow it down by 20%
    //if (mobility->getRoadId() == "a") {
    //set vehicle speed
    //traciVehicle->setSpeed(5);
    //reduce vehicle speed by a certain percentage
     //   traciVehicle->setSpeed(mobility->getSpeed() * 0.8);
    //};

}

void ParallelCarApp::onWSM(BaseFrame1609_4* frame)
{

    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

//output message to say vehicle has received a WSM (will be a lot of these potentially)
    //EV << "I am a car and I have received a WSM whos message is " << wsm->getDemoData() << std::endl;
    //EV << "contents of demodata is " << wsm->getDemoData() << std::endl;


    if ( strcmp(wsm->getDemoData(),"slow down on top road") == 0 && mobility->getRoadId() == "a") {

        EV << "I must slow down "  << std::endl;
       // traciVehicle->setMaxSpeed(traciVehicle->getMaxSpeed()/2);
        traciVehicle->setSpeed(8);
        slowCheck_a = true;
    }
    else {
        //EV << "if statement aint working"  << std::endl;
    }


//if vehicle is on road ID 'a', slow it down by 20%
//if (mobility->getRoadId()[0] == 'a') {

//what happens if remove the square brackets
//if (mobility->getRoadId() == "a") {
 //   traciVehicle->setSpeed(4);
//traciVehicle->setSpeed(mobility->getSpeed() * 0.8);
//};





    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void ParallelCarApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void ParallelCarApp::handleSelfMsg(cMessage* msg)
{
    DemoBaseApplLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
}

void ParallelCarApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

        // no longer on road a?
    if (slowCheck_a && mobility->getRoadId() != "a") {
        traciVehicle->setSpeed(-1);
        slowCheck_a = false;
    }

}

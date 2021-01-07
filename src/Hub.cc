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

#include "Hub.h"

Define_Module(Hub);

void Hub::initialize()
{

    scheduleAt(simTime(), new Frame_Base(""));
    sessionCount=0;
    for(int i=0;i<maxSessions;i++)
    {
        ofstream myfile;
        myfile.open ("SessionLOG"+to_string(i)+".txt",std::ofstream::trunc);
        myfile.close();
    }
}

void Hub::handleMessage(cMessage *msg)
{
    Frame_Base *mmsg = check_and_cast<Frame_Base *>(msg);
    if(sessionCount>maxSessions)
    {
        bubble("No More Sessions");
        exit(0);
        return;
    }
    if(mmsg->isSelfMessage() || mmsg->getType()==3)
    {
        if(mmsg->getType()==3)
        {
            bubble("Start New Session");
        }
        node1Index = uniform(0, 1)*par("n").intValue();
        do
        {
            node2Index=uniform(0, 1)*par("n").intValue();
        } while(node2Index==node1Index);
        mmsg=new Frame_Base("");
        mmsg->setName("activate"); //enable session
        mmsg->setSessionNumber(sessionCount);
        mmsg->setType(2);
        sessionCount++;
        send(mmsg,"outs",node1Index);
        send(mmsg->dup(),"outs",node2Index);  //TODO for GO Back N
        //choose two random nodes to start a session with a random time
    }
    else
    {
        if(mmsg->getSessionNumber()!=sessionCount-1){
            bubble("Message From Wrong Session");
            delete(mmsg);
        }
        else if (mmsg->getSenderModuleId()-3==node1Index || mmsg->getSenderModuleId()-3==node2Index )
        {
            int redirectTo=(mmsg->getSenderModuleId()-3==node1Index)?node2Index:node1Index;
            send(mmsg,"outs",redirectTo);
        }
    }
}

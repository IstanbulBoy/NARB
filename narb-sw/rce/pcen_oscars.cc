/*
 * Copyright (c) 2008
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2008
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "radixtree.hh"
#include "resource.hh"
#include "rce_pcen.hh"
#include "pcen_mrn.hh"
#include "pcen_oscars.hh"


bool PCEN_OSCARS::PostBuildTopology()
{
    PCEN_MRN::PostBuildTopology();

    //special topology manipulations for PCEN_OSCARS
    //relaxing Bandwidth and VLAN Tag constraints...
    int i,j, lNum = links.size();
    PCENLink *pcen_link;

    for (i = 0; i < lNum; i++)
    {
        pcen_link = links[i];
        pcen_link->link = new Link(pcen_link->link);
        //The Link resource is self-allcated will will be deleted in ~PCENLink()...
        pcen_link->link_self_allocated = true;
        if (pcen_link->link->maxBandwidth > 0)
        {
            for (j = 0; j < 7; j++)
            {
                if (pcen_link->link->maxBandwidth > pcen_link->link->unreservedBandwidth[j])
                    pcen_link->link->unreservedBandwidth[j] = pcen_link->link->maxBandwidth;
            }
        }
        list<ISCD*>::iterator it = pcen_link->link->iscds.begin();
        IfSwCapDesc* iscd;
        for (; it != pcen_link->link->iscds.end(); it++)
        {
            iscd = *it;
            //$$$$ using max bandwidth
            for (j = 0; j < 7; j++)
            {
                if (pcen_link->link->maxBandwidth > iscd->max_lsp_bw[j] )
                    iscd->max_lsp_bw[j] = pcen_link->link->maxBandwidth;
            }
            //$$$$ all possible vtagss
            if ((htons(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC) != 0)
            {
                for (j = 0; j < MAX_VLAN_NUM/8; j++)
                {
                    iscd->vlan_info.bitmask[j] = (iscd->vlan_info.bitmask[j] | iscd->vlan_info.bitmask_alloc[j]);
                    iscd->vlan_info.bitmask_alloc[j] = 0;
                }
            }
            //$$$$ all timeslots ???? (@@@@ picked timeslots shoud be ignored by OSCARS!)
            if ((htons(iscd->vlan_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI) != 0)
            {
                iscd->subnet_uni_info.first_timeslot = 0;
                for (j = 0; j < MAX_TIMESLOTS_NUM/8; j++)
                {
                    iscd->subnet_uni_info.timeslot_bitmask[j] = 0xff;
                }
            }            
        }
            
    }
    
}

int PCEN_OSCARS::PerformComputation()
{
}

void PCEN_OSCARS::Run()
{
    //modifying topology 
        //1. subnet handling in PCEN_MRN::PostBuildTopology
        //2. relaxing constraints for OSCARS requests
            //-->augmenting bandwidth and vtag set wherever needed


    //PerformComputation --> Generating initial CSPF path (shortest)
        //return error code if failed

    //Storing initial CSPF path 
        //destNode->path
        //or ERO + SubnetERO

    //Cleaning up and modifying topology
        //reset TSPEC and interim variables
        //trim links on forward path and assign VERY_BIG metric value to links on reverse path 
            //$$$$ This should work since we only look at the metric value (cost) of forward links
            //destNode->path
                //Or Using ERO and SubnetERO together to revisit the path (checking on the fly)

    //PerformComputation --> Generating maximally CSPF diverse path

    //Cutting off common links and exchanging head and tail for the two paths

    //Generating and returninig the two diverse paths 
        //if the initial shortest path is different than the two diverse paths return it too (three path set)
        //otherwise return the two path set
}


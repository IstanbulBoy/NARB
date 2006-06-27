/*
 * Copyright (c) 2005
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2006
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
#include "rce_filter.hh"

int SwitchingTypeFilter::Process(vector<PCENNode*>& routers, vector<PCENLink*>& links)
{
    vector<PCENLink*>::iterator link_iter;
    PCENLink * pcen_link;
    PCENNode * pcen_node;
    list<ISCD*>::iterator iscd_iter;
    ISCD * iscd;

    bool found;
    for (link_iter = links.begin(); link_iter != links.end(); link_iter++)
    {
        pcen_link = *link_iter;
        if (!pcen_link)
            break;

        found = false;
        for (iscd_iter = pcen_link->link->Iscds().begin(); iscd_iter != pcen_link->link->Iscds().end(); iscd_iter++)
        {
            iscd = *iscd_iter;
            if (switching_type == iscd->swtype)
            {
                found = true;
                break;
            }
        }
        if (!found) // remove the link
        {
            //first, remove the link from in_links and out_link of assocaited routers
            pcen_node = (*link_iter)->lcl_end;
            assert(pcen_node);
            pcen_node->out_links.remove(*link_iter);
            pcen_node = (*link_iter)->rmt_end;
            assert(pcen_node);
            pcen_node->in_links.remove(*link_iter);
            // remove the link itself
            links.erase(link_iter);
            if (links.size() == 0)
                break;
            link_iter--;
        }
    }
}


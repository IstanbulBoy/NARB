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

#include "cmdtree.hh"

cmdnode::~cmdnode()
{
    //All currently suported comands are static commands.
    //Do not delete the command.
    //if (command)
    //    delete command;
    for (int i = 0; i < children.size(); i++)
        if (children[i])
            delete children[i];
}


int cmdtree::Insert (CLICommand* cmd)
{
    cmdnode* node = root;
    vector<string>& cmdv = cmd->CmdVector();
    int i, j;
    for (i = 0; i < cmdv.size(); i++)
    {
        string& cmd_tag = cmdv[i];
        cmdnode_type type;
        switch (cmd_tag[0])
        {
            case '@':
                type = _ARG_S;
                break;
            case '{':
                type = _ARG_M;
                break;
            default:
                type = _CMD;
        }
        vector<cmdnode*>& subnodes = node->Children();
        for (j = 0; j < subnodes.size(); j++)
        {
            string& node_tag = subnodes[j]->Tag();
             if ( (type == _CMD && node_tag == cmd_tag)
                || (type == _ARG_S && node_tag[0] == '@') )
            {
                node = subnodes[j];
                goto _continue_i;
            }
            else if (type == _ARG_M && node_tag[0] == '{' )
            {
                if (node_tag != cmd_tag)
                    return -1;
                node = subnodes[j];
                goto _continue_i;
            }
        }
        node = node->MakeChild(type);
        node->SetTag(cmd_tag);
        if (cmd->HelpVector().size() > i)
            node->SetHelp(cmd->HelpVector()[i]);

    _continue_i:
        ;
    }

    node->SetCommand(cmd);
}

int cmdtree::Search(vector<string>& iv, cmdnode* &node, vector<string>& av)
{
    node = root;
    char buf [CMD_BUF_SIZE];
    string arg;
    av.clear();
    vector<string> ov;
    char* ps;

    int i, j, k;
    for (i = 0; i < iv.size(); i++)
    {
        string& i_tag = iv[i];
        vector<cmdnode*>& subnodes = node->Children();
        ov.clear();
        arg = "";
        for (j = 0; j < subnodes.size(); j++)
        {
            cmdnode_type type = subnodes[j]->Type();
            string& node_tag = subnodes[j]->Tag();
            if (type == _CMD && node_tag.find(i_tag, 0) == 0)
            {
                ov.push_back(node_tag);
                node = subnodes[j];

                if (node_tag == i_tag)
                {
                    ov.clear();
                    ov.push_back(node_tag);
                    break;
                }
            }
            else if (type == _ARG_M && node_tag[0] == '{')
            {
                strcpy(buf, node_tag.c_str());
                ps = strtok(buf, "{|} \t");
                do 
                {
                    if (strstr(ps, i_tag.c_str()) == ps)
                    {
                        arg = ps;
                        ov.push_back(arg);
                    }
                } while ((ps = strtok(NULL, "{|} \t")) != NULL);
                
                node = subnodes[j];
            }
            else if (type == _ARG_S && node_tag[0] == '@')
            {
                arg = node_tag.c_str() + 1;
                ov.push_back(arg);
                node = subnodes[j];
                arg = iv[i];
            }
        }

        if (ov.size() == 0)
        {
            node = NULL;
            iv.erase (iv.begin()+i, iv.end());
            //av.clear();
            return 0;
        }
        else if (ov.size() > 1)
        {
            node = NULL;
            av.clear();
            for (k = 0; k< ov.size(); k++)
                av.push_back(ov[k]);
            iv.erase (iv.begin()+i, iv.end());
            return k;
        }
        if (arg!="")
            av.push_back(iv[i]);
    }

    if (node && node->Command() == NULL)
    {
        av.clear();
        vector<cmdnode*>& subnodes = node->Children();
        for (i = 0; i< node->Children().size(); i++)
        {
            switch(subnodes[i]->Tag()[0])
            {
            case '{':
                strcpy(buf, subnodes[i]->Tag().c_str());
                ps = strtok(buf, "{|} \t");
                do 
                {
                    arg = ps;
                    av.push_back(arg);
                } while ((ps = strtok(NULL, "{|} \t")) != NULL);                
                break;
            case '@':
                arg = subnodes[i]->Tag().c_str() + 1;
                av.push_back(arg);
                break;
             default:
                av.push_back(subnodes[i]->Tag());
            }
        }
    }

    return 1;
}

int cmdtree::Complete(vector<string>& iv, vector<string>& ov)
{
    cmdnode* node = root;
    char buf [CMD_BUF_SIZE];
    string complete;
    char* ps;
    int i, j;
    for (i = 0; i < iv.size(); i++)
    {
        string& i_tag = iv[i];
        vector<cmdnode*>& subnodes = node->Children();
        ov.clear();
        for (j = 0; j < subnodes.size(); j++)
        {
            cmdnode_type type = subnodes[j]->Type();
            string& node_tag = subnodes[j]->Tag();
            if (type == _CMD && node_tag.find(i_tag, 0) == 0)
            {
                complete = node_tag;
                node = subnodes[j];
                ov.push_back(complete);

                if (complete == i_tag)
                {
                    ov.clear();
                    ov.push_back(complete);
                    break;
                }
            }
            else if (type == _ARG_M && node_tag[0] == '{')
            {
                strcpy(buf, node_tag.c_str());
                ps = strtok(buf, "{|} \t");
                do 
                {
                    complete = ps;
                    if (complete.find(i_tag, 0) == 0)
                    {
                        ov.push_back(complete);
                    }
                } while ((ps = strtok(NULL, "{|} \t")) != NULL);
                
                node = subnodes[j];
            }
            else if (type == _ARG_S && node_tag[0] == '@')
            {
                complete = i_tag;
                ov.push_back(complete);
                node = subnodes[j];
            }
        }
        if (ov.size() != 1)
        {
            node = NULL;
            return i;
        }
        i_tag = ov[0];
    }    

    ov.clear();
    if (node && node->Command() == NULL)
    {
        vector<cmdnode*>& subnodes = node->Children();
        for (i = 0; i< node->Children().size(); i++)
        {
            switch(subnodes[i]->Tag()[0])
            {
            case '{':
                strcpy(buf, subnodes[i]->Tag().c_str());
                ps = strtok(buf, "{|} \t");
                do 
                {
                    complete = ps;
                    ov.push_back(complete);
                } while ((ps = strtok(NULL, "{|} \t")) != NULL);                
                break;
            case '@':
                complete = subnodes[i]->Tag().c_str() + 1;
                ov.push_back(complete);
                break;
             default:
                ov.push_back(subnodes[i]->Tag());
            }
        }
    }
    return iv.size();
}


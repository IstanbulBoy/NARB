/*
 * Copyright (c) 2007
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
 *
 * Created by Xi Yang 2004-2007
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

#include "rce_clicmd.hh"


CLINode * CLICommand::Node() 
{ 
    return cli_node;  
}

void CLICommand::SetNode (CLINode * node)
{
    cli_node = node;
}

int CLICommand::cmdstr_to_vector(char* s, vector<string> &v)
{
    assert (s);
    v.clear();

    char* ps1, *ps2;
    char buf[256];
    int len = strlen(s);
    assert (len < 512);

    ps1 = s;
    while (*ps1 && ps1 - s <= len)
    {
        if (isspace(*ps1))
        {
            ps1++;
            continue;
        }

        ps2 = ps1;

        if (isalpha(*ps1) && islower(*ps1))
        {
            while (!isspace(*ps2) && ps2 - s <= len)
            {
                ps2++;
            }
            strncpy(buf, ps1, ps2-ps1);
            buf[ps2-ps1] = '\0';
        }
        else if (*ps1 == '{')
        {
            while (*ps2 != '}')
            {
                if (ps2 - s >= len )
                    return -1;
                ps2++;
            }
            strncpy(buf, ps1, ps2-ps1+1);
            buf[ps2-ps1+1] = '\0';
        }
        else if (isalpha(*ps1) && isupper(*ps1))
        {
            while (!isspace(*ps2)  && ps2 - s <= len)
            {
                ps2++;
            }
            buf [0] = '@';
            buf[1] = '\0';
            strncat(buf, ps1, ps2-ps1);
            buf[ps2-ps1+2] = '\0';
        }
        else if (*ps1 == '[' || *ps1 == '<')
        {
            while (*ps2 != *ps1+2 )
            {
                if (ps2 - s >= len )
                    return -1;
                ps2++;
            }
            buf[0] = '@';
            buf[1] = '\0';
            strncat(buf, ps1+1, ps2-ps1-2);
            buf[ps2-ps1] = '\0';
        }
        else
            return -1;
        v.push_back(string(buf));
        ps1 = ps2+1;
    }
    
    return 0;
}

int CLICommand::helpstr_to_vector(char* s, vector<string> &v)
{
    assert (s);
    char buf[CMD_BUF_SIZE];
    strcpy(buf, s);
    char* token = strtok(buf, "\n\r");
    v.clear();
    while (token)
    {
        if (strlen(token) < 2)
            return -1;
        string hstr(token);
        v.push_back(hstr);
        token = strtok(NULL, "\n\r");
    }
    return 0;
}

//COMMAND(cmd_test6, "set narb IP port NUM", "Peering NARB Daemon\n NARB IP
// Address \n Port \n Port Number")
//{   
//    cli_node->Writer()->Printf("I'm fine. Thanks.%s", cli_cstr_newline);
//    cli_node->ShowPrompt();
//}   


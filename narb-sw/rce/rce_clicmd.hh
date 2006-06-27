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
#ifndef __RCE_CLICMD_H__
#define __RCE_CLICMD_H__

#include "rce_types.hh"
#include "event.hh"
#include "rce_cli.hh"
#include "rce_log.hh"
#include <ctype.h>
using namespace std;
#include <string>
#include <vector>

#define CMD_BUF_SIZE 512

class CLINode;
class CLICommand: public Event
{
protected:
    string cmd;
    vector<string> cmdv;   //vector of comands
    vector<string> helpv;  //vector of help strings
    vector<string> argv;    //vector of arguments
    CLINode* cli_node;    //the CLI node this command is assocaited to

public:
    CLICommand(char* cmd_cstr, char* help_cstr) 
        { 
            cmd = cmd_cstr;
            int ret = cmdstr_to_vector(cmd_cstr, cmdv); assert (ret == 0);
            ret = helpstr_to_vector(help_cstr, helpv); assert (ret == 0); 
        }
    CLICommand(string& cmd_str, string& help_str)
        { 
            cmd = cmd_str;
            char * cmd_cstr = (char*)cmd_str.c_str();
            char * help_cstr = (char*)help_str.c_str();
            int ret = cmdstr_to_vector(cmd_cstr, cmdv); assert (ret == 0);
            ret = helpstr_to_vector(help_cstr, helpv); assert (ret == 0); 
        }
 
    CLINode * Node();
    void SetNode (CLINode * node);
    
    virtual void Run() {}
    
    //func to take a vector of arguments and do self schedule 
    void Execute(vector<string>& args)
        {
            argv.clear();
            for (int i = 0; i < args.size(); i++)
            {
                argv.push_back(args[i]);
            }
            //Do NOT SetAutoDelete for static commands.
            //SetAutoDelete only for dynamically created commands.
            eventMaster.Schedule(this);
        }
    //execute using existing args
    void Execute(void)
        {
            //Do NOT SetAutoDelete for static commands.
            //SetAutoDelete only for dynamically created commands.
            eventMaster.Schedule(this);
        }

    //func to take a vector of arguments and do self schedule 
    void ExecuteNow(vector<string>& args)
        {
            argv.clear();
            for (int i = 0; i < args.size(); i++)
            {
                argv.push_back(args[i]);
            }

            this->Run();
        }

    vector<string>& CmdVector() {return cmdv;}
    vector<string>& HelpVector() {return helpv;}
    vector<string>& ArgVector() {return argv;}

    void MakeAlias (char* cs, char* cs_h) { cmdstr_to_vector(cs, cmdv); helpstr_to_vector(cs_h, helpv); }
    void MakeAlias (string& s, string& s_h) { MakeAlias((char*)s.c_str(), (char*)s_h.c_str()); }
    
    //cmdv ==> a catenated string (separated by spaces)
    string CmdsToStr(int n = 0)
    {
        if ((cmdv.size() == 0))
            return NULL;

        if (n == 0)
            n = cmdv.size();

        string str(cmdv[0]);
        for (int i = 1; i < cmdv.size() && i < n; i++)
        {
            str += ' ';
            str += cmdv[i];
        }
        return str;
    }

    //vectors of cmdv + argv ==> a catenated string (separated by spaces)
    string CmdsArgsToStr(int n = 0)
    {
        assert((cmdv.size() == argv.size()) || (cmdv.size() == 1 && argv.size() == 0));
        assert (n <=argv.size());
        if (n ==0)
            n = cmdv.size();
        string str(cmdv[0]);
        str += ' ';
        str += argv[0];
        for (int i = 1; i < n; i++)
        {
            str += ' ';
            str += cmdv[i];
            str += ' ';
            str += argv[i];
        }
        return str;
    }

    static int cmdstr_to_vector(char* s, vector<string> &v);
    static int helpstr_to_vector(char* s, vector<string> &v);
};


#define COMMAND(class_name, cmd_cstr, help_cstr)  \
    class class_name: public CLICommand \
    { \
     public:\
        class_name(char* c_cstr, char* h_cstr):CLICommand (c_cstr, h_cstr) { }\
        virtual void Run();\
    };\
    class_name class_name##_instance(cmd_cstr, help_cstr);\
    inline void class_name::Run() 

#define COMMAND_CLASS(class_name, cmd_cstr, help_cstr) \
    class class_name: public CLICommand \
    { \
     public:\
        class_name(char* c_cstr, char* h_cstr):CLICommand (c_cstr, h_cstr) { }\
        virtual void Run();\
    };\
    inline void class_name::Run()

#define COMMAND_DECLARE(class_name) extern class class_name class_name##_instance; \
static CLICommand* class_name##_pointer = (CLICommand*)&class_name##_instance


#endif


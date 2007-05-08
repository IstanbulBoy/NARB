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

 #ifndef __RCE_CLI_H__
 #define __RCE_CLI_H__

#include "rce_types.hh"
#include "event.hh"
#include "rce_apiserver.hh"
#include "rce_clicmd.hh"
#include "cmdtree.hh"
#include "rce_config.hh"
using namespace std;
#include <vector>

class CLIReader;
class CLIWriter;
class CLINode;
class CLIServer: public APIServer
{
public:
    CLIServer(u_int32_t port_val):APIServer(port_val){}
    virtual ~CLIServer() { }
    virtual void Run();
    //virtual APIReader* CreateAPIReader(int sock);
    //virtual APIWriter* CreateAPIWriter(int sock);
};

 
#define cli_char_backward ((char)0x08)
#define cli_char_space ((char)' ')
const string _newline("\r\n");
#define cli_newline (string&)_newline
#define cli_cstr_newline ((char*)(_newline.c_str()))
#define MAX_HISTORY_DEPTH 20
#define CLI_BUF_SIZE 256
#define max_subcmd_size 16
#define CTRL(X)  ((X) - '@')


class CLINode;
class CLIWriter;
class cmdnode;
class CLIReader: public Reader
{
private:
    CLIServer *server;
    CLIWriter* cli_writer;
    string buffer;
    vector<string> history;
    int hi;
    // Current CLI status
    enum {CLI_STATUS_NORMAL, CLI_STATUS_CLOSE, CLI_STATUS_FILE} status;
    // IAC handling
    bool iac;
    //IAC SB handling
    bool iac_sb;
    string sb_buffer;
    // Window width/height.
    int width;
    int height;
    // command cursor and length
    int cp;
    int clen;
    bool insert;
    int retry;

    class CLITimer: public Timer
    {
    private:
        CLIReader *cli_reader;
    public:
        CLITimer(int seconds, CLIReader* reader): Timer(seconds, 0)
            {
                assert(reader);
                cli_reader = reader;
            }
        virtual void Run()
            {
                Timer::Run();
                cli_reader->PromptTimeout();
                cli_reader->Exit();
            }
    } timer;

    int HandleTelnetOption (unsigned char *buf, int nbytes);
    int Authenticate();

    int ProcessEscape(u_int16_t val);
    int ProcessChar(unsigned char c);
    void CompleteCommand();
    void DescribeCommand();
    int ExecuteCommand(bool immediate = false);

    void PrintChar(char c);
    void WriteChar (char c);
    void InsertChar (char c);
    void BackwardChar ();
    void DeleteChar ();
    void ForwardChar ();
    void LineBeginning ();
    void LineEnd ();
    void KillLine ();
    void DeleteLine ();
    void InterruptLine ();
    void PreviousLine ();
    void NextLine ();
    void PrintHelpStr(cmdnode * node);
    void PromptTimeout();

public:
    CLIReader(int fd, CLIServer *server_ptr):Reader(fd), server(server_ptr), timer(SystemConfig::cli_timeout, this)
        {
            assert(server_ptr);
            cli_writer = NULL;
            status = CLI_STATUS_NORMAL; 
            iac = iac_sb = false;
            width = height = 0;
            cp = clen = 0; 
            insert = true; 
            hi =0;
            retry = 3;
        }
    virtual ~CLIReader();
    virtual void Run ();
    void Exit();
    CLIWriter* GetWriter() { return cli_writer; }
    void SetWriter(CLIWriter* writer) { cli_writer = writer; }
    void GoTop() { current_node = cli_root;} 
    void GoUp();
    void GoDown(string& tag);
    void GoDown(char * val) { string tag(val); GoDown(tag); }
    void GoDirect(string& tag_s);
    void GoDirect(char * val) { string tag(val); GoDirect(tag); }
    CLINode* RootNode() {return cli_root;}
    CLINode* CurrentNode() {return current_node;}
    void InitSession();
    int Execute(string& cmd_str, bool immediate =  false);
    int Execute(char* cmd_cstr, bool immediate =  false);

private:
    //CLI nodes and command elements initilization
    CLINode* cli_root;
    CLINode* current_node;
    CLINode * FindChild(CLINode *node, string &tag);
};

class CLIWriter: public Writer
{
private:
    CLIServer *server;
    CLIReader* cli_reader;
    list<string> buf_queue;
    bool first_run;
    bool is_passwd;

public:
    CLIWriter(int fd, CLIServer *server_ptr): Writer(fd), server(server_ptr) 
        { assert(server_ptr); first_run = true; is_passwd =false; }
    virtual ~CLIWriter() {}
    void InitTerminal();
    virtual void Run ();
    CLIReader* GetReader() { return cli_reader; }    
    bool IsPasswd() { return is_passwd; }
    void SetIsPasswd(bool b) { is_passwd = b; }
    void SetReader(CLIReader* reader) { cli_reader = reader; }
    void WriteBuffer(string &buf);
    int FlushBuffer(string& buf);
    int Printf(const char *format, ...);
    void PointTo (int x);
    void PointTo (int x, vector<string>&v);
};

////////////////// Definition of CLI Tree Structure //////////////////
class CLICommand;
class cmdtree;
class CLINode
{
private:
    CLINode* top;
    CLINode* parent;
    vector<CLINode*> children;
    cmdtree* cmd_tree;
    string tag;
    string prompt;
    CLIReader *reader;
    CLIWriter *writer;

    static bool dumb;

    CLINode() {}

public:
    CLINode(CLIReader* r, CLIWriter* w);
    CLINode(CLINode* p, CLIReader* r, CLIWriter* w);
    ~CLINode();
    CLIReader* Reader() { return reader; }
    CLIWriter* Writer() { return writer; }
    CLINode* Top() {return top;}
    CLINode* SetTop(CLINode *t) {top = t;}
    CLINode* Parent() {return parent;}
    CLINode* SetParent(CLINode *p) {parent = p; top = p->Top();}
    string& Tag() {  return tag;  }
    void SetTag(string& val) { tag = val; }
    string& Prompt() {  return prompt;  }
    void SetPrompt(string& val) { prompt = val; }
    void SetPrompt(char* val) { prompt = val; }
    bool Dumb() { return dumb; }
    void SetDumb(bool b) { dumb = b; }
    void ShowPrompt();
    vector<CLINode*>& Children(){  return children;  }
    CLINode* MakeChild(char * tag_cstr)
        {
            CLINode *child = new CLINode(this, reader, writer);
            assert(tag_cstr);
            string s(tag_cstr);
            child->SetTag(s);
            children.push_back(child); 
            return child;
        }
    void AddChild(CLINode *child) 
        { assert(child); children.push_back(child);}
    cmdtree* CmdTree() {  return cmd_tree;  }
    void AddCommand(CLICommand *cmd);
    int ParseInput(string& input, vector<string>& cmd_v, vector<int>& len_v);
};

#define CLI_OUT if (!cli_node || !cli_node->Writer() || cli_node->Dumb()) ; else cli_node->Writer()->Printf
#define CLI_EXECUTE cli_node->Reader()->Execute

#endif


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

#include "types.hh"
#include "cli.hh"
#include "log.hh"
#include <arpa/telnet.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include "dts.hh"
#include "narb_config.hh"
#include "rce_apiclient.hh"
#include "zebra_ospfclient.hh"
#include "toolbox.hh"

// NSF Dragon motd string. 
static string dragon_welcome(
"\r\n\
 ____                              \r\n\
|  _ \\ _ __ __ _  __ _  ___  _ __  \r\n\
| | | | '__/ _` |/ _` |/ _ \\| '_ \\ \r\n\
| |_| | | | (_| | (_| | (_) | | | |\r\n\
|____/|_|  \\__,_|\\__, |\\___/|_| |_|\r\n\
                 |___/             \r\n\
(D)ynamic (R)esource (A)llocation via\r\n\
(G)MPLS (O)ptical (N)etworks\r\n\
Copyright 2003-2005 the Dragon Team.\r\n\
\r\n");

 void CLIServer::Run()
{
    struct sockaddr_in sa_in;
    sa_in.sin_family = AF_INET;
    sa_in.sin_port = htons(port);
    u_int32_t addr_u32 = htonl(INADDR_ANY);
    memcpy(&(sa_in.sin_addr), &addr_u32, sizeof(struct in_addr));
    socklen_t len = sizeof(struct sockaddr_in);
    int new_sock = accept (fd, (struct sockaddr *)&sa_in, &len);
    if (new_sock < 0)
    {
        LOGF("CLIServer::Run: cannot accept socket on %d\n", fd);
        return;
    }

    CLIWriter* cli_writer = new CLIWriter(new_sock, this);
    cli_writer->SetAutoDelete(false);
    cli_writer->SetRepeats(0);
    CLIReader* cli_reader = new CLIReader(new_sock, this);
    cli_reader->SetAutoDelete(true);
    cli_reader->SetRepeats(FOREVER);
    
    cli_reader->SetWriter(cli_writer);
    cli_writer->SetReader(cli_reader);
    cli_reader->InitSession();

    //Schedule read event for each accepted connection
    cli_writer->InitTerminal();
    eventMaster.Schedule(cli_reader);

    LOG_DEBUG("Accepted a CLI connection on socket(" <<new_sock <<")" << endl);
}

CLIReader::CLIReader(int fd, CLIServer *server_ptr):Reader(fd), server(server_ptr), timer(SystemConfig::cli_timeout, this)
{
    //assert(server_ptr);
    cli_writer = NULL;
    status = CLI_STATUS_NORMAL; 
    iac = iac_sb = false;
    width = height = 0;
    cp = clen = 0; 
    insert = true; 
    hi =0;
    retry = 3;
}

CLIReader::~CLIReader() 
{
    delete cli_root;
    eventMaster.Remove(&timer);
}

void CLIReader::Exit() 
{
    status = CLI_STATUS_CLOSE;
	if (fd > 0)
	{
        LOGF("CLI connection on socket (%d) closed\n", fd);
        cli_writer->Close();
        Close();
	}
}

void CLIReader::Run()
{
    int i, n;
    int ret;
    unsigned char buf[CMD_BUF_SIZE];

    //reset timer
    timer.Restart();
    
    // check status before processing
    if (status == CLI_STATUS_CLOSE)
    {
		if (fd > 0)
		{
            cli_writer->Close();
            Close();
		}
        return;
    }

    n = read (fd, buf, CMD_BUF_SIZE);
    if (n <= 0)
    {
        status = CLI_STATUS_CLOSE;
    }

    for (i = 0; i < n; i++) 
    {
        if (buf[i] == IAC)
        {
            iac = !iac;
            if (iac)
                continue;
        }
      
        if ( !iac && iac_sb)
        {
            sb_buffer += buf[i];
            continue;
        }

        if (iac)
        {
            // handling telnet command
            ret = HandleTelnetOption (buf + i, n - i);
            i += ret;
            iac = 0;
            continue;
        }

        while (buf[i] == 27)
        {
            //processing escape key
            i += ProcessEscape(*(u_int16_t*)(buf+i+1));
            if (i >= n)
                break;
        }

        if (i >=n)
            break;

        ProcessChar(buf[i]);
    }

    // check status after processing
    if (status == CLI_STATUS_CLOSE)
    {
        LOGF("CLI connection on socket (%d) closed\n", fd);
		if (fd > 0)
		{
            cli_writer->Close();
            Close();
		}
    }
}

int CLIReader::ProcessEscape(u_int16_t val)
{
    switch (val)
    {
    case 17499: //lleft key...
        ProcessChar(CTRL('B'));
        return 3;
        break;
    case 17243: //right key...
        ProcessChar(CTRL('F'));
        return 3;
        break;
    case 12891: //inset key
        insert = !insert;
        return 4;
        break;
    case 16731: //up key
        ProcessChar(CTRL('P'));
        return 3;
        break;
    case 16987: //down key
        ProcessChar(CTRL('N'));
        return 3;
        break;
    }
    return 3;
}

int CLIReader::ProcessChar(unsigned char c)
{
    switch (c)
    {
    case CTRL('A'):
        LineBeginning();
        break;
    case CTRL('B'):
        BackwardChar();
        break;
    case CTRL('C'):
        InterruptLine();
        break;
    case CTRL('D'):
        DeleteChar();
        break;
    case CTRL('E'):
        LineEnd();
        break;
    case CTRL('F'):
        ForwardChar ();
        break;
    case CTRL('H'):
    case 0x7f:
        BackwardChar();
        DeleteChar();
        break;
    case CTRL('K'):
        KillLine();
        break;
    case CTRL('N'):
        NextLine();
        break;
    case CTRL('P'):
        PreviousLine();
        break;
    case CTRL('U'):
        DeleteLine ();
        break;
    case ' ':
        if (cp < clen || cli_writer->IsPasswd())
            PrintChar(c);
        else
            CompleteCommand ();
        break;
    case '\t':
        if (!cli_writer->IsPasswd())
            CompleteCommand ();
        break;
    case '?':
        if (!cli_writer->IsPasswd())
            DescribeCommand();
        else
            PrintChar(c);
        break;
    case '\r':
        if (!cli_writer->IsPasswd())
            return ExecuteCommand();
        else
            return Authenticate();
       break;
    case '\n':
        break;
    default:
        PrintChar(c);
        break;
    }

    return 0;
}

void CLIReader::PrintChar(char c)
{
    if (c > 31 && c < 127)
    {
        if (insert)
        {
            InsertChar(c);
            clen++;
        }
        else
        {
            WriteChar(c);
            if (cp == clen)
                clen++;
        }
        cp++;
    }
}

int CLIReader::HandleTelnetOption (unsigned char *buf, int nbytes)
{
    int len;

    switch (buf[0])
    {
    case SB:
        sb_buffer = "";
        iac_sb = true;
        return 0;
        break;
    case SE: 
        if (!iac_sb)
        return 0;

        len = sb_buffer.length();
        if (len == 0)
  	     return 0;
  
        if (sb_buffer[0] == '\0')
  	 {
            iac_sb = false;
  	     return 0;
  	 }

        switch (sb_buffer[0])
        {
        case TELOPT_NAWS:
            if (len >= 5)
            { 
                width = (unsigned char)sb_buffer[2];
                height   = (unsigned char)sb_buffer[4];
            }
  	     break;
        }

        iac_sb = false;
        return 0;
        break;
    default:
        break;
    }
    return 1;
}

int CLIReader::Authenticate()
{
    string s;
    if (buffer == SystemConfig::cli_password)
    {
        s = "narb:cli>";
        cli_writer->SetIsPasswd(false);
        current_node->SetPrompt(s);
    }
    else if (retry-- == 0)
    {
        Exit();
		return 0;
    }

    s = cli_cstr_newline;
    cli_writer->WriteBuffer(s);
    current_node->ShowPrompt();
    buffer.clear();
    cp = clen = 0;

    return 0;    
}
    
void CLIReader::CompleteCommand()
{
    assert(cli_writer);

    vector<string> cmd_v;
    vector<string> choices;
    vector<int> len_v;
    int ret = current_node->ParseInput(buffer, cmd_v, len_v);
    if (ret <= 0)
        return;
    
    ret = current_node->CmdTree()->Complete(cmd_v, choices);

    string tail = "";

    if (ret < cmd_v.size())
    {
        assert(cli_writer);
        cli_writer->PointTo(len_v[ret], choices);
    }
    else if(buffer[buffer.size() -1] == ' ')
    {
        if (choices.size() == 1)
        {
            if (isalpha(choices[0][0]) && islower(choices[0][0]))
            {
                DeleteLine();
                cmd_v.push_back(choices[0]);
            }
            else
            {
                cli_writer->PointTo(buffer.size() - 1, choices);
            }             
        }
        else if (choices.size() > 1)
        {
            cli_writer->PointTo(buffer.size(), choices);
        }
        else
        {
            choices.push_back(string("<ret>"));
            cli_writer->PointTo(buffer.size() - 1, choices);
        }
        tail = " ";
    }
    else 
    {
        DeleteLine();
        tail = " ";
    }

    buffer  = cmd_v[0];
    for (int i = 1; i < cmd_v.size(); i++)
    {
        buffer += ' ';
        buffer += cmd_v[i];
    }
    buffer += tail;

    //post command completion (buffer may have been changed)
    string buffer_dup = buffer;
    buffer = buffer_dup;
    cli_writer->WriteBuffer(buffer);    
    cp = clen = buffer.size();
}

static void space_padding(char *buf, int total_size, int str_size)
{
    assert(buf);
    if (total_size <= str_size)
    {
        buf[0] = ' ';
        buf[1] = '\0';
    }
    int i;
    for (i = 0; i < total_size - str_size; i++)
        buf[i] = ' ';
    buf[i] = '\0';
}

void CLIReader::PrintHelpStr(cmdnode * node)
{
    char *ps;
    char buf[CLI_BUF_SIZE];
    char padding[max_subcmd_size];
    vector<string> str_v;
    string sbuf;

    assert(node);

    if (node->Tag()[0] == '@')
    {
        sbuf = node->Tag();
        sbuf.erase(0, 1);
        space_padding(padding, max_subcmd_size, sbuf.size());
        sbuf += padding;
        sbuf += node->Help();
    }
    else if (node->Tag()[0] == '{')
    {
        strcpy(buf, node->Help().c_str());
        ps = strtok(buf, "{|}");
        while (ps)
        {
            str_v.push_back(string(ps));
            ps = strtok(NULL, "{|}");
        }
        int i = 0;
        strcpy(buf, node->Tag().c_str());
        ps = strtok(buf, " {|}");
        while (ps)
        {
            sbuf += ps;
            space_padding(padding, max_subcmd_size, strlen(ps));
            if (str_v.size() > i)
            {
                sbuf += padding;
                sbuf += str_v[i];
                i++;
            }
            ps = strtok(NULL, " {|}");
            if (ps)
            {
                sbuf += cli_newline;
                sbuf += "  ";
            }
        }
    }
    else
    {
        sbuf = node->Tag();
        space_padding(padding, max_subcmd_size, sbuf.size());
        sbuf += padding;
        sbuf += node->Help();
    }

    cli_writer->Printf("  %s%s",sbuf.c_str(), cli_cstr_newline);
}

void CLIReader::PromptTimeout()
{
    string str = "CLI timeout. Connection is going to be closed...";
    str += cli_newline;
    if (cli_writer)
        cli_writer->FlushBuffer(str);
}

void CLIReader::DescribeCommand()
{
    assert(cli_writer);
    vector<string> cmd_v;
    vector<string> arg_v;
    vector<int> len_v;
    cmdnode * node;
    int i, n_input = current_node->ParseInput(buffer, cmd_v, len_v);
    if (n_input < 0)
    {
        cli_writer->WriteBuffer(cli_newline);
        cli_writer->WriteBuffer(current_node->Prompt());
        buffer = "";
        cp = clen = 0;
        return;
    }

    int ret = current_node->CmdTree()->Search(cmd_v, node, arg_v);
    if (ret == 1) // found a unique command branch
    {
        if (buffer[buffer.size()-1] == ' ' || node->Tag() == "")
        {
            if (node->Command() == NULL)
            {
                cli_writer->WriteBuffer(cli_newline);
                cmdnode *subnode;
                for (i = 0; i < node->Children().size(); i++)
                {
                    subnode = node->Children()[i];
                    PrintHelpStr(subnode);
                }
            }
            else
            {
                arg_v.clear();
                arg_v.push_back(string("<ret>"));
                cli_writer->PointTo(buffer.size() - 1, arg_v);
                cli_writer->WriteBuffer(cli_newline);
            }
        }
        else
        {
            cli_writer->WriteBuffer(cli_newline);
            PrintHelpStr(node);
        } 
    }
    else if (ret == 0) // nothing found
    {
        cli_writer->PointTo(len_v[cmd_v.size()]);
        cli_writer->WriteBuffer(cli_newline);
    }
    else if (ret > 1) // ambiguous
    {
        cli_writer->PointTo(len_v[cmd_v.size()], arg_v);
        cli_writer->WriteBuffer(cli_newline);
    }
    else
    {
        buffer = "";
    }
    
    //after description of the command, reprint the command
    cli_writer->WriteBuffer(current_node->Prompt());
    cli_writer->WriteBuffer(buffer);
    cp = clen = buffer.size();
}

int CLIReader::ExecuteCommand(bool immediate)
{
    int rc = -1;
    assert(cli_writer);

    vector<string> cmd_v;
    vector<string> arg_v;
    vector<int> len_v;
    cmdnode * node;
    int n_input = current_node->ParseInput(buffer, cmd_v, len_v);
    if (n_input < 0)
    {
        cli_writer->WriteBuffer(cli_newline);
        cli_writer->WriteBuffer(current_node->Prompt());
        buffer = "";
        cp = clen = 0;
        return rc;
    }

    //queue the current command line
    if (n_input > 0 && (history.size() == 0 || buffer != history.back()))
    {
        if (history.size() == MAX_HISTORY_DEPTH)
            history.erase(history.begin());
        history.push_back(buffer);
        hi = history.size() - 1;
    }

    int ret = current_node->CmdTree()->Search(cmd_v, node, arg_v);
    CLICommand *cmd = NULL;
    if (ret == 1) // found a unique command branch
    {
        cmd = node->Command();
        if (cmd != NULL)
        {
            cmd->SetNode(current_node);
            if (immediate)
                cmd->ExecuteNow(arg_v);
            else
                cmd->Execute(arg_v);
            buffer ="";
            hi++;
            rc = 0;
        }
        else
        {
            assert(arg_v.size() > 0);
            cli_writer->PointTo(buffer.size(), arg_v);
        }
    }
    else if (ret == 0) // nothing found
    {
        cli_writer->PointTo(len_v[cmd_v.size()]);
    }
    else if (ret > 1) // ambiguous
    {
        cli_writer->PointTo(len_v[cmd_v.size()], arg_v);
    }
    else
    {
        buffer = "";
    }

    //after execution of the command, print a brandnew newline
    cli_writer->WriteBuffer(cli_newline);
    cp = clen = buffer.size();
    if (cmd == NULL)
    {
        cli_writer->WriteBuffer(current_node->Prompt());
        cli_writer->WriteBuffer(buffer);
    }

    return rc;
}

int CLIReader::Execute(string& cmd_str, bool immediate)
{
    buffer = cmd_str;
    return ExecuteCommand(immediate);
}

int CLIReader::Execute(char* cmd_cstr, bool immediate)
{
    buffer = cmd_cstr;
    return ExecuteCommand(immediate);
}

void CLIReader::GoUp() 
{ 
    if (current_node == cli_root) 
        return;
    assert (current_node && current_node->Parent());
    current_node = current_node->Parent();
}
    
void CLIReader::GoDown(string& tag)
{
    CLINode * node = FindChild (current_node, tag);
    if (node)
        current_node = node;
    else
        cli_writer->Printf("There is no sub-node named '%s'! %s", tag.c_str(), cli_cstr_newline);
}
void CLIReader::GoDirect(string& tag_s)
{
    CLINode * node = cli_root;
    char buf[CMD_BUF_SIZE];
    strcpy(buf, tag_s.c_str());
    char *tok = strtok(buf, "/\\,;| ");
    string tag;
    while (tok)
    {
        tag = string(tok);
        node = FindChild(node, tag);
        if (!node)
            break;
        tok = strtok(NULL, "/\\,;| ");
    }
    if (node)
        current_node = node;
}

CLINode * CLIReader::FindChild(CLINode *node, string &tag)
{
    if (node == NULL || node->Children().size() == 0)
        return NULL;
    for (int i = 0; i < node->Children().size(); i++)
    {
        if (tag == node->Children()[i]->Tag())
        {
            return node->Children()[i];
        }
    }
    return NULL;
}
inline void CLIReader::WriteChar(char c)
{
    string cs;
    cs = "";
    cs += c;

    if (c != cli_char_backward)
    {
        if (cp == clen)
        {
            buffer += c;
        }
        else
        {
            assert (cp < clen);
            buffer.replace(cp, 1, cs);
        }
    }

    if (!cli_writer->IsPasswd())
        cli_writer->WriteBuffer(cs);
}

inline void CLIReader::InsertChar(char c)
{
    string cs(1, c);

    if (c != cli_char_backward)
    {
        if (cp == clen)
        {
            buffer += c;
        }
        else
        {
            assert (cp < clen);
            buffer.insert(cp, cs);
            cs = buffer.substr(cp, clen-cp+1);
        }    
    }

    for (int i = 0; i < clen-cp; i++)
    {
        cs += cli_char_backward;
    }

    if (!cli_writer->IsPasswd())
        cli_writer->WriteBuffer(cs);
}

inline void CLIReader::BackwardChar()
{
    if (cp == 0)
        return;

    if ((cp + current_node->Prompt().size()) % width == 0)
        return;

    WriteChar(cli_char_backward);

    cp --;
}

inline void CLIReader::DeleteChar()
{
    if (cp == clen)
        return;
 
    int size = clen - cp;
    
    buffer.erase(cp, 1);
    int i = cp;
    for (; i < buffer.size(); i++)
    {
        WriteChar(buffer[i]);
        cp++;
    }

    WriteChar(' ');
    cp++;
    buffer.erase(buffer.size() - 1, 1);
    for (i = 0; i < size; i++)
    {
        WriteChar(cli_char_backward);
        cp--;
    }
    clen--;
}

inline void CLIReader::ForwardChar ()
{
    if (cp == clen)
        return;
    WriteChar(buffer[cp]);
    cp++;
}

inline void CLIReader::LineBeginning ()
{
    if (cp + current_node->Prompt().size() >= width)
        return;

    while(cp > 0)
        BackwardChar();
}

inline void CLIReader::LineEnd ()
{
    while (cp < clen)
        ForwardChar();
}

inline void CLIReader::KillLine ()
{ 
    int size = clen - cp;
    int i;

    buffer.erase(cp, size);

    if (size == 0)
        return;

    for (i = 0; i < size; i++)
        WriteChar(cli_char_space);
        
    for (i = 0; i < size; i++)
        WriteChar(cli_char_backward);

    buffer.erase(buffer.size()-1, 1);

    clen = cp;
}

inline void CLIReader::DeleteLine ()
{
    if (cp + current_node->Prompt().size() > width)
    {
        InterruptLine();
        return;
    }

    LineBeginning();
    KillLine();

    buffer = "";

    cp = clen = 0;
}
inline void CLIReader::InterruptLine ()
{
    cli_writer->WriteBuffer(cli_newline);
    cli_writer->WriteBuffer(current_node->Prompt());
    buffer = "";
    cp = 0;
    clen = 0;
}

inline void CLIReader::PreviousLine ()
{
    if (hi == 0)
        return;
    hi--;
    if (hi < (int)history.size())
    {
        DeleteLine();
        buffer = history[hi];
        cp = clen = buffer.size();
        cli_writer->WriteBuffer(buffer);
    }
}

inline void CLIReader::NextLine ()
{
    if (hi >= (int)(history.size() -1))
        return;

    hi++;
    DeleteLine();
    buffer = history[hi];
    cp = clen = buffer.size();
    cli_writer->WriteBuffer(buffer);
}

void CLIWriter::WriteBuffer(string &buf)
{ 
    if (fd <= 0) // non-terninal execution
        return;

    buf_queue.push_back(buf); 
    this->SetObsolete(false);
    this->SetRepeats(0);

    if (buf_queue.size() == 1)
        eventMaster.Schedule(this);
}

int CLIWriter::FlushBuffer(string& buf)
{
    if (buf.size() <= 0)
        return 0;
    const char *cstr = buf.c_str();
    assert (fd);
    int ret = write(fd, (void*)cstr, buf.size());
    if (ret != buf.size())
        return -1;
    return 0;
}

void CLIWriter::PointTo (int x)
{
        x += cli_reader->CurrentNode()->Prompt().size();
        string s = cli_newline;
        for (int k = 0; k < x; k++)
            s += cli_char_space;
        s += '^';
        s += cli_newline;
        string &prompt = cli_reader->CurrentNode()->Prompt();
        s += prompt;
        WriteBuffer(s);
}

void CLIWriter::PointTo (int x, vector<string>&v)
{
        x += cli_reader->CurrentNode()->Prompt().size();
        string s0 = cli_newline;
        for (int k = 0; k < x; k++)
            s0 += cli_char_space;
        string s = s0;
        s += '^';
        for (int i = 0; i < v.size(); i++)
        {
            s += s0;
            s += v[i];
        }
        s += cli_newline;
        string &prompt = cli_reader->CurrentNode()->Prompt();
        s += prompt;
        WriteBuffer(s);
}

int CLIWriter::Printf(const char *format, ...)
{
    static char buf[CMD_BUF_SIZE];

    buf[0] = 0;
    va_list ap;
    va_start(ap, format);
    int ret=vsprintf(buf, format, ap);
    string s(buf);
    WriteBuffer(s);
    va_end(ap);
    return ret;
}

void CLIWriter::InitTerminal()
{
    char cmd1[] = { IAC, WILL, TELOPT_ECHO, '\0' };
    char cmd2[] = { IAC, WILL, TELOPT_SGA, '\0' };
    char cmd3[] = { IAC, DONT, TELOPT_LINEMODE, '\0' };
    char cmd4[] = { IAC, DO, TELOPT_NAWS, '\0' };
    string s(cmd1);
    WriteBuffer(s);
    s = cmd2;
    WriteBuffer(s);
    s = cmd3;
    WriteBuffer(s);
    s = cmd4;
    WriteBuffer(s);

    //print out the welcome and copyright messages here
    WriteBuffer(dragon_welcome);

    string& prompt = cli_reader->RootNode()->Prompt();
    WriteBuffer(prompt);
}

void CLIWriter::Run()
{
    Writer::Run();

    //write an API message
    assert (fd > 0);

    if (buf_queue.empty())
        return;

    while (buf_queue.size() > 0)
    {
        string& buf = buf_queue.front();
        int ret = FlushBuffer(buf);
        buf_queue.pop_front();
        //something is wrong with socket write
        if (ret < 0)
        {
			if (fd > 0)
			{
	            if(cli_reader != NULL)
	                cli_reader->Close();
	            Close();
			}
            return;
        }   
    }
}

bool CLINode::dumb = false;

CLINode::CLINode(CLIReader *r, CLIWriter* w): reader(r), writer(w), top(NULL), parent(NULL)
{ 
    cmd_tree = new cmdtree; 
}

CLINode::CLINode(CLINode* p, CLIReader *r, CLIWriter* w): reader(r), writer(w),  parent(p)
{
    assert(p); top = p->Top(); 
    cmd_tree = new cmdtree;
}

CLINode::~CLINode()
{
    assert(cmd_tree);
    delete cmd_tree;

    for (int i = 0; i < children.size(); i++)
        if (children[i])
            delete children[i];
}

void CLINode::ShowPrompt()
{
    if (!dumb)
    {    
        assert(writer);
        writer->WriteBuffer(prompt);
    }
}

void CLINode::AddCommand(CLICommand *cmd) 
{
    assert(cmd);
    cmd->SetNode(this);
    cmd_tree->Insert(cmd);
}

int CLINode::ParseInput(string& input, vector<string>& cmd_v, vector<int>& len_v)
{   
    char buf[CMD_BUF_SIZE];
    strcpy(buf, input.c_str());
    cmd_v.clear();
    len_v.clear();
    char *ps = strtok(buf, " \t");
    if (!ps)
        return 0;
    string s(ps);
    cmd_v.push_back(s);
    len_v.push_back(ps - buf);
    while ((ps = strtok(NULL, " \t")) != NULL)
    {
        s = ps;
        cmd_v.push_back(s);
        len_v.push_back(ps - buf);
    }
    return cmd_v.size();
}

/////////////////////////////////////////////////////////////////////
/////////////////// Definitions of NARB CLI Commands////////////////////
/////////////////////////////////////////////////////////////////////

//Example for external command
//COMMAND_DECLARE(cmd_test6);

//Example for ALIAS
//cmd_test5 cmd_test5_ins2("configure ospfd-peer IP port NUM", "Configure \n OSPF Daemon \n IP");

// Trying connection to protocol daemon.
bool module_connectable (char * host, int port)
{
  int ret;
  struct hostent *hp;
  int sock;
  struct sockaddr_in addr;
  /*struct timeval timeout;*/

  hp = gethostbyname (host);
  if (!hp)
      return false;

  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
      return false;

  memset (&addr, 0, sizeof (struct sockaddr_in));
  addr.sin_family = AF_INET;
  memcpy (&addr.sin_addr, hp->h_addr, hp->h_length);
  addr.sin_port = htons(port);
  ret = connect (sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
  close (sock);
  return (ret == 0);
}

extern ConfigFile configMaster;
extern ZebraOspfSync* zebra_client;
extern DomainTopologyOriginator* dts_originator;

struct string_value_conversion str_val_conv_switching = 
{
	8,
	{{ "psc1", 	LINK_IFSWCAP_SUBTLV_SWCAP_PSC1, 		4},
	{ "psc2", 		LINK_IFSWCAP_SUBTLV_SWCAP_PSC2, 		4}, 
	{ "psc3", 		LINK_IFSWCAP_SUBTLV_SWCAP_PSC3, 		4}, 
	{ "psc4", 		LINK_IFSWCAP_SUBTLV_SWCAP_PSC4, 		4},
	{ "l2sc", 		LINK_IFSWCAP_SUBTLV_SWCAP_L2SC, 		2},
	{ "tdm", 		LINK_IFSWCAP_SUBTLV_SWCAP_TDM, 		1}, 
	{ "lsc", 		LINK_IFSWCAP_SUBTLV_SWCAP_LSC, 		2}, 
	{ "fsc", 		LINK_IFSWCAP_SUBTLV_SWCAP_FSC, 		1}}
};

struct string_value_conversion str_val_conv_encoding = 
{
	8,
	{{ "packet", 	LINK_IFSWCAP_SUBTLV_ENC_PKT, 			2}, 
	{ "ethernet", 	LINK_IFSWCAP_SUBTLV_ENC_ETH, 			1}, 
	{ "pdh", 		LINK_IFSWCAP_SUBTLV_ENC_PDH, 			2}, 
	{ "sdh", 		LINK_IFSWCAP_SUBTLV_ENC_SONETSDH, 		1},
	{ "dwrapper", LINK_IFSWCAP_SUBTLV_ENC_DIGIWRAP, 		1}, 
	{ "lambda", 	LINK_IFSWCAP_SUBTLV_ENC_LAMBDA, 		1}, 
	{ "fiber", 		LINK_IFSWCAP_SUBTLV_ENC_FIBER, 			2}, 
	{ "fchannel", 	LINK_IFSWCAP_SUBTLV_ENC_FIBRCHNL, 		2}}
};

/////////////////////////////////////////////////////////////////////

COMMAND(cmd_exit, "exit", "Exit the current command level\n")
{
    cli_node->Reader()->Exit();
}

//Alias of "exit"
cmd_exit cmd_quit("quit", "Exit the current command level\n");

COMMAND(cmd_configure, "configure", "Enter the configure command level\n")
{
    cli_node->Reader()->GoDown("configure-node");
    cli_node->Reader()->CurrentNode()->ShowPrompt();    
}

COMMAND(cmd_test, "test", "Enter the test command level\n")
{
    cli_node->Reader()->GoDown("test-node");
    cli_node->Reader()->CurrentNode()->ShowPrompt();    
}


COMMAND(cmd_show_topology, "show topology",  "Show configureation: \n Domain summary topology\n")
{
    char addr_buf1[20], addr_buf2[20], addr_buf3[20], addr_buf4[20];
      
    CLI_OUT("\t  \t .......Router ID Opaque LSA.......%s", cli_cstr_newline);
    router_id_info * router_id = NarbDomainInfo.FirstRouterId();
    while (router_id)
    {
        in_addr id, adv_id;
        id.s_addr = router_id->Id();
        adv_id.s_addr = router_id->AdvRtId();
        strcpy(addr_buf1, inet_ntoa(id));
        strcpy(addr_buf2, inet_ntoa(adv_id));
        CLI_OUT("[%s] Opaque ID (%d), Adv_router (%s), Router_id (%s), Rt_type (%d) %s", 
            router_id->hide? "off":"on", router_id->opaque_id,  addr_buf1, addr_buf2, router_id->rt_type, 
            cli_cstr_newline);
    
        router_id = NarbDomainInfo.NextRouterId();
    }

    CLI_OUT("\t  \t .......TE Link Opaque LSA.......%s", cli_cstr_newline);
    link_info * link =  NarbDomainInfo.FirstLink(); 
    while (link)
    {
       in_addr id, adv_id, lcl_if, rmt_if;
        id.s_addr = link->Id();
        adv_id.s_addr = link->AdvRtId();
        lcl_if.s_addr = link->LclIfAddr();
        rmt_if.s_addr = link->RmtIfAddr();
        strcpy(addr_buf1, inet_ntoa(adv_id));
        strcpy(addr_buf2, inet_ntoa(id));
        strcpy(addr_buf3, inet_ntoa(lcl_if));
        strcpy(addr_buf4, inet_ntoa(rmt_if));
        CLI_OUT("[%s] Opaque ID (%d), Adv_router (%s), Router_id (%s), IfAddr[%s-%s] %s", 
            link->hide? "off":"on", link->opaque_id,  addr_buf1, addr_buf2, addr_buf3, addr_buf4, cli_cstr_newline);
    
        link = NarbDomainInfo.NextLink();
    }
    
    CLI_OUT("\t  \t .......The End.......%s %s", cli_cstr_newline, cli_cstr_newline);
    cli_node->ShowPrompt();
}

COMMAND(cmd_show_module, "show module", "Show \n Status of software modules\n")
{
    bool connectable;
    int alive;
    if_narb_info *if_narb;
    
    // print header
    CLI_OUT ("                  **NARB Module Status** %s%s", cli_cstr_newline, cli_cstr_newline);
    CLI_OUT("Module                 IP/Port              Status         Connection%s", cli_cstr_newline);
  
    CLI_OUT("intER-domain OSPFd   %s/%-13d ", NarbDomainInfo.ospfd_inter.addr, NarbDomainInfo.ospfd_inter.port);
    connectable = module_connectable(NarbDomainInfo.ospfd_inter.addr, NarbDomainInfo.ospfd_inter.port);
    if (connectable)
    {
         alive = zebra_client != NULL && zebra_client->GetReader() && zebra_client->GetReader()->Socket() > 0;
         if (alive)
             CLI_OUT("online       connected %s", cli_cstr_newline);
         else
             CLI_OUT("online       disconnected %s", cli_cstr_newline);
    }
    else
    {
      	  CLI_OUT("offline      disconnected %s", cli_cstr_newline);
    }
  
    CLI_OUT("intRA-domain OSPFd   %s/%-13d ", NarbDomainInfo.ospfd_intra.addr, NarbDomainInfo.ospfd_intra.port);
    connectable = module_connectable(NarbDomainInfo.ospfd_intra.addr, NarbDomainInfo.ospfd_intra.port);
    if (connectable)
    {
         alive = false;//ospf_apiclient_alive (oclient_intra);
         if (alive)
             CLI_OUT("online       connected %s", cli_cstr_newline);
         else
             CLI_OUT("online       disconnected %s", cli_cstr_newline);
    }
    else
    {
      	CLI_OUT("offline      disconnected %s", cli_cstr_newline);
    }
  
    if (SystemConfig::rce_pri_host.length() > 0 && SystemConfig::rce_pri_port > 0)
    {
        CLI_OUT("resource comp engine  %s/%-13d ", SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port);
        connectable = module_connectable((char*)SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port);
        if (connectable)
        {
               CLI_OUT("online%s", cli_cstr_newline);
        }
        else
        {
          	CLI_OUT("offline%s", cli_cstr_newline);
        }
    }
  
    for (int i = 0; i < NarbDomainInfo.if_narb_table.size(); i++)
    {
        char ip[20];
        in_addr if_addr;
        if_narb = NarbDomainInfo.if_narb_table[i];
        
        CLI_OUT("Next-domain  NARB    %s/%-13d ", if_narb->addr, if_narb->port);
        connectable = module_connectable(if_narb->addr, if_narb->port);
         if (connectable)
             CLI_OUT("online");
         else
             CLI_OUT("offline");
         
         list<in_addr>::iterator it = if_narb->if_addr_list.begin();
         if (it != if_narb->if_addr_list.end())
         {
             if_addr = *it;
             CLI_OUT("       via %s %s", inet_ntop(AF_INET, &if_addr, ip, 20), cli_cstr_newline);
             it++;
         }
         for (; it != if_narb->if_addr_list.end(); it++)
         {
             if_addr = *it;
             CLI_OUT("\t\t\t\t\t\talso     via %s %s", inet_ntop(AF_INET, &if_addr, ip, 20), cli_cstr_newline);
          }
      }
    cli_node->ShowPrompt();
}

COMMAND(cmd_show_lsp, "show lsp {responded|established|deleted|all}",  "Show configureation:\nLSP\n State of LSP\nComplete ERO returned|LSP established|LSP torn down\n")
{
    u_char state;
    if (argv.size() > 0)
    {
        if (argv[0] == "responded")
            state = STATE_ERO_COMPLETE;
        else if (argv[0] == "established")
            state = STATE_RESV_CONFIRM;
        else if (argv[0] == "deleted")
            state = STATE_RESV_RELEASE_CONFIRM;
        else
            state = 0xff;
    }
    else
        state = STATE_RESV_CONFIRM;

    vector<string> lsp_desc_v;
    LSP_Broker * lspb;
    list<LSP_Broker*>::iterator it;
    for (it = NARB_APIServer::lsp_brokers.begin(); it != NARB_APIServer::lsp_brokers.end(); it++)
    {
        (*it)->DescribeLSPbyState(state, lsp_desc_v);
    }

    CLI_OUT ("                  **NARB LSP Status** %s%s", cli_cstr_newline, cli_cstr_newline);
    if (lsp_desc_v.size() == 0)
        CLI_OUT("There is no current LSP information.%s", cli_cstr_newline);
    else
        CLI_OUT("Src             Dest            Bandwidth SwType EncType (Vtag)   State      GRI%s", cli_cstr_newline);

    for (int i = 0; i < lsp_desc_v.size(); i++)
    {
        CLI_OUT("%s%s", lsp_desc_v[i].c_str(), cli_cstr_newline);
    }
    cli_node->ShowPrompt();
}

//Alias of cmd_show_lsp
cmd_show_lsp cmd_show_lsp_alias1("show lsp", "Show configurations \n Established LSP \n IP");

COMMAND(cmd_show_lsp_detail, "show lsp GRI",  "Show configureation:\nLSP\nGRI in UCID-SEQNUM format\n")
{
    u_int32_t  ucid = 0, seqnum = 0;
    if (sscanf(argv[0].c_str(), "%u-%u", &ucid, &seqnum) != 2)
    {
        CLI_OUT(" #### LSP with the GRI (ucid-seqnum): %s does not exist...%s", argv[0].c_str(), cli_cstr_newline);
        return;
    }
    LSPQ * lspq = NARB_APIServer::LspqLookup(ucid, seqnum);
    string lsp_detail;
    lspq->DescribeLSPDetail(lsp_detail);
    CLI_OUT("%s%s", lsp_detail, cli_cstr_newline);
    cli_node->ShowPrompt();
}


///////////////////////// configure command level  ////////////////////////////

COMMAND(cmd_load_config, "load-config FILE",
       "Load configuration commands from a file\nFile path")
{
    char *cwd;
    ifstream inFile;
    string fullpath ;
    string line;
    vector<string> cmd_v;    
    int i, ret;
    string cli_path("/");

    if (argv[0][0] != '/')
    {
        cwd = getcwd (NULL, MAXPATHLEN);
        fullpath = cwd;;
        fullpath += '/';
        fullpath +=  argv[0];
    }
    else
        fullpath = argv[0];

    inFile.open (fullpath.c_str(), ifstream::in);
    if (!inFile  || inFile.bad()) 
    {
        CLI_OUT("Failed to open the config file: %s%s ", fullpath.c_str(), cli_cstr_newline);
        goto _out;
    }

    while (getline(inFile, line))
    {
        if (line[0] == '!' || line[0] == '#' || line.length() < 3)
            continue;
        
        cmd_v.push_back(line);
    }

    cli_node->Reader()->GoDirect(cli_path);
    for (i = 0; i < cmd_v.size(); i++)
    {
        cli_node->Reader()->CurrentNode()->SetDumb(true);
        ret = CLI_EXECUTE(cmd_v[i], true);
        cli_node->Reader()->CurrentNode()->SetDumb(false);
        CLI_OUT("\t'%s'... %s ", cmd_v[i].c_str(), (ret == 0)? "done": "failed%s");
    }
    CLI_OUT("%s%s", cli_cstr_newline, cli_cstr_newline);
    cli_path = "test-node";
    cli_node->Reader()->GoDirect(cli_path);

_out:
    cli_node->ShowPrompt();
}


COMMAND(cmd_set_topology, "set topology FILE", "Set/Reset configureation: \n Domain summary topology\n Topology config file\n")
{
    assert(cli_node->Reader());
    cli_node->SetDumb(true);
    cli_node->Reader()->Execute("delete topology", true);
    cli_node->SetDumb(false);
    NarbDomainInfo.ClearRouterIds();
    NarbDomainInfo.ClearLinks();
    if (configMaster.ReadConfig((char*)argv[0].c_str(), (char*)NARB_DEFAULT_CONFIG_FILE, (char*)NARB_DEFAULT_CONFIG_FILE, NarbDomainInfo) < 0)
    {
        CLI_OUT("Open configure file '%s' failed...... %s", argv[0].c_str(), cli_cstr_newline);
        goto _out;
    }

    SystemConfig::config_file = argv[0];

    if (zebra_client)
        NarbDomainInfo.OriginateTopology(zebra_client->GetWriter());

_out:
    cli_node->ShowPrompt();
}

COMMAND(cmd_delete_topology, "delete topology", "Delete configureation: \n Domain summary topology\n")
{
    if (zebra_client)
        NarbDomainInfo.DeleteTopology(zebra_client->GetWriter());
    NarbDomainInfo.HideTopology();
    cli_node->ShowPrompt();
}

COMMAND(cmd_undelete_topology, "undelete topology", "Undelete configureation: \n Domain summary topology\n")
{
    assert(cli_node->Reader());

    NarbDomainInfo.ExposeTopology();
    if (zebra_client)
        NarbDomainInfo.OriginateTopology(zebra_client->GetWriter());
    cli_node->ShowPrompt();
}

COMMAND (cmd_set_ospfd, "set ospfd {interdomain|intradomain} HOST  LCL_PORT RMT_PORT ORI_IF AREA",
       "Set/Reset Configuration \n OSPF daemon \n Pick inter-domain instancce |Pick intra-domain instance\n Host of the OSPFd\nThe local sync port on the NARB host\nThe apiserver port on the OSPFd host\nThe interface address on OSPFd through which LSA's are originated\nOSPF area ID")
{
    ospfd_info* ospfd;
    if (argv[0] == "intradomain")
    	ospfd = &NarbDomainInfo.ospfd_intra;
    else
    	ospfd = &NarbDomainInfo.ospfd_inter;
    strcpy(ospfd->addr, argv[1].c_str());
    ospfd->localport = atoi(argv[2].c_str());
    ospfd->port = atoi(argv[3].c_str());
    inet_aton(argv[4].c_str(), &ospfd->ori_if);  
    inet_aton(argv[5].c_str(), &ospfd->area);  
    cli_node->ShowPrompt();
}

COMMAND (cmd_show_ospfd, "show ospfd {interdomain|intradomain}",
       "Show Configuration \n OSPF daemon \n Pick inter-domain instancce |Pick intra-domain instance")
{
    char ip1[20], ip2[20];
    ospfd_info* ospfd;
    if (argv[0] == "intradomain")
    	ospfd = &NarbDomainInfo.ospfd_intra;
    else
    	ospfd = &NarbDomainInfo.ospfd_inter;
    CLI_OUT("Host: %s, Port: %d, (local_port: %d), Origination_interface: %s, Area_id: %s%s",
        ospfd->addr, ospfd->port, ospfd->localport, inet_ntop(AF_INET, &ospfd->ori_if, ip1, 20),
        inet_ntop(AF_INET, &ospfd->area, ip2, 20), cli_cstr_newline);
    cli_node->ShowPrompt();
}

COMMAND(cmd_connect_ospfd, "connect ospfd {interdomain | intradomain}", "(Re)Connect to an OSPFd\nContinue...\n")
{
    int ret;
    
    if (argv[0] == "intradomain")
    {
       //void
    }      
    else
    {
        if (zebra_client && zebra_client->GetReader() && zebra_client->GetReader()->Socket() >0)
        {
            CLI_OUT("IntRA-domain OSPFd apiclient has already existed.%s Please disconnect before reconnecting.%s", cli_cstr_newline, cli_cstr_newline);
            goto _out;
        }
  
        if (!zebra_client)
            zebra_client = new ZebraOspfSync(NarbDomainInfo.ospfd_inter.addr, NarbDomainInfo.ospfd_inter.port, DOMAIN_MASK_GLOBAL, 0);
        if (zebra_client->RunWithoutSyncLsdb() < 0)
        {
            CLI_OUT("communication error with OSPFd %s:%d....\n", NarbDomainInfo.ospfd_inter.addr, NarbDomainInfo.ospfd_inter.port);
            delete zebra_client;
            zebra_client = NULL;
            goto _out;
        }
        ZebraOspfWriter* ospf_apiwriter = zebra_client->GetWriter();
        if(!ospf_apiwriter)
        {
            CLI_OUT("cannot create ZebraOspfWriter for OSPFd %s:%d....\n", NarbDomainInfo.ospfd_inter.addr, NarbDomainInfo.ospfd_inter.port);
            delete zebra_client;
            zebra_client = NULL;
            goto _out;
        }
    
        // Register type (10, 1) Opaque-TE LSA.
        ret = ospf_apiwriter->RegisterOpqaueType(10, 1);
        if (ret < 0)
        {
            CLI_OUT("RegisterOpqaueType[10, 1] failed ! %s", cli_cstr_newline);
        }
        //Start abstract domain topology origination    
        NarbDomainInfo.OriginateTopology(ospf_apiwriter);

        if(dts_originator)
        {
            eventMaster.Remove(dts_originator);
        }
        else
            dts_originator = new DomainTopologyOriginator(SystemConfig::topology_refresh_interval);
        dts_originator->SetRepeats(FOREVER);
        dts_originator->SetAutoDelete(true);
        dts_originator->SetOspfClient(zebra_client);
        eventMaster.Schedule(dts_originator);
    }

_out:
    cli_node->ShowPrompt();
}

COMMAND (cmd_delete_ospfd, "delete ospfd {interdomain | intradomain}",
       "Delete and disconnect an OSPFd\nContinue...\n")
{
    if (argv[0] == "interdomain")
    {
        cli_node->Reader()->Execute("delete topology", true);
        if (zebra_client && zebra_client->GetReader() && zebra_client->GetReader()->Socket() >0)
        {
            zebra_client->GetWriter()->Close();
            zebra_client->GetWriter()->SetSocket(-1);
            zebra_client->GetReader()->Close();
            zebra_client->GetReader()->SetSocket(-1);
        }
    }
    cli_node->ShowPrompt();
}

COMMAND(cmd_set_peer_narb, "set peer-narb HOST port PORT via IF_ADDR",
       "Set/Reset configuration\n Configure peering NARB\n Host of the peering NARB\nThe port on the NARB host\nThe interface that the NARB is associaed with\n")
{
    in_addr if_addr;
    inet_aton(argv[2].c_str(), &if_addr);
    NarbDomainInfo.AddPeerNarb((char*)argv[0].c_str(), atoi(argv[1].c_str()), if_addr);
    cli_node->ShowPrompt();
}

COMMAND (cmd_set_routing_mode, "set routing-mode {all-strict-only|mixed-allowed|mixed-preferred|mixed-confirmed|all-loose-allowed}",
       "Set/Reset configuration\nPick a NARB routing mode.\n")
{
    if(argv[0] == "all-strict-only")
        SystemConfig::routing_mode= RT_MODE_ALL_STRICT_ONLY;
    else if(argv[0] =="mixed-allowed")
        SystemConfig::routing_mode= RT_MODE_MIXED_ALLOWED;
    else if(argv[0] == "mixed-preferred")
        SystemConfig::routing_mode= RT_MODE_MIXED_PREFERRED;
    else if(argv[0] == "mixed-confirmed")
        SystemConfig::routing_mode= RT_MODE_MIXED_CONFIRMED;
    else if(argv[0] == "all-loose-allowed")
      SystemConfig::routing_mode= RT_MODE_ALL_LOOSE_ALLOWED;
    CLI_OUT("Routing mode changed succesfully.%sThe new mode will be effective for new requests.%sHowever, it does not apply to requests in progress. %s", cli_cstr_newline, cli_cstr_newline, cli_cstr_newline);
    cli_node->ShowPrompt();
}

COMMAND (cmd_show_routing_mode, "show routing-mode",
       "Show configuration\nNARB routing mode.\n")
{
    string routing_mode;
    switch(SystemConfig::routing_mode)
    {
    case RT_MODE_ALL_STRICT_ONLY:
        routing_mode = "all-strict-only";
        break;
    case RT_MODE_MIXED_ALLOWED:
        routing_mode = "mixed-allowed";
        break;
    case RT_MODE_MIXED_PREFERRED:
        routing_mode = "mixed-preferred";
        break;
    case RT_MODE_MIXED_CONFIRMED:
        routing_mode = "mixed-confirmed";
        break;
    case RT_MODE_ALL_LOOSE_ALLOWED:
        routing_mode = "all-loose-allowed";
        break;
    default:
        routing_mode = "unknow";
    }
    CLI_OUT("%s\tCurrent routing mode: %s%s", cli_cstr_newline, routing_mode.c_str(), cli_cstr_newline);
    cli_node->ShowPrompt();
}

COMMAND (cmd_set_working_mode, "set working-mode {static-interdomain-topology|dynamic-interdomain-topology}",
       "Set/Reset configurartion\nPick aNARB working mode\n")
{
    if(argv[0] == "static-interdomain-topology")
        SystemConfig::working_mode = WORKING_MODE_STATIC_INTERDOMAIN_TOPOLOGY;
    else if (argv[0] == "dynamic-interdomain-topology")
        SystemConfig::working_mode = WORKING_MODE_DYNAMIC_INTERDOMAIN_TOPOLOGY;
    cli_node->ShowPrompt();
}

COMMAND (cmd_show_working_mode, "show working-mode",
       "Show configuration\nNARB working mode.\n")
{
    string working_mode;
    switch (SystemConfig::working_mode)
    {
    case WORKING_MODE_STATIC_INTERDOMAIN_TOPOLOGY:
        working_mode = "static-interdomain-topology";
        break;
    case WORKING_MODE_DYNAMIC_INTERDOMAIN_TOPOLOGY:
        working_mode = "dynamic-interdomain-topology";
        break;
    default:
        working_mode = "unknow";
    }
    CLI_OUT("%s\tCurrent working mode: %s%s", cli_cstr_newline, working_mode.c_str(), cli_cstr_newline);
    cli_node->ShowPrompt();
}


COMMAND (cmd_set_forced_merge, "set forced-merge {on|off}",
       "Set/Reset configurartion\nforced merging inter- and intra-domain ERO in recursive path computation\nOn or Off\n")
{
    if(argv[0] == "on")
        SystemConfig::forced_merge = true;
    else 
        SystemConfig::forced_merge = false;
    cli_node->ShowPrompt();
}

COMMAND(cmd_configure_exit, "exit", "Exit the current command level\n")
{
    cli_node->Reader()->GoUp();
    cli_node->Reader()->CurrentNode()->ShowPrompt();
}

COMMAND(cmd_set_rce, "set rce HOST port PORT",
    "Set/Reset configurartion\nResource computation engine (RCE)\nHost of the RCE\nPort on the RCE\n")
{
    SystemConfig::rce_pri_host = argv[0];
    SystemConfig::rce_pri_port = atoi(argv[1].c_str());
    cli_node->ShowPrompt();
}

COMMAND(cmd_delete_rce, "delete rce",
    "Delete configurartion\nResource Computation engine (RCE)\n")
{
    SystemConfig::rce_pri_host = "";
    SystemConfig::rce_pri_port = -1;
    cli_node->ShowPrompt();
}

COMMAND(cmd_show_rce, "show rce",
    "Show configurartion...\nResource Computation Engine (RCE)\n")
{
    if (SystemConfig::rce_pri_host.length() == 0 || SystemConfig::rce_pri_port <= 0)
    {
        CLI_OUT("RCE not configured. %s", cli_cstr_newline);
    }
    else
    {
        if (module_connectable((char*)SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port))
        {
            CLI_OUT("RCE '%s:%d' is online.%s", SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port, cli_cstr_newline);
        }
        else
        {
            CLI_OUT("RCE '%s:%d' is offline.%s", SystemConfig::rce_pri_host.c_str(), SystemConfig::rce_pri_port, cli_cstr_newline);
        }
    }
    cli_node->ShowPrompt();
}

COMMAND (cmd_set_topology_refresh_interval, "set refresh-interval SECONDS",
        "Set/Reset configureation...\nDomain topology origination refresh interval\n")
{
    int interval = atoi(argv[0].c_str());
  
    if (interval < 60 || interval > 3500)
    {
        CLI_OUT("The  interval value must be between 60 and 3500 (seconds) %s", cli_cstr_newline);
        goto _out;
    }
    SystemConfig::topology_refresh_interval = interval;
  
    if (zebra_client&&zebra_client->GetWriter())
    {
        // cancel the old refresh thread and refresh now
        NarbDomainInfo.DeleteTopology(zebra_client->GetWriter());
        NarbDomainInfo.OriginateTopology(zebra_client->GetWriter());
        if(dts_originator)
        {
            dts_originator->SetInterval(interval, 0);
        }      
    }
_out:
    cli_node->ShowPrompt();
}

static link_info* link_to_update = NULL;

#define SHOW_LINK(L)   in_addr ip; int i; \
    char addr_buf1[20], addr_buf2[20], addr_buf3[20], addr_buf4[3]; \
    ip.s_addr = L->Id(); \
    strcpy (addr_buf1, inet_ntoa (ip)); \
    ip.s_addr = L->AdvRtId();    \
    strcpy (addr_buf2, inet_ntoa (ip)); \
    ip.s_addr = L->LclIfAddr();  \
    strcpy (addr_buf3, inet_ntoa (ip)); \
    ip.s_addr = L->RmtIfAddr();  \
    strcpy (addr_buf4, inet_ntoa (ip)); \
    CLI_OUT("%s\t ##LINK Area-Local Opaque-Type/ID %d##%s\t Advertising Router: %s%s\t Link ID: %s %s", \
        cli_cstr_newline, L->opaque_id, cli_cstr_newline, addr_buf2,cli_cstr_newline, addr_buf1, cli_cstr_newline);   \
    CLI_OUT("\t Local Interface %s %s\t Remote Interface %s%s", \
                  addr_buf3, cli_cstr_newline, addr_buf4, cli_cstr_newline);    \
    CLI_OUT("\t Traffic Engineering Metric: %d%s", L->Metric(), cli_cstr_newline);   \
    CLI_OUT ("\t Maximum Bandwidth: %g (Mbps)%s", L->MaxBandwidth(), cli_cstr_newline);  \
    CLI_OUT ("\t Maximum Reservable Bandwidth: %g (Mbps)%s", L->MaxReservableBandwidth(), cli_cstr_newline);    \
    CLI_OUT("\t Interface Switching Capability Descriptor: %s %s%s",    \
            value_to_string(&str_val_conv_switching, (u_int32_t)L->GetISCD()->swtype),   \
            value_to_string(&str_val_conv_encoding, (u_int32_t)L->GetISCD()->encoding), cli_cstr_newline);   \
    for (i = 0; i < 8; i++) \
    {   \
        CLI_OUT ("\t Max LSP Bandwidth (pri %d): %g (Mbps)%s", i, L->GetISCD()->max_lsp_bw[i], cli_cstr_newline);    \
    }   \
    if (LINK_PARA_FLAG(L->info_flag, LINK_PARA_FLAG_VLAN) !=0) \
    {   \
	    CLI_OUT ("\t -- L2SC specific information--%s\t    --> Available VLAN tag set:", cli_cstr_newline); \
	    for (i = 1; i <= MAX_VLAN_NUM; i++) \
		if (L->IsVtagSet(i)) CLI_OUT (" %d", i); \
	    CLI_OUT("%s", cli_cstr_newline);    \
	    CLI_OUT ("\t   --> Allocated VLAN tag set:");   \
	    for (i = 1; i <= MAX_VLAN_NUM; i++) \
		if (L->IsVtagAllocated(i)) CLI_OUT (" %d", i);   \
	    CLI_OUT("%s", cli_cstr_newline);    \
    }   


#define SHOW_ERO_SUBOBJECTS(P)       char ip_addr[20], if_id[20]; \
    list<ero_subobj*>::iterator it = P->ero.begin(); \
    for (int num=1; it != P->ero.end(); num++, it++) \
    { \
        strcpy(ip_addr, inet_ntoa((*it)->addr)); \
        if ((*it)->if_id == 0) \
        { \
            strcpy(if_id, "IPv4"); \
        } \
        else \
        { \
            sprintf(if_id, "Unum ifID=0x%x", ntohl((*it)->if_id)); \
        } \
        CLI_OUT("    >>%-2d: (%s-hop) -- %s -- %s%s", num, (*it)->hop_type == 0 ? "strict" : "loose", ip_addr, if_id, cli_cstr_newline); \
    }


COMMAND (cmd_show_link, "show link local_if_addr LCL_IF_ADDR remote_if_addr RMT_IF_ADDR",
    "Show \n TE link \nLocal interface address\nIP\nRemote interface address\nIP")
{
    in_addr lcl_if, rmt_if;
    link_info * link;
  
    inet_aton(argv[0].c_str(), &lcl_if);
    inet_aton(argv[1].c_str(), &rmt_if);
  
    link = NarbDomainInfo.LookupLinkByLclRmtIf(lcl_if, rmt_if);
    if (!link)
    {
        CLI_OUT("Unknown TE link [%s-%s] in the current domain!%s", argv[0].c_str(), argv[1].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    SHOW_LINK(link);

    cli_node->Reader()->CurrentNode()->ShowPrompt();
}

COMMAND (cmd_add_link, "add link adv_router IP link_id IP lcl_if_addr IP remote_if_addr IP",
    "Add \n TE link \nAdvertising rotuter\nIP\nLink ID\nIP\nLocal interface address\nIP\nRemote interface address\nIP")
{
    in_addr adv_id, link_id, lcl_if, rmt_if;
  
    inet_aton(argv[0].c_str(), &adv_id);
    inet_aton(argv[1].c_str(), &link_id);
    inet_aton(argv[2].c_str(), &lcl_if);
    inet_aton(argv[3].c_str(), &rmt_if);
  
    if (!NarbDomainInfo.LookupRouterById(adv_id))
    {
        CLI_OUT("Unknown advertising router '%s' in the current domain!%s", argv[0].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    link_info * link = NarbDomainInfo.LookupLinkByLclRmtIf(lcl_if, rmt_if);
    if(link)
    {
        CLI_OUT("TE Link [%s-%s] already exists in the current domain!%s", argv[2].c_str(), argv[3].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    link = new link_info(DOMAIN_MASK_GLOBAL, adv_id, link_id);
    link->opaque_id = narb_ospf_opaque_id();
    link->SetLclIfAddr(lcl_if.s_addr);
    link->SetRmtIfAddr(rmt_if.s_addr);
    link->hide = true;
    NarbDomainInfo.AddLink(link);

    link_to_update = new link_info(*link);
    cli_node->Reader()->GoDown("update-link-node");
    string prompt = cli_node->Prompt(); 
    prompt += '(';
    prompt += argv[2];
    prompt +='-';
    prompt += argv[3];
    prompt +=')';
    cli_node->Reader()->CurrentNode()->SetPrompt(prompt);
    cli_node->Reader()->CurrentNode()->ShowPrompt();
}

COMMAND (cmd_delete_link, "delete link local_if_addr LCL_IF_ADDR remote_if_addr RMT_IF_ADDR",
    "Delete \n TE link \nLocal interface address\nIP\nRemote interface address\nIP")
{
    in_addr lcl_if, rmt_if;
    link_info * link;
  
    inet_aton(argv[0].c_str(), &lcl_if);
    inet_aton(argv[1].c_str(), &rmt_if);
  
    link = NarbDomainInfo.LookupLinkByLclRmtIf(lcl_if, rmt_if);
    if (!link)
    {
        CLI_OUT("Unknown TE link [%s-%s] in the current domain!%s", argv[0].c_str(), argv[1].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    in_addr adv_id;
    adv_id.s_addr = link->AdvRtId();
    if (!link->hide)
    {
        zebra_client->GetWriter()->DeleteLsa(adv_id, NarbDomainInfo.ospfd_inter.area, 10, 1, link->opaque_id);
        link->hide = true;
    }

    CLI_OUT("\t Sending TE link deleteion request to the intER-domain OSPFd!%s", cli_cstr_newline);

    cli_node->Reader()->CurrentNode()->ShowPrompt();
}

COMMAND (cmd_edit_link, "edit link local_if_addr LCL_IF_ADDR remote_if_addr RMT_IF_ADDR",
    "Edit/Update \n TE link \nLocal interface address\nIP\nRemote interface address\nIP")
{
    in_addr lcl_if, rmt_if;
    link_info * link;
  
    inet_aton(argv[0].c_str(), &lcl_if);
    inet_aton(argv[1].c_str(), &rmt_if);
  
    link = NarbDomainInfo.LookupLinkByLclRmtIf(lcl_if, rmt_if);
    if (!link)
    {
        CLI_OUT("Unknown TE link [%s-%s] in the current domain!%s", argv[0].c_str(), argv[1].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }
  
    if (link)
    {
        link_to_update = new link_info;
        *link_to_update = *link;
        link_to_update->opaque_id = narb_ospf_opaque_id();
        cli_node->Reader()->GoDown("edit-link-node");
        string prompt = cli_node->Prompt(); 
        prompt += '(';
        prompt += argv[0];
        prompt +='-';
        prompt += argv[1];
        prompt +=')';
        cli_node->Reader()->CurrentNode()->SetPrompt(prompt);
    }
    cli_node->Reader()->CurrentNode()->ShowPrompt();
}

COMMAND (cmd_set_link, "set {link_id | adv_router |metric | lcl_if |rmt_if} VALUE",
       "Set configuration...\n")
{
    assert (link_to_update);
    in_addr ip;
    if (argv[0] == "link_id")
    {
        
        inet_aton(argv[1].c_str(), &ip);
        link_to_update->SetId(ip.s_addr);
    }
    else if (argv[0] == "adv_router")
    {
        inet_aton(argv[1].c_str(), &ip);
        link_to_update->SetAdvRtId(ip.s_addr);
    }
    else if (argv[0] == "metric")
    {
        link_to_update->SetMetric(atoi(argv[1].c_str()));
        SET_LINK_PARA_FLAG(link_to_update->info_flag, LINK_PARA_FLAG_METRIC);
    }
    else if (argv[0] == "lcl_if")
    {
        inet_aton(argv[1].c_str(), &ip);
        link_to_update->SetLclIfAddr(ip.s_addr);
        SET_LINK_PARA_FLAG(link_to_update->info_flag, LINK_PARA_FLAG_LOC_IF);
    }
    else if (argv[0] == "rmt_if")
    {
        inet_aton(argv[1].c_str(), &ip);
        link_to_update->SetRmtIfAddr(ip.s_addr);
        SET_LINK_PARA_FLAG(link_to_update->info_flag, LINK_PARA_FLAG_REM_IF);
    }
    cli_node->ShowPrompt();
}

COMMAND (cmd_set_link_swcap,"set swcap {lsc|tdm|psc1|psc2|psc3|psc4} encoding {lambda|ethernet|packet|sdh}",
       "Set configuration \n link interface switching capability\nPick a switching type\nEncoding")
{
    assert (link_to_update);
  
    link_to_update->GetISCD()->swtype= string_to_value(&str_val_conv_switching, argv[0].c_str());
    link_to_update->GetISCD()->encoding = string_to_value(&str_val_conv_encoding, argv[1].c_str());
    
    SET_LINK_PARA_FLAG(link_to_update->info_flag, LINK_PARA_FLAG_IFSW_CAP);
    cli_node->ShowPrompt();
}

COMMAND (cmd_set_link_bandwidth,"set bandwidth FLOAT",
       "Set configuration \nBandwidth\nFloat in mbps")
{
    assert (link_to_update);
    float bw;
    sscanf(argv[0].c_str(), "%f", &bw);
    //htonf_mbps(bw);
    if (LINK_PARA_FLAG(link_to_update->info_flag, LINK_PARA_FLAG_IFSW_CAP) ==0 || link_to_update->GetISCD() == NULL)
    {
        CLI_OUT("Cannot update bandwidth: no switch_cap_if descriptor for this link!%s", cli_cstr_newline);
    }
    else 
    {
        for (int i = 0; i < 8; i++)
        {
            link_to_update->UnreservedBandwidth()[i] = bw;
            link_to_update->GetISCD()->max_lsp_bw[i] = bw;
        }
    }
    cli_node->ShowPrompt();
}

COMMAND (cmd_set_link_vlan,"set vlan {add|delete} VLAN",
       "Set configuration \nVLAN\nAdd|Delete\nVLAN tag")
{
    assert (link_to_update);
    int vlan;
    bool adding = (strncmp(argv[0].c_str(), "add", 1) == 0 ? true : false);
    sscanf(argv[1].c_str(), "%d", &vlan);
    if (LINK_PARA_FLAG(link_to_update->info_flag, LINK_PARA_FLAG_VLAN) ==0 || link_to_update->GetISCD() == NULL) 
    {
        CLI_OUT("Cannot update VLAN for this link!%s", cli_cstr_newline);
    }
    else
    {
        if (adding)
        {
            link_to_update->SetVtag(vlan);
            link_to_update->DeallocateVtag(vlan);
        }
        else
        {
            link_to_update->ResetVtag(vlan);
        }
    }
    cli_node->ShowPrompt();
}

COMMAND (cmd_set_link_vlan_range,"set vlan-range {add|delete} VLAN1 to VLAN2",
       "Set configuration \nVLAN\nAdd|Delete\nVLAN tag - range begin\nVLAN tag range end")
{
    assert (link_to_update);
    int vlan, vlan1, vlan2;
    bool adding = (strncmp(argv[0].c_str(), "add", 1) == 0 ? true : false);

    sscanf(argv[1].c_str(), "%d", &vlan1);
    sscanf(argv[2].c_str(), "%d", &vlan2);
    if (LINK_PARA_FLAG(link_to_update->info_flag, LINK_PARA_FLAG_VLAN) ==0 || link_to_update->GetISCD() == NULL) 
    {
        CLI_OUT("Cannot update VLAN for this link!%s", cli_cstr_newline);
    }
    else
    {
        for (vlan = vlan1; vlan <= vlan2; vlan++)
        {
            if (adding)
            {
                link_to_update->SetVtag(vlan);
                link_to_update->DeallocateVtag(vlan);
            }
            else
            {
                link_to_update->ResetVtag(vlan);
            }
        }
    }
    cli_node->ShowPrompt();
}

COMMAND(cmd_edit_link_show, "show {updated|original}",
       "Show editing\n Updated | Original TE parameters")
{
    assert (link_to_update);
    link_info * link = NULL;

    if (argv[0] == "updated")
    {
        link = link_to_update;
    }
    else
    {
        in_addr ip1, ip2;
        ip1.s_addr = link_to_update->LclIfAddr();
        ip2.s_addr = link_to_update->RmtIfAddr();
        link = NarbDomainInfo.LookupLinkByLclRmtIf(ip1, ip2);
        assert (link);
    }

    SHOW_LINK(link);

    cli_node->ShowPrompt();
}
  
COMMAND(cmd_edit_link_cancel, "cancel",
       "Cancel edited TE link parameters\n")
{
    if (link_to_update)
    {
        delete link_to_update;
        link_to_update = NULL;
    }
    cli_node->Reader()->GoUp();
    CLI_OUT("\t Cancelled the currently edited link... %s", cli_cstr_newline);

    cli_node->Reader()->CurrentNode()->ShowPrompt();
}

//Alias of "cancel"
cmd_edit_link_cancel cmd_edit_link_exit_instance("exit", "Exit the current command level (cancel)\n");

COMMAND(cmd_edit_link_commit, "commit",
       "Commit edited TE link parameters\n")
{
    link_info * link;

    assert (link_to_update);

    if (!zebra_client || !zebra_client->GetWriter() || zebra_client->GetWriter()->Socket() <= 0)
    {
      CLI_OUT("IntER-domain OSPFd connection is dead now.%s", cli_cstr_newline);
      cli_node->ShowPrompt();
      return;
    }

    CLI_OUT("\t Sending update to intER-domain OSPFd...%s", cli_cstr_newline);
    in_addr ip1, ip2;
    ip1.s_addr = link_to_update->LclIfAddr();
    ip2.s_addr = link_to_update->RmtIfAddr();
    link = NarbDomainInfo.LookupLinkByLclRmtIf(ip1, ip2);
    assert (link);
    link_to_update->hide = false;

    in_addr adv_id;
    adv_id.s_addr = link->AdvRtId();
    zebra_client->GetWriter()->DeleteLsa(adv_id, NarbDomainInfo.ospfd_inter.area, 10, 1, link->opaque_id);

    NarbDomainInfo.OriginateTeLink (zebra_client->GetWriter(), link_to_update);
    *link = *link_to_update;
    delete link_to_update;
    link_to_update = NULL;

    cli_node->Reader()->GoUp();  
    cli_node->Reader()->CurrentNode()->ShowPrompt();
}

COMMAND(cmd_set_intradomain_topology, "set intradomain topology FILE",
       "Set/originate intradomain topology\n cont...\ncont...\nTopology config file")
{
    CLI_OUT("*******Obsolete command******%sThis command is not supported in this version of NARB any more...%s", cli_cstr_newline, cli_cstr_newline);
    cli_node->ShowPrompt();
}

COMMAND(cmd_delete_intradomain_topology, "delete intradomain topology",
       "Delete intradomain topology\n cont...\ncont...\nTopology config file")
{
    CLI_OUT("*******Obsolete command******%sThis command is not supported in this version of NARB... more%s", cli_cstr_newline, cli_cstr_newline);
    cli_node->ShowPrompt();
}

COMMAND(cmd_set_static_ero, "set static_ero SRCDEST {enabled|disabled}",
       "Set manual ERO\n cont...\nSource-destination IPs\nEnable \nDisable")
{
    char str[40], *pstr;
    strcpy(str, argv[0].c_str());
    in_addr src, dest;
    indexed_ero* p_ero = NULL;
    if ((pstr=strstr(str, "-")) != NULL)
    {
        *pstr = 0;
        pstr++;
        inet_aton(str, &src);
        inet_aton(pstr, &dest);
        p_ero = SystemConfig::LookupStaticERO(src.s_addr, dest.s_addr);
    }

    CLI_OUT(" ### static ERO configuration for src-dest: %s", argv[0].c_str());
    if (p_ero == NULL)
    {
        CLI_OUT(" unvailable...%s",  cli_cstr_newline);
    }
    else if (argv[1] == "enabled") {
        p_ero->enabled = true;
        CLI_OUT(" enabled...%s",  cli_cstr_newline);
    }
    else {
        p_ero->enabled = false;
        CLI_OUT(" disabled...%s",  cli_cstr_newline);
    }
    cli_node->ShowPrompt();
}

COMMAND(cmd_show_static_ero, "show static_ero SRCDEST",
       "Show manual ERO configuration\n cont...\nERO Source-destination IPs")
{
    char str[40], *pstr;
    in_addr src, dest;
    if (argv.size() > 0)
        strcpy(str, argv[0].c_str());

    if (argv.size() == 0 || argv[0] == "all" || argv[0] == "ALL")
    {
        IndexedEROList::iterator it = SystemConfig::indexed_static_ero_list.begin();
        CLI_OUT(" #### Showing a total of %d ERO's >>%s", SystemConfig::indexed_static_ero_list.size(), cli_cstr_newline);
        for (; it != SystemConfig::indexed_static_ero_list.end(); it++)
        {
            pstr = str+20;
            src.s_addr = (*it)->src_ip;
            dest.s_addr = (*it)->dest_ip;
            strcpy(str, inet_ntoa(src));
            strcpy(pstr, inet_ntoa(dest));
            CLI_OUT(" ## ERO %s-%s with %d hops: status '%s'%s", str, pstr, (*it)->ero.size(), (*it)->enabled ? "enabled" : "disabled", cli_cstr_newline);
        }
    }
    else if ((pstr=strstr(str, "-")) != NULL)
    {
        *pstr = 0;
        pstr++;
        inet_aton(str, &src);
        inet_aton(pstr, &dest);
        indexed_ero* p_ero = SystemConfig::LookupStaticERO(src.s_addr, dest.s_addr);
        if (p_ero == NULL)
        {
            CLI_OUT(" ### static ERO configuration for %s-%s unvailable...%s", str, pstr,  cli_cstr_newline);
        }
        else
        {
            pstr = str+20;
            src.s_addr = p_ero->src_ip;
            dest.s_addr = p_ero->dest_ip;
            strcpy(str, inet_ntoa(src));
            strcpy(pstr, inet_ntoa(dest));
            CLI_OUT(" ## ERO %s-%s with %d hops, status '%s', showing subobjects >>%s", str, pstr, p_ero->ero.size(), p_ero->enabled ? "enabled" : "disabled", cli_cstr_newline);

            SHOW_ERO_SUBOBJECTS(p_ero);
        }
    }
    else
    {
        CLI_OUT("  %s: unrecognized 'Src-Dest IP pair' formatted index for ERO configuration retrieval%s", str,  cli_cstr_newline);        
    }
    
    cli_node->ShowPrompt();
}

//Alias
cmd_show_static_ero cmd_show_static_ero_default("show static_ero", "\nShow\nStatic/Manual ERO\ncont...");

COMMAND(cmd_delete_static_ero, "delete static_ero SRCDEST",
       "Delete manual ERO configuration\n cont...\nSource-destination IPs")
{
    char str[40], *pstr;
    strcpy(str, argv[0].c_str());
    in_addr src, dest;
    indexed_ero* p_ero = NULL;
    if ((pstr=strstr(str, "-")) != NULL)
    {
        *pstr = 0;
        pstr++;
        inet_aton(str, &src);
        inet_aton(pstr, &dest);
        p_ero = SystemConfig::RemoveStaticERO(src.s_addr, dest.s_addr);
    }

    if (p_ero == NULL)
    {
        CLI_OUT(" #### static ERO configuration for src-dest: %s unvailable...%s", argv[0].c_str(), cli_cstr_newline);
    }
    else {
        CLI_OUT(" #### static ERO configuration for src-dest: %s removed...%s", argv[0].c_str(), cli_cstr_newline);
        delete p_ero;
    }
    cli_node->ShowPrompt();
}

// edit_static_ero
static indexed_ero* current_static_ero = NULL;

COMMAND(cmd_edit_static_ero, "edit static_ero SRCDEST",
       "Edit manual/static ERO \n cont ... \nSource-Destination IPs")
{
    char str[40], *pstr;
    strcpy(str, argv[0].c_str());
    in_addr src, dest;
    current_static_ero = NULL;
    if ((pstr=strstr(str, "-")) != NULL)
    {
        *pstr = 0;
        pstr++;
        inet_aton(str, &src);
        inet_aton(pstr, &dest);
        current_static_ero = SystemConfig::LookupStaticERO(src.s_addr, dest.s_addr);
    }

    if (current_static_ero == NULL)
    {
        current_static_ero = new indexed_ero;
        current_static_ero->src_ip = src.s_addr;
        current_static_ero->dest_ip = dest.s_addr;
        SystemConfig::indexed_static_ero_list.push_back(current_static_ero);
    }
    
    cli_node->Reader()->GoDown("edit-static-ero");
    string prompt = cli_node->Prompt(); 
    prompt += '(';
    prompt += argv[0];
    prompt +=')';
    cli_node->Reader()->CurrentNode()->SetPrompt(prompt);
    cli_node->Reader()->CurrentNode()->ShowPrompt();
}


COMMAND(cmd_ero_show, "show config",
       "Show subobjects \n cont ... ")
{
    CLI_OUT(" ## showing %d ERO (status '%s') subobjects (cursor=%d) >> %s",  current_static_ero->ero.size(), 
        current_static_ero->enabled ? "enabled" : "disabled", current_static_ero->cursor, cli_cstr_newline);
    SHOW_ERO_SUBOBJECTS(current_static_ero);
    cli_node->ShowPrompt();
}

COMMAND(cmd_ero_clear, "clear",
       "Clear all subobjects")
{
    list<ero_subobj*>::iterator it = current_static_ero->ero.begin();
    for (; it != current_static_ero->ero.end(); it++)
        delete (*it);
    current_static_ero->ero.clear();
    CLI_OUT(" ## number of ERO (status '%s') subobjects = 0 %s",  current_static_ero->enabled ? "enabled" : "disabled", cli_cstr_newline);
    cli_node->ShowPrompt();
}

COMMAND(cmd_ero_enable, "enable",
       "Enable this ERO")
{
    current_static_ero->enabled = true;
    cli_node->ShowPrompt();
}

COMMAND(cmd_ero_disable, "disable",
       "Disable this ERO")
{
    current_static_ero->enabled = false;
    cli_node->ShowPrompt();
}

COMMAND(cmd_ero_insert, "insert {before|after} NUM",
       "Insert subobject \n before \n after \n the referred subobject number")
{
    bool insert_before = (argv[0] == "before" ? true : false);
    int cursor = 0;
    sscanf(argv[1].c_str(), "%d", &cursor);

    if (cursor > 0 && cursor <= current_static_ero->ero.size())
    {
        CLI_OUT(" >>> new subobject to be added %s the current suboject #%d <<< %s", insert_before ? "Before" : "After", cursor, cli_cstr_newline);
        current_static_ero->cursor = cursor - (insert_before ? 1 : 0);
    }
    else
    {
        CLI_OUT(" >>> Wrong cursor number %s <<< %s", argv[1].c_str(), cli_cstr_newline);
    }
    CLI_OUT(" ## Current ERO with %d subobjects >> %s", current_static_ero->ero.size(), cli_cstr_newline);
    SHOW_ERO_SUBOBJECTS(current_static_ero);
    cli_node->ShowPrompt();
}

COMMAND(cmd_ero_append, "append",
       "Append subobjects")
{
    current_static_ero->cursor = -1;
    CLI_OUT(" ## Current ERO with %d subobjects >> %s", current_static_ero->ero.size(), cli_cstr_newline);
    cli_node->ShowPrompt();
}


COMMAND(cmd_ero_delete, "delete subobject NUM",
       "Delete subobject \n cont... \nthe referred subobject number")
{
    int cursor = -1;
    sscanf(argv[0].c_str(), "%d", &cursor);

    if (cursor > 0 && cursor <= current_static_ero->ero.size())
    {
        list<ero_subobj*>::iterator it = current_static_ero->ero.begin();
        for (int num = 1; it != current_static_ero->ero.end(); it++, num++)
        {
            if (num == cursor)
            {
                current_static_ero->ero.erase(it);
                CLI_OUT(" >>> the current suboject #%d has been removed <<< %s", cursor, cli_cstr_newline);
                cli_node->ShowPrompt();
                return;
            }
        }
    }
    CLI_OUT(" >>> the suboject #%s does not exist <<< %s", argv[0].c_str(), cli_cstr_newline);
    cli_node->ShowPrompt();
}

static void insert_subobject_with_cursor(indexed_ero* p_ero, ero_subobj* subobj)
{
    assert(p_ero);
    if (p_ero->cursor < 0 || p_ero->cursor >= p_ero->ero.size())
    {
        p_ero->ero.push_back(subobj);
        return;
    }

    list<ero_subobj*>::iterator it = p_ero->ero.begin();
    for (int num = 0; it != p_ero->ero.end(); it++, num++)
    {
        if (num == p_ero->cursor)
        {
            p_ero->ero.insert(it, subobj);
            p_ero->cursor++;
            return;
        }
    }    
}
    
COMMAND(cmd_set_subobject_ipv4, "set subobject {strict|loose} ipv4 IP",
       "Set/Add subobject \ncont... \n Strict hop | Loose hop \n type IPv4 \n IP address")
{
    bool is_loose = (argv[0] == "loose" ? true : false);
    in_addr ipv4; ipv4.s_addr = 0;
    inet_aton(argv[1].c_str(), &ipv4);
    if (ipv4.s_addr == 0)
    {
        CLI_OUT(" ## Wrong  IPv4 IP address: %s %s", argv[1].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    ero_subobj* subobj = new ero_subobj;
    memset(subobj, 0, sizeof(ero_subobj));
    subobj->hop_type = (is_loose? 1 : 0);
    subobj->addr = ipv4;

    insert_subobject_with_cursor(current_static_ero, subobj);
    cli_node->ShowPrompt();
}


COMMAND(cmd_set_subobject_unum_ifid, "set subobject {strict|loose} unnumbered interface-ipv4 IP id ID",
       "Set/Add subobject \ncont...\n Strict hop | Loose hop \n type Unnumbered Interface ID \n cont ... \n IP \n cont ... \n ID")
{
    bool is_loose = (argv[0] == "loose" ? true : false);
    in_addr ipv4; ipv4.s_addr = 0;
    inet_aton(argv[1].c_str(), &ipv4);
    if (ipv4.s_addr == 0)
    {
        CLI_OUT(" ## Wrong  interface (IPv4) address: %s %s", argv[1].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }
    u_int32_t id = 0;
    sscanf(argv[2].c_str(), "%d", &id);
    if (id == 0)
    {
        CLI_OUT(" ## Wrong  interface ID (unsigned number): %s %s", argv[2].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    ero_subobj* subobj = new ero_subobj;
    memset(subobj, 0, sizeof(ero_subobj));
    subobj->hop_type = (is_loose? 1 : 0);
    subobj->addr = ipv4;
    subobj->if_id = htonl(id);
    insert_subobject_with_cursor(current_static_ero, subobj);
    cli_node->ShowPrompt();
}

COMMAND(cmd_set_subobject_vlan_te, "set subobject {strict|loose} vlan-te interface-ipv4 IP vtag ID",
       "Set/Add subobject \ncont...\n Strict hop | Loose hop \n type VLAN TE \n Interface IPv4 Address ... \n IP \n VLAN Tag ... \n ID")
{
    bool is_loose = (argv[0] == "loose" ? true : false);
    in_addr ipv4; ipv4.s_addr = 0;
    inet_aton(argv[1].c_str(), &ipv4);
    if (ipv4.s_addr == 0)
    {
        CLI_OUT(" ## Wrong  interface (IPv4) address: %s %s", argv[1].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }
    u_int32_t vtag = 0;
    sscanf(argv[2].c_str(), "%d", &vtag);
    if (vtag == 0 || vtag >= MAX_VLAN_NUM)
    {
        CLI_OUT(" ## Wrong  VTAG Number : %s is not in [1, 4095]%s", argv[2].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    ero_subobj* subobj = new ero_subobj;
    memset(subobj, 0, sizeof(ero_subobj));
    subobj->hop_type = (is_loose? 1 : 0);
    subobj->addr = ipv4;
    subobj->if_id = htonl((LOCAL_ID_TYPE_TAGGED_GROUP_GLOBAL<<16) | (vtag&0xffff));
    subobj->sw_type = LINK_IFSWCAP_SUBTLV_SWCAP_L2SC;
    subobj->encoding = LINK_IFSWCAP_SUBTLV_ENC_ETH;
    subobj->l2sc_vlantag = (u_int16_t)vtag;
    insert_subobject_with_cursor(current_static_ero, subobj);
    cli_node->ShowPrompt();
}

COMMAND(cmd_set_subobject_subobj_if, "set subobject {strict|loose} subnet {source|destination} interface-ipv4 IP id ID first-timeslot TS",
       "Set/Add subobject \ncont...\n Strict hop|Loose hop \n type Subnet Interface ... \n Source|Destination \n Interface IPv4 \n IP \n Sunbet ID Number \n ID \n First Availalbe Timeslot \n Time Slot number")
{
    bool is_loose = (argv[0] == "loose" ? true : false);
    u_int32_t subnet_if_type = (argv[1] == "source" ? 0x10 : 0x11);
    in_addr ipv4; ipv4.s_addr = 0;
    inet_aton(argv[2].c_str(), &ipv4);
    if (ipv4.s_addr == 0)
    {
        CLI_OUT(" ## Wrong  interface (IPv4) address: %s %s", argv[2].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }
    u_int32_t id = 0, ts = 0; 
    sscanf(argv[3].c_str(), "%d", &id);
    if (id == 0 || id > 0xff)
    {
        CLI_OUT(" ## Wrong Subnet ID number : %s is not in [1, 255]%s", argv[3].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    if (argv[4] == "any" || argv[4] == "ANY")
    {
        ts = 0xff;
    }
    else 
    {
        sscanf(argv[4].c_str(), "%d", &ts);
        if (ts == 0 || ts > 192)
        {
            CLI_OUT(" ## Wrong  First Availalable Timeslot number : %s is not in [1, 192]%s", argv[4].c_str(), cli_cstr_newline);
            cli_node->ShowPrompt();
            return;
        }
    }

    ero_subobj* subobj = new ero_subobj;
    memset(subobj, 0, sizeof(ero_subobj));
    subobj->hop_type = (is_loose? 1 : 0);
    subobj->addr = ipv4;
    subobj->if_id = htonl((subnet_if_type<<16) | ((id&0xff)<<8) | (ts&0xff));
    insert_subobject_with_cursor(current_static_ero, subobj);
    cli_node->ShowPrompt();
}

COMMAND(cmd_set_subobject_lcl_id, "set subobject {strict|loose} local-id {port|group|tagged-group|subnet-interface} loopback-ipv4 IP id ID",
       "Set/Add subobject \ncont...\n Strict hop|Loose hop  \n cont...\nPORT|UNTAGGED-GROUP|TAGGED-GROUP|SUBNET-INTERFACE\nInterface IPv4 Address ... \n IP \n LocalID value... \n ID value")
{
    bool is_loose = (argv[0] == "loose" ? true : false);

    u_int32_t lcl_id_type = 0;
    if (argv[1] == "port")
        lcl_id_type = LOCAL_ID_TYPE_PORT;
    else if (argv[1] == "group")    
        lcl_id_type = LOCAL_ID_TYPE_GROUP;
    else if (argv[1] == "tagged-group")    
        lcl_id_type = LOCAL_ID_TYPE_TAGGED_GROUP;
    else if (argv[1] == "subnet-interface")    
        lcl_id_type = LOCAL_ID_TYPE_SUBNET_INTERFACE;
    in_addr ipv4; ipv4.s_addr = 0;
    inet_aton(argv[2].c_str(), &ipv4);
    if (ipv4.s_addr == 0)
    {
        CLI_OUT(" ## Wrong  interface (IPv4) address: %s %s", argv[2].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }
    u_int32_t lcl_id = 0;
    if (sscanf(argv[3].c_str(), "%d", &lcl_id) != 1)
        sscanf(argv[3].c_str(), "%x", &lcl_id);
    if (lcl_id == 0 || lcl_id> 0xffff)
    {
        CLI_OUT(" ## Wrong LocalID Value : %s is not in [1, 0xffff]%s", argv[3].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    ero_subobj* subobj = new ero_subobj;
    memset(subobj, 0, sizeof(ero_subobj));
    subobj->hop_type = (is_loose? 1 : 0);
    subobj->addr = ipv4;
    subobj->if_id = htonl((lcl_id_type<<16) | (lcl_id&0xffff));
    insert_subobject_with_cursor(current_static_ero, subobj);
    cli_node->ShowPrompt();
}


COMMAND(cmd_ero_exit, "exit",
       "Exit static ERO editing\n")
{
    current_static_ero = NULL;
    cli_node->Reader()->GoUp();
    cli_node->Reader()->CurrentNode()->ShowPrompt();
}


/////////////////////////////////////////////////////////////
void CLIReader::InitSession()
{
    cli_root = new CLINode(this, cli_writer);
    CLINode* node, *node_test, *node_level2;
    string s;
    if (SystemConfig::cli_password.size() > 0)
    {
        s = "password:";
        assert(cli_writer);
        cli_writer->SetIsPasswd(true);
    }
    else
    {    
        s = "narb:cli>";
        assert(cli_writer);
        cli_writer->SetIsPasswd(false);
    }
    cli_root->SetPrompt(s);
    s = "root";
    cli_root->SetTag(s);

    //Start timer
    eventMaster.Schedule(&timer);

    //////// Node and Cmd Structures ///////
    // Root level
    cli_root->AddCommand(&cmd_exit_instance);
    cli_root->AddCommand(&cmd_quit);
    cli_root->AddCommand(&cmd_configure_instance);
    cli_root->AddCommand(&cmd_test_instance);
    cli_root->AddCommand(&cmd_show_topology_instance);
    cli_root->AddCommand(&cmd_show_ospfd_instance);
    cli_root->AddCommand(&cmd_show_module_instance);
    cli_root->AddCommand(&cmd_show_lsp_instance);
    cli_root->AddCommand(&cmd_show_lsp_alias1);
    cli_root->AddCommand(&cmd_show_lsp_detail_instance);
    cli_root->AddCommand(&cmd_show_link_instance);
    cli_root->AddCommand(&cmd_show_static_ero_instance);
    cli_root->AddCommand(&cmd_show_static_ero_default);
    node_test = cli_root->MakeChild("test-node");
    node = cli_root->MakeChild("configure-node");
    //Test level
    node_test->SetPrompt("narb:cli#(test)");
    node_test->AddCommand(&cmd_load_config_instance);
    node_test->AddCommand(&cmd_set_intradomain_topology_instance);
    node_test->AddCommand(&cmd_delete_intradomain_topology_instance);
    node_test->AddCommand(&cmd_configure_exit_instance);
    //Configure level
    node->SetPrompt("narb:cli#");
    node->AddCommand(&cmd_show_topology_instance);
    node->AddCommand(&cmd_set_topology_instance);
    node->AddCommand(&cmd_delete_topology_instance);
    node->AddCommand(&cmd_undelete_topology_instance);
    node->AddCommand(&cmd_set_ospfd_instance);
    node->AddCommand(&cmd_connect_ospfd_instance);
    node->AddCommand(&cmd_delete_ospfd_instance);
    node->AddCommand(&cmd_set_peer_narb_instance);
    node->AddCommand(&cmd_set_routing_mode_instance);
    node->AddCommand(&cmd_show_routing_mode_instance);
    node->AddCommand(&cmd_set_working_mode_instance);
    node->AddCommand(&cmd_show_working_mode_instance);
    node->AddCommand(&cmd_set_forced_merge_instance);
    node->AddCommand(&cmd_set_rce_instance);
    node->AddCommand(&cmd_delete_rce_instance);
    node->AddCommand(&cmd_show_rce_instance);
    node->AddCommand(&cmd_set_topology_refresh_interval_instance);
    node->AddCommand(&cmd_show_link_instance);
    node->AddCommand(&cmd_add_link_instance);
    node->AddCommand(&cmd_delete_link_instance);
    node->AddCommand(&cmd_edit_link_instance);
    node->AddCommand(&cmd_configure_exit_instance);
    node->AddCommand(&cmd_set_static_ero_instance);
    node->AddCommand(&cmd_show_static_ero_instance);
    node->AddCommand(&cmd_show_static_ero_default);
    node->AddCommand(&cmd_delete_static_ero_instance);
    node->AddCommand(&cmd_edit_static_ero_instance);
    node_level2 = node->MakeChild("edit-link-node");
    ///////Edit/Update-link level under the cofigure level/////
    //node_level2->SetPrompt("narb:cli#");
    node_level2->AddCommand(&cmd_set_link_instance);
    node_level2->AddCommand(&cmd_set_link_swcap_instance);
    node_level2->AddCommand(&cmd_set_link_bandwidth_instance);
    node_level2->AddCommand(&cmd_set_link_vlan_instance);
    node_level2->AddCommand(&cmd_set_link_vlan_range_instance);
    node_level2->AddCommand(&cmd_edit_link_show_instance);
    node_level2->AddCommand(&cmd_edit_link_cancel_instance);
    node_level2->AddCommand(&cmd_edit_link_exit_instance);
    node_level2->AddCommand(&cmd_edit_link_commit_instance);
    //////////////// End /////////////////
    node_level2 = node->MakeChild("edit-static-ero");
    ///////Edit-Static-ERO level under the cofigure level/////
    node_level2->AddCommand(&cmd_ero_show_instance);
    node_level2->AddCommand(&cmd_ero_enable_instance);
    node_level2->AddCommand(&cmd_ero_disable_instance);
    node_level2->AddCommand(&cmd_ero_clear_instance);
    node_level2->AddCommand(&cmd_ero_insert_instance);
    node_level2->AddCommand(&cmd_ero_append_instance);
    node_level2->AddCommand(&cmd_ero_delete_instance);
    node_level2->AddCommand(&cmd_set_subobject_ipv4_instance);
    node_level2->AddCommand(&cmd_set_subobject_unum_ifid_instance);
    node_level2->AddCommand(&cmd_set_subobject_vlan_te_instance);
    node_level2->AddCommand(&cmd_set_subobject_subobj_if_instance);
    node_level2->AddCommand(&cmd_set_subobject_lcl_id_instance);
    node_level2->AddCommand(&cmd_ero_exit_instance);
    //////////////// End /////////////////

    current_node = cli_root;
}


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

#include "rce_cli.hh"
#include "rce_log.hh"
#include "rce_config.hh"
#include "zebra_ospfclient.hh"
#include "resource.hh"
#include <arpa/telnet.h>
#include <stdarg.h>

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
	    Close();
	    cli_writer->Close();
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
            Close();
            cli_writer->Close();
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
    	if (fd > 0)
   		{
            LOGF("CLI connection on socket (%d) closed\n", fd);
            Close();
            cli_writer->Close();
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
        s = "rce:cli>";
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

    //if (first_run)
    //{
    //    InitTerminal();
    //    first_run = false;
    //}
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
                Close();
                assert(cli_reader);
                cli_reader->Close();
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

extern ZebraOspfSync *zebra_client_inter;
extern ZebraOspfSync *zebra_client_intra;

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


COMMAND(cmd_exit, "exit", "Exit the current command level\n")
{
    cli_node->Reader()->Exit();
}

//Alias of "exit"
cmd_exit cmd_quit("quit", "Exit the current command level\n");

COMMAND(cmd_show_module, "show module", "Show \n Status of software modules\n")
{
    bool connectable;
    int alive;
    
    // print header
    CLI_OUT ("                  **RCE Module Status** %s%s", cli_cstr_newline, cli_cstr_newline);
    CLI_OUT("Module                 IP/Port              Status         Connection%s", cli_cstr_newline);
  
    CLI_OUT("intER-domain OSPFd   %s/%-13d ", SystemConfig::ospfd_inter_host.c_str(), SystemConfig::ospfd_inter_port);
    connectable = module_connectable((char*)SystemConfig::ospfd_inter_host.c_str(), SystemConfig::ospfd_inter_port);
    if (connectable)
    {
         alive = zebra_client_inter != NULL && zebra_client_inter->GetReader() && zebra_client_inter->GetReader()->Socket() > 0;
         if (alive)
             CLI_OUT("online       connected %s", cli_cstr_newline);
         else
             CLI_OUT("online       disconnected %s", cli_cstr_newline);
    }
    else
    {
      	  CLI_OUT("offline      disconnected %s", cli_cstr_newline);
    }
  
    CLI_OUT("intRA-domain OSPFd   %s/%-13d ", SystemConfig::ospfd_intra_host.c_str(), SystemConfig::ospfd_intra_port);
    connectable = module_connectable((char*)SystemConfig::ospfd_intra_host.c_str(), SystemConfig::ospfd_intra_port);
    if (connectable)
    {
         alive = zebra_client_intra != NULL && zebra_client_intra->GetReader() && zebra_client_intra->GetReader()->Socket() > 0;
         if (alive)
             CLI_OUT("online       connected %s", cli_cstr_newline);
         else
             CLI_OUT("online       disconnected %s", cli_cstr_newline);
    }
    else
    {
      	CLI_OUT("offline      disconnected %s", cli_cstr_newline);
    }
  
    if (SystemConfig::narb_host.length() > 0 && SystemConfig::narb_port > 0)
    {
        CLI_OUT("NARB                 %s/%-13d ", SystemConfig::narb_host.c_str(), SystemConfig::narb_port);
        connectable = module_connectable((char*)SystemConfig::narb_host.c_str(), SystemConfig::narb_port);
        if (connectable)
        {
               CLI_OUT("online%s", cli_cstr_newline);
        }
        else
        {
          	CLI_OUT("offline%s", cli_cstr_newline);
        }
    }
  
    cli_node->ShowPrompt();
}


COMMAND(cmd_show_topology, "show topology {interdomain | intradomain}",  "Show configureation: \n Domain summary topology\n")
{
    char addr_buf1[20], addr_buf2[20], addr_buf3[20], addr_buf4[20];

    ResourceType rt_type = RTYPE_GLO_RTID, link_type = RTYPE_GLO_ABS_LNK;

    if (argv[0] == "intradomain")
    {
        rt_type = RTYPE_LOC_RTID;
        link_type = RTYPE_LOC_PHY_LNK;
    }
        
    CLI_OUT("\t  \t .......Router ID Opaque LSA.......%s", cli_cstr_newline);
    RadixTree<Resource> *tree = RDB.Tree(rt_type);
    RadixNode<Resource> * node = tree->Root();
    if (node && !node->Data())
        node = tree->NextNodeHavingData(node);
    while (node)
    {
        RouterId * router_id = (RouterId *)node->Data();
        in_addr id, adv_id;
        id.s_addr = router_id->Id();
        adv_id.s_addr = router_id->AdvRtId();
        strcpy(addr_buf1, inet_ntoa(id));
        strcpy(addr_buf2, inet_ntoa(adv_id));
        CLI_OUT("Adv_router (%s), Router_id (%s)%s", addr_buf1, addr_buf2, cli_cstr_newline);
    
        node = tree->NextNodeHavingData(node);
    }

    tree = RDB.Tree(link_type);
    CLI_OUT("\t  \t .......TE Link Opaque LSA.......%s", cli_cstr_newline);
    node = tree->Root();
    if (node && !node->Data())
        node = tree->NextNodeHavingData(node);
    while (node)
    {
        Link * link = (Link *)node->Data();
        in_addr id, adv_id, lcl_if, rmt_if;
        id.s_addr = link->Id();
        adv_id.s_addr = link->AdvRtId();
        lcl_if.s_addr = link->LclIfAddr();
        rmt_if.s_addr = link->RmtIfAddr();
        strcpy(addr_buf1, inet_ntoa(adv_id));
        strcpy(addr_buf2, inet_ntoa(id));
        strcpy(addr_buf3, inet_ntoa(lcl_if));
        strcpy(addr_buf4, inet_ntoa(rmt_if));
        CLI_OUT("Adv_router (%s), Link_id (%s), IfAddrs[%s-%s] %s", 
            addr_buf1, addr_buf2, addr_buf3, addr_buf4, cli_cstr_newline);
    
        node = tree->NextNodeHavingData(node);
    }
    
    CLI_OUT("\t  \t .......The End.......%s %s", cli_cstr_newline, cli_cstr_newline);
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

    //cli_node->Reader()->GoDirect(cli_path);
    for (i = 0; i < cmd_v.size(); i++)
    {
        cli_node->Reader()->CurrentNode()->SetDumb(true);
        ret = CLI_EXECUTE(cmd_v[i], true);
        cli_node->Reader()->CurrentNode()->SetDumb(false);
        CLI_OUT("\t'%s'... %s ", cmd_v[i].c_str(), (ret == 0)? "done": "failed%s");
    }
    CLI_OUT("%s%s", cli_cstr_newline, cli_cstr_newline);

    cli_path = "/";
    cli_node->Reader()->GoDirect(cli_path);

_out:
    cli_node->ShowPrompt();
}

COMMAND(cmd_configure, "configure", "Enter the configure command level\n")
{
    cli_node->Reader()->GoDown("configure-node");
    cli_node->Reader()->CurrentNode()->ShowPrompt();    
}

COMMAND(cmd_configure_exit, "exit", "Exit the current command level\n")
{
    cli_node->Reader()->GoUp();
    cli_node->Reader()->CurrentNode()->ShowPrompt();
}

COMMAND (cmd_set_ospfd, "set ospfd {interdomain|intradomain} HOST  LCL_PORT RMT_PORT ORI_IF AREA",
       "Set/Reset Configuration \n OSPF daemon \n Pick inter-domain instancce |Pick intra-domain instance\n Host of the OSPFd\nThe local sync port on the NARB host\nThe apiserver port on the OSPFd host\nThe interface address on OSPFd through which LSA's are originated\nOSPF area ID")
{
    string* ospfd_host;
    int* ospfd_port, *ospfd_port_local;
    if (argv[0] == "intradomain")
    {
        ospfd_host = &SystemConfig::ospfd_intra_host;
        ospfd_port = &SystemConfig::ospfd_intra_port;
        ospfd_port_local = &SystemConfig::ospfd_intra_port_local;
    }
    else
    {
        ospfd_host = &SystemConfig::ospfd_inter_host;
        ospfd_port = &SystemConfig::ospfd_inter_port;
        ospfd_port_local = &SystemConfig::ospfd_inter_port_local;
    }

    *ospfd_host = argv[1];
    *ospfd_port_local = atoi(argv[2].c_str());
    *ospfd_port = atoi(argv[3].c_str());
    cli_node->ShowPrompt();
}

COMMAND (cmd_show_link, "show link {interdomain|intradomain} local_if_addr LCL_IF_ADDR remote_if_addr RMT_IF_ADDR",
    "Show \n TE link \nLocal interface address\nIP\nRemote interface address\nIP")
{
    in_addr lcl_if, rmt_if;
    Link * link;
  
    inet_aton(argv[1].c_str(), &lcl_if);
    inet_aton(argv[2].c_str(), &rmt_if);
    
    ResourceType rcType = (strncmp(argv[0].c_str(), "interdomain", 5) == 0 ? RTYPE_GLO_ABS_LNK : RTYPE_LOC_PHY_LNK);
    link = RDB.LookupLinkByLclRmtIf(rcType, lcl_if, rmt_if);
    if (!link)
    {
        CLI_OUT("Unknown TE link [%s-%s] in the current domain!%s", argv[1].c_str(), argv[2].c_str(), cli_cstr_newline);
        cli_node->ShowPrompt();
        return;
    }

    //general link TE parameters
    in_addr ip; 
    int i, k; 
    char addr_buf1[20], addr_buf2[20], addr_buf3[20], addr_buf4[3];
    list<ISCD*>::iterator iter;

_show_more_links:

    ip.s_addr = link->Id();
    strcpy (addr_buf1, inet_ntoa (ip));
    ip.s_addr = link->AdvRtId();
    strcpy (addr_buf2, inet_ntoa (ip));
    ip.s_addr = link->LclIfAddr();
    strcpy (addr_buf3, inet_ntoa (ip));
    ip.s_addr = link->RmtIfAddr();
    strcpy (addr_buf4, inet_ntoa (ip));
    CLI_OUT("%s\t ## TE LINK - %s  - Modified @ %d.%d ## %s\t Advertising Router: %s%s\t Link ID: %s%s", cli_cstr_newline,
        rcType == RTYPE_GLO_ABS_LNK? "Abstract Global Link":"Physical Local Link",  link->ModifiedTime().tv_sec, 
        link->ModifiedTime().tv_usec, cli_cstr_newline, addr_buf2, cli_cstr_newline, addr_buf1, cli_cstr_newline); 
    CLI_OUT("\t Local Interface %s %s\t Remote Interface %s%s",
                  addr_buf3, cli_cstr_newline, addr_buf4, cli_cstr_newline);
    CLI_OUT("\t Traffic Engineering Metric: %d%s", link->Metric(), cli_cstr_newline);
    CLI_OUT ("\t Maximum Bandwidth: %g (Mbps)%s", link->MaxBandwidth(), cli_cstr_newline);
    CLI_OUT ("\t Maximum Reservable Bandwidth: %g (Mbps)%s", link->MaxReservableBandwidth(), cli_cstr_newline);

    //ISCDs
    for (k=1, iter = link->Iscds().begin(); iter != link->Iscds().end(); k++, iter++)
    {
        CLI_OUT("\t Interface Switching Capability Descriptor #%d: %s %s%s", k,
                value_to_string(&str_val_conv_switching, (u_int32_t)(*iter)->swtype),
                value_to_string(&str_val_conv_encoding, (u_int32_t)(*iter)->encoding), cli_cstr_newline); 
        for (i = 0; i < 8; i++)
        {
            CLI_OUT ("\t    Max LSP Bandwidth (pri %d): %g (Mbps)%s", i, (*iter)->max_lsp_bw[i], cli_cstr_newline);
        }
        if ((*iter)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC && (ntohs((*iter)->vlan_info.version) & IFSWCAP_SPECIFIC_VLAN_BASIC))
        {
            CLI_OUT ("\t    -- L2SC specific information--%s\t       --> Available VLAN tag set:", cli_cstr_newline);
            for (i = 1; i <= MAX_VLAN_NUM; i++)
                if (HAS_VLAN((*iter)->vlan_info.bitmask, i)) CLI_OUT (" %d", i);
            CLI_OUT("%s", cli_cstr_newline); 
            CLI_OUT ("\t      --> Allocated VLAN tag set:");
            for (i = 1; i <= MAX_VLAN_NUM; i++)
                if (HAS_VLAN((*iter)->vlan_info.bitmask_alloc, i)) CLI_OUT (" %d", i);
            CLI_OUT("%s", cli_cstr_newline);
        }
        if (((*iter)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC || (*iter)->swtype == LINK_IFSWCAP_SUBTLV_SWCAP_TDM) 
            && (ntohs((*iter)->subnet_uni_info.version) & IFSWCAP_SPECIFIC_SUBNET_UNI))
        {
            CLI_OUT ("\t    -- Subnet UNI specific information--%s\t       --> Available time slots:", cli_cstr_newline);
            for (i = 1; i <= MAX_TIMESLOTS_NUM; i++)
                if (HAS_TIMESLOT((*iter)->subnet_uni_info.timeslot_bitmask, i)) CLI_OUT (" %d", i);
            CLI_OUT("%s", cli_cstr_newline);
        }
    }
    //Link state deltas
    list<LinkStateDelta*>* pDeltaList = link->DeltaListPointer();
    if (pDeltaList)
    {
        list<LinkStateDelta*>::iterator it;
        LinkStateDelta* delta;
        for (k = 1, it = pDeltaList->begin(); it != pDeltaList->end(); k++, it++)
        {
            delta = *it;
            CLI_OUT ("%s\t >>> Link State Delta [%d] - Status: %s @ %d.%d<<<%s", cli_cstr_newline, k, 
                (delta->expiration.tv_sec == SystemConfig::delta_expire_query) ? "Queried" : "Reserved", 
                delta->create_time.tv_sec, delta->create_time.tv_usec, cli_cstr_newline);
            CLI_OUT ("\t    ---> Used Bandwidth: %g (Mbps)%s", delta->bandwidth, cli_cstr_newline);
            if (delta->flags & DELTA_VLANTAG)
            {
                CLI_OUT ("\t    ---> Used VLAN tag: %d%s", delta->vlan_tag, cli_cstr_newline);
            }
            else if (delta->flags & DELTA_WAVELENGTH)
            {
                CLI_OUT ("\t    ---> Used wavelength: %d%s", delta->wavelength, cli_cstr_newline);
         }
            else if (delta->flags & DELTA_VTAGMASK)
            {
                CLI_OUT ("\t    ---> Used VLAN tags:");
                for (i = 1; i <= MAX_VLAN_NUM; i++)
                {
                    if (HAS_VLAN(delta->vtag_mask, i)) 
                    {
                        CLI_OUT (" %d", i);
                    }
                }
                CLI_OUT("%s", cli_cstr_newline);
         }         
            else if (delta->flags & DELTA_TIMESLOTS)
            {
               CLI_OUT ("\t    ---> Used time slots:");
                for (i = 1; i <= MAX_TIMESLOTS_NUM; i++)
                {
                    if (HAS_TIMESLOT(delta->timeslots, i)) 
                    {
                        CLI_OUT (" %d", i);
                    }
                }
                CLI_OUT("%s", cli_cstr_newline);
            }
        }
    }

    if ((link = RDB.LookupNextLinkByLclRmtIf(link)) != NULL)
        goto _show_more_links;

    cli_node->Reader()->CurrentNode()->ShowPrompt();
}


COMMAND(cmd_connect_ospfd, "connect ospfd {interdomain | intradomain}", "(Re)Connect to an OSPFd\nContinue...\n")
{
    int ret;

    string* ospfd_host;
    int* ospfd_port, *ospfd_port_local;
    ZebraOspfSync * zebra_client;
    u_int32_t domain_mask;
    if (argv[0] == "intradomain")
    {
        ospfd_host = &SystemConfig::ospfd_intra_host;
        ospfd_port = &SystemConfig::ospfd_intra_port;
        ospfd_port_local = &SystemConfig::ospfd_intra_port_local;
        zebra_client = zebra_client_intra;
        domain_mask = DOMAIN_MASK_LOCAL;
    }
    else
    {
        ospfd_host = &SystemConfig::ospfd_inter_host;
        ospfd_port = &SystemConfig::ospfd_inter_port;
        ospfd_port_local = &SystemConfig::ospfd_inter_port_local;
        zebra_client = zebra_client_inter;
        domain_mask = DOMAIN_MASK_GLOBAL;
    }

    if (zebra_client && zebra_client->GetReader() && zebra_client->GetReader()->Socket() >0)
    {
        CLI_OUT("IntRA-domain OSPFd apiclient has already existed.%s Please disconnect before reconnecting.%s", cli_cstr_newline, cli_cstr_newline);
        goto _out;
    }

    if (!zebra_client)
        zebra_client = new ZebraOspfSync((char*)ospfd_host->c_str(), *ospfd_port, domain_mask, SystemConfig::ospf_sync_interval);

    eventMaster.Remove(zebra_client);
    eventMaster.Schedule(zebra_client);

_out:
    cli_node->ShowPrompt();
}

/////////////////////////////////////////////////////////////////////
//COMMAND_DECLARE(cmd_test6);
//cmd_test5 cmd_test5_ins2("configure ospfd-peer IP port NUM", "Configure \n OSPF Daemon \n IP");
/////////////////////////////////////////////////////////////////////
void CLIReader::InitSession()
{
    cli_root = new CLINode(this, cli_writer);
    CLINode* node;
    string s;
    if (SystemConfig::cli_password.size() > 0)
    {
        s = "password:";
        assert(cli_writer);
        cli_writer->SetIsPasswd(true);
    }
    else
    {    
        s = "rce:cli>";
        assert(cli_writer);
        cli_writer->SetIsPasswd(false);
    }
    cli_root->SetPrompt(s);
    s = "root";
    cli_root->SetTag(s);

    //Start timer
    eventMaster.Schedule(&timer);

    //////// Node and Cmd Structures ///////
    //Root level
    cli_root->AddCommand(&cmd_exit_instance);
    cli_root->AddCommand(&cmd_quit);
    cli_root->AddCommand(&cmd_show_module_instance);
    cli_root->AddCommand(&cmd_show_topology_instance);
    cli_root->AddCommand(&cmd_show_link_instance);
    cli_root->AddCommand(&cmd_load_config_instance);
    cli_root->AddCommand(&cmd_configure_instance);
    //Configure level
    node = cli_root->MakeChild("configure-node");
    node->SetPrompt("rce:cli#");
    node->AddCommand(&cmd_configure_exit_instance);
    node->AddCommand(&cmd_configure_exit_instance);
    node->AddCommand(&cmd_set_ospfd_instance);
    node->AddCommand(&cmd_connect_ospfd_instance);
    //////////////// End /////////////////
    current_node = cli_root;
}


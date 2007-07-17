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

#ifndef __CMDTREE_H__
#define __CMDTREE_H__

#include "clicmd.hh"
using namespace std;
#include <vector>
#include <string>

enum cmdnode_type
{
    _ROOT, _CMD, _ARG_S, _ARG_M
};

class CLICommand;
class cmdnode
{
private:
    enum cmdnode_type type;
    cmdnode* top;
    cmdnode* parent;
    vector<cmdnode*> children;
    string tag;
    string help;
    CLICommand* command;
public:
    cmdnode(cmdnode_type t): type(t), top(NULL), parent(NULL), command(NULL) {}
    cmdnode(cmdnode* p, cmdnode_type t): type(t), parent(p), command(NULL) {assert(p); top = p->Top();}
    ~cmdnode ();
    cmdnode_type Type() {return type;}
    void SetType(enum cmdnode_type t) {type = t;}
    cmdnode* Top() {return top;}
    cmdnode* SetTop(cmdnode *t) {top = t;}
    cmdnode* Parent() {return parent;}
    cmdnode* SetParent(cmdnode *p) {parent = p; top = p->Top();}
    string& Tag() {  return tag;  }
    void SetTag(string& val) { tag = val; }
    string& Help() {  return help;  }
    void SetHelp(string& val) { help = val; }
    vector<cmdnode*>& Children(){  return children;  }
    cmdnode* MakeChild(cmdnode_type t)
        {cmdnode *child = new cmdnode(this, t); children.push_back(child); return child;}
    void AddChild(cmdnode *child) 
        { assert(child); children.push_back(child);}
    CLICommand* Command() {return command;}
    void SetCommand(CLICommand *cmd) { assert (cmd); command = cmd; }
};

class cmdtree
{
private:
    cmdnode *root;
public:
    cmdtree() {root = new cmdnode(_ROOT);}
    ~cmdtree() {delete root;}

    int Insert(CLICommand* cmd);
    
   //0: no match; 1: successful; >1: ambiguous;
   //node ==NULL: no finding; node->Command() == NULL, incomplete.
    int Search(vector<string>& iv, cmdnode* &node, vector<string>& av);

    //compare return and iv.size to detimine whether the input is partial or fully correct
    //alway complete the trimed strings
    int Complete(vector<string>& iv, vector<string>& ov); 
};

#endif

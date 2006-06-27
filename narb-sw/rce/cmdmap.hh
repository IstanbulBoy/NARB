/*
 * Copyright (c) 2005
 * DRAGON Project.
 * University of Southern California/Information Sciences Institute.
 * All rights reserved.
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

#ifndef __CMDMAP_H__
#define __CMDMAP_H__

#include "rce_clicmd.hh"
using namespace usd;
#include <map>
#include <string>

struct unmatched_or_less_than
{
  bool operator() (const string &a, const string &b)
  {
    if (b.find(a, 0) != string::npos)
       return false;
    return (a < b);
  }
};

class cmdmap: public map<string, CLICommand*, unmatched_or_less_than>
{
public:
    map<string, CLICommand*, unmatched_or_less_than>::iterator match_first(const char *str)
    {
       return map<string, CLICommand*, unmatched_or_less_than>::find(str);
    }

    map<string, CLICommand*, unmatched_or_less_than>::iterator match_next(map<string, CLICommand*, unmatched_or_less_than>::iterator &i, const char *str)
    {
      if (i == map<string, CLICommand*, unmatched_or_less_than>::end())
          return i;
      i++;
      while (i != map<string, CLICommand*, unmatched_or_less_than>::end())
      {
          if ((*i).first.find(str, 0) != string::npos)
              return i;
          i++;
      }
      return i;
    }

    int match_all(const char *str, cmdmap& filtered_cmds)
    {
        int ret = 0;
        assert(cmd_v.size() == 0);
        map<string, CLICommand*, unmatched_or_less_than>::iterator iter;
        iter = match_first(str);
        if (iter == map<string, CLICommand*, unmatched_or_less_than>::end())
            return ret;
        CLICommand *cmd = (*iter).second;
        filtered_cmds[cmd->CmdsToStr()] = cmd;
        ret++;
        while ((iter = match_next(iter, str)) != map<string, CLICommand*, unmatched_or_less_than>::end())
        {
            cmd = (*iter).second;
            filtered_cmds[cmd->CmdsToStr()] = cmd;
            ret++;
        }
        return ret;
    }
    
    static void map_find_next(map<string, CLICommand*, unmatched_or_less_than> &m, 
        map<string, CLICommand*, unmatched_or_less_than>::iterator &i, const char* str)
    {
       i++;
       if (i != m.end())
           if ((*i).first.find(str, 0) != string::npos)
               i = m.end();
    }
};

#endif

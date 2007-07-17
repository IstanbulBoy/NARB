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
 
#ifndef __SCHEMA_H__
#define __SCHEMA_H__
#include "rce_types.hh"
#include "radixtree.hh"
#include "resource.hh"
#include <vector>
#include <string>
using namespace std;

enum RcAttrDataType
{
    //fixed length
    U_INT8 = 1,
    #define U_CHAR U_INT8
    U_INT16,
    U_INT32,
    #define NUMBER U_INT32
    INTEGER,
    U_INT64,
    FLOAT,
    DOUBLE,
    IPV4,
    IPV6,
    //varible length
    C_STR,
    RAW_BLK,
    CPP_OBJ,
    //list of vector with embedded size() info
    N_LIST, //list of numbers
    P_LIST, //list of pointers
    N_VECTOR,
    P_VECTOR
};

extern u_int16_t str_to_datatype (const char *type_str);

class ResourceIndexingElement
{
public:
    u_int16_t rcId; //lsa type id
    u_int16_t attrId; //tlv type id
    u_int16_t rcIndex;
    u_int16_t attrIndex;
    string rcAttrTag;
    u_int16_t dataType; // as defined in RcAttrDataType; 0 = unkonw type/empty 
    u_int16_t dataLen; // max. size of each block in the case or LIST or VECTOR
    ResourceIndexingElement(): rcId(0), attrId(0), rcIndex(0), attrIndex(0), rcAttrTag(""), dataType(0), dataLen(0) { }
    inline Prefix GetPrefix();
};

#define MAX_SCHEMA_FILE_SIZE 64000

class ParserElement
{
public:
    string tag;
    string value;
    list<ParserElement*> attributes;
    list<ParserElement*> children;

    ~ParserElement();
};

class ResourceSchema
{
private:
    u_int32_t version;
    u_int16_t rcType;
    string rcTag;
    vector<ResourceIndexingElement> indexArrays[8];
public:
    static RadixTree<ResourceIndexingElement> indexTree;
    static ResourceIndexingElement*  AttrById (u_int16_t r_id, u_int16_t a_id);
    static ResourceIndexingElement* AttrByTag (string tag);
    static int AttrIndexById (u_int16_t r_id, u_int16_t a_id);
    static int AttrIndexByTag (string tag);

    ResourceSchema (u_int16_t type, const char *schema_file = NULL): rcType(type) { }
    ~ResourceSchema();
    void Init(const char *schema_file = NULL);
    int AddElement(u_int16_t r_id, u_int16_t a_id, string ra_tag, u_int16_t dType, u_int16_t dLen);

    //parsing the schema file
    int ParseSchemaFromFile(const char *schema_file);
    int BuildNestedBlocks(list<string> &lines, ParserElement* pe);
    int ParseSchemaTree(ParserElement* pE);
    int ParseSchemaAttributes(list<ParserElement*>& attributes);
    int ParseResourceAttributes(list<ParserElement*>& attributes);
    int ParseTLVAttributes(list<ParserElement*>& attributes);
};

#define ATTR_INDEX(X, Y) (ResourceSchema::AttrIndexById(X, Y))
#define ATTR_INDEX_BY_TAG(T) (ResourceSchema::AttrIndexByTag(T))
#define GET_ATTR(X, Y) (ResourceSchema::AttrById(X, Y))
#define GET_ATTR_BY_TAG(T) (ResourceSchema::AttrByTag(T))

#endif

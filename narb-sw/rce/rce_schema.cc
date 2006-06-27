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

#include "resource.hh"
#include "rce_lsa.hh"
#include "rce_schema.hh"


ParserElement::~ParserElement()
{
    list<ParserElement*>::iterator it;
    for (it = attributes.begin(); it != attributes.end(); it++)
        if (*it)
            delete (ParserElement*)(*it);

//    for (it = children.begin(); it != children.end(); it++)
//        if (*it)
//            delete (ParserElement*)(*it);
}

Prefix ResourceIndexingElement::GetPrefix()
{ 
    Prefix prefix; 
    prefix.length = 32; 
    prefix_word(prefix, 0) = (rcId << 16 | attrId);
    return prefix;
}

RadixTree<ResourceIndexingElement> ResourceSchema::indexTree;

ResourceSchema::~ResourceSchema()
{
    indexTree.ClearTree();
}

void ResourceSchema::Init (const char *schema_file)
{
    for (rcType= 0; rcType < 8; rcType++)
        indexArrays[rcType].clear();
    Prefix prefix;
    if (schema_file == NULL)
    {
        for (rcType= 0; rcType < 8; rcType++)
        {
            switch (rcType)
            {
            case RTYPE_GLO_ABS_LNK:
            case RTYPE_GLO_PHY_LNK:
            case RTYPE_LOC_ABS_LNK:
            case RTYPE_LOC_PHY_LNK:
                indexArrays[rcType].reserve(32);
                indexArrays[rcType].resize(16);
                for (int i = 0; i < 16; i++)
                {
                    indexArrays[rcType][i].rcId = indexArrays[rcType][i].rcIndex = rcType;
                    indexArrays[rcType][i].attrId = i+1;
                    indexArrays[rcType][i].attrIndex = i;
                    prefix = indexArrays[rcType][i].GetPrefix();
                    indexTree.InsertNode(&prefix, &indexArrays[rcType][i]);
                }
                indexArrays[rcType][0].dataType = U_INT8; indexArrays[rcType][0].dataLen = 1;
                indexArrays[rcType][1].dataType = IPV4; indexArrays[rcType][1].dataLen = 4;
                indexArrays[rcType][2].dataType = IPV4; indexArrays[rcType][2].dataLen = 4;
                indexArrays[rcType][3].dataType = IPV4; indexArrays[rcType][3].dataLen = 4;
                indexArrays[rcType][4].dataType = U_INT32; indexArrays[rcType][4].dataLen = 4;
                indexArrays[rcType][5].dataType = FLOAT; indexArrays[rcType][5].dataLen = 4;
                indexArrays[rcType][6].dataType = FLOAT; indexArrays[rcType][6].dataLen = 4;
                indexArrays[rcType][7].dataType = FLOAT; indexArrays[rcType][7].dataLen = 32;
                indexArrays[rcType][8].dataType = U_INT32; indexArrays[rcType][8].dataLen = 4;
                indexArrays[rcType][10].dataType = U_INT64; indexArrays[rcType][10].dataLen = 8;
                indexArrays[rcType][13].dataType = U_INT64; indexArrays[rcType][13].dataLen = 8;
                indexArrays[rcType][14].dataType = P_LIST; indexArrays[rcType][14].dataLen = sizeof(ISCD);
                indexArrays[rcType][15].dataType = N_LIST; indexArrays[rcType][15].dataLen = 4;

                //////////testing code//////////
                indexArrays[rcType].resize(23);
                indexArrays[rcType][16].rcId = indexArrays[rcType][16].rcIndex = rcType;
                indexArrays[rcType][16].attrId = 33025; indexArrays[rcType][16].attrIndex = 16;
                prefix = indexArrays[rcType][16].GetPrefix();
                indexTree.InsertNode(&prefix, &indexArrays[rcType][16]);
                indexArrays[rcType][16].dataType = U_INT64; indexArrays[rcType][16].dataLen = 8; //Movaz data, TBD

                indexArrays[rcType][17].rcId = indexArrays[rcType][17].rcIndex = rcType;
                indexArrays[rcType][17].attrId = 33033; indexArrays[rcType][17].attrIndex = 17;
                prefix = indexArrays[rcType][17].GetPrefix();
                indexTree.InsertNode(&prefix, &indexArrays[rcType][17]);
                indexArrays[rcType][17].dataType = U_INT64; indexArrays[rcType][17].dataLen = 8; //Movaz data, TBD

                indexArrays[rcType][18].rcId = indexArrays[rcType][18].rcIndex = rcType;
                indexArrays[rcType][18].attrId = 33035; indexArrays[rcType][18].attrIndex = 18;
                prefix = indexArrays[rcType][18].GetPrefix();
                indexTree.InsertNode(&prefix, &indexArrays[rcType][18]);
                indexArrays[rcType][18].dataType = U_INT32; indexArrays[rcType][18].dataLen = 4; //Movaz data, TBD

                indexArrays[rcType][19].rcId = indexArrays[rcType][19].rcIndex = rcType;
                indexArrays[rcType][19].attrId = 33031; indexArrays[rcType][19].attrIndex = 19;
                prefix = indexArrays[rcType][19].GetPrefix();
                indexTree.InsertNode(&prefix, &indexArrays[rcType][19]);
                indexArrays[rcType][19].dataType = P_LIST; indexArrays[rcType][19].dataLen = 0x24; //Movaz data, TBD

                indexArrays[rcType][20].rcId = indexArrays[rcType][20].rcIndex = rcType;
                indexArrays[rcType][20].attrId = 0x4001; indexArrays[rcType][20].attrIndex = 20;
                prefix = indexArrays[rcType][20].GetPrefix();
                indexTree.InsertNode(&prefix, &indexArrays[rcType][20]);
                indexArrays[rcType][20].dataType = P_LIST; indexArrays[rcType][20].dataLen = sizeof(struct Reservation);

                indexArrays[rcType][21].rcId = indexArrays[rcType][21].rcIndex = rcType;
                indexArrays[rcType][21].attrId = 0x4002; indexArrays[rcType][21].attrIndex = 21;
                prefix = indexArrays[rcType][21].GetPrefix();
                indexTree.InsertNode(&prefix, &indexArrays[rcType][21]);
                indexArrays[rcType][21].dataType = P_LIST; indexArrays[rcType][21].dataLen = sizeof(IACD);

                indexArrays[rcType][22].rcId = indexArrays[rcType][22].rcIndex = rcType;
                indexArrays[rcType][22].attrId = 0x4010; indexArrays[rcType][22].attrIndex = 22;
                prefix = indexArrays[rcType][22].GetPrefix();
                indexTree.InsertNode(&prefix, &indexArrays[rcType][22]);
                indexArrays[rcType][22].dataType = U_INT32; indexArrays[rcType][22].dataLen = 4;
                ////// end of testing code //////
                break;
            case RTYPE_GLO_RTID:
            case RTYPE_LOC_RTID:
                indexArrays[rcType].reserve(4);
                indexArrays[rcType].resize(1);
                indexArrays[rcType][0].rcId = indexArrays[rcType][0].rcIndex = rcType;
                indexArrays[rcType][0].attrId = 1;
                indexArrays[rcType][0].attrIndex = 0;
                prefix = indexArrays[rcType][0].GetPrefix();
                indexTree.InsertNode(&prefix, &indexArrays[rcType][0]);
                break;
            default:
                LOG("Wrong rcType value!"<<endl);
            }
        }
    }
    else
    {
        //Parse schema in schema_file
        if (ParseSchemaFromFile(schema_file) < 0)
            Init(NULL);
    }
}

bool is_comment_out(string &line)
{
    if(line.length() <= 2)
        return true;
    if (line.find("<!", 0) != string::npos)
        return true;
    if (line.find("<", 0) == string::npos && line.find(">", 0) == string::npos)
        return true;

    return false;
}

int num_open_blks (string &line)
{
    int num = 0;

    string::size_type pos = 0;
    while((pos = line.find("<", pos)) != string::npos)
    {
        num++;
        pos++;
    }
    pos = 0;
    while((pos = line.find("/>", pos)) !=string::npos)
    {
        num--;
        pos++;
    }

    while((pos = line.find("</", pos)) !=string::npos)
    {
        num-=2;
        pos++;
    }

    return num;
}

int ResourceSchema::BuildNestedBlocks(list<string> &lines, ParserElement* pe)
{
    string line = lines.front();
    //assert (num_open_blks (line) > 0);

    const char* line_cstr = line.c_str();
    char buf[255];
    strcpy(buf, line_cstr);
    
    lines.pop_front();

    char* p_str = strtok(buf, " =\t?<>\"");
    pe->tag = string(p_str);
    pe->value = string("");
    ParserElement* pe_attr;
    while (p_str = strtok(NULL, " =\t?<>\""))
    {
        ParserElement* pe_attr = new ParserElement;
        pe_attr->tag = string(p_str);
        p_str = strtok(NULL, " =\t?<>\"");
        if (!p_str)
        {
            delete pe_attr;
            break;
        }
        pe_attr->value = string(p_str);
        pe->attributes.push_back(pe_attr);
    };

    while (lines.size() > 0)
    {
        int num = 0;
        list<string> newlines;
        line = lines.front();
        do
        {
            num += num_open_blks(line);
            newlines.push_back(line);
            lines.pop_front();
            if (lines.size() > 0)
                line = lines.front();
            else
                break;
        } while (num > 0);
        if (num < 0)
            break;
        ParserElement * newpe = new ParserElement;
        pe->children.push_back(newpe);
        BuildNestedBlocks(newlines, newpe);
    }

    return 0;
}

u_int16_t str_to_datatype (const char *type_str)
{
    if (strcasecmp(type_str, "U_INT8") == 0)
        return U_INT8;
    else if (strcasecmp(type_str, "U_CHAR") == 0)
        return U_CHAR;
    else if (strcasecmp(type_str, "U_INT16") == 0)
        return U_INT16;
    else if (strcasecmp(type_str, "U_INT32") == 0)
        return U_INT32;
    else if (strcasecmp(type_str, "NUMBER") == 0)
        return NUMBER;
    else if (strcasecmp(type_str, "U_INT64") == 0)
        return U_INT64;
    else if (strcasecmp(type_str, "FLOAT") == 0)
        return FLOAT;
    else if (strcasecmp(type_str, "INTEGER") == 0)
        return INTEGER;
    else if (strcasecmp(type_str, "DOUBLE") == 0)
        return DOUBLE;
    else if (strcasecmp(type_str, "IPV4") == 0)
        return IPV4;
    else if (strcasecmp(type_str, "IPV6") == 0)
        return IPV6;
    else if (strcasecmp(type_str, "C_STR") == 0)
        return C_STR;
    else if (strcasecmp(type_str, "RAW_BLK") == 0)
        return RAW_BLK;
    else if (strcasecmp(type_str, "LIST") == 0)
        return P_LIST;
    else if (strcasecmp(type_str, "VECTOR") == 0)
        return P_VECTOR;

    LOG("Unknown data type: "<<type_str<<endl);
    return 0;
}

// read schema file and create the parse tree
int ResourceSchema::ParseSchemaFromFile (const char *schema_file)
{
    list<string> lines;
    string line;
    ParserElement * tope = new ParserElement;

    assert (schema_file);
    ifstream inFile(schema_file);
    if (!inFile  || inFile.bad()) 
    {
        LOG_CERR << "Failed to open the schema file: " << schema_file << endl;
        return -1;
    }

    while (getline(inFile, line))
    {
        if (is_comment_out(line))
            continue;
        lines.push_back(line);
    }

    if (BuildNestedBlocks(lines, tope) < 0)
    {
        LOG_CERR<<"parse_blk() failed!"<<endl;
        return -1;
    }

    //now we have a tree under tope with nested blocks
    int ret = ParseSchemaTree(tope);
    return ret;
}

int ResourceSchema::ParseSchemaTree(ParserElement* pE)
{
    int ret = 0;
    ParserElement *pE_sub;
    list<ParserElement*>::iterator it;

    if (strcasecmp(pE->tag.c_str(), "SCHEMA" ) ==0)
    {
        ParseSchemaAttributes(pE->attributes);
    }
    else if (strcasecmp(pE->tag.c_str(), "RESOURCE" ) ==0)
    {
        ParseResourceAttributes(pE->attributes);
    }
    else if (strcasecmp(pE->tag.c_str(), "TLV" ) ==0)
    {
        ParseTLVAttributes(pE->attributes);
    }
    else
    {
        LOG("ParseSchemaTree()::unknown tag:"<<pE->tag<<endl);
        ret = -1;
        goto _out;
    }

    for (it = pE->children.begin(); it != pE->children.end(); it++)
    {
        if (*it)
        {
            ParseSchemaTree(*it);
        }
    }

_out:

    delete pE;
    return ret;
}

// parse common schema attributes (in /Schema/attributes)
int ResourceSchema::ParseSchemaAttributes(list<ParserElement*>& attributes)
{
    int ret = 0;
    list<ParserElement*>::iterator iter;

    for (iter = attributes.begin(); iter != attributes.end(); iter++)
    {
        ParserElement * pE = *iter;
        assert(pE->value.length() > 0);
        const char *value_str = pE->value.c_str();

        if (strcasecmp(pE->tag.c_str(), "VERSION" ) ==0)
        {
            int vn1 = 0, vn2 = 0;
            sscanf(value_str, "%d.%d", &vn1, &vn2);
            version = vn1*100 + vn2;
        }
        else
        {
            LOG("ParseSchemaAttributes()::Error: unknown attribute '"<<pE->tag<<"' in SCHEMA"<<endl);
            ret = -1;
        }
    }

    return ret;
}

//parse common resource attributes (in /Schema/Reosurce/attributes)
int ResourceSchema::ParseResourceAttributes(list<ParserElement*>& attributes)
{
    int ret = 0;
    list<ParserElement*>::iterator iter;

    for (iter = attributes.begin(); iter != attributes.end(); iter++)
    {
        ParserElement * pE = *iter;
        assert(pE->value.length() > 0);
        const char *value_str = pE->value.c_str();

        if (strcasecmp(pE->tag.c_str(), "DOMAIN" ) ==0)
        {
            if (strcasecmp(value_str, "GLOBAL") == 0)
                rcType = RTYPE_GLO_ABS_LNK;
            else if (strcasecmp(value_str, "LOCAL") == 0)
                rcType = RTYPE_LOC_PHY_LNK;
            else
            {
                LOG("Unknow DOMAIN definition: "<<value_str<<endl);
                rcType = 7; //unused rcType
            }
        }        
        else if (strcasecmp(pE->tag.c_str(), "TAG" ) ==0)
        {
            rcTag = pE->value;
        }
        else if (strcasecmp(pE->tag.c_str(), "RID" ) ==0)
        {
            //do nothing for now
        }
        else
        {
            LOG("ParseResourceAttributes()::Error: unknown attribute '"<<pE->tag<<"' in RESROUCE"<<endl);
            ret = -1;
        }
    }
    
    return ret;
}

// parse TLV attributes (in /Schema/Reosurce/children)
int ResourceSchema::ParseTLVAttributes(list<ParserElement*>& attributes)
{
    int ret = 0;
    list<ParserElement*>::iterator iter;

    ResourceIndexingElement * pRIE = new ResourceIndexingElement;
    pRIE->rcId = pRIE->rcIndex = rcType;

    for (iter = attributes.begin(); iter != attributes.end(); iter++)
    {
        ParserElement * pE = *iter;
        assert(pE->value.length() > 0);
        const char *value_str = pE->value.c_str();

        if (strcasecmp(pE->tag.c_str(), "TYPE" ) ==0)
        {
            sscanf(value_str, "%hd", &pRIE->attrId);
        }
        else if (strcasecmp(pE->tag.c_str(), "LENGTH" ) ==0)
        {
            sscanf(value_str, "%hd", &pRIE->dataLen);
        }
        else if (strcasecmp(pE->tag.c_str(), "DATA_TYPE" ) ==0)    
        {
            pRIE->dataType = str_to_datatype(value_str);
        }
        else if (strcasecmp(pE->tag.c_str(), "TAG" ) ==0)    
        {
            pRIE->rcAttrTag = rcTag + "/" + pE->value;
        }
        else
        {
            LOG("ParseTLVAttributes()::Error: unknown attribute '"<<pE->tag<<"' in TLV"<<endl);
            ret = -1;
        }
    }

    if (ret < 0)
        delete pRIE;
    else
    {
        pRIE->attrIndex = indexArrays[rcType].size();
        indexArrays[rcType].push_back(*pRIE);
        Prefix prefix = pRIE->GetPrefix();
        indexTree.InsertNode(&prefix, pRIE);
    }
    return ret;
}
    
ResourceIndexingElement* ResourceSchema::AttrById (u_int16_t r_id, u_int16_t a_id)
{
    ResourceIndexingElement* pInElement;
    Prefix prefix; 
    prefix.length = 32; 
    prefix_word(prefix, 0) = (r_id << 16 | a_id);
    assert(r_id < 8);
    RadixNode<ResourceIndexingElement>* node = indexTree.LookupNode(&prefix);
    if (!node)
        return NULL;
    pInElement = (ResourceIndexingElement*)node->Data(); 
    return pInElement;
}

ResourceIndexingElement* ResourceSchema::AttrByTag (string tag)
{
    ResourceIndexingElement* pInElement;
    RadixNode<ResourceIndexingElement>* node = indexTree.Root();
    if (!node)
        return NULL;

    pInElement = (ResourceIndexingElement*) node->Data();
    if (pInElement && pInElement->rcAttrTag == tag)
        return pInElement;
            
    while (node = indexTree.NextNodeHavingData(node))
    {
        pInElement = (ResourceIndexingElement*) node->Data();
        if (pInElement->rcAttrTag == tag)
            return pInElement;
    }

    return NULL;
}

int ResourceSchema::AttrIndexById (u_int16_t r_id, u_int16_t a_id)
{
    ResourceIndexingElement* pInElement = AttrById(r_id, a_id);
    if (pInElement)
        return pInElement->attrIndex;
    else
        return -1;
}

int ResourceSchema::AttrIndexByTag (string tag)
{
    ResourceIndexingElement* pInElement = AttrByTag(tag);
    if (pInElement)
        return pInElement->attrIndex;
    else
        return -1;
}

int ResourceSchema::AddElement (u_int16_t r_id, u_int16_t a_id, string ra_tag, u_int16_t dType, u_int16_t dLen)
{
    if (indexArrays[rcType].capacity() == indexArrays[rcType].size())
        indexArrays[rcType].resize(indexArrays[rcType].capacity() + 16);
    indexArrays[rcType].resize(indexArrays[rcType].capacity() + 1);

    assert(r_id < 8);

    ResourceIndexingElement *pInElement = &indexArrays[rcType][indexArrays[rcType].size()-1];
    pInElement->rcId = r_id;
    pInElement->attrId = a_id;
    pInElement->rcIndex = r_id;
    pInElement->attrIndex = indexArrays[rcType].size()-1;
    Prefix prefix = pInElement->GetPrefix();
    if (indexTree.LookupNode(&prefix) !=NULL)
    {
        LOGF("ResourceSchema::AddElement: (rcId = %d, a_id = %d) has already existed!\n", r_id, a_id);
        indexArrays[rcType].resize(indexArrays[rcType].size() - 1);
        return -1;
    }
    indexTree.InsertNode(&prefix, pInElement);
    return 0;
}

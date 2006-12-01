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

ResourceDB ResourceDB::r_db;
RadixTree<Resource> ResourceDB::r_trees[8];

#ifdef HAVE_EXT_ATTR
TLP * MAKE_TLP (u_int16_t T,  u_int16_t L, void* P) 
{
    TLP* tlp = new (struct TLP); 
    tlp->t = T; 
    tlp->l = L;
    tlp->p = P;
    return tlp;
}

void Resource::SetAttribute(int attrIndex, u_int16_t type, u_int16_t len, void* data)
{
    if (attrIndex < 0)
        return;
    TLP *tlp = MAKE_TLP(type, len, data);
    SetAttribute(attrIndex, tlp);
}

void Resource::SetAttribute(int attrIndex, TLP *tlp)
{
    if (attrIndex < 0)
        return;

    if (attrTable.capacity() == 0)
        attrTable.resize(16);

    if (attrTable.size() < attrIndex+1)
    {
        attrTable.resize(attrIndex+1);
    }
    if (tlp->t == N_LIST || tlp->t == P_LIST || tlp->t == N_VECTOR || tlp->t == P_VECTOR)
    {
        if (attrTable[attrIndex].p == NULL)
        {
            attrTable[attrIndex] = *tlp;
            attrTable[attrIndex].p = new list<void*>;
        }
        ((list<void*> *)attrTable[attrIndex].p)->push_back(tlp->p);
    }
    else
    {
        attrTable[attrIndex] = *tlp;
    }
   
    delete tlp;
}

TLP* Resource::GetAttribute(int attrIndex)
{
    if (attrIndex < 0)
        return NULL;
    assert(attrTable.size() > attrIndex);
    return &attrTable[attrIndex];
}
#endif


Resource::~Resource()
{
    //clean and delete attrTable
}


Link::~Link()
{
    list<ISCD*>::iterator iter1;
    for (iter1 = iscds.begin(); iter1 != iscds.end(); iter1++)
    {
        assert (*iter1);
        delete *iter1;
    }
    
    list<IACD*>::iterator iter2;
    for (iter2 = iacds.begin(); iter2 != iacds.end(); iter2++)
    {
        assert (*iter2);
        delete *iter2;
    }

    // How about dependency?

    //Clean resvTable ...
}

void Link::Dump()
{ 
    #ifdef HAVE_EXT_ATTR
        if (attrTable.size() == 0)
            return;
        if (type == RTYPE_GLO_ABS_LNK)
            LOGF("RTYPE_GLO_ABS_LNK(EXT_ATTR)::Link[%x->%x]: lclAddr(%x), rmtAddr(%x), metric(%d), number of ISCD (%d)\n", 
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 2)].p?*(int*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 2)].p):0, advRtId, 
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 3)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 3)].p): 0, 
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 4)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 4)].p): 0,
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 5)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 5)].p): 0, 
                 attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 15)].p? ((list<ISCD*>*)(attrTable[ATTR_INDEX(RTYPE_GLO_ABS_LNK, 15)].p))->size(): 0);
        else if (type == RTYPE_LOC_PHY_LNK)
            LOGF("RTYPE_LOC_PHY_LNK(EXT_ATTR)::Link[%x->%x]: lclAddr(%x), rmtAddr(%x), metric(%d), number of ISCD (%d)\n", 
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 2)].p?*(int*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 2)].p):0, advRtId, 
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 3)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 3)].p): 0, 
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 4)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 4)].p): 0,
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 5)].p? *(u_int32_t*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 5)].p): 0, 
                 attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 15)].p? ((list<ISCD*>*)(attrTable[ATTR_INDEX(RTYPE_LOC_PHY_LNK, 15)].p))->size(): 0);
    #else
        LOGF("Link[%x->%x]: lclAddr(%x), rmtAddr(%x)\n", id, advRtId, lclIfAddr, rmtIfAddr);
    #endif
}

ResourceDB::~ResourceDB() 
{
    ClearTree(RTYPE_LOC_PHY_LNK);
    ClearTree(RTYPE_LOC_ABS_LNK);
    ClearTree(RTYPE_LOC_RTID);
    ClearTree(RTYPE_GLO_PHY_LNK);
    ClearTree(RTYPE_GLO_ABS_LNK);
    ClearTree(RTYPE_GLO_RTID);

    list<RouterId2DomainId*>::iterator it;

    for (it = routerToDomainDirectory.begin(); it != routerToDomainDirectory.end(); it++)
        if (*it)
            delete (RouterId2DomainId*)(*it);
}

RadixNode<Resource>* ResourceDB::Allocate(ResourceType type, Prefix* prefix)
{
    assert(prefix);
    if (LookupByPrefix(type, prefix))
        return NULL;
    
    Resource *rc;
    switch (type)
    {
    case RTYPE_LOC_ABS_LNK:
    case RTYPE_LOC_PHY_LNK:
    case RTYPE_GLO_ABS_LNK:
    case RTYPE_GLO_PHY_LNK:
        rc = (Resource*)(new Link(type, 0, 0, 0));
        rc->domainId = prefix_word(*prefix, 0);
        rc->advRtId = prefix_word(*prefix, 1);
        rc->id = prefix_word(*prefix, 2);
        ((Link*)rc)->lclIfAddr = prefix_word(*prefix, 3);
        ((Link*)rc)->rmtIfAddr = prefix_word(*prefix, 4);
        break;
    case RTYPE_LOC_RTID:
    case RTYPE_GLO_RTID:
        rc = (Resource*)(new RouterId(type, 0, 0));
        rc->domainId = prefix_word(*prefix, 0);
        rc->advRtId = prefix_word(*prefix, 1);
        rc->id = prefix_word(*prefix, 2);
        break;
    default:
        return NULL;
    }

    RadixNode<Resource>* node =  r_trees[rc->Type()].InsertNode(prefix, rc);
    return node;
}

RadixNode<Resource>* ResourceDB::Insert(Resource* rc)
{
    RadixNode<Resource> *node;
    assert(rc);
    Prefix index = rc->Index();
    r_trees[rc->Type()].InsertNode(&index, rc); //Memory leakage here. Need Update function!
    return node;
}

RadixNode<Resource>* ResourceDB::Lookup(Resource* rc)
{
    RadixNode<Resource> *node;
    assert(rc);
    Prefix index = rc->Index();
    if (index.length == 0)
        return r_trees[rc->Type()].Root();
    node = r_trees[rc->Type()].LookupNode(&index);
    return node;
}

RadixNode<Resource>* ResourceDB::LookupByPrefix(ResourceType type, Prefix* prefix)
{
    RadixNode<Resource> *node;
    assert(prefix);
    node = r_trees[type].LookupNode(prefix);
    return node;
}

RadixNode<Resource>* ResourceDB::Update(Resource* rc)
{
    assert(rc);
    Prefix index = rc->Index();
    RadixNode<Resource> *node = r_trees[rc->Type()].GetNode(&index);
    if (node && node->Data())
        delete node->Data();
    node->SetData(rc);
    return node;
}

void ResourceDB::Delete(Resource* rc)
{
    RadixNode<Resource> *node;
    assert(rc);
    Prefix index = rc->Index();
    Resource * data = r_trees[rc->Type()].DeleteNode(&index);
    if (data)
        delete data;
}

void ResourceDB::DeleteByPrefix(ResourceType type, Prefix* prefix)
{
    RadixNode<Resource> *node;
    assert(prefix);
    Resource * data = r_trees[type].DeleteNode(prefix);
    if (data)
        delete data;
}

void ResourceDB::ClearTree(ResourceType type)
{
    r_trees[type].ClearTree();
}

void ResourceDB::WalkTree(ResourceType type)
{
    if (!Log::Debug())
        return;

    RadixTree<Resource> &tree = r_trees[type];
    RadixNode<Resource> *node = tree.Root();
    if (!node)
        return;
    do
    {
        if (!node->Data())
            continue;

        switch (type)
        {
        case RTYPE_GLO_ABS_LNK:
        case RTYPE_GLO_PHY_LNK:
        case RTYPE_LOC_ABS_LNK:
        case RTYPE_LOC_PHY_LNK:
            ((Link*)node->Data())->Dump();
            break;
        case RTYPE_GLO_RTID:
        case RTYPE_LOC_RTID:
            ((RouterId*)node->Data())->Dump();
            break;
        default:
            LOG_CERR<<"Unkown RESOURCE TYPE "<<type<<endl;
        }

        tree.UnlockNode(node);
    } while (node = tree.NextNodeHavingData(node)); 
}


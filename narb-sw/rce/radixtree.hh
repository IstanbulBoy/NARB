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

#ifndef __RADIX_TREE_H__
#define __RADIX_TREE_H__
#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <assert.h>
#include "rce_types.hh"
using namespace std;

#define PREFIX_SIZE 6 //Counted by u_int32 = 192 bits
#define HAVING_LOCK

struct Prefix
{
    int length;
    u_int32_t prefix [PREFIX_SIZE];
};

//Public prefix operation functions
const u_char bitmask[] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
const u_char bit[] =  {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
//Check value of the X[L] bit 
#define check_bit(X,L) ((((u_char *)(X))[(L) / 8]) >> (7 - ((L) % 8)) & 1)
//Set X[L] bit 
#define set_bit(X, L) ((((u_char *)(X))[(L) / 8]) |= bit[L%8])
//Reset X[L] bit 
#define reset_bit(X, L) ((((u_char *)(X))[(L) / 8]) &= (~bit[L%8]))
//Get the N byte
#define prefix_byte(P, N) ((((u_char *)((P).prefix))[N]))
//Get the N 32int word
#define prefix_word(P, N) (((P).prefix)[N])


// This prefix matches p2 iff it contains p2's prefix.
extern inline bool operator<=(const Prefix& p1, const Prefix& p2)
{
  int offset;
  int shift;

  //pointers to the begining of the prefix
  u_char *pp1 = (u_char *)p1.prefix;
  u_char *pp2 = (u_char *)p2.prefix;

  // return false,  if p1's prefix is longer than p2
  if (p1.length > p2.length)
    return false;

  offset = p1.length / 8;
  shift =  p1.length % 8;

  if (shift)
    if (bitmask[shift] & (pp1[offset] ^ pp2[offset]))
      return false;
  
  while (offset--)
    if (pp1[offset] != pp2[offset])
      return false;
  return true;
}

// Compare this prefix with p2.
extern inline bool operator==(const Prefix& p1, const Prefix& p2)
{
  int offset;
  int shift;

  //pointers to the begining of the prefix
  u_char *pp1 = (u_char *)p1.prefix;
  u_char *pp2 = (u_char *)p2.prefix;

  // return false,  if p1's prefix is longer than p2
  if (p1.length != p2.length)
    return false;

  offset = p1.length / 8;
  shift =  p1.length % 8;

  if (shift)
    if (bitmask[shift] & (pp1[offset] ^ pp2[offset]))
      return false;
  
  while (offset--)
    if (pp1[offset] != pp2[offset])
      return false;
  return true;
}

// Return the common prefix of p1 and p2.
extern inline const Prefix operator&(const Prefix& p1, const Prefix& p2)
{
  Prefix newIndex;
  int i;
  u_char diff;
  u_char mask;

  u_char *pp1 = (u_char *)p1.prefix;
  u_char *pp2 = (u_char *)p2.prefix;
  u_char *newp = (u_char *)newIndex.prefix;

  memset(&newIndex, 0, sizeof(Prefix));
  
  for (i = 0; i < p2.length / 8; i++)
    {
      if (pp1[i] == pp2[i])
	newp[i] = pp1[i];
      else
	break;
    }

  newIndex.length = i * 8;

  if (newIndex.length != p2.length)
    {
      diff = pp1[i] ^ pp2[i];
      mask = 0x80;
      while (newIndex.length < p2.length && !(mask & diff))
	{
	  mask >>= 1;
	  newIndex.length++;
	}
      newp[i] = pp1[i] & bitmask[newIndex.length % 8];
    }

  return newIndex;
}

#define set_link(X,Y) (X)->link[check_bit((Y)->index.prefix, (X)->index.length)] = (Y); (Y)->parent = (X)
#define set_node(X,Y, Z) (X)->index=(Y); (X)->tree = (Z)

template <class T>
class RadixTree;

template <class T>
class RadixNode
{
    Prefix index;
    // Tree links.
    RadixNode<T>* parent;
    RadixNode<T>* link[2];
    #define l_left link[0]
    #define l_right link[1]
    RadixTree<T>* tree;
    // Lock of this node
    unsigned short lock;
    //Node data
    T * data;
public:
    RadixNode() {
        memset(&index, 0, sizeof(Prefix));
        tree = NULL;
        parent = link[0] = link[1] = NULL;
        lock = 0;
        data = NULL;
        }
    RadixNode(Prefix* pIndex) {
        index = *pIndex;
        tree = NULL;
        parent = link[0] = link[1] = NULL;
        lock = 0;
        data = NULL;
        }
    RadixNode(Prefix* pIndex, T* theData) {
        index = *pIndex;
        tree = NULL;
        parent = link[0] = link[1] = NULL;
        lock = 0;
        data = theData;
        }
    RadixNode(RadixNode<T> *node) {
        index = node->index;
        tree = NULL;
        parent = link[0] = link[1] = NULL;
        lock = 0;
        data = node->getData();
        }
    ~RadixNode() {
        //Leave deletion of data to caller
        //FreeData(); //?$$? Dec 1 2006
        }

    T* Data() {return data;}
    void SetData(T* theData) {data = theData;}
    void FreeData() {if (data) delete data; data = NULL;}

    unsigned short Lock() {return lock;}
    Prefix Index () {return index;}
    RadixTree<T>* Tree() {return tree;}
    RadixNode<T>* Parent() {return parent;}
    RadixNode<T>* LeftBranch() {return link[0];}
    RadixNode<T>* RightBranch() {return link[1];}

    friend class RadixTree<T>;
};


template <class T>
class RadixTree
{
private:
    RadixNode<T> *root;
public:
    u_int32_t total;
    RadixTree(): root(NULL), total(0) {}
    RadixTree(RadixNode<T>* node): total(0) {root = new RadixNode<T>(node); root->parent = this; LockNode(root);}
    RadixTree(Prefix *pIndex, T* rootData): total(0) {root = new RadixNode<T>(pIndex, rootData); root->parent = (RadixNode<T> *)this; LockNode(root);}
    ~RadixTree();
    void ClearTree ();

    //The following functions return a locked node
    RadixNode<T>* Root () {if (root) LockNode(root); return root;}
    RadixNode<T>* NextNode (RadixNode<T>* node);
    RadixNode<T>* NextNodeUntil (RadixNode<T>* node, RadixNode<T>* last);
    RadixNode<T>* NextNodeHavingData (RadixNode<T>* node);
    RadixNode<T>* GetNode (Prefix* pIndex);
    RadixNode<T>* LookupNode (Prefix* pIndex);
    RadixNode<T>* LookupMatchedNode (Prefix* pIndex);
    RadixNode<T>* LookupNextMatchedNode (RadixNode<T>* node, Prefix* pIndex);
    RadixNode<T>* LockNode (RadixNode<T>* node); 
    void UnlockNode (RadixNode<T>* node);

    // InsertNode will replace the node data and return the old data; or NULL, if this is a new node
    RadixNode<T>* InsertNode (Prefix* pIndex, T *theData) {
        RadixNode<T>* node = GetNode(pIndex);
        T* retData = node->data;
        node->data = theData;
        if (retData) 
            UnlockNode(node);
        return node; }
    RadixNode<T>* InsertNode (RadixNode<T>* node) {return InsertNode(&node->index, node->data);}
    
    RadixNode<T>* UpdateNode (Prefix* pIndex, T *theData) {} //@@@
    RadixNode<T>* UpdateNode (RadixNode<T>* node) {} //@@@
    
    T* DeleteNode (Prefix* pIndex) 
        {RadixNode<T> *node = LookupNode(pIndex); if (node) return DeleteNode(node); return NULL;}
    T* DeleteNode (RadixNode<T>* node);

   //Clone a RadixTree staring from a branch node (useful for indexing)
   RadixTree<T>& operator=(RadixNode<T>& node);
   //Merge two RadixTree's into a new tree
   const RadixTree<T> operator+(const RadixTree<T>& otherTree) const;
   //Merge  another RadixTree into the current one
   RadixTree<T>& operator+=(const RadixTree<T>& otherTree);
};

template <class T> 
RadixTree<T>::~RadixTree()
{
  ClearTree();
}

template <class T> 
void RadixTree<T>::ClearTree()
{
  RadixNode<T> *tmp_node, *node;

  node = root;
  while (node)
    {
      if (node->l_left)
	{
	  node = node->l_left;
	  continue;
	}

      if (node->l_right)
	{
	  node = node->l_right;
	  continue;
	}

      tmp_node = node;
      node = node->parent;

      if (node != NULL)
	{
	  if (node->l_left == tmp_node)
	    node->l_left = NULL;
	  else
	    node->l_right = NULL;
	  tmp_node->FreeData();
	  delete tmp_node;
	}
      else
	{
	  tmp_node->FreeData();
	  delete tmp_node;
	  break;
	}
    }
  
  root = NULL;
  return;
}



//Get and lock node
template <class T> 
RadixNode<T>* RadixTree<T>::GetNode (Prefix* pIndex)
{
  RadixNode<T>* newNode;
  RadixNode<T>* node;
  RadixNode<T>* match = NULL;

 if (pIndex->length == 0)
    return Root();

  node = root;
  while (node && node->index.length <= pIndex->length && 
	node->index <= *pIndex)
  {
      if (node->index.length == pIndex->length)
      {
	  LockNode (node);
	  return node;
      }
      match = node;
      node = node->link[check_bit(pIndex->prefix, node->index.length)];
  }

  if (node == NULL)
  {
        newNode = new RadixNode<T>(pIndex);
        newNode->tree = this;
        if (match)
        {
            set_link (match, newNode);
        }
        else
            root = newNode;
  }
  else
  {
      newNode = new RadixNode<T>();
      newNode->index = node->index & (*pIndex);
      newNode->tree = this;
      set_link (newNode, node);

      if (match)
      {
            set_link(match, newNode);
      }
      else
          root = newNode;

      if (newNode->index.length != pIndex->length)
      {
 	  match = newNode;
         newNode = new RadixNode<T>(pIndex);
         newNode->tree = this;
	  set_link(match, newNode);
      }
  }
  LockNode(newNode);
  
  return newNode;
}

template <class T> 
RadixNode<T>* RadixTree<T>::NextNode (RadixNode<T>* node)
{
  RadixNode<T> *next, *start;

  start = node;
  LockNode (start); //locking input node;
  
  if (node->l_left)
    {
      next = node->l_left;
      UnlockNode (start);
      return LockNode (next);
    }
  if (node->l_right)
    {
      next = node->l_right;
      UnlockNode (start);
      return LockNode (next);
    }

  while (node->parent)
    {
      if (node->parent->l_left == node && node->parent->l_right)
	{
	  next = node->parent->l_right;
	  UnlockNode (start);
	  return LockNode (next);
	}
      node = node->parent;
    }
  UnlockNode (start);
  return NULL;
}

template <class T> 
RadixNode<T>* RadixTree<T>::NextNodeUntil (RadixNode<T>* node, RadixNode<T>* last)
{
  RadixNode<T> *next, *start;

  start = node;
  LockNode (start); //locking input node;

  if (node->l_left)
    {
      next = node->l_left;
      UnlockNode (start);
      return LockNode (next);
    }
  if (node->l_right)
    {
      next = node->l_right;
      UnlockNode (start);
      return LockNode (next);
    }

  while (node->parent && node != last)
    {
      if (node->parent->l_left == node && node->parent->l_right)
	{
	  next = node->parent->l_right;
	  UnlockNode (start);
	  return LockNode (next);
	}
      node = node->parent;
    }
  UnlockNode (start);
  return NULL;
}

template <class T> 
RadixNode<T>* RadixTree<T>::NextNodeHavingData (RadixNode<T>* node)
{
  RadixNode<T> *next, *start;

  start = node;
  LockNode (start); //locking input node;
  next = node;
  
_next:

  node = next;

  if (node->l_left)
  {
      next = node->l_left;
      if (!next->data)
          goto _next;
      UnlockNode (start);
      return LockNode (next);
  }
  if (node->l_right)
  {
      next = node->l_right;
      if (!next->data)
          goto _next;
      UnlockNode (start);
      return LockNode (next);
  }

  while (node->parent)
  {
      if (node->parent->l_left == node && node->parent->l_right)
      {
         next = node->parent->l_right;
         if (next->data)
         {
             UnlockNode (start);
             return LockNode (next);
         }
         else
            goto _next;
      }
      node = node->parent;
  }
  UnlockNode (start);
  return NULL;
}

//A node is deleted in the context of RadixTree. User should not call 'delete' directly on a node in the tree.
template <class T> 
T* RadixTree<T>::DeleteNode (RadixNode<T>* node)
{
  RadixNode<T> *child, *parent;
  T* data = NULL;

  //assert (node->lock == 0); //Forced node deletion without checking lock status.
  //assert (node->data == NULL); //Data are not autonatically freed. Caller NUST free them before deleting the node.


  if (node->l_left && node->l_right)
    return data;

  if (node->l_left)
    child = node->l_left;
  else
    child = node->l_right;

  parent = node->parent;

  if (child)
    child->parent = parent;

  if (parent)
    {
      if (parent->l_left == node)
	parent->l_left = child;
      else
	parent->l_right = child;
    }
  else
    node->tree->root = child;

  data = node->data;
  delete node;
  node = NULL;
  /* If parent node is stub then delete it also. */
  if (parent && parent->lock == 0)
    return DeleteNode(parent);
  return data;
}

template <class T>
RadixNode<T>* RadixTree<T>::LockNode (RadixNode<T>* node)
{
#ifdef HAVING_LOCK
  node->lock++;
#endif
  return node;
}

template <class T> 
void RadixTree<T>::UnlockNode (RadixNode<T>* node)
{
#ifdef HAVING_LOCK
  node->lock--;

  if (node->lock == 0)
  {
      //assert(!node->data);
      DeleteNode(node);
  }
#endif
}

//lookup and lock node
template <class T> 
RadixNode<T>* RadixTree<T>::LookupNode (Prefix* pIndex)
{
  RadixNode<T>*node;

  if (pIndex->length == 0)
    return Root();

  node = root;
  while (node && node->index.length <= pIndex->length && 
	 node->index <= *pIndex)
    {
      if (node->index.length == pIndex->length && node->data)
	return LockNode (node);

      node = node->link[check_bit(pIndex->prefix, node->index.length)];
    }

  return NULL;
}


template <class T> 
RadixNode<T>* RadixTree<T>::LookupMatchedNode (Prefix* pIndex)
{
  RadixNode<T>*node, *p_node, *matched = NULL;

  if (pIndex->length == 0)
    return Root();

  p_node = node = root;
  while (node && node->index.length <= pIndex->length && 
	 node->index <= *pIndex)
    {
      if (node->data)
          matched = (node);

      p_node = node;
      node = node->link[check_bit(pIndex->prefix, node->index.length)];
    }

  if (matched)
  {
     return LockNode(matched);
  }
  else if (!matched && p_node)
  {
      return LookupNextMatchedNode(p_node, pIndex);
  }

  return NULL;
}



template <class T> 
RadixNode<T>* RadixTree<T>::LookupNextMatchedNode (RadixNode<T>* node, Prefix* pIndex)
{
    while (node = NextNodeHavingData(node))
   {
        if (*pIndex <= node->index)
            return node;
        else
            UnlockNode(node);
    }
    return NULL;
}
    
//Clone a RadixTree from a branch (indexing)
template <class T> 
RadixTree<T>& RadixTree<T>::operator=(RadixNode<T>& node)
{
  assert(!this->root || !this->root->l_left && !this->root->l_right && this->root->parent);
  RadixTree<T>* tree = node.tree;
  tree->LockNode(&node);
  if (!this->root)
    this->root =new RadixNode<T>();
  LockNode(this->root);
  set_node(this->root, node.index, this);
  this->root->data = node.data;
  
  RadixNode<T>* next = &node;
  while(next = tree->LookupNextMatchedNode(next, &node.index)) 
  {
     this->InsertNode(next); //InsertNode copy attributes of the 'node' except for node.data.
     tree->UnlockNode(next);
  }
  tree->UnlockNode(&node);
  return *this;
}

//Merge two RadixTree's into a new tree
template <class T> 
const RadixTree<T> RadixTree<T>::operator+(const RadixTree<T>& otherTree) const
{
  RadixTree<T> newTree = this->root;
  RadixNode<T>* next = otherTree->root;
  while(next = otherTree.LookupNextMatchedNode(next, &otherTree.root->index)) 
  {
     newTree.InsertNode(next); //InsertNode copy attributes of the 'node' except for node.data.
     newTree->UnlockNode(next);
  }
  return newTree;
}

//Merge  another RadixTree into the current one
template <class T> 
RadixTree<T>& RadixTree<T>::operator+=(const RadixTree<T>& otherTree)
{
  RadixNode<T>* next = otherTree->root;
  while(next = otherTree.LookupNextMatchedNode(next, &otherTree.root->index)) 
  {
     this->InsertNode(next); //InsertNode copy attributes of the 'node' except for node.data.
     this->UnlockNode(next);
  }
  return *this;
}


#endif

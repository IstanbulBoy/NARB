/*
 * dcsEROHopSet.java
 *
 * Created on November 3, 2007, 8:08 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.dcs;

import java.util.Vector;

/**
 *
 * @author jflidr
 */
public class dcsEROHopSet extends Vector<dcsEROHop> {
    
    private boolean isHidden = false;
    
    /**
     * Creates a new instance of dcsEROHopSet
     */
    public dcsEROHopSet(int cap, int inc) {
        super(cap, inc);
    }
    
    public void load(String[] ero_a) {
        dcsNode[] np = null;
        dcsEROHop e, e1;
        String[] sa;
        
        for (int i = 0; i < size(); i++) {
            e = get(i);
            e.setStale(true);
            e.setLeader(true);
        }
        
        for(int x = 0; x<ero_a.length; x++) {
            sa = ero_a[x].split(";");
            if(sa.length%2 == 0)
                continue;
            if(!contains(sa[0])) {
                for (int i = 1; i < sa.length; i+=2) {
                    np = dcsGlobals.dcsNodes.getLinkedNodes(sa[i], sa[i+1]);
                    if(np != null) {
                        e = new dcsEROHop(sa[0], np);
                        add(e);
                    }
                }
            }
            for (int i = size()-1; i>=0 ; i--) {
                e = get(i);
                if(e.isStale()) {
                    removeERO(e);
                }
            }
        }
        
        int cnt;
        for (int i=0; i<size(); i++) {
            e = get(i);
            if(!e.isLeader())
                continue;
            np = e.getEPs();
            cnt = 1;
            
            for (int j = i+1; j < size(); j++) {
                e1 = get(j);
                if(e1.connects(np[0], np[1])) {
                    e1.setLeader(false);
                    cnt++;
                }
            }
            e.setBunchSize(cnt);
        }
        dcsGlobals.currMapPane.repaint();
    }
    
    private void removeERO(dcsEROHop e) {
        dcsNode[] np = e.getEPs();
        dcsGlobals.dcsLSPs.removeLSPSegment(e.getID(), np);
        np[0].removeEROHop(e);
        np[1].removeEROHop(e);
        remove(e);
    }
    
    //getters
    public int getIdx() {
        if(size() == 0)
            return 1;
        return Integer.parseInt(get(size()-1).getID().split("_")[1]) + 1;
    }
    
    //setters
    public void setHidden(boolean b) {
        isHidden = b;
    }
    
    // queries
    public boolean isHidden() {
        return isHidden;
    }
    
    private boolean contains(String s) {
        dcsEROHop e;
        boolean ret = false;
        for (int i = 0; i < size(); i++) {
            e = get(i);
            if(e.getID().equals(s)) {
                e.setStale(false);
                ret = true;
            }
        }
        return ret;
    }
    
    public void setHighlight(String s, boolean b) {
        boolean lsp_found = false;
        dcsEROHop e;
        dcsLSP l;
        if(b) {
            for (int i = 0; i < size(); i++) {
                e = get(i);
                if(e.getID().equals(s)){
                    dcsGlobals.dcsLSPs.addLSP(s, e.getEPs());
                }
            }
        } else {
            dcsGlobals.dcsLSPs.removeLSP(s);
        }
    }
    
    public void highlightNodeEROs(dcsNode n, boolean b) {
        Vector<dcsEROHop> es = n.getEROEndPts();
        dcsLSP l;
        dcsGlobals.dcsLSPs.removeAllElements();
        if(b) {
            for (int i = 0; i < es.size(); i++) {
                setHighlight(es.get(i).getID(), true);
            }
        }
    }
    
    public void toggleERO(String s, boolean b) {
        dcsGlobals.dcsLSPs.removeAllElements();
        if(b) {
            setHighlight(s, b);
        }
    }
}

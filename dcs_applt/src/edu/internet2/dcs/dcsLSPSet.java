/*
 * dcsLSPSet.java
 *
 * Created on November 6, 2007, 10:25 PM
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
public class dcsLSPSet extends Vector<dcsLSP> {
    
    private boolean isHidden = false;
    
    /** Creates a new instance of dcsLSPSet */
    public dcsLSPSet(int cap, int inc) {
        super(cap, inc);
    }
    
    public boolean isHidden() {
        return isHidden;
    }
    
    boolean isHighlighted(String id) {
        for (int i = 0; i < size(); i++) {
            if(get(i).getID().equals(id)) {
                return true;
            }
        }
        return false;
    }
    
    public void addLSP(String id, dcsNode[] np) {
        dcsLSP l0, l1;
        for (int i = 0; i < size(); i++) {
            l0 = get(i);
            if(l0.getID().equals(id) && l0.connects(np)) {
                return;
            }
        }
        int cnt = 1;
        l1 = new dcsLSP(id, np);
        for (int i = 0; i < size(); i++) {
            l0 = get(i);
            if(l0.connects(np)) {
                cnt++;
                l0.setLeader(false);
                l0.setBunchSize(cnt);
            }
        }
        l1.setBunchSize(cnt);
        add(l1);
    }
    
    public void removeLSP(String s) {
        boolean needLeader;
        dcsLSP l0, l1;
        dcsNode[] np;
        for (int i = (size()-1); i >= 0 ; i--) {
            l0 = get(i);
            if(l0.getID().equals(s)) {
                np = l0.getEPs();
                needLeader = l0.isLeader();
                for (int j = 0; j < size(); j++) {
                    l1 = get(j);
                    if(l1.connects(np)) {
                        l1.decBunchSize();
                        if(needLeader) {
                            l1.setLeader(true);
                            needLeader = false;
                        }
                    }
                }
                np[0].removeLSP(l0);
                np[1].removeLSP(l0);
                remove(l0);
            }
        }
    }
    
    public void removeLSPSegment(String id, dcsNode[] np) {
        boolean needLeader;
        dcsLSP l0, l1;
        for (int i = 0; i < size(); i++) {
            l0 = get(i);
            if(l0.getID().equals(id) && l0.connects(np)) {
                needLeader = l0.isLeader();
                for (int j = 0; j < size(); j++) {
                    l1 = get(j);
                    if(l1.connects(np)) {
                        l1.decBunchSize();
                        if(needLeader) {
                            l1.setLeader(true);
                            needLeader = false;
                        }
                    }
                }
                np[0].removeLSP(l0);
                np[1].removeLSP(l0);
                remove(l0);
                break;
            }
        }
    }
}

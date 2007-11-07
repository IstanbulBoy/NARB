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

    public void addLSP(dcsLSP l) {
        dcsLSP l0;
        int cnt = 1;
        for (int i = 0; i < size(); i++) {
            l0 = get(i);
            if(l0.connects(l.getEPs())) {
                l.setLeader(false);
                cnt++;
            }
        }
        add(l);
    }
}

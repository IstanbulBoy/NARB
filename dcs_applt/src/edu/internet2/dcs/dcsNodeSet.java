/*
 * dcsNodeSet.java
 *
 * Created on October 31, 2007, 10:41 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.dcs;

import java.util.Vector;
import java.util.prefs.Preferences;

/**
 *
 * @author jflidr
 */
public class dcsNodeSet extends Vector<dcsNode> {
    
    private int nodeCount;
    private Preferences prefs;
    
    /** Creates a new instance of dcsNodeSet */
    public dcsNodeSet(int cap, int inc) {
        super(cap, inc);
        nodeCount = 0;
        //prefsManager must be initialized before astNodeSet
        prefs = dcsGlobals.prefsRoot.node("/DcsMap/Nodes");
    }
    
    public int getIdx() {
        if(size() == 0)
            return 1;
        return Integer.parseInt(get(size()-1).getID().split("_")[1]) + 1;
    }
    
    int getNumHihlights() {
        dcsNode n;
        int ret = 0;
        for (int i = 0; i < this.size(); i++) {
            n = this.get(i);
            if(n.isHighlighted()) {
                ret++;
            }
        }
        return ret;
    }
    
    public void normHighlits() {
        dcsNode n;
        for (int i = 0; i < this.size(); i++) {
            n = this.get(i);
            if(n.isHighlighted()) {
                n.setHighlight(false);
                break;
            }
        }
    }
    
}

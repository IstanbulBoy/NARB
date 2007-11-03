/*
 * dcsFiberSet.java
 *
 * Created on November 2, 2007, 7:23 AM
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
public class dcsFiberSet extends Vector<dcsFiber> {

    private boolean isHidden = false;
    
    /** Creates a new instance of dcsFiberSet */
    public dcsFiberSet(int cap, int inc) {
        super(cap, inc);
    }
    
    public int getIdx() {
        if(size() == 0)
            return 1;
        return Integer.parseInt(get(size()-1).getID().split("_")[1]) + 1;
    }
    
    // queries
    boolean isHidden() {
        return isHidden;
    }
}

/*
 * dcsGlobals.java
 *
 * Created on October 30, 2007, 10:34 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.dcs;

import java.util.prefs.Preferences;
import javax.swing.JApplet;
import javax.swing.JLayeredPane;

/**
 *
 * @author jflidr
 */
public class dcsGlobals {
    
    static final int DCS_MAP_W = 750;
    static final int DCS_MAP_H = 500;
    
    static final int DCS_PANE_FIBER_LAYER = new Integer(200);
    static final int DCS_PANE_NODE_LAYER = new Integer(300);
    
    static Preferences prefsRoot = null;
    static JApplet dcsApplet = null;
    static JLayeredPane dcsLayeredPane = null;
    
    static dcsMap currMap = null;
    static dcsMapPane currMapPane = null;
    
    static dcsNodeSet dcsNodes = null;
    static dcsFiberSet dcsFibers = null;
    static dcsEROHopSet dcsEROHops = null;
    static dcsLSPSet dcsLSPs = null;
}

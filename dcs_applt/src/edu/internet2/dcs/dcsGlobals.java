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
    static Preferences prefsRoot = null;
    static JApplet dcsApplet = null;
    static JLayeredPane dcsLayeredPane = null;
    
    static dcsMap currMap = null;
    static dcsMapPane currMapPane = null;
    
    static final int DCS_MAP_PANE_W = 750;
    static final int DCS_MAP_PANE_H = 500;
    
    static dcsNodeSet dcsNodes = null;
}

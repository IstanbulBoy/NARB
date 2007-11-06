/*
 * dcsApplt.java
 *
 * Created on October 30, 2007, 9:08 PM
 */

package edu.internet2.dcs;

import java.util.prefs.Preferences;
import javax.swing.JLayeredPane;

/**
 *
 * @author  jflidr
 */
public class dcsApplt extends javax.swing.JApplet {
    
    /** Initializes the applet dcsApplt */
    public void init() {
        dcsGlobals.dcsApplet = this;
        try {
            java.awt.EventQueue.invokeAndWait(new Runnable() {
                public void run() {
                    dcsGlobals.prefsRoot = Preferences.userNodeForPackage(dcsApplt.class);
                    dcsGlobals.dcsNodes = new dcsNodeSet(10, 1);
                    dcsGlobals.dcsFibers = new dcsFiberSet(10, 1);
                    dcsGlobals.dcsEROHops = new dcsEROHopSet(10, 1);
                    
                    setBackground(new java.awt.Color(153, 153, 0));
                    
                    dcsGlobals.dcsLayeredPane = new JLayeredPane();
                    dcsGlobals.dcsLayeredPane.setOpaque(true);
                    setContentPane(dcsGlobals.dcsLayeredPane);
                    loadMap();
                    loadNodes();
                    fiberUp();
                }
            });
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
    
    private void loadMap() {
        dcsGlobals.currMap = new dcsMap();
        dcsGlobals.currMap.setOpaque(true);
        dcsGlobals.dcsLayeredPane.add(dcsGlobals.currMap, java.awt.BorderLayout.CENTER, JLayeredPane.DEFAULT_LAYER);
    }
    
    private void loadNodes() {
        dcsGlobals.dcsNodes.load();
        dcsGlobals.dcsNodes.addShowAll();
    }
    
    private void fiberUp() {
        dcsGlobals.dcsNodes.fiberNodes();
    }
    
    //JavaScript API
    public static void appletZoomIn() {
        if(dcsGlobals.currMapPane.isZoomedOut()) {
            dcsGlobals.currMapPane.zoomMap();
        }
    }
    
    public static void appletZoomOut() {
        if(!dcsGlobals.currMapPane.isZoomedOut()) {
            dcsGlobals.currMapPane.zoomMap();
        }
    }
    
    public static void loadERO(String s) {
        if(s.length()>0) {
            dcsGlobals.dcsEROHops.load(s.split(" "));
        }
    }
    
    public static void toggleFiber() {
        dcsGlobals.dcsFibers.setHidden(!dcsGlobals.dcsFibers.isHidden());
        dcsGlobals.currMapPane.repaint();
    }
    
    public static void toggleEROs() {
        dcsGlobals.dcsEROHops.setHidden(!dcsGlobals.dcsEROHops.isHidden());
        dcsGlobals.currMapPane.repaint();
    }
}


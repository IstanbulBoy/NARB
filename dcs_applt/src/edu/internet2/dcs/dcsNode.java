/*
 * dcsNode.java
 *
 * Created on October 31, 2007, 10:40 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.dcs;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.geom.Arc2D;
import java.awt.geom.Rectangle2D;
import java.util.prefs.Preferences;

/**
 *
 * @author jflidr
 */
public class dcsNode extends Arc2D.Double {
    private Preferences prefs;
    
    private String id = "";
    private int nodeR = -1;
    private double cx, cy;
    
    private String sysName = "";
    private String ctrlIP = "";
    private String rtrIDIP = "";
    
    private dcsNodeShadow shadow = null;
    private dcsNodeTip nodeTip = null;
    
    private final Color HIGHLIGHT_COLOR = new Color(255, 90, 90, 255);
    private final Color INSET_COLOR = new Color(255, 190, 90, 255);
    private final Color CD_COLOR = new Color(108, 128, 106, 255);
    private final Color SHADOW_COLOR = new Color(0, 0, 0, 100);
    private final Color NODE_OUTL_COLOR = new Color(0, 0, 0, 127);
    private final Color NODE_TIP_COLOR = new Color(255,255,102,222);
    private final Color NODE_TIP_FONT_COLOR = new Color(0, 0, 0, 255);
    private final Color NODE_TIP_EM_FONT_COLOR = new Color(222, 0, 0, 255);
    private final Color TIP_OUTL_COLOR = new Color(55, 55, 55, 255);
    
    private boolean isHighlighted = false;
    
    /** Creates a new instance of dcsNode */
    public dcsNode(int nx, int ny) {
        int idx = 0;
        nodeR = 7;
        cx = nx-3; cy = ny-3;
        
        idx = dcsGlobals.dcsNodes.getIdx();
        prefs = dcsGlobals.prefsRoot.node("/DcsMap/Nodes/Node_" + idx);
        id = prefs.name();
        prefs.put("id", id);
        
        prefs.putDouble("center_x", cx);
        prefs.putDouble("center_y", cy);
        
        shadow = new dcsNodeShadow();
        nodeTip = new dcsNodeTip();
    }
    
    public void paint(Graphics g) {
        Color currentColor;
        Graphics2D g2d = (Graphics2D)g;
        start = 0;
        extent = 360;
        int type = Arc2D.OPEN;
        
        shadow.paint(g);
        nodeTip.paint(g);
        setArcByCenter(cx, cy, nodeR, start, extent, type);
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        
        g2d.setPaint(NODE_OUTL_COLOR);
        g2d.setStroke(new BasicStroke(4));
        g2d.draw(g2d.getStroke().createStrokedShape(this));
        
        currentColor = CD_COLOR;
        g2d.setPaint(currentColor);
        g2d.fill(this);
        g2d.draw(this);
    }
    
    //getters
    
    public String getID() {
        return id;
    }
    
    //setters
    public void updateCoordsPrefs() {
        prefs.putDouble("center_x", cx);
        prefs.putDouble("center_y", cy);
    }
    
    public void setHighlight(boolean b) {
        boolean emph = false;
        isHighlighted = b;
        if(b) {
            displayNodeTip(sysName, emph);
        } else
            hideNodeTip();
    }
    
    //queries
    public boolean isHighlighted() {
        return isHighlighted;
    }
    
    //visual methods
    void moveBy(int dx, int dy) {
        cx -= dx; cy -= dy;
    }
    
    public void toggleHighlight() {
        boolean emph = false;
        if(isHighlighted) {
            isHighlighted = false;
            hideNodeTip();
        } else {
            int n = dcsGlobals.dcsNodes.getNumHihlights();
            if(n>=2) {
                dcsGlobals.dcsNodes.normHighlits();
            }
            isHighlighted = true;
            displayNodeTip(sysName, emph);
        }
    }
    
    private void displayNodeTip(String s, boolean emph) {
        nodeTip.setMsg(s);
        nodeTip.setEmph(emph);
        nodeTip.setVisible(true);
    }
    
    private void hideNodeTip() {
        nodeTip.setVisible(false);
    }
    
    // private classes
    
    private class dcsNodeShadow extends Arc2D.Double {
        
        dcsNodeShadow() {
        }
        
        public void paint(Graphics g) {
            Graphics2D g2d = (Graphics2D)g;
            start = 0;
            extent = 360;
            int type = Arc2D.OPEN;
            
            this.setArcByCenter(cx+2, cy+1, nodeR*1.2, start, extent, type);
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            g2d.setPaint(SHADOW_COLOR);
            g2d.fill(this);
            g2d.draw(this);
        }
    }
    
    private class dcsNodeTip extends Rectangle2D.Double {
        private String msg;
        private boolean isVisible = false;
        private boolean isEmph = false;
        
        public dcsNodeTip() {
            msg = "";
        }
        
        public void paint(Graphics g) {
            if(isVisible) {
                Graphics2D g2d = (Graphics2D)g;
                Font f = new Font("Courier", Font.PLAIN, 9);
                Rectangle2D sb = f.getStringBounds(msg, g2d.getFontRenderContext());
                
                Rectangle2D shadow = new Rectangle2D.Double(cx - (sb.getWidth() + 4)/2 + 1, cy + nodeR + 9, sb.getWidth() + 5, sb.getHeight() + 5);
                g2d.setPaint(SHADOW_COLOR);
                g2d.fill(shadow);
                g2d.draw(shadow);
                
                setFrame(cx - (sb.getWidth() + 4)/2, cy + nodeR + 8, sb.getWidth() + 4, sb.getHeight() + 4);
                g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                
                g2d.setPaint(TIP_OUTL_COLOR);
                g2d.setStroke(new BasicStroke(1));
                g2d.draw(g2d.getStroke().createStrokedShape(this));
                
                g2d.setPaint(NODE_TIP_COLOR);
                g2d.fill(this);
                g2d.draw(this);
                
                if(isEmph)
                    g2d.setPaint(NODE_TIP_EM_FONT_COLOR);
                else
                    g2d.setPaint(NODE_TIP_FONT_COLOR);
                g2d.setFont(f);
                g2d.drawString(msg, (int)(cx - (sb.getWidth() + 4)/2 + 2), (int)cy + nodeR + 8 + 9);
            }
        }
        
        public void setMsg(String s) {
            msg = s;
        }
        
        private void setVisible(boolean b) {
            isVisible = b;
        }
        
        private void setEmph(boolean emph) {
            isEmph = emph;
        }
    }
}

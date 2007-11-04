/*
 * dcsFiber.java
 *
 * Created on November 2, 2007, 7:22 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.dcs;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.geom.CubicCurve2D;
import java.util.prefs.Preferences;

/**
 *
 * @author jflidr
 */
public class dcsFiber extends CubicCurve2D.Double {
    
    private dcsNode endPoint1 = null;
    private dcsNode endPoint2 = null;
    
    private String id;
    
    private final Color PHY_LH_COLOR = new Color(255, 217, 0, 220);
    private final Color PHY_SH_COLOR = new Color(255, 127, 0, 220);
    private final Color SHADOW_COLOR = new Color(64, 64, 64, 90);
    private final double BETA = Math.PI/16;
    private dcsFiberShadow shadow = null;
    
    /** Creates a new instance of dcsFiber */
    public dcsFiber(dcsNode[] p) {
        endPoint1 = p[0];
        endPoint2 = p[1];
        endPoint1.addFiber(this);
        endPoint2.addFiber(this);
        int idx = dcsGlobals.dcsFibers.getIdx();
        id = "Fiber_" + idx;
        shadow = new dcsFiberShadow(this);
    }
    
    public void paint(Graphics g) {
        double d;
        int f = dcsGlobals.currMapPane.isZoomedOut()?2:1;
                
        Color currentColor;
        Graphics2D g2d = (Graphics2D)g;
        int dx1 = 0, dy1 = 0;
        int dx2 = 0, dy2 = 0;
        
        int p;
        
        double x1, y1, cx1, cy1;
        double x2, y2, cx2, cy2;
        dcsNode ep1 = endPoint1;
        dcsNode ep2 = endPoint2;
        
        cx1 = ep1.getCenterX() - 20/f;
        cy1 = ep1.getCenterY();
        cx2 = ep2.getCenterX() - 20/f;
        cy2 = ep2.getCenterY();
        
        int r1 = 3;
        int r2 = 3;
        
        cx1 += dx1;
        cy1 += dy1;
        cx2 += dx2;
        cy2 += dy2;
        
        double alpha = Math.atan((cy2 - cy1)/(cx2 - cx1));
        
        //java trigon functs suck, so swap the notion of the angle if necessary
        if(cx1 > cx2) {
            p = -1;
        } else {
            p = 1;
        }
        
        x1 = cx1 + p*(r1+7) * Math.cos(alpha);
        y1 = cy1 + p*(r1+7) * Math.sin(alpha);
        x2 = cx2 - p*(r2+7) * Math.cos(alpha);
        y2 = cy2 - p*(r2+7) * Math.sin(alpha);
        
        d = Math.sqrt((y2 - y1)*(y2 - y1) + (x2 - x1)*(x2 - x1));
        
        ctrlx1 = x1 + p*((d/4) * Math.cos((alpha + BETA)));
        ctrly1 = y1 + p*((d/4) * Math.sin((alpha + BETA)));
        ctrlx2 = x2 - p*((d/4) * Math.cos((alpha + BETA)));
        ctrly2 = y2 - p*((d/4) * Math.sin((alpha + BETA)));
        
        setCurve(x1, y1, ctrlx1, ctrly1,
                ctrlx2, ctrly2, x2, y2);
        shadow.paint(g);
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        
        g2d.setStroke(new BasicStroke(2));
        g2d.draw(g2d.getStroke().createStrokedShape(this));
        
        currentColor = PHY_LH_COLOR;
        
        g2d.setPaint(currentColor);
        g2d.draw(this);
    }
    
    //getters
    public String getID() {
        return id;
    }
    
    private class dcsFiberShadow extends CubicCurve2D.Double {
        dcsFiber caster;
        public dcsFiberShadow(dcsFiber l) {
            caster = l;
        }
        
        public void paint(Graphics g) {
            Graphics2D g2d = (Graphics2D)g;
            Color currentColor;
            double x1 = caster.x1; double y1 = caster.y1; double x2 = caster.x2; double y2 = caster.y2;
            double ctrlx1 = caster.ctrlx1; double ctrly1 = caster.ctrly1; double ctrlx2 = caster.ctrlx2; double ctrly2 = caster.ctrly2;
            setCurve(x1+2, y1+2, ctrlx1, ctrly1, ctrlx2, ctrly2, x2+2, y2+2);
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            currentColor = SHADOW_COLOR;
            
            g2d.setPaint(currentColor);
            
            g2d.draw(this);
        }
    }
}

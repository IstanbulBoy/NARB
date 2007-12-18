/*
 * dcsLSP.java
 *
 * Created on November 6, 2007, 9:30 PM
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
import java.awt.Stroke;
import java.awt.geom.Line2D;

/**
 *
 * @author jflidr
 */
public class dcsLSP extends Line2D.Double {
    
    private dcsNode endPoint1;
    private dcsNode endPoint2;
    
    private String id;
    
    private dcsLSPShadow shadow;
    private final Color LSP_COLOR = new Color(0, 128, 255, 250);
    private final Color SHADOW_COLOR = new Color(64, 64, 64, 90);
    private int bunchSize = 1;
    
    private boolean isHidden = false;;
    private boolean isLeader = true;
    
    private int strokeWeight = 2;
    
    /** Creates a new instance of dcsLSP */
    public dcsLSP(String s, dcsNode[] p) {
        endPoint1 = p[0];
        endPoint2 = p[1];
        endPoint1.addLSP(this);
        endPoint2.addLSP(this);
        id = s;
        shadow = new dcsLSPShadow(this);
    }
    
    public void paint(Graphics g) {
        if(isHidden)
            return;
        if(!isLeader)
            return;
        
        Color currentColor;
        Graphics2D g2d = (Graphics2D)g;
        Stroke str = g2d.getStroke();
        int f = dcsGlobals.currMapPane.isZoomedOut()?2:1;
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
        
        //java trigon functs suck, so swap the parity of the angle if necessary
        if(cx1 > cx2) {
            p = -1;
        } else {
            p = 1;
        }
        
        x1 = cx1 + p*(r1+7) * Math.cos(alpha);
        y1 = cy1 + p*(r1+7) * Math.sin(alpha);
        x2 = cx2 - p*(r2+7) * Math.cos(alpha);
        y2 = cy2 - p*(r2+7) * Math.sin(alpha);
        
        int cnt;
        double o, s;
        
        if(bunchSize>3) {
            strokeWeight = 9;
            cnt = 1;
            o = 0;
            s = 0;
        } else {
            strokeWeight = 2;
            cnt = bunchSize;
            if(cnt==1) {
                o = 0;
                s = 0;
            } else if(cnt==2) {
                o = 3;
                s = 6;
            } else {
                o = 7;
                s = 5;
            }
        }
        
        double dx, dy, xs, ys;
        dx = p*o*Math.cos(Math.PI/2 - alpha);
        dy = p*o*Math.sin(Math.PI/2 - alpha);
        xs = p*s*Math.cos(Math.PI/2 - alpha);
        ys = p*s*Math.sin(Math.PI/2 - alpha);
        x1 -= dx; x2 -= dx;
        y1 += dy; y2 += dy;
        for (int i = 0; i < cnt; i++) {
            setLine(x1, y1, x2, y2);
            x1 += xs; y1 -= ys;
            x2 += xs; y2 -= ys;       
            shadow.paint(g);
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            
            g2d.setStroke(new BasicStroke(strokeWeight));
            if(strokeWeight == 2) {
                currentColor = SHADOW_COLOR;
                g2d.setPaint(currentColor);
                g2d.draw(g2d.getStroke().createStrokedShape(this));
            }
            currentColor = LSP_COLOR;            
            g2d.setPaint(currentColor);
                
            g2d.draw(this);
            g2d.setStroke(str);
        }
    }
    
    //getters
    public boolean isHidden() {
        return isHidden;
    }
    
    public boolean isLeader() {
        return isLeader;
    }
    
    public String getID() {
        return id;
    }
    
    public dcsNode[] getEPs() {
        dcsNode[] np = {endPoint1, endPoint2};
        return np;
    }
    
//setters
    public void setHidden(boolean b) {
        isHidden = b;
    }
    
    void setLeader(boolean b) {
        isLeader = b;
    }
    
    public void setBunchSize(int i) {
        bunchSize = i;
    }
    
    public boolean connects(dcsNode[] np) {
        return connects(np[0], np[1]);
    }
    
    public boolean connects(dcsNode n1, dcsNode n2) {
        return ((endPoint1 == n1) && (endPoint2 == n2)) || ((endPoint2 == n1) && (endPoint1 == n2));
    }
    
    public void decBunchSize() {
        bunchSize--;
    }
    
    //private classes
    private class dcsLSPShadow extends Line2D.Double {
        dcsLSP caster;
        public dcsLSPShadow(dcsLSP l) {
            caster = l;
        }
        
        public void paint(Graphics g) {
            Graphics2D g2d = (Graphics2D)g;
            Color currentColor;
            Stroke s = g2d.getStroke();
            
            double x1 = caster.x1; double y1 = caster.y1; double x2 = caster.x2; double y2 = caster.y2;
            setLine(x1+2, y1+2, x2+2, y2+2);
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            currentColor = SHADOW_COLOR;
            g2d.setPaint(currentColor);
            g2d.setStroke(new BasicStroke(strokeWeight));
            
            g2d.draw(this);
            g2d.setStroke(s);
        }
    }
}

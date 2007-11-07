/*
 * dcsMap.java
 *
 * Created on October 30, 2007, 10:35 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.dcs;

import java.awt.GridLayout;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.net.URL;
import java.util.prefs.Preferences;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JViewport;
import javax.swing.ScrollPaneConstants;

/**
 *
 * @author jflidr
 */
public class dcsMap extends JPanel implements MouseListener, MouseMotionListener {
    
    private Preferences prefs;
    private JScrollPane scroller;
    
    private int mouseX = 0;
    private int mouseY = 0;
    private Point viewPoint = null;
    private boolean mapDragged = false;
    
    /** Creates a new instance of dcsMap */
    public dcsMap() {
        super(new GridLayout());
        setSize(dcsGlobals.DCS_MAP_W, dcsGlobals.DCS_MAP_H);
        setLocation(0, 0);
        prefs = dcsGlobals.prefsRoot.node("/edu/internet2/dcs/DcsMap");
        URL img_url = dcsApplt.class.getResource("/edu/internet2/dcs/images/logic_bg.jpg");
        Image img = Toolkit.getDefaultToolkit().getImage(img_url);
        
        try {
            MediaTracker tracker = new MediaTracker(this);
            tracker.addImage(img, 0);
            tracker.waitForID(0);
        } catch ( Exception e ) {}
        
        img_url = dcsApplt.class.getResource("/edu/internet2/dcs/images/logic_bg_sm.jpg");
        Image img_sm = Toolkit.getDefaultToolkit().getImage(img_url);
        try {
            MediaTracker tracker = new MediaTracker(this);
            tracker.addImage(img_sm, 0);
            tracker.waitForID(0);
        } catch ( Exception e ) {}
        
        dcsGlobals.currMapPane = new dcsMapPane(img, img_sm, this);
        scroller = new javax.swing.JScrollPane(dcsGlobals.currMapPane);
        
        scroller.setMaximumSize(new java.awt.Dimension(dcsGlobals.DCS_MAP_W, dcsGlobals.DCS_MAP_H));
        scroller.setMinimumSize(new java.awt.Dimension(dcsGlobals.DCS_MAP_W, dcsGlobals.DCS_MAP_H));
        scroller.setPreferredSize(new java.awt.Dimension(dcsGlobals.DCS_MAP_W, dcsGlobals.DCS_MAP_H));
        scroller.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        scroller.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_NEVER);
        
        //position the map pane
        scroller.getViewport().setViewPosition(new Point(
                prefs.getInt("dcsmap_view_x", 1668),
                prefs.getInt("dcsmap_view_y", 1069)
                ));
        
        add(scroller);
    }
    
    public void setViewPoint(Point p) {
        if(scroller != null) {
            JViewport v = scroller.getViewport();
            Rectangle r = v.getViewRect();

            if(p.x < 0)
                p.x = 0;
            if((p.x + r.width) > dcsGlobals.currMapPane.getWidth())
                p.x = dcsGlobals.currMapPane.getWidth() - r.width;
            if(p.y < 0)
                p.y = 0;
            if((p.y + r.height) > dcsGlobals.currMapPane.getHeight())
                p.y = dcsGlobals.currMapPane.getHeight() - r.height;
            v.setViewPosition(p);
            viewPoint = p;
            prefs.putInt("dcsmap_view_x", viewPoint.x);
            prefs.putInt("dcsmap_view_y", viewPoint.y);
        }
    }
    
    public Point getViewPoint() {
        return scroller.getViewport().getViewPosition();
    }
    
    public void mouseClicked(MouseEvent e) {
    }
    
    public void mousePressed(MouseEvent e) {
        mouseX = e.getX();
        mouseY = e.getY();
        
        if(!isPopUp(e)) {
            
        } else {
            
        }
    }
    
    public void mouseReleased(MouseEvent e) {
        if(!isPopUp(e)) {
            if(mapDragged) {
                prefs.putInt("dcsmap_view_x", viewPoint.x);
                prefs.putInt("dcsmap_view_y", viewPoint.y);
                mapDragged = false;
            }
        }
    }
    
    public void mouseEntered(MouseEvent e) {
    }
    
    public void mouseExited(MouseEvent e) {
    }
    
    public void mouseDragged(MouseEvent e) {
        if(!isPopUp(e)) {
            Point p;
            if(scroller != null) {
                mapDragged = true;
                JViewport v = scroller.getViewport();
                Rectangle r = v.getViewRect();
                p = v.getViewPosition();
                p.translate(mouseX - e.getX(), mouseY - e.getY());
                
                if(p.x < 0) p.x = 0; if(p.y < 0) p.y = 0;
                if(p.x + r.width > dcsGlobals.currMapPane.getWidth()) p.x = dcsGlobals.currMapPane.getWidth() - r.width;
                if(p.y + r.height > dcsGlobals.currMapPane.getHeight()) p.y = dcsGlobals.currMapPane.getHeight() - r.height;
                v.setViewPosition(p);
                viewPoint = p;
            }
        }
    }
    
    public void mouseMoved(MouseEvent e) {
    }
    
    private boolean isPopUp(MouseEvent e) {
        return (e.isPopupTrigger() ||
                ((e.getModifiersEx() & e.CTRL_DOWN_MASK) == e.CTRL_DOWN_MASK));
    }
}

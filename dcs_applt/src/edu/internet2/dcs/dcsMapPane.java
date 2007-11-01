/*
 * dcsMapPane.java
 *
 * Created on October 30, 2007, 10:38 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.dcs;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.util.prefs.Preferences;
import javax.swing.JLayeredPane;
import javax.swing.Scrollable;
import javax.swing.SwingConstants;

/**
 *
 * @author jflidr
 */
public class dcsMapPane extends JLayeredPane implements Scrollable, MouseListener {
    
    private Preferences prefs = null;
    
    private int iw;
    private int ih;
    private BufferedImage dcsCurrMapImg = null;
    private int maxUnitIncrement = 1;
    
    private int mouseX = 0;
    private int mouseY = 0;
    
    private dcsNode pressedNode = null;
    
    /** Creates a new instance of dcsMapPane */
    public dcsMapPane(Image img, dcsMap m) {
        prefs = dcsGlobals.prefsRoot.node("/DcsMap");
        prefs.put("name", "usa");
        iw = img.getWidth(this);
        ih = img.getHeight(this);
        setAutoscrolls(true);
        addMouseListener(m);
        addMouseMotionListener(m);
        addMouseListener(this);
        setOpaque(true);
        
        setPreferredSize(new Dimension(iw, ih));
        dcsCurrMapImg = new BufferedImage(iw, ih, BufferedImage.TYPE_INT_RGB);
        if(dcsCurrMapImg != null) {
            Graphics2D g2d = dcsCurrMapImg.createGraphics();
            g2d.drawImage(img,0,0,this);
        }
    }
    
    protected void paintComponent(Graphics g) {
        dcsNode n = null;
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);
        g2d.setRenderingHint(RenderingHints.KEY_RENDERING,
                RenderingHints.VALUE_RENDER_QUALITY);
        if(dcsCurrMapImg != null) {
            g2d.drawImage(dcsCurrMapImg,0,0,this);
        }
        for(int i = 0; i < dcsGlobals.dcsNodes.size(); i++) {
            n = dcsGlobals.dcsNodes.get(i);
            n.paint(g);
        }
    }
    
    public Dimension getPreferredSize() {
        return super.getPreferredSize();
    }
    
    public Dimension getPreferredScrollableViewportSize() {
        return getPreferredSize();
    }
    
    public int getScrollableUnitIncrement(Rectangle visibleRect, int orientation, int direction) {
        //Get the current position.
        int currentPosition = 0;
        if (orientation == SwingConstants.HORIZONTAL) {
            currentPosition = visibleRect.x;
        } else {
            currentPosition = visibleRect.y;
        }
        
        //Return the number of pixels between currentPosition
        //and the nearest tick mark in the indicated direction.
        if (direction < 0) {
            int newPosition = currentPosition -
                    (currentPosition / maxUnitIncrement)
                    * maxUnitIncrement;
            return (newPosition == 0) ? maxUnitIncrement : newPosition;
        } else {
            return ((currentPosition / maxUnitIncrement) + 1)
            * maxUnitIncrement
                    - currentPosition;
        }
    }
    
    public int getScrollableBlockIncrement(Rectangle visibleRect, int orientation, int direction) {
        if (orientation == SwingConstants.HORIZONTAL) {
            return visibleRect.width - maxUnitIncrement;
        } else {
            return visibleRect.height - maxUnitIncrement;
        }
    }
    
    public boolean getScrollableTracksViewportWidth() {
        return false;
    }
    
    public boolean getScrollableTracksViewportHeight() {
        return false;
    }
    
    
    public void setMaxUnitIncrement(int pixels) {
        maxUnitIncrement = pixels;
    }
    
    public void mouseClicked(MouseEvent e) {
        mouseX = e.getX();
        mouseY = e.getY();
        dcsNode n;
        
        if(!isPopUp(e)) {
            for(int i = 0; i < dcsGlobals.dcsNodes.size(); i++) {
                n = dcsGlobals.dcsNodes.get(i);
                if(n.contains(mouseX, mouseY)) {
                    displayInfo(n);
                }
            }
        }
    }
    
    public void mousePressed(MouseEvent e) {
        mouseX = e.getX();
        mouseY = e.getY();
        
        if(isPopUp(e)) {
            evaluatePopUp();
        }
    }
    
    public void mouseReleased(MouseEvent e) {
    }
    
    public void mouseEntered(MouseEvent e) {
    }
    
    public void mouseExited(MouseEvent e) {
    }
    
    private boolean isPopUp(MouseEvent e) {
        return (e.isPopupTrigger() ||
                ((e.getModifiersEx() & e.CTRL_DOWN_MASK) == e.CTRL_DOWN_MASK));
    }
    
    private void evaluatePopUp() {
        dcsNode n;
        
        for(int i = 0; i < dcsGlobals.dcsNodes.size(); i++) {
            n = dcsGlobals.dcsNodes.get(i);
            if(n.contains(mouseX, mouseY)) {
                pressedNode = n;
            }
        }
    }
    
    private void displayInfo(dcsNode n) {
        
    }
}

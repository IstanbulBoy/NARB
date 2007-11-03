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
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.util.prefs.Preferences;
import javax.swing.JLayeredPane;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.Scrollable;
import javax.swing.SwingConstants;

/**
 *
 * @author jflidr
 */
public class dcsMapPane extends JLayeredPane implements Scrollable, MouseListener {
    
    private Preferences prefs = null;
    private JPopupMenu popMenuMap = new JPopupMenu();
    private JMenuItem popMapZoom = new JMenuItem("Zoom Out");
    private JMenuItem popMapReset = new JMenuItem("Reset");
    
    private int iw;
    private int ih;
    private BufferedImage dcsCurrMapImgBig = null;
    private BufferedImage dcsCurrMapImgSm = null;
    private int maxUnitIncrement = 1;
    
    private int mouseX = 0;
    private int mouseY = 0;
    
    private dcsFiberLayer fiberLayer = null;
    
    private boolean isZoomedOut = false;
    private Image img_sm;
    private Image img_big;
    
    /** Creates a new instance of dcsMapPane */
    public dcsMapPane(Image i0, Image i1, dcsMap m) {
        img_big = i0;
        img_sm = i1;
        prefs = dcsGlobals.prefsRoot.node("/edu/internet2/dcs/DcsMap");
        isZoomedOut = prefs.getBoolean("zoomOut", false);
        prefs.putBoolean("zoomOut", isZoomedOut);
        
        setAutoscrolls(true);
        addMouseListener(m);
        addMouseMotionListener(m);
        addMouseListener(this);
        setOpaque(true);
        
        dcsCurrMapImgBig = new BufferedImage(img_big.getWidth(this), img_big.getHeight(this), BufferedImage.TYPE_INT_RGB);
        dcsCurrMapImgSm = new BufferedImage(img_sm.getWidth(this), img_sm.getHeight(this), BufferedImage.TYPE_INT_RGB);
        
        fiberLayer = new dcsFiberLayer(this);
        add(fiberLayer, JLayeredPane.DEFAULT_LAYER);
        formatMap();
        
        popMapZoom.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                zoomMap();
            }
        });
        if(isZoomedOut)
            popMapZoom.setText("Zoom In");
        else
            popMapZoom.setText("Zoom Out");
        
        popMapReset.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                resetMap();
            }
        });
        
        popMenuMap.add(popMapZoom);
        popMenuMap.add(popMapReset);
    }
    
    protected void paintComponent(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);
        g2d.setRenderingHint(RenderingHints.KEY_RENDERING,
                RenderingHints.VALUE_RENDER_QUALITY);
        if(isZoomedOut) {
            if(dcsCurrMapImgSm != null) {
                g2d.drawImage(dcsCurrMapImgSm,0,0,this);
            }
        } else {
            if(dcsCurrMapImgBig != null) {
                g2d.drawImage(dcsCurrMapImgBig,0,0,this);
            }
        }
    }
    
    private void formatMap() {
        Image img;
        if(isZoomedOut) {
            img = img_sm;
        } else {
            img = img_big;
        }
        
        iw = img.getWidth(this);
        ih = img.getHeight(this);
        setPreferredSize(new Dimension(iw, ih));
        fiberLayer.setSize(new Dimension(iw, ih));
        if(isZoomedOut) {
            if(dcsCurrMapImgSm != null) {
                Graphics2D g2d = dcsCurrMapImgSm.createGraphics();
                g2d.drawImage(img_sm,0,0,this);
            }
        } else {
            if(dcsCurrMapImgBig != null) {
                Graphics2D g2d = dcsCurrMapImgBig.createGraphics();
                g2d.drawImage(img_big,0,0,this);
            }
        }
        repaint();
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
            dcsGlobals.dcsNodes.removeHighlights();
            repaint();
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
        popMenuMap.show(this, mouseX, mouseY);
    }
    
    private void zoomMap() {
        Point p;
        isZoomedOut = !isZoomedOut;
        if(isZoomedOut) {
            popMapZoom.setText("Zoom In");
            dcsGlobals.dcsNodes.scaleDown();
            prefs.putBoolean("zoomOut", isZoomedOut);
            p = dcsGlobals.currMap.getViewPoint();
            p.translate(-p.x/2, -p.y/2);
            dcsGlobals.currMap.setViewPoint(p);
        } else {
            popMapZoom.setText("Zoom Out");
            dcsGlobals.dcsNodes.scaleUp();
            prefs.putBoolean("zoomOut", isZoomedOut);
            p = dcsGlobals.currMap.getViewPoint();
            p.translate(p.x, p.y);
            dcsGlobals.currMap.setViewPoint(p);
        }
        formatMap();
    }
    
    private void displayInfo(dcsNode n) {
        
    }
    
    public boolean isZoomedOut() {
        return isZoomedOut;
    }
    
    private void resetMap() {
        dcsGlobals.dcsNodes.resetView();
        repaint();
    }
}

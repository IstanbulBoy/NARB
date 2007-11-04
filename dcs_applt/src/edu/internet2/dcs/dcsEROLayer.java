/*
 * dcsEROLayer.java
 *
 * Created on November 3, 2007, 7:00 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.dcs;

import java.awt.Graphics;
import javax.swing.JPanel;

/**
 *
 * @author jflidr
 */
public class dcsEROLayer extends JPanel {
    
    /**
     * Creates a new instance of dcsEROLayer
     */
    public dcsEROLayer(dcsMapPane mp) {
        setLocation(0, 0);
        setSize(mp.getPreferredSize());
        setOpaque(false);
        setVisible(true);
    }
    
    protected void paintComponent(Graphics g) {
        if(!dcsGlobals.dcsEROHops.isHidden()) {
            for(int i = 0; i < dcsGlobals.dcsEROHops.size(); i++) {
                dcsGlobals.dcsEROHops.get(i).paint(g);
            }
        }
    }
}

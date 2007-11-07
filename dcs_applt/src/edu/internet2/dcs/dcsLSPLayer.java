/*
 * dcsLSPLayer.java
 *
 * Created on November 6, 2007, 10:37 PM
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
public class dcsLSPLayer extends JPanel {
    
    /** Creates a new instance of dcsLSPLayer */
    public dcsLSPLayer(dcsMapPane mp) {
        setLocation(0, 0);
        setSize(mp.getPreferredSize());
        setOpaque(false);
        setVisible(true);
    }
    
    protected void paintComponent(Graphics g) {
        if(!dcsGlobals.dcsLSPs.isHidden()) {
            for(int i = 0; i < dcsGlobals.dcsLSPs.size(); i++) {
                dcsGlobals.dcsLSPs.get(i).paint(g);
            }
        }
    }
}

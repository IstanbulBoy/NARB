/*
 * dcsFiberLayer.java
 *
 * Created on November 2, 2007, 10:18 AM
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
public class dcsFiberLayer extends JPanel {
    
    /** Creates a new instance of dcsFiberLayer */
    public dcsFiberLayer(dcsMapPane mp) {
        setLocation(0, 0);
        setSize(mp.getPreferredSize());
        setOpaque(false);
        setVisible(true);
    }
    
    protected void paintComponent(Graphics g) {
        if(!dcsGlobals.dcsFibers.isHidden()) {
            for(int i = 0; i < dcsGlobals.dcsFibers.size(); i++) {
                dcsGlobals.dcsFibers.get(i).paint(g);
            }
        }
    }
}

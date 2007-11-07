/*
 * dcsNodeSet.java
 *
 * Created on October 31, 2007, 10:41 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.dcs;

import java.util.Arrays;
import java.util.Comparator;
import java.util.Vector;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;
import javax.swing.JLayeredPane;

/**
 *
 * @author jflidr
 */
public class dcsNodeSet extends Vector<dcsNode> {
    
    private int nodeCount;
    private Preferences prefs;
    
    /** Creates a new instance of dcsNodeSet */
    public dcsNodeSet(int cap, int inc) {
        super(cap, inc);
        nodeCount = 0;
        //prefsManager must be initialized before astNodeSet
        prefs = dcsGlobals.prefsRoot.node("/edu/internet2/dcs/DcsMap/Nodes");
    }
    
    public void load() {
        int f = 1;
        if(dcsGlobals.currMapPane.isZoomedOut())
            f = 2;
        
        for (int i = 0; i < dcsDefaults.CDs.length; i++) {
            int x = prefs.node(dcsDefaults.CDs[i][0]).getInt("center_x", Integer.valueOf(dcsDefaults.CDs[i][1]));
            int y = prefs.node(dcsDefaults.CDs[i][0]).getInt("center_y", Integer.valueOf(dcsDefaults.CDs[i][2]));
            addNode(dcsDefaults.CDs[i][0], x/f, y/f, dcsDefaults.CDs[i]);
        }
    }
    
    public void addNode(String s, int x, int y, String[] sa) {
        dcsNode n = new dcsNode(s, x, y, sa[3]);
        for (int i = 4; i < sa.length; i++) {
            n.addTELink(sa[i]);
        }
        add(n);
    }
    
    private String[] getPrefNames() {
        try {
            return prefs.childrenNames();
        } catch (BackingStoreException ex) {
            ex.printStackTrace();
        }
        return null;
    }
    
    public int getIdx() {
        if(size() == 0)
            return 1;
        return Integer.parseInt(get(size()-1).getID().split("_")[1]) + 1;
    }
    
    int getNumHihlights() {
        dcsNode n;
        int ret = 0;
        for (int i = 0; i < this.size(); i++) {
            n = this.get(i);
            if(n.isHighlighted()) {
                ret++;
            }
        }
        return ret;
    }
    
    public void normHighlits() {
        dcsNode n;
        for (int i = 0; i < this.size(); i++) {
            n = this.get(i);
            if(n.isHighlighted()) {
                n.setHighlight(false);
                break;
            }
        }
    }
    
    public void removeHighlights() {
        dcsNode n;
        for (int i = 0; i < this.size(); i++) {
            n = this.get(i);
            if(n.isHighlighted()) {
                n.setHighlight(false);
            }
        }
    }
    
    public void addShowAll() {
        dcsNode n;
        for (int i = 0; i < size(); i++) {
            n = get(i);
            dcsGlobals.currMapPane.add(n, JLayeredPane.POPUP_LAYER);
            n.setVisible(true);
            dcsGlobals.currMapPane.add(n.getLabel(), JLayeredPane.POPUP_LAYER);
        }
    }
    
    public dcsNode[] getLinkedNodes(String s0, String s1) {
        if(!isTELinked(s0, s1))
            return null;
        dcsNode n0, n1;
        Vector<String> te_l0, te_l1;
        
        String tes0, tes1;
        for (int i = 0; i < size(); i++) {
            n0 = get(i);
            te_l0 = n0.getTELinkLabels();
            for (int j = i+1; j < size(); j++) {
                n1 = get(j);
                te_l1 = n1.getTELinkLabels();
                for (int k = 0; k < te_l0.size(); k++) {
                    tes0 = te_l0.get(k);
                    for (int l = 0; l < te_l1.size(); l++) {
                        tes1 = te_l1.get(l);
                        if((tes0.equals(s0) && tes1.equals(s1)) || (tes1.equals(s0) && tes0.equals(s1))) {
                            dcsNode ret[] = {n0, n1};
                            return ret;
                        }
                    }
                }
            }
        }
        return null;
    }
    
    public void fiberNodes() {
        boolean linkFound = false;
        dcsNode n0, n1;
        Vector<String> te_l0, te_l1;
        String tes0, tes1;
        for (int i = 0; i < size(); i++) {
            n0 = get(i);
            te_l0 = n0.getTELinkLabels();
            for (int j = i+1; j < size(); j++) {
                n1 = get(j);
                linkFound = false;
                te_l1 = n1.getTELinkLabels();
                for (int k = 0; k < te_l0.size(); k++) {
                    tes0 = te_l0.get(k);
                    for (int l = 0; l < te_l1.size(); l++) {
                        tes1 = te_l1.get(l);
                        if(isTELinked(tes0, tes1)) {
                            dcsNode[] np = {n0, n1};
                            String[] tep = {tes0, tes1};
                            dcsGlobals.dcsFibers.add(new dcsFiber(np, tep));
                            linkFound = true;
                            break;
                        }
                    }
                    if(linkFound) {
                        break;
                    }
                }
            }
        }
    }
    
    private boolean isTELinked(String s0, String s1) {
        int i8_0 = Integer.valueOf(s0.split("\\.")[3]);
        int i8_1 = Integer.valueOf(s1.split("\\.")[3]);
        int b0 = 4*(i8_0/4);
        int b1 = 4*(i8_1/4);
        return ((b0 == b1) && (i8_0 != i8_1) && (i8_0>b0) && (i8_0<(b0+4)) && (i8_1>b0) && (i8_1<(b0+4)));
    }
    
    public void scaleDown() {
        dcsNode n;
        for (int i = 0; i < size(); i++) {
            n = get(i);
            n.scaleDown();
            n.formatNode();
        }
    }
    
    public void scaleUp() {
        dcsNode n;
        for (int i = 0; i < size(); i++) {
            n = get(i);
            n.scaleUp();
            n.formatNode();
        }
    }
    
    public void resetView() {
        dcsNode n;
        int f = 1;
        if(dcsGlobals.currMapPane.isZoomedOut())
            f = 2;
        
        for (int i = 0; i < this.size(); i++) {
            n = get(i);
            for (int j = 0; j < dcsDefaults.CDs.length; j++) {
                if(n.getSysName().equals(dcsDefaults.CDs[j][0])) {
                    n.setCenterX(Integer.valueOf(dcsDefaults.CDs[j][1])/f);
                    n.setCenterY(Integer.valueOf(dcsDefaults.CDs[j][2])/f);
                    n.formatNode();
                    n.updateCoordsPrefs();
                    break;
                }
            }
        }
    }
}

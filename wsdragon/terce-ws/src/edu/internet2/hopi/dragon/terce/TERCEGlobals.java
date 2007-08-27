/*
 * TERCEGlobals.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce;

import java.util.Vector;

/**
 *
 * @author jflidr
 */
public class TERCEGlobals {
    
    public static strDesc tlvStrDescs = null;
    public static strDesc stlvStrDescs = null;
    
    /** Creates a new instance of TERCEGlobals */
    public TERCEGlobals() {
    }
    
    public static class strDesc extends Vector<tuple> {
        public strDesc() {
            
        }
        
        public void add(String s, int i) {
            add(new tuple(s, i));
        }
        
        public String getStr(long type) {
            tuple t;
            for (int i = 0; i < size(); i++) {
                t = get(i);
                if(t.getVal() == type)
                    return t.getDesc();
            }
            return "unknown";
        }
        
    }
    
    private static class tuple {
        private String desc = null;
        private int val = -1;
        private tuple(String s, int i) {
            desc = s;
            val = i;
        }
        
        private long getVal() {
            return val;
        }
        
        private String getDesc() {
            return desc;
        }
    }
    
}

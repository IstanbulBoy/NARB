/*
 * TERCEMessageProcessor.java
 *
 * Created on August 23, 2007, 11:45 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce;

import edu.internet2.hopi.dragon.terce.api.TERCEMessageHeader;
import edu.internet2.hopi.dragon.terce.tedb.LSA;

/**
 *
 * @author jflidr
 */
public class TERCEMessageProcessor implements Runnable {
    
    private byte[] data = null;
    private byte[] hdr = null;
    private TERCEMessageHeader mh;
    private TERCECore core;
    
    /**
     * Creates a new instance of TERCEMessageProcessor
     */
    public TERCEMessageProcessor(TERCECore tc, byte[] h, byte[] d) {
        core = tc;
        hdr = h;
        data = d;
        mh = new TERCEMessageHeader(hdr);
    }
    
    public void run() {
        /* open the async channel */
        if(mh.isSyncInit()) {
            core.openAsync((int)mh.getTag());
        }
        else if(mh.isSyncInsert()) {
            LSA lsa;
            if(data != null) {      
                try {
                    lsa = new LSA(data);
                } catch (TERCELSAException ex) {
                    System.err.println("SyncInsert: " + ex.getMessage());
                    return;
                }
                core.addLSA(lsa);
            }
        }
        else if(mh.isSyncQuery()) {
            
        }
    }
    
}

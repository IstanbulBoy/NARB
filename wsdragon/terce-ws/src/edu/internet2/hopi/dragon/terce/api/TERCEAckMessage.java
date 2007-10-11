/*
 * TERCEAckMessage.java
 *
 * Created on August 24, 2007, 12:37 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package edu.internet2.hopi.dragon.terce.api;

import edu.internet2.hopi.dragon.terce.TERCECore;


/**
 *
 * @author jflidr
 */
public class TERCEAckMessage extends TERCEMessage {
    private TERCEMessageHeader mh;
    private long type;
    private long seqn;

    /**
     * Creates a new instance of TERCEAckMessage
     */
    public TERCEAckMessage(TERCECore s, long n) {
        super(s);
        seqn = n;
        type = (TERCEMessageHeader.RCE_MSG_TERCE_TOPO_SYNC << 8) |
            TERCEMessageHeader.ACT_ACK;
        mh = new TERCEMessageHeader(type, 0, seqn);
        mh.insertChkSum();
        msg = mh.toBytes();
    }
}

/*
 * TERCEResponder.java
 *
 * Created on August 23, 2007, 11:47 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package edu.internet2.hopi.dragon.terce;

import edu.internet2.hopi.dragon.terce.api.TERCEAckMessage;
import edu.internet2.hopi.dragon.terce.api.TERCEMessage;
import edu.internet2.hopi.dragon.terce.api.TERCEMessageHeader;


/**
 *
 * @author jflidr
 */
public class TERCEResponder implements Runnable {
    private byte[] hdr = null;
    private byte[] data = null;
    private TERCECore server;
    private TERCEMessageHeader mh;

    /** Creates a new instance of TERCEResponder */
    public TERCEResponder(TERCECore s, byte[] h, byte[] d) {
        server = s;
        hdr = h;
        data = d;
        mh = new TERCEMessageHeader(hdr);
    }

    public void run() {
        long seqn;
        long type;

        if (mh.isValid()) {
            type = mh.getMessageType();
            seqn = mh.getSequenceNumber();

            if (type == ((TERCEMessageHeader.RCE_MSG_TERCE_TOPO_SYNC << 8) |
                    TERCEMessageHeader.ACT_INIT)) {
                TERCEAckMessage reply = new TERCEAckMessage(server, seqn);
                reply.send();
            }

            if (type == ((TERCEMessageHeader.RCE_MSG_TERCE_TOPO_SYNC << 8) |
                    TERCEMessageHeader.ACT_QUERY)) {
                TERCEAckMessage reply = new TERCEAckMessage(server, seqn);
                reply.send();
            }

            if (type == ((TERCEMessageHeader.RCE_MSG_TERCE_TOPO_SYNC << 8) |
                    TERCEMessageHeader.ACT_INSERT)) {
                TERCEAckMessage reply = new TERCEAckMessage(server, seqn);
                reply.send();
            }
        } else {
            System.err.println("invalid mesage");
        }
    }
}

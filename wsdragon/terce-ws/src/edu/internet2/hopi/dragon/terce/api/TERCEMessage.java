/*
 * TERCEMessage.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce.api;

import edu.internet2.hopi.dragon.terce.TERCECore;
import java.io.IOException;
import java.io.OutputStream;

/**
 *
 * @author jflidr
 */
public abstract class TERCEMessage {
    
    protected byte[] msg = null;
    private TERCECore core;
    private OutputStream os;
    
    /** Creates a new instance of TERCEMessage */
    public TERCEMessage(TERCECore tc) {
        core = tc;
    }
    
    public void send() {
        if(msg != null) {
            os = core.getOutputStream();
            try {
                os.write(msg);
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }
    
}

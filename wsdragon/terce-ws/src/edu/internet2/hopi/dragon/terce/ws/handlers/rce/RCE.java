package edu.internet2.hopi.dragon.terce.ws.handlers.rce;

import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessage;
import edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault;


/**
 * Abstract class for extending to create RCE modules when combine with RCEInterface
 *
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public abstract class RCE {
    /**
     * Sets fault message and returns an RCEFaultMessage
     * @param msg fault message to set in exception
     * @return the generated exception
     */
    protected RCEFaultMessage generateRCEException(String msg) {
        TerceRceFault fault = new TerceRceFault();
        fault.setMsg(msg);

        RCEFaultMessage e = new RCEFaultMessage();
        e.setFaultMessage(fault);

        return e;
    }
}

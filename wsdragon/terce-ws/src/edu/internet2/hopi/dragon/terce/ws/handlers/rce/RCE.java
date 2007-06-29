package edu.internet2.hopi.dragon.terce.ws.handlers.rce;

import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault;

/**
 * Abstract class for extending to create RCE modules when combine with RCEInterface
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public abstract class RCE {
	
	/**
	 * Sets fault message and returns an RCEFaultMessageException
	 * @param msg fault message to set in exception
	 * @return the generated exception
	 */
    protected RCEFaultMessageException generateRCEException(String msg){
   	 TerceRceFault fault = new TerceRceFault();
   	 fault.setMsg(msg);
   	 RCEFaultMessageException e = new RCEFaultMessageException();
   	 e.setFaultMessage(fault);
			
   	 return e;
    }
}

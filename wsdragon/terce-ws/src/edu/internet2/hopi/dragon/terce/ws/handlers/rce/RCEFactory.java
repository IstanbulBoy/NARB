package edu.internet2.hopi.dragon.terce.ws.handlers.rce;

import edu.internet2.hopi.dragon.terce.ws.handlers.rce.dynamicdb.DynamicRCE;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.staticdb.StaticRCE;

import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;

/**
 * Factory that generates classes which implement RCEInterface. Can be extended to 
 * support multiple types of RCEs.
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public class RCEFactory {
	
	/**
	 * Generates a class that implements RCEInterface based on the type passed.
	 * @param rceType the type of the RCE module to generate
	 * @return a class that implements RCEInterface
	 * @throws RCEFaultMessageException
	 */
	public RCEInterface createRCE(String rceType) throws RCEFaultMessageException{
		if(rceType.equals("static")){
			return new StaticRCE();
		}else if(rceType.equals("dynamic")){
			return new DynamicRCE();
		}else{
			throw new RCEFaultMessageException("Unsupported RCE type configured");
		}
	}
}

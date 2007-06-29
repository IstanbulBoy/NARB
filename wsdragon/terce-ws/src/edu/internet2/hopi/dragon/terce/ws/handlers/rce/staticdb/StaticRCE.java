package edu.internet2.hopi.dragon.terce.ws.handlers.rce.staticdb;

import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCE;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCEInterface;
import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;

/**
 * RCE implementation that returns static paths. The paths will be kept in
 * a flat file and indexed by source and destination pairs.
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public class StaticRCE extends RCE implements RCEInterface{
	
	/**
	 * Statically looks up a path based on parameters.
	 * 
	 * @param request the SOAP request that contains the path computation parameters
	 * @return The result of a path computation in a SOAP format
	 */
	public FindPathResponse findPath(FindPath request) throws RCEFaultMessageException{
		// TODO ANDY WILL IMPLEMENT
		return null;
	}

}

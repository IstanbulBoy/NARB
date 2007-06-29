package edu.internet2.hopi.dragon.terce.ws.handlers.rce;

import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;

/**
 * Interface implemented by RCE modules. New RCE types can be developed by 
 * implementing this interface.
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public interface RCEInterface {
	FindPathResponse findPath(FindPath request) throws RCEFaultMessageException;
}

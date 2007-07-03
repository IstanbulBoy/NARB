
/**
 * TERCESkeleton.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.2 Apr 27, 2007 (04:35:37 IST)
 */
package edu.internet2.hopi.dragon.terce.ws.service;

import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCEFactory;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCEInterface;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;

/**
 *  TERCESkeleton java skeleton for the axisService
 */
public class TERCESkeleton{


	/**
	 * Auto generated method signature


	 * @param findPath
	 */


	public FindPathResponse findPath(FindPath findPath) throws RCEFaultMessageException{
		RCEFactory factory = new RCEFactory();
		RCEInterface rce = factory.createRCE("dynamic");
		FindPathResponse response = rce.findPath(findPath);
		
		return response;
	}


	/**
	 * Auto generated method signature


	 * @param selectNetworkTopology
	 */


	public edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse selectNetworkTopology
	(
			edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology selectNetworkTopology
	)
	throws TEDBFaultMessageException{
		//Todo fill this with the necessary business logic
		throw new  java.lang.UnsupportedOperationException("Please implement " + this.getClass().getName() + "#selectNetworkTopology");
	}


	/**
	 * Auto generated method signature


	 * @param insertNetworkTopology
	 */


	public edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse insertNetworkTopology
	(
			edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology insertNetworkTopology
	)
	throws TEDBFaultMessageException{
		//Todo fill this with the necessary business logic
		throw new  java.lang.UnsupportedOperationException("Please implement " + this.getClass().getName() + "#insertNetworkTopology");
	}

}

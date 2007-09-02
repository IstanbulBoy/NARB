
/**
 * TERCESkeleton.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.2 Apr 27, 2007 (04:35:37 IST)
 */
package edu.internet2.hopi.dragon.terce.ws.service;

import edu.internet2.hopi.dragon.terce.TERCECore;
import edu.internet2.hopi.dragon.terce.TERCEGlobals;
import java.io.IOException;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.ws.handlers.TERCEHandler;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCEFactory;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCEInterface;
import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDBFactory;
import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDBInterface;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse;
import org.apache.axis2.context.ServiceContext;

/**
 *  TERCESkeleton java skeleton for the axisService
 */
public class TERCESkeleton{

    private Thread serverThread;
    
        public void init(ServiceContext serviceContext) {
            TERCEGlobals.core = new TERCECore(2690);
            serverThread = new Thread(TERCEGlobals.core);
           serverThread.start();
        }
        
        public void destroy(ServiceContext serviceContext) {
            TERCEGlobals.core.stopServer();
            TERCEGlobals.core = null;
        }
        
	/**
	 * Auto generated method signature
	 * @param findPath
	 */
	public FindPathResponse findPath(FindPath findPath) throws RCEFaultMessage{
		PropertyReader props = null;
		/* Open properties file */
		try {
			props = TERCEHandler.createPropertyReader();
		} catch (IOException e) {
			throw TERCEHandler.generateRCEException("IO Exception: " + e.getMessage());
		}
		
		/* Contact underlying RCE module */
		RCEFactory factory = new RCEFactory();
		RCEInterface rce = factory.createRCE(props.getProperty("rce.type"));
		FindPathResponse response = rce.findPath(findPath);
		System.out.println("RCE DONE");
		return response;
	}

	/**
	 * Auto generated method signature
	 * @param selectNetworkTopology
	 */
	public SelectNetworkTopologyResponse selectNetworkTopology(SelectNetworkTopology selectNetworkTopology)
		throws TEDBFaultMessage{
		PropertyReader props = null;
		
		/* Open properties file */
		try {
			props = TERCEHandler.createPropertyReader();
		} catch (IOException e) {
			TERCEHandler.generateTEDBException("IO Exception: " + e.getMessage());
		}
		
		/* Select topology from underlying module */
		TEDBFactory factory = new TEDBFactory();
		TEDBInterface tedb = factory.createTEDB(props.getProperty("tedb.type"));
		SelectNetworkTopologyResponse response = tedb.selectNetworkTopology(selectNetworkTopology);
		
		return response;
	}

	/**
	 * Auto generated method signature
	 * @param insertNetworkTopology
	 */
	public InsertNetworkTopologyResponse insertNetworkTopology(InsertNetworkTopology insertNetworkTopology)
		throws TEDBFaultMessage{
		PropertyReader props = null;
		
		/* Open properties file */
		try {
			props = TERCEHandler.createPropertyReader();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		/* Insert topology into underlyig database */
		TEDBFactory factory = new TEDBFactory();
		TEDBInterface tedb = factory.createTEDB(props.getProperty("tedb.type"));
		InsertNetworkTopologyResponse response = tedb.insertNetworkTopology(insertNetworkTopology);
		
		return response;
	}
}

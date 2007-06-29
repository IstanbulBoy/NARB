package edu.internet2.hopi.dragon.terce.ws.handlers.tedb.dynamicdb;

import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDB;
import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDBInterface;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse;

/**
 * Accesses the TERCE database and selects/inserts topology information 
 * from that database
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public class DynamicTEDB extends TEDB implements TEDBInterface {

	public InsertNetworkTopologyResponse insertNetworkTopology(InsertNetworkTopology insertRequest) {
		// TODO JARDA WILL IMPLEMENT
		return null;
	}

	public SelectNetworkTopologyResponse selectNetworkTopology(SelectNetworkTopology selectRequest) {
		// TODO JARDA WILL IMPLEMENT
		return null;
	}

}

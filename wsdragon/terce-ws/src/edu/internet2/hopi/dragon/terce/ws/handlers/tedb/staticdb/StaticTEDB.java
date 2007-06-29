package edu.internet2.hopi.dragon.terce.ws.handlers.tedb.staticdb;

import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDB;
import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDBInterface;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse;

/**
 * Accesses a TEDB stored in a flat file. Will NOT insert schema
 * information, only selects. The intent is that this database will 
 * be used for testing and allows a domain to share
 * topology even when they cannot receive it from others.
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public class StaticTEDB extends TEDB implements TEDBInterface {

	public InsertNetworkTopologyResponse insertNetworkTopology(InsertNetworkTopology insertRequest) {
		// TODO ANDY WILL IMPLEMENT
		return null;
	}

	public SelectNetworkTopologyResponse selectNetworkTopology(SelectNetworkTopology selectRequest) {
		//TODO ANDY WILL IMPLEMENT
		return null;
	}

}

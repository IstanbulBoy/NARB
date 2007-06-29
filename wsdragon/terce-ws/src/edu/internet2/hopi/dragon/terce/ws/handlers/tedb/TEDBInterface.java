package edu.internet2.hopi.dragon.terce.ws.handlers.tedb;

import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse;

/**
 * Interface for creating frontend to traffic engineering database
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public interface TEDBInterface {
	InsertNetworkTopologyResponse insertNetworkTopology(InsertNetworkTopology insertRequest);
	SelectNetworkTopologyResponse selectNetworkTopology(SelectNetworkTopology selectRequest);
}

package edu.internet2.hopi.dragon.terce.ws.handlers.tedb.dynamicdb;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.TERCEGlobals;
import edu.internet2.hopi.dragon.terce.ws.handlers.TERCEHandler;
import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDB;
import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDBInterface;
import edu.internet2.hopi.dragon.terce.ws.service.TEDBFaultMessage;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponseContent;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneTopologyContent;


/**
 * Accesses the TERCE database and selects/inserts topology information
 * from that database
 *
 * @author jflidr
 *
 */
public class DynamicTEDB extends TEDB implements TEDBInterface {
    public InsertNetworkTopologyResponse insertNetworkTopology(
        InsertNetworkTopology insertRequest) throws TEDBFaultMessage {
        return null;
    }

    public SelectNetworkTopologyResponse selectNetworkTopology(
        SelectNetworkTopology selectRequest) throws TEDBFaultMessage {
        SelectNetworkTopologyResponse response = new SelectNetworkTopologyResponse();
        SelectNetworkTopologyResponseContent responseContent = new SelectNetworkTopologyResponseContent();
        CtrlPlaneTopologyContent topology = TERCEGlobals.core.getTopology();
        responseContent.setTopology(topology);

        response.setSelectNetworkTopologyResponse(responseContent);

        return response;
    }
}

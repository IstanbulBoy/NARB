package edu.internet2.hopi.dragon.terce.ws.handlers;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessage;
import edu.internet2.hopi.dragon.terce.ws.service.TEDBFaultMessage;
import edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent;

import java.io.IOException;

import java.util.StringTokenizer;


/**
 * This class contains static utility methods used by TERCE components.
 *
 * @author Andrew Lake
 *
 */
public class TERCEHandler {
    /**
     * Reads the properties file at $TERCEWS_HOME/terce-ws.properties
     * @return PropertyReader object with properties from file
     * @throws IOException
     */
    public static PropertyReader createPropertyReader()
        throws IOException {
        PropertyReader props = null;

        props = new PropertyReader(System.getenv("TERCEWS_HOME") +
                "/terce-ws.properties");

        return props;
    }

    /**
     * Converts a string in the format of urn:ogf:network:domainId:nodeId:portId:linkId
     * to a CtrlPlaneDomainContent object useable by Axis2. In the urn everything beyond
     * domain ID is optional. This method returns null if the urn is incorrect.
     *
     * @param urn the urn to convert that points to a domain, node, port or link
     * @return a CtrlPlaneDomainContent useable by Axis2
     */
    public static CtrlPlaneDomainContent urnToDomain(String urn) {
        CtrlPlaneDomainContent domain = null;
        StringTokenizer st = new StringTokenizer(urn, ":");

        int i = 0;

        for (String token = st.nextToken(); st.hasMoreTokens();
                token = st.nextToken()) {
            System.out.println("Token: " + token);

            if (token.equals("") || ((i == 0) && token.equals("urn")) ||
                    ((i == 1) && token.equals("ogf")) ||
                    ((i == 2) && token.equals("network"))) {
                i++;

                continue;
            } else if (i == 3) {
                domain = new CtrlPlaneDomainContent();
                domain.setId(token);
            } else if (i == 4) {
                CtrlPlaneNodeContent node = new CtrlPlaneNodeContent();
                node.setId(token);
                domain.addNode(node);
            } else if (i == 5) {
                CtrlPlanePortContent port = new CtrlPlanePortContent();
                port.setId(token);
                domain.getNode()[0].addPort(port);
            } else if (i == 6) {
                CtrlPlaneLinkContent link = new CtrlPlaneLinkContent();
                link.setId(token);
                domain.getNode()[0].getPort()[0].addLink(link);
            } else {
                domain = null;

                break;
            }

            i++;
        }

        return domain;
    }

    /**
     * Sets fault message and returns an RCEFaultMessageException
     * @param msg fault message to set in exception
     * @return the generated exception
     */
    public static RCEFaultMessage generateRCEException(String msg) {
        TerceRceFault fault = new TerceRceFault();
        fault.setMsg(msg);

        RCEFaultMessage e = new RCEFaultMessage();
        e.setFaultMessage(fault);

        return e;
    }

    /**
     * Sets fault message and returns an TEDBFaultMessageException
     * @param msg fault message to set in exception
     * @return the generated exception
     */
    public static TEDBFaultMessage generateTEDBException(String msg) {
        TerceTedbFault fault = new TerceTedbFault();
        fault.setMsg(msg);

        TEDBFaultMessage e = new TEDBFaultMessage();
        e.setFaultMessage(fault);

        return e;
    }
}

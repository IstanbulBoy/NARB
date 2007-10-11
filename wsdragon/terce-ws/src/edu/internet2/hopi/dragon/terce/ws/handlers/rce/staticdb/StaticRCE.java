package edu.internet2.hopi.dragon.terce.ws.handlers.rce.staticdb;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.ws.handlers.TERCEHandler;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCE;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCEInterface;
import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessage;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponseContent;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneHopContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePathContent;

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import org.xml.sax.SAXException;

import java.io.File;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;


/**
 * RCE implementation that returns static paths. The paths will be kept in
 * a flat file and indexed by source and destination pairs.
 *
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public class StaticRCE extends RCE implements RCEInterface {
    /**
     * Statically looks up a path based on parameters.
     *
     * @param request the SOAP request that contains the path computation parameters
     * @return The result of a path computation in a SOAP format
     */
    public FindPathResponse findPath(FindPath request)
        throws RCEFaultMessage {
        PropertyReader props = null;
        FindPathResponse response = new FindPathResponse();
        FindPathContent requestContent = request.getFindPath();

        try {
            props = TERCEHandler.createPropertyReader();

            File fin = new File(props.getProperty("rce.static.file"));
            DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder docBuilder = null;
            Document rceXMLDoc = null;

            docBuilder = docFactory.newDocumentBuilder();
            rceXMLDoc = docBuilder.parse(fin);

            NodeList children = rceXMLDoc.getChildNodes();

            for (int i = 0; i < children.getLength(); i++) {
                Node child = children.item(i);
                String nodeName = child.getNodeName();

                if ((nodeName != null) &&
                        nodeName.equalsIgnoreCase("staticPathDatabase")) {
                    FindPathResponseContent responseContent = this.parseStaticPathDatabase(child,
                            requestContent.getSrcEndpoint(),
                            requestContent.getDestEndpoint(),
                            requestContent.getAllvtags());
                    response.setFindPathResponse(responseContent);
                }
            }
        } catch (ParserConfigurationException e) {
            throw this.generateRCEException("Parser Config Exception: " +
                e.getMessage());
        } catch (SAXException e) {
            throw this.generateRCEException("SAX Exception: " + e.getMessage());
        } catch (IOException e) {
            throw this.generateRCEException("IO Exception: " + e.getMessage());
        }

        /* Verify a path was found */
        if (response.getFindPathResponse() == null) {
            throw this.generateRCEException(
                "Unable to calculate a path between given source and destination");
        }

        return response;
    }

    /**
     * Utility function for parsing the &lt;staticPathDatabase&gt; element information
     * in the static route file and converting it to a response to a findPath request
     *
     * @param elem the staticPathDatabase element to parse
     * @param src the source endpoint of the request
     * @param returnVtags boolean value that is set to true if available vlan tags should be included in response
     * @param dest the destination endpoint of the request
     * @return the findPath  response including the path in the format needed by Axis2
     */
    private FindPathResponseContent parseStaticPathDatabase(Node elem,
        String src, String dest, boolean returnVtags) {
        FindPathResponseContent responseContent = null;
        NodeList children = elem.getChildNodes();

        /* Parse elements */
        for (int i = 0;
                (i < children.getLength()) && (responseContent == null); i++) {
            Node child = children.item(i);
            String nodeName = child.getNodeName();

            if (nodeName == null) {
                continue;
            } else if (nodeName.equalsIgnoreCase("staticPathEntry")) {
                responseContent = this.lookupPath(child, src, dest, returnVtags);
            }
        }

        return responseContent;
    }

    /**
     * Parses the static route file and looks up a path with the same source and destination
     * as the request.
     *
     * @param elem the staticPathEntry element to parse
     * @param requestSrc the source endpoint of the request
     * @param requestDest the destination endpoint of the request
     * @return the findPath  response including the path in the format needed by Axis2
     */
    private FindPathResponseContent lookupPath(Node elem, String requestSrc,
        String requestDest, boolean returnVtags) {
        FindPathResponseContent responseContent = null;
        NodeList children = elem.getChildNodes();
        String entrySrc = null;
        String entryDest = null;
        CtrlPlanePathContent entryRoute = null;
        String entryVtags = null;

        /* Parse elements */
        for (int i = 0; i < children.getLength(); i++) {
            Node child = children.item(i);
            String nodeName = child.getNodeName();

            if (nodeName == null) {
                continue;
            } else if (nodeName.equalsIgnoreCase("srcEndpoint")) {
                entrySrc = child.getTextContent();
            } else if (nodeName.equalsIgnoreCase("destEndpoint")) {
                entryDest = child.getTextContent();
            } else if (nodeName.equalsIgnoreCase("path")) {
                entryRoute = this.parsePath(child);
            } else if (nodeName.equalsIgnoreCase("availableVtags")) {
                entryVtags = child.getTextContent();
            }
        }

        /* check source and destination */
        entrySrc = entrySrc.replaceAll("\\s", "");
        requestSrc = requestSrc.replaceAll("\\s", "");
        entryDest = entryDest.replaceAll("\\s", "");
        requestDest = requestDest.replaceAll("\\s", "");

        if (requestSrc.equals(entrySrc) && requestDest.equals(entryDest)) {
            /* Initialize response */
            responseContent = new FindPathResponseContent();

            /* Check if vlan range should be returned */
            if (returnVtags) {
                responseContent.setAvailableVtags(entryVtags);
            }

            /* Set path in response */
            responseContent.setPath(entryRoute);
        }

        return responseContent;
    }

    /**
     * Parses a path element in the static route file and returns the path
     * as an object used in the Axis2 response
     *
     * @param elem the path element to parse
     * @return the path information in the format needed by Axis2
     */
    private CtrlPlanePathContent parsePath(Node elem) {
        CtrlPlanePathContent route = new CtrlPlanePathContent();
        NodeList children = elem.getChildNodes();
        NamedNodeMap attrs = elem.getAttributes();

        /* Parse Attribues */
        route.setId(this.getIdAttribute(attrs));

        /* Parse elements */
        for (int i = 0; i < children.getLength(); i++) {
            Node child = children.item(i);
            String nodeName = child.getNodeName();

            if (nodeName == null) {
                continue;
            } else if (nodeName.equalsIgnoreCase("hop")) {
                CtrlPlaneHopContent hop = this.parseHop(child);
                route.addHop(hop);
            }
        }

        return route;
    }

    /**
     * Parses a &lt;node&gt; object and maps it to a CtrlPlaneNodeContent object
     * for use in an Axis2 response
     *
     * @param elem the node element to parse
     * @return CtrlPlaneNodeContent object for use in an Axis2 response
     */
    private CtrlPlaneHopContent parseHop(Node elem) {
        CtrlPlaneHopContent hop = new CtrlPlaneHopContent();
        NamedNodeMap attrs = elem.getAttributes();
        NodeList children = elem.getChildNodes();

        /* Parse Attribues */
        hop.setId(this.getIdAttribute(attrs));

        /* Parse elements */
        for (int i = 0; i < children.getLength(); i++) {
            Node child = children.item(i);
            String nodeName = child.getNodeName();

            if (nodeName == null) {
                continue;
            } else if (nodeName.equalsIgnoreCase("domainIdRef")) {
                hop.setDomainIdRef(child.getTextContent());
            } else if (nodeName.equalsIgnoreCase("nodeIdRef")) {
                hop.setNodeIdRef(child.getTextContent());
            } else if (nodeName.equalsIgnoreCase("portIdRef")) {
                hop.setPortIdRef(child.getTextContent());
            } else if (nodeName.equalsIgnoreCase("linkIdRef")) {
                hop.setLinkIdRef(child.getTextContent());
            }
        }

        return hop;
    }

    /**
     * Utility function for extracting the id atrribute from an element
     *
     * @param attrs a list of an element's attributes
     * @return the value of the id attribute
     */
    private String getIdAttribute(NamedNodeMap attrs) {
        Node attrId = attrs.getNamedItem("id");

        if (attrId != null) {
            return attrId.getNodeValue();
        }

        return null;
    }
}

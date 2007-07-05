package edu.internet2.hopi.dragon.terce.ws.handlers.rce.staticdb;

import java.io.File;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent;
import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.ws.handlers.TERCEHandler;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCE;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCEInterface;
import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.types.rce.EndpointContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponseContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.HopList;
import edu.internet2.hopi.dragon.terce.ws.types.rce.Path;

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
			for(int i = 0; i < children.getLength(); i ++){
				Node child = children.item(i);
				String nodeName = child.getNodeName();
				if(nodeName != null && nodeName.equalsIgnoreCase("staticPathDatabase")){
					FindPathResponseContent responseContent = this.parseStaticPathDatabase(child, 
							requestContent.getSrcEndpoint(), requestContent.getDestEndpoint());
					response.setFindPathResponse(responseContent);
				}
			}
		}catch (ParserConfigurationException e) {
			throw this.generateRCEException("Parser Config Exception: " + e.getMessage());
		} catch (SAXException e) {
			throw this.generateRCEException("SAX Exception: " + e.getMessage());
		} catch (IOException e) {
			throw this.generateRCEException("IO Exception: " + e.getMessage());
		} 
		
		/* Verify a path was found */
		if(response.getFindPathResponse() == null){
			throw this.generateRCEException("Unable to calculate a path between given source and destination");
		}
		return response;
	}
	
	/**
	 * Utility function for parsing the &lt;staticPathDatabase&gt; element information
	 * in the static route file and converting it to a response to a findPath request
	 * 
	 * @param elem the staticPathDatabase element to parse
	 * @param src the source endpoint of the request
	 * @param dest the destination endpoint of the request
	 * @return the findPath  response including the path in the format needed by Axis2
	 */
	private FindPathResponseContent parseStaticPathDatabase(Node elem, EndpointContent src, EndpointContent dest){
		FindPathResponseContent responseContent = null;
		NodeList children = elem.getChildNodes();
		
		/* Parse elements */
		for(int i = 0; i < children.getLength() && responseContent == null; i++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("staticPathEntry")){
				responseContent = this.lookupPath(child, src, dest);
			}
		}
		return responseContent;
	}
	
	/**
	 * Parses the static route file and looks up a path with the same source and destination
	 * as the request.
	 * 
	 * @param elem the staticPathEntry element to parse
	 * @param src the source endpoint of the request
	 * @param dest the destination endpoint of the request
	 * @return the findPath  response including the path in the format needed by Axis2
	 */
	private FindPathResponseContent lookupPath(Node elem, EndpointContent requestSrc, EndpointContent requestDest){
		FindPathResponseContent responseContent = null;
		NodeList children = elem.getChildNodes();
		EndpointContent entrySrc = null;
		EndpointContent entryDest = null;
		Path entryRoute = null;
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();

			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("srcEndpoint")){
				entrySrc = this.parseEndpoint(child);
			}else if(nodeName.equalsIgnoreCase("destEndpoint")){
				entryDest = this.parseEndpoint(child);
			}else if(nodeName.equalsIgnoreCase("path")){
				entryRoute = this.parsePath(child);
			}
		}
		
		/* check source and destination */
		if(this.endpointsAreEqual(requestSrc, entrySrc) && 
				this.endpointsAreEqual(requestDest, entryDest)){
			responseContent = new FindPathResponseContent();
			responseContent.setPath(entryRoute);
		}
		
		System.out.println(responseContent);
		return responseContent;
	}
	
	/**
	 * Parses elements that contain elements domainId, nodeId, portId,
	 * and an optional linkId
	 * 
	 * @param elem the element to parse
	 * @return an EndpointContent object used by Axis2 in the web service response
	 */
	private EndpointContent parseEndpoint(Node elem){
		EndpointContent endpoint = new EndpointContent();
		NodeList children = elem.getChildNodes();
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("domain")){
				CtrlPlaneDomainContent domain = this.parseDomain(child);
				endpoint.setDomain(domain);
			}
		}
		
		return endpoint;
	}
	
	/**
	 * Parses a &lt;domain&gt; object and maps it to a CtrlPlaneDomainContent object
	 * for use in an Axis2 response.
	 * 
	 * @param elem the domain element to parse
	 * @return CtrlPlaneDomainContent object for use in an Axis2 response
	 */
	private CtrlPlaneDomainContent parseDomain(Node elem){
		CtrlPlaneDomainContent domain = new CtrlPlaneDomainContent();
		NamedNodeMap attrs = elem.getAttributes();
		NodeList children = elem.getChildNodes();
		
		/* Parse Attribues */
		domain.setId(this.getIdAttribute(attrs));
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("node")){
				CtrlPlaneNodeContent node = this.parseNode(child);
				domain.addNode(node);
			}
		}
		
		return domain;
	}
	
	/**
	 * Parses a &lt;node&gt; object and maps it to a CtrlPlaneNodeContent object
	 * for use in an Axis2 response
	 * 
	 * @param elem the node element to parse
	 * @return CtrlPlaneNodeContent object for use in an Axis2 response
	 */
	private CtrlPlaneNodeContent parseNode(Node elem) {
		CtrlPlaneNodeContent node = new CtrlPlaneNodeContent();
		NamedNodeMap attrs = elem.getAttributes();
		NodeList children = elem.getChildNodes();
		
		/* Parse Attribues */
		node.setId(this.getIdAttribute(attrs));
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("nodeAddress")){
				CtrlPlaneAddressContent addr = this.parseAddress(child);
				node.setNodeAddress(addr);
			}else if(nodeName.equalsIgnoreCase("port")){
				CtrlPlanePortContent port = this.parsePort(child);
				node.addPort(port);
			}
		}
		
		return node;
	}
	
	/**
	 * Parses a &lt;port&gt; object and maps it to a CtrlPlanePortContent object
	 * for use in an Axis2 response
	 * 
	 * @param elem the port element to parse
	 * @return CtrlPlanePortContent object for use in an Axis2 response
	 */
	private CtrlPlanePortContent parsePort(Node elem) {
		CtrlPlanePortContent port = new CtrlPlanePortContent();
		NamedNodeMap attrs = elem.getAttributes();
		NodeList children = elem.getChildNodes();
		
		/* Parse Attribues */
		port.setId(this.getIdAttribute(attrs));
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("link")){
				CtrlPlaneLinkContent link = this.parseLink(child);
				port.addLink(link);
			}
		}
		
		return port;
	}
	
	/**
	 * Parses a &lt;link&gt; object and maps it to a CtrlPlaneLinkContent object
	 * for use in an Axis2 response
	 * 
	 * @param elem the link element to parse
	 * @return CtrlPlaneLinkContent object for use in an Axis2 response
	 */
	private CtrlPlaneLinkContent parseLink(Node elem) {
		CtrlPlaneLinkContent link = new CtrlPlaneLinkContent();
		NamedNodeMap attrs = elem.getAttributes();
		
		/* Parse Attribues */
		link.setId(this.getIdAttribute(attrs));
		
		return link;
	}

	/**
	 * Parses a path element in the static route file and returns the path 
	 * as an object used in the Axis2 response
	 * 
	 * @param elem the path element to parse
	 * @return the path information in the format needed by Axis2
	 */
	private Path parsePath(Node elem){
		Path route = new Path();
		NodeList children = elem.getChildNodes();
		HopList hops = new HopList();
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("hop")){
				EndpointContent hop = this.parseEndpoint(child);
				hops.addHop(hop);
			}
		}
		
		route.setHops(hops);
		
		return route;
	}
	
	/**
	 * Tests if two given endpoints refer to the same topological location. It does this 
	 * by matching domainId, nodeId, portId and linkId(if present).
	 * 
	 * @param endpoint1 the first endpoint to compare
	 * @param endpoint2 the second endpoint to compare
	 * @return true if the two endpoints refer to the same location, false otherwise
	 */
	private boolean endpointsAreEqual(EndpointContent endpoint1, EndpointContent endpoint2){
		/* Get endpoint 1 information */
		CtrlPlaneDomainContent domain1 = endpoint1.getDomain();
		CtrlPlaneNodeContent node1 = domain1.getNode()[0];
		CtrlPlanePortContent port1 = node1.getPort()[0];
		CtrlPlaneLinkContent[] link1 = port1.getLink();
		
		/* Get endpoint 2 information */
		CtrlPlaneDomainContent domain2 = endpoint2.getDomain();
		CtrlPlaneNodeContent node2 = domain2.getNode()[0];
		CtrlPlanePortContent port2 = node2.getPort()[0];
		CtrlPlaneLinkContent[] link2 = port2.getLink();
		
		
		return (domain1.getId().equals(domain2.getId()) &&
				node1.getId().equals(node2.getId()) &&
				port1.getId().equals(port2.getId()) &&
				((link1 == null && link2 == null)
				|| (link1 != null && link2 !=null && 
						(link1[0].getId().equals(link2[0].getId())))));
	}
	
	/**
	 * Utility function for extracting the id atrribute from an element
	 * 
	 * @param attrs a list of an element's attributes
	 * @return the value of the id attribute
	 */
	private String getIdAttribute(NamedNodeMap attrs){
		Node attrId = attrs.getNamedItem("id");
		if(attrId != null){
			return attrId.getNodeValue();
		}
		
		return null;
	}
	
	/**
	 * Utility function for creating an address object.
	 * 
	 * @param the element with data that has to be mapped to an address type
	 * @return the data in control plane address object
	 */
	private CtrlPlaneAddressContent parseAddress(Node elem){
		//TODO: add suport for type and value
		CtrlPlaneAddressContent addr = new CtrlPlaneAddressContent();
		addr.setString(elem.getTextContent());
		
		return addr;
	}

}

package edu.internet2.hopi.dragon.terce.ws.handlers.tedb.staticdb;

import java.io.File;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwcapContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwitchingCapabilitySpecficInfo;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneTopologyContent;
import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.ws.handlers.TERCEHandler;
import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDB;
import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.TEDBInterface;
import edu.internet2.hopi.dragon.terce.ws.service.TEDBFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponseContent;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponseContent;

/**
 * Accesses a TEDB stored in a flat file. Will NOT insert schema
 * information, only selects. The intent is that this database will 
 * be used for testing and allows a domain to share
 * topology even when they cannot receive it from others.
 * 
 * Two databases can be selected from: interdomain and intradomain. If
 * neither is specified in a selectNetworkTopology request interdomain is used.
 * 
 * @author Andrew Lake(alake@internet2.edu)
 * *
 */
public class StaticTEDB extends TEDB implements TEDBInterface {
	
	/**
	 * Mimics an the inertNetworkTopology call of a dynamic database. Since the database in the implementation
	 * is static (the topology does not change) this call does not do anything with the given topology. It set
	 * the result code to 0 (SUCCESS) and a message indicating this.
	 * 
	 * @param insertRequest an Axis2 object representation of an insertNetworkTopology request
	 * @return an Axis2 representation of a successful response to an insertNetworkTopology request
	 */
	public InsertNetworkTopologyResponse insertNetworkTopology(InsertNetworkTopology insertRequest) throws TEDBFaultMessageException{
		InsertNetworkTopologyResponse response = new InsertNetworkTopologyResponse();
		InsertNetworkTopologyResponseContent responseContent = new InsertNetworkTopologyResponseContent();
		
		responseContent.setResultCode(0);
		responseContent.setResultMessage("Request successful. " +
				"NOTE: The underlying database is static so this topology was ignored");
		response.setInsertNetworkTopologyResponse(responseContent);
		
		return response;
	}
	
	/**
	 * Used by the selectNetworkTopology web service to obtain topology information.
	 * This implementation reads and XML file from disk and returns the data as an 
	 * object useable by Axis2. Data can be selected from two database: interdomain
	 * and intradomain. If no database is specified, interdomain is used.
	 * 
	 * @param selectRequest an Axis2 selectNetworkTopology web service request
	 * @return an Axis2 response to a selectNetworkTopology request
	 */
	public SelectNetworkTopologyResponse selectNetworkTopology(SelectNetworkTopology selectRequest) throws TEDBFaultMessageException {
		PropertyReader props = null;
		SelectNetworkTopologyResponse response = new SelectNetworkTopologyResponse();
		SelectNetworkTopologyResponseContent responseContent = new SelectNetworkTopologyResponseContent();
		String database = selectRequest.getSelectNetworkTopology().getDatabase();
		
		try {
			props = TERCEHandler.createPropertyReader();
			File fin = null;
			
			/* Load database file */
			if(database == null || database.equalsIgnoreCase("interdomain")){
				fin = new File(props.getProperty("tedb.static.db.interdomain"));
			}else if(database.equalsIgnoreCase("intradomain")){
				fin = new File(props.getProperty("tedb.static.db.intradomain"));
			}else{
				throw this.generateTEDBException("Specified database not found");
			}
			
			/* Parse file */
			DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder docBuilder = null;
			Document topologyXMLDoc = null;
			
			docBuilder = docFactory.newDocumentBuilder();
			topologyXMLDoc = docBuilder.parse(fin);
			
			NodeList children = topologyXMLDoc.getChildNodes();
			for(int i = 0; i < children.getLength(); i ++){
				Node child = children.item(i);
				String nodeName = child.getNodeName();
				if(nodeName != null && nodeName.equalsIgnoreCase("topology")){
					CtrlPlaneTopologyContent topology = this.parseTopology(child);
					responseContent.setTopology(topology);
				}
			}
		} catch (ParserConfigurationException e) {
			throw this.generateTEDBException("Parser Config Exception: " + e.getMessage());
		} catch (SAXException e) {
			throw this.generateTEDBException("SAX Exception: " + e.getMessage());
		} catch (IOException e) {
			throw this.generateTEDBException("IO Exception: " + e.getMessage());
		}
		
		response.setSelectNetworkTopologyResponse(responseContent);
		
		return response;
	}
	
	/**
	 * Utility function for parsing the &lt;topology&gt; element information
	 * in the topology file and converting it to an Axis2 topology object.
	 * 
	 * @param elem the topology element to parse
	 * @return the topology information in the format needed by Axis2
	 */
	private CtrlPlaneTopologyContent parseTopology(Node elem){
		CtrlPlaneTopologyContent topology = new CtrlPlaneTopologyContent();
		NamedNodeMap attrs = elem.getAttributes();
		NodeList children = elem.getChildNodes();
		
		/* Parse Attribues */
		topology.setId(this.getIdAttribute(attrs));
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i ++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("idcId")){
				topology.setIdcId(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("domain")){
				CtrlPlaneDomainContent domain = this.parseDomain(child);
				topology.addDomain(domain);
			}
		}
		return topology;
	}
	
	/**
	 * Utility function for parsing the &lt;domain&gt; element information
	 * in the topology file and converting it to an Axis2 topology object.
	 * 
	 * @param elem the &lt;domain&gt; element to parse
	 * @return the domain information in the format needed by Axis2
	 */
	private CtrlPlaneDomainContent parseDomain(Node elem){
		CtrlPlaneDomainContent domain = new CtrlPlaneDomainContent();
		NamedNodeMap attrs = elem.getAttributes();
		NodeList children = elem.getChildNodes();
		
		/* Parse Attribues */
		domain.setId(this.getIdAttribute(attrs));
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i ++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("node")){
				CtrlPlaneNodeContent node = this.parseNode(child);
				domain.addNode(node);
			}else if(nodeName.equalsIgnoreCase("port")){
				CtrlPlanePortContent port = this.parsePort(child);
				domain.addPort(port);
			}else if(nodeName.equalsIgnoreCase("link")){
				CtrlPlaneLinkContent link = this.parseLink(child);
				domain.addLink(link);
			}
		}
		
		return domain;
	}
	
	/**
	 * Utility function for parsing the &lt;node&gt; element information
	 * in the topology file and converting it to an Axis2 topology object.
	 * 
	 * @param elem the &lt;node&gt; element to parse
	 * @return the node information in the format needed by Axis2
	 */
	private CtrlPlaneNodeContent parseNode(Node elem){
		CtrlPlaneNodeContent node = new CtrlPlaneNodeContent();
		NamedNodeMap attrs = elem.getAttributes();
		NodeList children = elem.getChildNodes();
		
		/* Parse Attribues */
		node.setId(this.getIdAttribute(attrs));
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i ++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("address")){
				node.setAddress(this.parseAddress(child));
			}else if(nodeName.equalsIgnoreCase("port")){
				CtrlPlanePortContent port = this.parsePort(child);
				node.addPort(port);
			}
		}
		
		return node;
	}
	
	/**
	 * Utility function for parsing the &lt;port&gt; element information
	 * in the topology file and converting it to an Axis2 topology object.
	 * 
	 * @param elem the &lt;port&gt; element to parse
	 * @return the port information in the format needed by Axis2
	 */
	private CtrlPlanePortContent parsePort(Node elem){
		CtrlPlanePortContent port = new CtrlPlanePortContent();
		NamedNodeMap attrs = elem.getAttributes();
		NodeList children = elem.getChildNodes();
		
		/* Parse Attribues */
		port.setId(this.getIdAttribute(attrs));
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i ++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("capacity")){
				port.setCapacity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("maximumReservableCapacity")){
				port.setMaximumReservableCapacity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("minimumReservableCapacity")){
				port.setMinimumReservableCapacity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("granularity")){
				port.setGranularity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("unreservedCapacity")){
				port.setUnreservedCapacity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("link")){
				CtrlPlaneLinkContent link = this.parseLink(child);
				port.addLink(link);
			}
		}
		
		return port;
	}
	
	/**
	 * Utility function for parsing the &lt;link&gt; element information
	 * in the topology file and converting it to an Axis2 topology object.
	 * 
	 * @param elem the &lt;link&gt; element to parse
	 * @return the link information in the format needed by Axis2
	 */
	private CtrlPlaneLinkContent parseLink(Node elem){
		CtrlPlaneLinkContent link = new CtrlPlaneLinkContent();
		NamedNodeMap attrs = elem.getAttributes();
		NodeList children = elem.getChildNodes();
		
		/* Parse Attribues */
		link.setId(this.getIdAttribute(attrs));
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i ++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("localPortId")){
				link.setCapacity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("remotePortId")){
				link.setRemotePortId(this.parseAddress(child));
			}else if(nodeName.equalsIgnoreCase("remoteNodeId")){
				link.setRemoteNodeId(this.parseAddress(child));
			}else if(nodeName.equalsIgnoreCase("remoteDomainId")){
				link.setRemoteDomainId(this.parseAddress(child));
			}else if(nodeName.equalsIgnoreCase("trafficEngineeringMetric")){
				link.setTrafficEngineeringMetric(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("linkProtectionTypes")){
				link.addLinkProtectionTypes(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("capacity")){
				link.setCapacity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("maximumReservableCapacity")){
				link.setMaximumReservableCapacity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("minimumReservableCapacity")){
				link.setMinimumReservableCapacity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("granularity")){
				link.setGranularity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("unreservedCapacity")){
				link.setUnreservedCapacity(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("administrativeGroups")){
				link.addAdministrativeGroups(this.parseAdministrativeGroup(child));
			}else if(nodeName.equalsIgnoreCase("SwitchingCapabilityDescriptors")){
				link.setSwitchingCapabilityDescriptors(this.parseSwcapDescriptors(child));
			}
		}
		
		return link;
	}
	
	/**
	 * Utility function for creating an address object.
	 * 
	 * @param elem the element with data that has to be mapped to an address type
	 * @return the data in control plane address object
	 */
	private CtrlPlaneAddressContent parseAddress(Node elem){
		//TODO: add suport for type and value
		CtrlPlaneAddressContent addr = new CtrlPlaneAddressContent();
		addr.setString(elem.getTextContent());
		
		return addr;
	}
	
	/**
	 * Utility function for parsing the &lt;administrativeGroups&gt; element information
	 * in the topology file and converting it to an Axis2 topology object.
	 * 
	 * @param elem the &lt;administrativeGroups&gt; element to parse
	 * @return the administrativeGroups information in the format needed by Axis2
	 */
	private CtrlPlaneAdministrativeGroup parseAdministrativeGroup(Node elem){
		CtrlPlaneAdministrativeGroup group = new CtrlPlaneAdministrativeGroup();
		NodeList children = elem.getChildNodes();
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i ++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("groupId")){
				group.setGroupID(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("group")){
				group.setGroup(Integer.parseInt(child.getTextContent()));
			}
		}
		
		return group;
	}
	
	/**
	 * Utility function for parsing the &lt;SwitchingCapabilityDescriptors&gt; 
	 * element information in the topology file and converting it to an Axis2 
	 * topology object.
	 * 
	 * @param elem the &lt;SwitchingCapabilityDescriptors&gt; element to parse
	 * @return the SwitchingCapabilityDescriptors information in the format needed by Axis2
	 */
	private CtrlPlaneSwcapContent parseSwcapDescriptors(Node elem){
		CtrlPlaneSwcapContent swcap = new CtrlPlaneSwcapContent();
		NodeList children = elem.getChildNodes();
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i ++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("switchingcapType")){
				swcap.setSwitchingcapType(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("encodingType")){
				swcap.setEncodingType(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("switchingCapabilitySpecficInfo")){
				swcap.setSwitchingCapabilitySpecficInfo(this.parseSwcapSpecInfo(child));
			}
		}
		
		return swcap;
	}
	
	/**
	 * Utility function for parsing the &lt;switchingCapabilitySpecficInfo&gt; 
	 * element information in the topology file and converting it to an Axis2 
	 * topology object.
	 * 
	 * @param elem the &lt;switchingCapabilitySpecficInfo&gt; element to parse
	 * @return the switchingCapabilitySpecficInfo information in the format needed by Axis2
	 */
	private CtrlPlaneSwitchingCapabilitySpecficInfo parseSwcapSpecInfo(Node elem){
		CtrlPlaneSwitchingCapabilitySpecficInfo specInfo = new CtrlPlaneSwitchingCapabilitySpecficInfo();
		NodeList children = elem.getChildNodes();
		
		/* Parse elements */
		for(int i = 0; i < children.getLength(); i ++){
			Node child = children.item(i);
			String nodeName = child.getNodeName();
			
			if(nodeName == null){
				continue;
			}else if(nodeName.equalsIgnoreCase("capability")){
				specInfo.setCapability(child.getTextContent());
			}else if(nodeName.equalsIgnoreCase("interfaceMTU")){
				specInfo.setInterfaceMTU(Integer.parseInt(child.getTextContent()));
			}else if(nodeName.equalsIgnoreCase("vlanRangeAvailability")){
				specInfo.setVlanRangeAvailability(child.getTextContent());
			}
		}
		return specInfo;
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

}

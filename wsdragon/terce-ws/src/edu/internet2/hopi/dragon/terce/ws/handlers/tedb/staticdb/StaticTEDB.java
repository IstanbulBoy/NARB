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
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponseContent;

/**
 * Accesses a TEDB stored in a flat file. Will NOT insert schema
 * information, only selects. The intent is that this database will 
 * be used for testing and allows a domain to share
 * topology even when they cannot receive it from others.
 * 
 * @author Andrew Lake(alake@internet2.edu)
 * *
 */
public class StaticTEDB extends TEDB implements TEDBInterface {

	public InsertNetworkTopologyResponse insertNetworkTopology(InsertNetworkTopology insertRequest) throws TEDBFaultMessageException{
		// TODO ANDY WILL IMPLEMENT
		return null;
	}

	public SelectNetworkTopologyResponse selectNetworkTopology(SelectNetworkTopology selectRequest) throws TEDBFaultMessageException {
		PropertyReader props = TERCEHandler.createPropertyReader();
		SelectNetworkTopologyResponse response = new SelectNetworkTopologyResponse();
		SelectNetworkTopologyResponseContent responseContent = new SelectNetworkTopologyResponseContent();
		
		try {
			File fin = new File(props.getProperty("tedb.static.file"));
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
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SAXException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		response.setSelectNetworkTopologyResponse(responseContent);
		
		return response;
	}
	
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
			}else if(nodeName.equalsIgnoreCase("domainId")){
				domain.setDomainId(child.getTextContent());
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
			}else if(nodeName.equalsIgnoreCase("nodeId")){
				node.setNodeid(this.parseAddress(child));
			}else if(nodeName.equalsIgnoreCase("nodeAddress")){
				node.setNodeAddress(this.parseAddress(child));
			}else if(nodeName.equalsIgnoreCase("port")){
				CtrlPlanePortContent port = this.parsePort(child);
				node.addPort(port);
			}
		}
		
		return node;
	}
	
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
			}else if(nodeName.equalsIgnoreCase("portId")){
				port.setPortid(this.parseAddress(child));
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
				link.setLinkProtectionTypes(child.getTextContent());
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
	
	private CtrlPlaneAddressContent parseAddress(Node elem){
		//TODO: add suport for type and value
		CtrlPlaneAddressContent addr = new CtrlPlaneAddressContent();
		addr.setString(elem.getTextContent());
		
		return addr;
	}
	
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
	
	private String getIdAttribute(NamedNodeMap attrs){
		Node attrId = attrs.getNamedItem("id");
		if(attrId != null){
			return attrId.getNodeValue();
		}
		
		return null;
	}

}

package edu.internet2.hopi.dragon.terce.ws.handlers.rce.dynamicdb;

import java.io.IOException;
import java.net.UnknownHostException;
import java.util.ArrayList;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.TERCEClient;
import edu.internet2.hopi.dragon.terce.api.App2TERCERequest;
import edu.internet2.hopi.dragon.terce.api.EROSubobject;
import edu.internet2.hopi.dragon.terce.api.IPv4PrefixSubobject;
import edu.internet2.hopi.dragon.terce.api.TERCEMessageHeader;
import edu.internet2.hopi.dragon.terce.api.TERCEReply;
import edu.internet2.hopi.dragon.terce.api.UnumInterfaceSubobject;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCE;
import edu.internet2.hopi.dragon.terce.ws.handlers.rce.RCEInterface;
import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponseContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.HopList;
import edu.internet2.hopi.dragon.terce.ws.types.rce.Path;
import edu.internet2.hopi.dragon.terce.ws.types.rce.VtagList;

/**
 * RCE implementation that talks to a dynamic TERCE database (formerly the NARB) via its API.
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public class DynamicRCE extends RCE implements RCEInterface{
	
	/**
	 * Requests a path calculation from the dynamic TERCE database using the TERCE API.
	 * The path calculation parameters are from the given SOAP request
	 * 
	 * @param request the SOAP request that contains the path computation parameters
	 * @return The result of a path computation in a SOAP format
	 */
	public FindPathResponse findPath(FindPath request) throws RCEFaultMessageException {
		FindPathResponse response = new FindPathResponse();
        FindPathResponseContent responseContent = new FindPathResponseContent();
        Path path = new Path();
        HopList hops = new HopList();
        App2TERCERequest terceRequest;
        
        /* Contact TERCE */
		try{
			PropertyReader props = new PropertyReader(System.getenv("TERCEWS_HOME") + "/terce-ws.properties");
			terceRequest = generateTERCEAPIRequest(request.getFindPath());
			TERCEClient client = new TERCEClient(props.getProperty("terce.address"), 
									Integer.parseInt(props.getProperty("terce.port"))
								);

			TERCEReply reply = client.sendRequest(terceRequest);
			if(reply.getType() == TERCEReply.TLV_TYPE_TERCE_ERO){
				/* Parse ERO subobjects */
				ArrayList<EROSubobject> ero = reply.getERO();
				for(int i = 0; i < ero.size(); i++){
					CtrlPlaneDomainContent d = new CtrlPlaneDomainContent();
					CtrlPlaneNodeContent n = new CtrlPlaneNodeContent();
					CtrlPlanePortContent p = new CtrlPlanePortContent();
					CtrlPlaneLinkContent l = new CtrlPlaneLinkContent();
					
					/* Right now this just sets the ip address as the domain:node:port:link
					 * values. This is really just a test to see how the topology types map.
					 * I need to manually fix some of the types to make them look better.
					 * Jarda will modify the NARB to return domain:node:port:link info.
					 */
					if(ero.get(i).getType() == EROSubobject.IPV4_PREFIX_SUBOBJ){
						IPv4PrefixSubobject sub = (IPv4PrefixSubobject)ero.get(i);
						d.setId(sub.getIp().getHostAddress());
						n.setId(sub.getIp().getHostAddress());
						CtrlPlaneAddressContent nid = new CtrlPlaneAddressContent();
						nid.setString(sub.getIp().getHostAddress());
						n.setNodeid(nid);
						p.setId(sub.getIp().getHostAddress());
						l.setId(sub.getIp().getHostAddress());
						p.addLink(l);
						n.addPort(p);
						d.addNode(n);
						hops.addDomain(d);
					}else if(ero.get(i).getType() == EROSubobject.UNUM_IF_SUBOBJ){
						UnumInterfaceSubobject sub = (UnumInterfaceSubobject)ero.get(i);
						d.setId(sub.getIp().getHostAddress());
						
						n.setId(sub.getIp().getHostAddress());
						CtrlPlaneAddressContent nid = new CtrlPlaneAddressContent();
						nid.setString(sub.getIp().getHostAddress());
						CtrlPlaneAddressContent nadd = new CtrlPlaneAddressContent();
						nadd.setString(sub.getIp().getHostAddress());
						n.setNodeAddress(nadd);
						n.setNodeid(nid);
						
						p.setId(sub.getIp().getHostAddress());
						l.setId(sub.getIp().getHostAddress());
						p.addLink(l);
						n.addPort(p);
						d.addNode(n);
						hops.addDomain(d);
					}
				}
				
				/* Get vlan tag list */
				ArrayList<Integer> availVTags = reply.getVtagList();
				if(!availVTags.isEmpty()){
					VtagList vtags = new VtagList();
					for(int i = 0; i < availVTags.size(); i++){
						vtags.addVtag(availVTags.get(i).toString());
					}
					path.setAvailableVtags(vtags);
				}
			}else if(reply.getType() == TERCEReply.TLV_TYPE_TERCE_ERROR_CODE){
				throw this.generateRCEException("TERCE returned error type " + reply.getErrorCode());
			}else{
				throw this.generateRCEException("Unknown reply type from TERCE server");
			}
		} catch (UnknownHostException e) {
			throw this.generateRCEException("Invalid IP address in request");
		} catch (IOException e) {
			throw this.generateRCEException("Problem contacting TERCE server: " + e.getMessage());
		}
        
		/* add hops to path and path to response*/
        path.setHops(hops);
        responseContent.setPath(path);
        response.setFindPathResponse(responseContent);
        return response;
	}
	
	/**
	 * Converts a TERCE-WS request to a TERCE-API request
	 * @param soapRequest the SOAP request to be converted
	 * @return a request in the API format
	 * @throws UnknownHostException
	 */
	 private App2TERCERequest generateTERCEAPIRequest(FindPathContent soapRequest) throws UnknownHostException{
     	App2TERCERequest terceRequest;
     	
     	/* Get source and destination */
     	String srcHost = soapRequest.getSrcHost();
     	String dstHost = soapRequest.getDstHost();
     	
     	/* get bandwidth */
     	float bandwidth = soapRequest.getBandwidth();
     	if(bandwidth == 0){
     		bandwidth = 500;
     	}
     	
     	/* get encoding */
     	String encodingString = soapRequest.getEncoding();
     	byte encoding = 0;
     	if(encodingString == null || encodingString.equals("") || encodingString.toLowerCase().equals("ethernet")){
     		encoding = App2TERCERequest.ENCODING_ETHERNET;
     	}
     	
     	/* get swcap */
     	String swcapString = soapRequest.getSwcap();
     	byte swcap = 0;
     	if(swcapString == null || swcapString.equals("") || swcapString.toLowerCase().equals("l2sc")){
     		swcap = App2TERCERequest.SWCAP_L2SC;
     	}
     	
     	/* get swcap */
     	String gpidString = soapRequest.getSwcap();
     	byte[] gpid = {};
     	if(gpidString == null || gpidString.equals("") || gpidString.toLowerCase().equals("ethernet")){
     		gpid = App2TERCERequest.GPID_ETHERNET;
     	}
     	
     	/* Genrate request */
     	terceRequest = new App2TERCERequest(srcHost, dstHost, encoding, swcap, gpid, bandwidth);
     	setTERCEHeaderOptions(soapRequest, terceRequest.getHeader());

     	return terceRequest;
     }
     
	 /**
	  * Sets the header option in a TERCE API request using a TERCE-WS request. 
	  * @param soapRequest the SOAP request containing the necessary header information
	  * @param header the header to be filled with the soapRequest information
	  */
     private void setTERCEHeaderOptions(FindPathContent soapRequest, TERCEMessageHeader header){
     	/* Set vtag */
     	String vtagString = soapRequest.getVtag();
     	int vtag = 0;
     	if(vtagString == null || vtagString.equals("") || vtagString.toLowerCase().equals("none")){
     		vtag = TERCEMessageHeader.VTAG_NONE;
     	}else if(vtagString.toLowerCase().equals("any")){
     		vtag = TERCEMessageHeader.VTAG_ANY;
     		header.addOption(TERCEMessageHeader.OPT_E2E_VTAG);
     	}else{
     		vtag = Integer.parseInt(vtagString);
     		header.addOption(TERCEMessageHeader.OPT_E2E_VTAG);
     	}
     	header.setTag(vtag);
     	
     	/* Set which layers to exclude */
     	Exclude[] excludeList = soapRequest.getExclude();
     	if(excludeList != null){
	        	for(int i = 0; i < excludeList.length; i++){
	        		String layer = excludeList[i].getLayer();
	        		if(layer.equals("1")){
	        			header.addOption(TERCEMessageHeader.OPT_EXCLUDE_L1);
	        		}else if(layer.equals("2")){
	        			header.addOption(TERCEMessageHeader.OPT_EXCLUDE_L2);
	        		}else if(layer.equals("3")){
	        			header.addOption(TERCEMessageHeader.OPT_EXCLUDE_L3);
	        		}else if(layer.equals("TDM")){
	        			header.addOption(TERCEMessageHeader.OPT_EXCLUDE_TDM);
	        		}
	        	}
     	}
     	
     	/* Set other options */
     	if(soapRequest.getAllvtags()){
     		header.addOption(TERCEMessageHeader.OPT_REQ_ALL_VTAGS);
     	}
     	
     	if(soapRequest.getAllwaves()){
     		header.addOption(TERCEMessageHeader.OPT_REQ_ALL_WAVES);
     	}
     	
     	if(soapRequest.getBidirectional()){
     		header.addOption(TERCEMessageHeader.OPT_BIDIRECTIONAL);
     	}
     	
     	if(soapRequest.getMrn()){
     		header.addOption(TERCEMessageHeader.OPT_MRN);
     	}
     	
     	if(soapRequest.getPreferred()){
     		header.addOption(TERCEMessageHeader.OPT_PREFERRED);
     	}
     	
     	if(soapRequest.getStrict()){
     		header.addOption(TERCEMessageHeader.OPT_STRICT);
     	}
     	
     	if(soapRequest.getViamovaz()){
     		header.addOption(TERCEMessageHeader.OPT_VIA_MOVAZ);
     	}
     }
}

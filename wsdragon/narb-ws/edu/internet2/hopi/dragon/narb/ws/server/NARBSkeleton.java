/**
 * NARBSkeleton.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.1.1 Jan 09, 2007 (06:20:51 LKT)
 */
package edu.internet2.hopi.dragon.narb.ws.server;

import java.io.IOException;
import java.net.UnknownHostException;
import java.util.ArrayList;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.narb.NARBClient;
import edu.internet2.hopi.dragon.narb.api.App2NARBRequest;
import edu.internet2.hopi.dragon.narb.api.EROSubobject;
import edu.internet2.hopi.dragon.narb.api.IPv4PrefixSubobject;
import edu.internet2.hopi.dragon.narb.api.NARBMessageHeader;
import edu.internet2.hopi.dragon.narb.api.NARBReply;
import edu.internet2.hopi.dragon.narb.api.UnumInterfaceSubobject;
import edu.internet2.hopi.dragon.narb.ws.FindPath;
import edu.internet2.hopi.dragon.narb.ws.FindPathContent;
import edu.internet2.hopi.dragon.narb.ws.FindPathResponse;
import edu.internet2.hopi.dragon.narb.ws.FindPathResponseContent;
import edu.internet2.hopi.dragon.narb.ws.Hop;
import edu.internet2.hopi.dragon.narb.ws.HopList;
import edu.internet2.hopi.dragon.narb.ws.NARBFault;
import edu.internet2.hopi.dragon.narb.ws.Path;
import edu.internet2.hopi.dragon.narb.ws.VtagList;
import edu.internet2.hopi.dragon.narb.ws.Exclude;
    /**
     *  NARBSkeleton java skeleton for the axisService
     */
    public class NARBSkeleton{
     
         
        /**
         * Auto generated method signature
         
          * @param request
         
         */
        public FindPathResponse findPath(FindPath request) throws NARBFaultMessageException{
        	FindPathResponse response = new FindPathResponse();
            FindPathResponseContent responseContent = new FindPathResponseContent();
            Path path = new Path();
            HopList hops = new HopList();
            App2NARBRequest narbRequest;
			
            /* Conact NARB */
			try{
				PropertyReader props = new PropertyReader(System.getenv("NARBWS_HOME") + "/narb-ws.properties");
				narbRequest = generateNARBAPIRequest(request.getFindPath());
				
				NARBClient client = new NARBClient(props.getProperty("narb.address"), 
										Integer.parseInt(props.getProperty("narb.port"))
									);
				NARBReply reply = client.sendRequest(narbRequest);
				
				if(reply.getType() == NARBReply.TLV_TYPE_NARB_ERO){
					/* Check for VLAN Tag */
					int vtag = reply.getHeader().getTag();
					if(vtag != NARBMessageHeader.VTAG_ANY && vtag != NARBMessageHeader.VTAG_NONE){
						path.setVtag(vtag+"");
					}
					
					/* Parse ERO subobjects */
					ArrayList<EROSubobject> ero = reply.getERO();
					for(int i = 0; i < ero.size(); i++){
						Hop h = new Hop();
						if(ero.get(i).getType() == EROSubobject.IPV4_PREFIX_SUBOBJ){
							IPv4PrefixSubobject sub = (IPv4PrefixSubobject)ero.get(i);
							h.setLoose(sub.isLoose());
							h.setType("ipv4");
							h.setString(sub.getIp().getHostAddress());
							hops.addHop(h);
						}else if(ero.get(i).getType() == EROSubobject.UNUM_IF_SUBOBJ){
							UnumInterfaceSubobject sub = (UnumInterfaceSubobject)ero.get(i);
							h.setLoose(sub.isLoose());
							h.setString(sub.getIp().getHostAddress());
							hops.addHop(h);
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
				}else if(reply.getType() == NARBReply.TLV_TYPE_NARB_ERROR_CODE){
					throw generateNARBException("NARB returned error type " + reply.getErrorCode());
				}else{
					throw generateNARBException("Unknown reply type from NARB server");
				}
			} catch (UnknownHostException e) {
				throw generateNARBException("Invalid IP address in request");
			} catch (IOException e) {
				throw generateNARBException("Problem contacting NARB server: " + e.getMessage());
			}
            
			/* add hops to path and path to response*/
            path.setHops(hops);
            responseContent.setPath(path);
            response.setFindPathResponse(responseContent);
            return response;
        }
        
        private App2NARBRequest generateNARBAPIRequest(FindPathContent soapRequest) throws UnknownHostException{
        	App2NARBRequest narbRequest;
        	
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
        		encoding = App2NARBRequest.ENCODING_ETHERNET;
        	}
        	
        	/* get swcap */
        	String swcapString = soapRequest.getSwcap();
        	byte swcap = 0;
        	if(swcapString == null || swcapString.equals("") || swcapString.toLowerCase().equals("l2sc")){
        		swcap = App2NARBRequest.SWCAP_L2SC;
        	}
        	
        	/* get swcap */
        	String gpidString = soapRequest.getSwcap();
        	byte[] gpid = {};
        	if(gpidString == null || gpidString.equals("") || gpidString.toLowerCase().equals("ethernet")){
        		gpid = App2NARBRequest.GPID_ETHERNET;
        	}
        	
        	/* Genrate request */
        	narbRequest = new App2NARBRequest(srcHost, dstHost, encoding, swcap, gpid, bandwidth);
        	setNARBHeaderOptions(soapRequest, narbRequest.getHeader());

        	return narbRequest;
        }
        
        private void setNARBHeaderOptions(FindPathContent soapRequest, NARBMessageHeader header){
        	/* Set vtag */
        	String vtagString = soapRequest.getVtag();
        	int vtag = 0;
        	if(vtagString == null || vtagString.equals("") || vtagString.toLowerCase().equals("none")){
        		vtag = NARBMessageHeader.VTAG_NONE;
        	}else if(vtagString.toLowerCase().equals("any")){
        		vtag = NARBMessageHeader.VTAG_ANY;
        		header.addOption(NARBMessageHeader.OPT_E2E_VTAG);
        	}else{
        		vtag = Integer.parseInt(vtagString);
        		header.addOption(NARBMessageHeader.OPT_E2E_VTAG);
        	}
        	header.setTag(vtag);
        	
        	/* Set which layers to exclude */
        	Exclude[] excludeList = soapRequest.getExclude();
        	if(excludeList != null){
	        	for(int i = 0; i < excludeList.length; i++){
	        		String layer = excludeList[i].getLayer();
	        		if(layer.equals("1")){
	        			header.addOption(NARBMessageHeader.OPT_EXCLUDE_L1);
	        		}else if(layer.equals("2")){
	        			header.addOption(NARBMessageHeader.OPT_EXCLUDE_L2);
	        		}else if(layer.equals("3")){
	        			header.addOption(NARBMessageHeader.OPT_EXCLUDE_L3);
	        		}else if(layer.equals("TDM")){
	        			header.addOption(NARBMessageHeader.OPT_EXCLUDE_TDM);
	        		}
	        	}
        	}
        	
        	/* Set other options */
        	if(soapRequest.getAllvtags()){
        		header.addOption(NARBMessageHeader.OPT_REQ_ALL_VTAGS);
        	}
        	
        	if(soapRequest.getAllwaves()){
        		header.addOption(NARBMessageHeader.OPT_REQ_ALL_WAVES);
        	}
        	
        	if(soapRequest.getBidirectional()){
        		header.addOption(NARBMessageHeader.OPT_BIDIRECTIONAL);
        	}
        	
        	if(soapRequest.getMrn()){
        		header.addOption(NARBMessageHeader.OPT_MRN);
        	}
        	
        	if(soapRequest.getPreferred()){
        		header.addOption(NARBMessageHeader.OPT_PREFERRED);
        	}
        	
        	if(soapRequest.getStrict()){
        		header.addOption(NARBMessageHeader.OPT_STRICT);
        	}
        	
        	if(soapRequest.getViamovaz()){
        		header.addOption(NARBMessageHeader.OPT_VIA_MOVAZ);
        	}
        }
        
        private NARBFaultMessageException generateNARBException(String msg){
        	NARBFault fault = new NARBFault();
			fault.setMsg(msg);
			NARBFaultMessageException e = new NARBFaultMessageException();
			e.setFaultMessage(fault);
			
			return e;
        }
    }
    
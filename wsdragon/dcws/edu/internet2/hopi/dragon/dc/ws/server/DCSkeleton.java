/**
 * DCSkeleton.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.1.1 Jan 09, 2007 (06:20:51 LKT)
 */
package edu.internet2.hopi.dragon.dc.ws.server;

import java.io.IOException;
import java.net.UnknownHostException;
import java.util.ArrayList;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.dc.DCClient;
import edu.internet2.hopi.dragon.dc.api.App2DCRequest;
import edu.internet2.hopi.dragon.dc.api.EROSubobject;
import edu.internet2.hopi.dragon.dc.api.IPv4PrefixSubobject;
import edu.internet2.hopi.dragon.dc.api.DCMessageHeader;
import edu.internet2.hopi.dragon.dc.api.DCReply;
import edu.internet2.hopi.dragon.dc.api.UnumInterfaceSubobject;
import edu.internet2.hopi.dragon.dc.ws.FindPath;
import edu.internet2.hopi.dragon.dc.ws.FindPathContent;
import edu.internet2.hopi.dragon.dc.ws.FindPathResponse;
import edu.internet2.hopi.dragon.dc.ws.FindPathResponseContent;
import edu.internet2.hopi.dragon.dc.ws.Hop;
import edu.internet2.hopi.dragon.dc.ws.HopList;
import edu.internet2.hopi.dragon.dc.ws.DCFault;
import edu.internet2.hopi.dragon.dc.ws.Path;
import edu.internet2.hopi.dragon.dc.ws.VtagList;
import edu.internet2.hopi.dragon.dc.ws.Exclude;
    /**
     *  DCSkeleton java skeleton for the axisService
     */
    public class DCSkeleton{
     
         
        /**
         * Auto generated method signature
         
          * @param request
         
         */
        public FindPathResponse findPath(FindPath request) throws DCFaultMessageException{
        	FindPathResponse response = new FindPathResponse();
            FindPathResponseContent responseContent = new FindPathResponseContent();
            Path path = new Path();
            HopList hops = new HopList();
            App2DCRequest dcRequest;
			
            /* Conact DC */
			try{
				PropertyReader props = new PropertyReader(System.getenv("DCWS_HOME") + "/dcws.properties");
				dcRequest = generateDCAPIRequest(request.getFindPath());
				
				DCClient client = new DCClient(props.getProperty("dc.address"), 
										Integer.parseInt(props.getProperty("dc.port"))
									);
				DCReply reply = client.sendRequest(dcRequest);
				
				if(reply.getType() == DCReply.TLV_TYPE_DC_ERO){
					/* Check for VLAN Tag */
					int vtag = reply.getHeader().getTag();
					if(vtag != DCMessageHeader.VTAG_ANY && vtag != DCMessageHeader.VTAG_NONE){
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
				}else if(reply.getType() == DCReply.TLV_TYPE_DC_ERROR_CODE){
					throw generateDCException("DC returned error type " + reply.getErrorCode());
				}else{
					throw generateDCException("Unknown reply type from DC server");
				}
			} catch (UnknownHostException e) {
				throw generateDCException("Invalid IP address in request");
			} catch (IOException e) {
				throw generateDCException("Problem contacting DC server: " + e.getMessage());
			}
            
			/* add hops to path and path to response*/
            path.setHops(hops);
            responseContent.setPath(path);
            response.setFindPathResponse(responseContent);
            return response;
        }
        
        private App2DCRequest generateDCAPIRequest(FindPathContent soapRequest) throws UnknownHostException{
        	App2DCRequest dcRequest;
        	
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
        		encoding = App2DCRequest.ENCODING_ETHERNET;
        	}
        	
        	/* get swcap */
        	String swcapString = soapRequest.getSwcap();
        	byte swcap = 0;
        	if(swcapString == null || swcapString.equals("") || swcapString.toLowerCase().equals("l2sc")){
        		swcap = App2DCRequest.SWCAP_L2SC;
        	}
        	
        	/* get swcap */
        	String gpidString = soapRequest.getSwcap();
        	byte[] gpid = {};
        	if(gpidString == null || gpidString.equals("") || gpidString.toLowerCase().equals("ethernet")){
        		gpid = App2DCRequest.GPID_ETHERNET;
        	}
        	
        	/* Genrate request */
        	dcRequest = new App2DCRequest(srcHost, dstHost, encoding, swcap, gpid, bandwidth);
        	setDCHeaderOptions(soapRequest, dcRequest.getHeader());

        	return dcRequest;
        }
        
        private void setDCHeaderOptions(FindPathContent soapRequest, DCMessageHeader header){
        	/* Set vtag */
        	String vtagString = soapRequest.getVtag();
        	int vtag = 0;
        	if(vtagString == null || vtagString.equals("") || vtagString.toLowerCase().equals("none")){
        		vtag = DCMessageHeader.VTAG_NONE;
        	}else if(vtagString.toLowerCase().equals("any")){
        		vtag = DCMessageHeader.VTAG_ANY;
        		header.addOption(DCMessageHeader.OPT_E2E_VTAG);
        	}else{
        		vtag = Integer.parseInt(vtagString);
        		header.addOption(DCMessageHeader.OPT_E2E_VTAG);
        	}
        	header.setTag(vtag);
        	
        	/* Set which layers to exclude */
        	Exclude[] excludeList = soapRequest.getExclude();
        	if(excludeList != null){
	        	for(int i = 0; i < excludeList.length; i++){
	        		String layer = excludeList[i].getLayer();
	        		if(layer.equals("1")){
	        			header.addOption(DCMessageHeader.OPT_EXCLUDE_L1);
	        		}else if(layer.equals("2")){
	        			header.addOption(DCMessageHeader.OPT_EXCLUDE_L2);
	        		}else if(layer.equals("3")){
	        			header.addOption(DCMessageHeader.OPT_EXCLUDE_L3);
	        		}else if(layer.equals("TDM")){
	        			header.addOption(DCMessageHeader.OPT_EXCLUDE_TDM);
	        		}
	        	}
        	}
        	
        	/* Set other options */
        	if(soapRequest.getAllvtags()){
        		header.addOption(DCMessageHeader.OPT_REQ_ALL_VTAGS);
        	}
        	
        	if(soapRequest.getAllwaves()){
        		header.addOption(DCMessageHeader.OPT_REQ_ALL_WAVES);
        	}
        	
        	if(soapRequest.getBidirectional()){
        		header.addOption(DCMessageHeader.OPT_BIDIRECTIONAL);
        	}
        	
        	if(soapRequest.getMrn()){
        		header.addOption(DCMessageHeader.OPT_MRN);
        	}
        	
        	if(soapRequest.getPreferred()){
        		header.addOption(DCMessageHeader.OPT_PREFERRED);
        	}
        	
        	if(soapRequest.getStrict()){
        		header.addOption(DCMessageHeader.OPT_STRICT);
        	}
        	
        	if(soapRequest.getViamovaz()){
        		header.addOption(DCMessageHeader.OPT_VIA_MOVAZ);
        	}
        }
        
        private DCFaultMessageException generateDCException(String msg){
        	DCFault fault = new DCFault();
			fault.setMsg(msg);
			DCFaultMessageException e = new DCFaultMessageException();
			e.setFaultMessage(fault);
			
			return e;
        }
    }
    

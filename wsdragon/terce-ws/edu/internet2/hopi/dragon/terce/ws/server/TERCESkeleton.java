/**
 * TERCESkeleton.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.1.1 Jan 09, 2007 (06:20:51 LKT)
 */
package edu.internet2.hopi.dragon.terce.ws.server;

import java.io.IOException;
import java.net.UnknownHostException;
import java.util.ArrayList;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.TERCEClient;
import edu.internet2.hopi.dragon.terce.api.App2TERCERequest;
import edu.internet2.hopi.dragon.terce.api.EROSubobject;
import edu.internet2.hopi.dragon.terce.api.IPv4PrefixSubobject;
import edu.internet2.hopi.dragon.terce.api.TERCEMessageHeader;
import edu.internet2.hopi.dragon.terce.api.TERCEReply;
import edu.internet2.hopi.dragon.terce.api.UnumInterfaceSubobject;
import edu.internet2.hopi.dragon.terce.ws.FindPath;
import edu.internet2.hopi.dragon.terce.ws.FindPathContent;
import edu.internet2.hopi.dragon.terce.ws.FindPathResponse;
import edu.internet2.hopi.dragon.terce.ws.FindPathResponseContent;
import edu.internet2.hopi.dragon.terce.ws.Hop;
import edu.internet2.hopi.dragon.terce.ws.HopList;
import edu.internet2.hopi.dragon.terce.ws.TERCEFault;
import edu.internet2.hopi.dragon.terce.ws.Path;
import edu.internet2.hopi.dragon.terce.ws.VtagList;
import edu.internet2.hopi.dragon.terce.ws.Exclude;
    /**
     *  TERCESkeleton java skeleton for the axisService
     */
    public class TERCESkeleton{
     
         
        /**
         * Auto generated method signature
         
          * @param request
         
         */
        public FindPathResponse findPath(FindPath request) throws TERCEFaultMessageException{
        	FindPathResponse response = new FindPathResponse();
            FindPathResponseContent responseContent = new FindPathResponseContent();
            Path path = new Path();
            HopList hops = new HopList();
            App2TERCERequest terceRequest;
			
            /* Conact TERCE */
			try{
				PropertyReader props = new PropertyReader(System.getenv("TERCEWS_HOME") + "/terce-ws.properties");
				terceRequest = generateTERCEAPIRequest(request.getFindPath());
				
				TERCEClient client = new TERCEClient(props.getProperty("terce.address"), 
										Integer.parseInt(props.getProperty("terce.port"))
									);
				TERCEReply reply = client.sendRequest(terceRequest);
				
				if(reply.getType() == TERCEReply.TLV_TYPE_TERCE_ERO){
					/* Check for VLAN Tag */
					int vtag = reply.getHeader().getTag();
					if(vtag != TERCEMessageHeader.VTAG_ANY && vtag != TERCEMessageHeader.VTAG_NONE){
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
				}else if(reply.getType() == TERCEReply.TLV_TYPE_TERCE_ERROR_CODE){
					throw generateTERCEException("TERCE returned error type " + reply.getErrorCode());
				}else{
					throw generateTERCEException("Unknown reply type from TERCE server");
				}
			} catch (UnknownHostException e) {
				throw generateTERCEException("Invalid IP address in request");
			} catch (IOException e) {
				throw generateTERCEException("Problem contacting TERCE server: " + e.getMessage());
			}
            
			/* add hops to path and path to response*/
            path.setHops(hops);
            responseContent.setPath(path);
            response.setFindPathResponse(responseContent);
            return response;
        }
        
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
        
        private TERCEFaultMessageException generateTERCEException(String msg){
        	TERCEFault fault = new TERCEFault();
			fault.setMsg(msg);
			TERCEFaultMessageException e = new TERCEFaultMessageException();
			e.setFaultMessage(fault);
			
			return e;
        }
    }
    

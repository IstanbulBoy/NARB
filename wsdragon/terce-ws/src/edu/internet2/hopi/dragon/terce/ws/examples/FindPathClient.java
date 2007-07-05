package edu.internet2.hopi.dragon.terce.ws.examples;

import java.rmi.RemoteException;
import java.util.StringTokenizer;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent;

import edu.internet2.hopi.dragon.terce.ws.types.rce.EndpointContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;
import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.service.TERCEStub;

/**
 * Test program for querying a TERCE-WS path calculation. Accepts a source and destination then requests 
 * a path calculation
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public class FindPathClient {
	public static void main(String[] args){
		try {
			FindPathClient fpc = new FindPathClient();
			TERCEStub terce = null;
			FindPath fp = new FindPath();
			FindPathContent request = new FindPathContent();
			EndpointContent src = null;
			EndpointContent dest = null;	
			
			/* Get arguments */
			if(args.length == 3){
				src = fpc.parseEndpointString(args[0]);
				dest = fpc.parseEndpointString(args[1]);
				terce = new TERCEStub(args[2]);
			}else{
				System.out.println("Expcted java TERCETest source-ip destination-ip terce-url");
			}
			
			/* Format Request */
			request.setSrcEndpoint(src);
			request.setDestEndpoint(dest);
			request.setBandwidth(100);
			request.setPreferred(true);
			request.setStrict(true);
			request.setVtag("any");
			request.setAllvtags(true);
			
			/* Send request and get response*/
			fp.setFindPath(request);
			FindPathResponse response;
			response = terce.findPath(fp);
			EndpointContent[] hops = response.getFindPathResponse().getPath().getHops().getHop();
			
			/* Output path in response */
			System.out.println("Path: ");
			for(EndpointContent hop : hops){
				CtrlPlaneDomainContent d = hop.getDomain();
				CtrlPlaneNodeContent n = d.getNode()[0];
				CtrlPlanePortContent p = n.getPort()[0];
				CtrlPlaneLinkContent[] l = p.getLink();
				System.out.print(d.getId() + ":");
				System.out.print(n.getId() + ":");
				System.out.print(p.getId());
				if(l != null){
					System.out.print(":" + l[0].getId());
				}
				System.out.println();
			}
				
		} catch (RemoteException e) {
			e.printStackTrace();
		} catch (RCEFaultMessageException e) {
			System.err.println(e.getFaultMessage().getMsg());
			e.printStackTrace();
		}
	}
	
	/**
	 * Parses a string in domain:node:port:link format and maps it to an 
	 * EndpointContent object
	 * 
	 * @param strEndpoint the string in domain:node:port:link format to convert
	 * @return an EndpointContent object that can be passed to Axis2
	 */
	public EndpointContent parseEndpointString(String strEndpoint){
		EndpointContent ep = new EndpointContent();
		CtrlPlaneDomainContent domain = new CtrlPlaneDomainContent();
		CtrlPlaneNodeContent node = new CtrlPlaneNodeContent();
		CtrlPlanePortContent port = new CtrlPlanePortContent();
		CtrlPlaneLinkContent link = new CtrlPlaneLinkContent();
		StringTokenizer st = new StringTokenizer(strEndpoint, ":");
		boolean hasLink = false;
		
		/* Convert string to Endpoint */
		domain.setId(st.nextToken());
		node.setId(st.nextToken());
		port.setId(st.nextToken());
		if(st.hasMoreTokens()){
			link.setId(st.nextToken());
			port.addLink(link);
			hasLink = true;
		}
		node.addPort(port);
		domain.addNode(node);
		ep.setDomain(domain);
		
		/* Output endpoint values */
		System.out.printf("Endpoint: %s:%s:%s", domain.getId(), 
				node.getId(), port.getId());
		if(hasLink){
			System.out.printf(":%s", link.getId());
		}
		System.out.println();
		
		return ep;
	}
}

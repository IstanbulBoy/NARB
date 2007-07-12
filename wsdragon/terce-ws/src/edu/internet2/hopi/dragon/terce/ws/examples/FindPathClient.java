package edu.internet2.hopi.dragon.terce.ws.examples;

import java.rmi.RemoteException;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneHopContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePathContent;

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
			TERCEStub terce = null;
			FindPath fp = new FindPath();
			FindPathContent request = new FindPathContent();
			String src = null;
			String dest = null;	
			
			/* Get arguments */
			if(args.length == 3){
				src = args[0];
				dest = args[1];
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
			CtrlPlanePathContent path = response.getFindPathResponse().getPath();
			CtrlPlaneHopContent[] hops = path.getHop();
			
			/* Output path in response */
			System.out.println("Path: ");
			for(CtrlPlaneHopContent hop : hops){
				String d = hop.getDomainIdRef();
				String n = hop.getNodeIdRef();
				String p = hop.getPortIdRef();
				String l = hop.getLinkIdRef();

				if(d != null){
					System.out.println("(Domain) " + d);
				}else if(n != null){
					System.out.println("(Node) " + n);
				}else if(p != null){
					System.out.println("(Port) " + p);
				}else if(l != null){
					System.out.println("(Link) " + l);
				}
			}
			System.out.println("Available VLAN tags: " + response.getFindPathResponse().getAvailableVtags());
		} catch (RemoteException e) {
			e.printStackTrace();
		} catch (RCEFaultMessageException e) {
			System.err.println(e.getFaultMessage().getMsg());
			e.printStackTrace();
		}
	}
}

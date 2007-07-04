package edu.internet2.hopi.dragon.terce.ws.examples;

import java.rmi.RemoteException;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent;

import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;
import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.service.TERCEStub;

/**
 * Test program for querying TERCE-WS accepts a source and destination then requests 
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
			request.setSrcHost(src);
			request.setDstHost(dest);
			request.setBandwidth(100);
			request.setPreferred(true);
			request.setStrict(true);
			request.setVtag("any");
			request.setAllvtags(true);
			
			/* Send request and get response*/
			fp.setFindPath(request);
			FindPathResponse response;
			response = terce.findPath(fp);
			CtrlPlaneDomainContent[] domains = response.getFindPathResponse().getPath().getHops().getDomain();
			
			/* Output path in response */
			System.out.println("Path: ");
			for(CtrlPlaneDomainContent d : domains){
				CtrlPlaneNodeContent n = d.getNode()[0];
				CtrlPlanePortContent p = n.getPort()[0];
				CtrlPlaneLinkContent l = p.getLink()[0];
				System.out.print(d.getId() + ":");
				System.out.print(n.getId() + ":");
				System.out.print(p.getId());
				if(l != null){
					System.out.print(":" + l.getId());
				}
				System.out.println();
			}
				
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (RCEFaultMessageException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

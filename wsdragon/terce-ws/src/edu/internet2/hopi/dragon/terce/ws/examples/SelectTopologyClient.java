package edu.internet2.hopi.dragon.terce.ws.examples;

import java.rmi.RemoteException;

import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent;

import edu.internet2.hopi.dragon.terce.ws.service.TEDBFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.service.TERCEStub;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyContent;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectTypes;

/**
 * Example client making a selectTopology query to the TERCE-WS.
 * It accepts the URL of the service as an argument
 * 
 * @author Andrew Lake (alake@internet2.edu)
 * 
 */
public class SelectTopologyClient {
	public static void main(String[] args){
		try {
			TERCEStub terce = null;
			SelectNetworkTopology selectTopology = new SelectNetworkTopology();
			SelectNetworkTopologyContent request = new SelectNetworkTopologyContent();
			String database = null;
			
			/* Get arguments */
			if(args.length == 1 || args.length == 2){
				terce = new TERCEStub(args[0]);
				if(args.length == 2)
					database = args[1];
			}else{
				System.out.println("Must provide TERCE URL as argument (and optionaly the database)");
				System.exit(0);
			}
			
			/* Format Request */
			request.setFrom(SelectTypes.all);
			if(database != null){
				request.setDatabase(database);
			}
			
			/* Send request and get response*/
			selectTopology.setSelectNetworkTopology(request);
			SelectNetworkTopologyResponse response;
			response = terce.selectNetworkTopology(selectTopology);
			CtrlPlaneDomainContent[] domains = response.getSelectNetworkTopologyResponse().getTopology().getDomain();
			
			/* Output topology in response */
			System.out.println("Topology: ");
			for(CtrlPlaneDomainContent d : domains){
				System.out.println("Domain: " + d.getId());
				CtrlPlaneNodeContent[] nodes = d.getNode();
				for(CtrlPlaneNodeContent n : nodes){
					System.out.println("\tNode:" + n.getId());
					CtrlPlanePortContent[] ports = n.getPort();
					for(CtrlPlanePortContent p : ports){
						System.out.println("\t\tPort: " + p.getId());
						CtrlPlaneLinkContent[] links = p.getLink();
						if(links != null){
							for(CtrlPlaneLinkContent l : links){
								System.out.println("\t\t\tLink:" + l.getId());
							}
						}
					}
				}
				System.out.println();
			}
				
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (TEDBFaultMessageException e) {
			// TODO Auto-generated catch block
			System.out.println(e.getFaultMessage().getMsg());
		}
	}
}

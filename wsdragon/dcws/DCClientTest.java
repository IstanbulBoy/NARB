import java.io.IOException;
import java.net.UnknownHostException;

import edu.internet2.hopi.dragon.dc.DCClient;
import edu.internet2.hopi.dragon.dc.api.App2DCRequest;
import edu.internet2.hopi.dragon.dc.api.DCMessageHeader;
import edu.internet2.hopi.dragon.dc.api.DCReply;
import edu.internet2.hopi.dragon.dc.api.UnumInterfaceSubobject;


public class DCClientTest {
	public static void main(String[] args){
		try {
			/* Make request */
			App2DCRequest request = new App2DCRequest("207.75.164.204", "207.75.164.207", App2DCRequest.ENCODING_ETHERNET, App2DCRequest.SWCAP_L2SC, App2DCRequest.GPID_ETHERNET, 500);
			
			request.getHeader().addOption(DCMessageHeader.OPT_PREFERRED);
			request.getHeader().addOption(DCMessageHeader.OPT_STRICT);
			request.getHeader().addOption(DCMessageHeader.OPT_E2E_VTAG);
			request.getHeader().addOption(DCMessageHeader.OPT_REQ_ALL_VTAGS);
			
			DCClient client = new DCClient("hopiuclp2.internet2.edu", 2609);
			DCReply reply = client.sendRequest(request);
			
			for(int i = 0; i < reply.getERO().size(); i++){
				System.out.println("HOP: ");
				System.out.println("\tLoose: " + reply.getERO().get(i).isLoose());
				System.out.println("\tType: " + reply.getERO().get(i).getType());
				System.out.println("\tTLV Length: " + reply.getERO().get(i).getTLVLength());
				System.out.println("\tReserved: " + ((((UnumInterfaceSubobject) reply.getERO().get(i)).getReserved()[0] << 8) + ((UnumInterfaceSubobject) reply.getERO().get(i)).getReserved()[1]));
				System.out.println("\tIP: " + ((UnumInterfaceSubobject) reply.getERO().get(i)).getIp().getHostAddress());
				System.out.println("\tInterface ID: " + ((UnumInterfaceSubobject) reply.getERO().get(i)).getInterfaceId());
			//	System.out.println("\tAddress Length: " + ((IPv4PrefixSubobject) reply.getERO().get(i)).getIpv4Length());
			//	System.out.println("\tPadding: " + ((IPv4PrefixSubobject) reply.getERO().get(i)).getPadding());
				System.out.println();
			}
			
			System.out.println("ERROR: " + reply.getErrorCode());
			System.out.println("VTAG: " + reply.getHeader().getTag());
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

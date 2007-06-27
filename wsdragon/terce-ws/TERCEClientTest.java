import java.io.IOException;
import java.net.UnknownHostException;

import edu.internet2.hopi.dragon.terce.TERCEClient;
import edu.internet2.hopi.dragon.terce.api.App2TERCERequest;
import edu.internet2.hopi.dragon.terce.api.TERCEMessageHeader;
import edu.internet2.hopi.dragon.terce.api.TERCEReply;
import edu.internet2.hopi.dragon.terce.api.UnumInterfaceSubobject;


public class TERCEClientTest {
	public static void main(String[] args){
		try {
			/* Make request */
			App2TERCERequest request = new App2TERCERequest("207.75.164.204", "207.75.164.207", App2TERCERequest.ENCODING_ETHERNET, App2TERCERequest.SWCAP_L2SC, App2TERCERequest.GPID_ETHERNET, 500);
			
			request.getHeader().addOption(TERCEMessageHeader.OPT_PREFERRED);
			request.getHeader().addOption(TERCEMessageHeader.OPT_STRICT);
			request.getHeader().addOption(TERCEMessageHeader.OPT_E2E_VTAG);
			request.getHeader().addOption(TERCEMessageHeader.OPT_REQ_ALL_VTAGS);
			
			TERCEClient client = new TERCEClient("hopiuclp2.internet2.edu", 2609);
			TERCEReply reply = client.sendRequest(request);
			
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

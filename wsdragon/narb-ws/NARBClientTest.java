import java.io.IOException;
import java.net.UnknownHostException;

import edu.internet2.hopi.dragon.narb.NARBClient;
import edu.internet2.hopi.dragon.narb.api.App2NARBRequest;
import edu.internet2.hopi.dragon.narb.api.NARBMessageHeader;
import edu.internet2.hopi.dragon.narb.api.NARBReply;
import edu.internet2.hopi.dragon.narb.api.UnumInterfaceSubobject;


public class NARBClientTest {
	public static void main(String[] args){
		try {
			/* Make request */
			App2NARBRequest request = new App2NARBRequest("207.75.164.204", "207.75.164.207", App2NARBRequest.ENCODING_ETHERNET, App2NARBRequest.SWCAP_L2SC, App2NARBRequest.GPID_ETHERNET, 500);
			
			request.getHeader().addOption(NARBMessageHeader.OPT_PREFERRED);
			request.getHeader().addOption(NARBMessageHeader.OPT_STRICT);
			request.getHeader().addOption(NARBMessageHeader.OPT_E2E_VTAG);
			request.getHeader().addOption(NARBMessageHeader.OPT_REQ_ALL_VTAGS);
			
			NARBClient client = new NARBClient("hopiuclp2.internet2.edu", 2609);
			NARBReply reply = client.sendRequest(request);
			
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

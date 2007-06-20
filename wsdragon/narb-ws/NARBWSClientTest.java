import java.io.IOException;
import java.net.UnknownHostException;
import java.security.NoSuchAlgorithmException;
import java.util.Scanner;

import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;

import edu.internet2.hopi.dragon.narb.NARBWSClient;
import edu.internet2.hopi.dragon.narb.ws.client.NARBFaultMessageException;
import edu.internet2.hopi.dragon.narb.ws.client.NARBStub;
import edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathContent;
import edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponseContent;

/**
 * Test NARB web service client. Accepts the URL of the service as 
 * a command-line parameter and allows user to enter source, destination, 
 * bandwidth and VLAN tag information.
 * 
 * @author arlake
 */
public class NARBWSClientTest {
	public static void main(String[] args){
		System.setProperty("javax.net.ssl.trustStore", "/Users/arlake/.keystore");
		System.setProperty("javax.net.ssl.trustStorePassword", "iamone2");
		
		if(args.length < 1){
			System.out.println("Usage: NARBWSClientTest http://my-narb-ws-server:8080/axis2/services/NARB");
			System.exit(0);
		}
		
		try{
			Scanner in = new Scanner(System.in);
			System.out.println(args[0]);
			NARBWSClient client = new NARBWSClient(args[0]);
			FindPathContent request = new FindPathContent();
			
			/* Read params */
			//String src = null;
			String src = "207.75.164.204"; //quicker for testing
			while(src == null || src.equals("")){
				System.out.print("Enter Source IP: ");
				src = in.next();
			}
			
			String dst = "207.75.164.207";
			//String dst = null;
			while(dst == null || dst.equals("")){
				System.out.print("Enter Destination IP: ");
				dst = in.next();
			}
			
			String bwidth = "100"; //quicker for testing
			while(bwidth == null || bwidth.equals("")){
				System.out.print("Enter Bandwidth in megabits: ");
				bwidth = in.next();
			}
			
			String vtag = "any"; //quicker for testing
			while(vtag == null || vtag.equals("")){
				System.out.print("Enter VLAN TAG[none, any, 1-4096]: ");
				vtag = in.next();
			}
			
			request.setSrcHost(src);
			request.setDstHost(dst);
			request.setBandwidth(Integer.parseInt(bwidth));
			request.setPreferred(true);
			request.setStrict(true);
			request.setVtag(vtag);
			request.setAllvtags(true);
		
			FindPathResponseContent response = client.sendRequest(request);
			System.out.println("HOP LIST:");
			NARBStub.Hop[] path = response.getPath().getHops().getHop();
			for(int i = 0; i < path.length; i++){
				System.out.println("\tHOP: " + path[i].getString() + "-" + (path[i].getLoose() ? "loose" : "strict"));
			}
			
			System.out.println("\tVTAG: " + response.getPath().getVtag());
			
			String[] availableVtags = response.getPath().getAvailableVtags().getVtag();
			for(String vt : availableVtags){
				System.out.println(vt);
			}
		}catch(NARBFaultMessageException e){
			System.out.println(e.getFaultMessage().getMsg());
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

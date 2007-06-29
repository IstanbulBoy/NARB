import java.rmi.RemoteException;

import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathContent;
import edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse;
import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.service.TERCEStub;



public class TERCETest {
	public static void main(String[] args){
		try {
			TERCEStub terce = new TERCEStub("http://127.0.0.1:8080/axis2/services/TERCE");
			FindPath fp = new FindPath();
			FindPathContent request = new FindPathContent();
			
			request.setSrcHost("207.75.164.204");
			request.setDstHost("207.75.164.207");
			request.setBandwidth(100);
			request.setPreferred(true);
			request.setStrict(true);
			request.setVtag("any");
			request.setAllvtags(true);
			
			fp.setFindPath(request);
			FindPathResponse response;
			response = terce.findPath(fp);
			response.getFindPathResponse();
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (RCEFaultMessageException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

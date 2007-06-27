package edu.internet2.hopi.dragon.terce;


import java.io.IOException;
import java.net.UnknownHostException;
import edu.internet2.hopi.dragon.terce.ws.client.TERCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.client.TERCEStub;
import edu.internet2.hopi.dragon.terce.ws.client.TERCEStub.FindPath;
import edu.internet2.hopi.dragon.terce.ws.client.TERCEStub.FindPathContent;
import edu.internet2.hopi.dragon.terce.ws.client.TERCEStub.FindPathResponse;
import edu.internet2.hopi.dragon.terce.ws.client.TERCEStub.FindPathResponseContent;

/**
 * Class used to issue web service requests to a TERCE server.
 * 
 * @author Andrew Lake (alake@internet2.edu)
 *
 */
public class TERCEWSClient {
	private String url;
	
	/**
	 * Constructor that accepts the URL to the web service
	 * @param url URL to the web service
	 */
	public TERCEWSClient(String url){
		this.url = url;
	}
	
	/**
	 * Submits a request from the application to the TERCE web servuce server and returns the response.
	 * @param request An object representation of the request parameters
	 * @return an object representation of the server's response
	 * @throws UnknownHostException thrown when unable to connect to the TERCE web service server because the host is not recognized
	 * @throws IOException thrown when there is an error reading or writing to the socket connected to the TERCE web service server
	 * @throws TERCEFaultMessageException  thrown when the TERCE web service returns an error 
	 */
	public FindPathResponseContent sendRequest(FindPathContent request) throws UnknownHostException, IOException, TERCEFaultMessageException{
		TERCEStub terce = new TERCEStub(url);
		FindPath fp = new FindPath();
		fp.setFindPath(request);
		FindPathResponse response = terce.findPath(fp);
		
		return response.getFindPathResponse();
	}

	/**
	 * Returns a string representation of the URL of the web service server
	 * 
	 * @return a string representation of the URL of the web service server
	 */
	public String getURL() {
		return url;
	}

	/**
	 * Set the URL of the TERCE web service server
	 * 
	 * @param url the URL of the TERCE web service server
	 */
	public void setURL(String url) {
		this.url = url;
	}
}

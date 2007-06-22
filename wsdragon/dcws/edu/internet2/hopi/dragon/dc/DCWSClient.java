package edu.internet2.hopi.dragon.dc;


import java.io.IOException;
import java.net.UnknownHostException;
import edu.internet2.hopi.dragon.dc.ws.client.DCFaultMessageException;
import edu.internet2.hopi.dragon.dc.ws.client.DCStub;
import edu.internet2.hopi.dragon.dc.ws.client.DCStub.FindPath;
import edu.internet2.hopi.dragon.dc.ws.client.DCStub.FindPathContent;
import edu.internet2.hopi.dragon.dc.ws.client.DCStub.FindPathResponse;
import edu.internet2.hopi.dragon.dc.ws.client.DCStub.FindPathResponseContent;

/**
 * Class used to issue web service requests to a DC server.
 * 
 * @author Andrew Lake (alake@internet2.edu)
 *
 */
public class DCWSClient {
	private String url;
	
	/**
	 * Constructor that accepts the URL to the web service
	 * @param url URL to the web service
	 */
	public DCWSClient(String url){
		this.url = url;
	}
	
	/**
	 * Submits a request from the application to the DC web servuce server and returns the response.
	 * @param request An object representation of the request parameters
	 * @return an object representation of the server's response
	 * @throws UnknownHostException thrown when unable to connect to the DC web service server because the host is not recognized
	 * @throws IOException thrown when there is an error reading or writing to the socket connected to the DC web service server
	 * @throws DCFaultMessageException  thrown when the DC web service returns an error 
	 */
	public FindPathResponseContent sendRequest(FindPathContent request) throws UnknownHostException, IOException, DCFaultMessageException{
		DCStub dc = new DCStub(url);
		FindPath fp = new FindPath();
		fp.setFindPath(request);
		FindPathResponse response = dc.findPath(fp);
		
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
	 * Set the URL of the DC web service server
	 * 
	 * @param url the URL of the DC web service server
	 */
	public void setURL(String url) {
		this.url = url;
	}
}

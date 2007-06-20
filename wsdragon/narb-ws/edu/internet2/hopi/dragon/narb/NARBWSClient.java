package edu.internet2.hopi.dragon.narb;


import java.io.IOException;
import java.net.UnknownHostException;
import edu.internet2.hopi.dragon.narb.ws.client.NARBFaultMessageException;
import edu.internet2.hopi.dragon.narb.ws.client.NARBStub;
import edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPath;
import edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathContent;
import edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponse;
import edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponseContent;

/**
 * Class used to issue web service requests to a NARB server.
 * 
 * @author Andrew Lake (alake@internet2.edu)
 *
 */
public class NARBWSClient {
	private String url;
	
	/**
	 * Constructor that accepts the URL to the web service
	 * @param url URL to the web service
	 */
	public NARBWSClient(String url){
		this.url = url;
	}
	
	/**
	 * Submits a request from the application to the NARB web servuce server and returns the response.
	 * @param request An object representation of the request parameters
	 * @return an object representation of the server's response
	 * @throws UnknownHostException thrown when unable to connect to the NARB web service server because the host is not recognized
	 * @throws IOException thrown when there is an error reading or writing to the socket connected to the NARB web service server
	 * @throws NARBFaultMessageException  thrown when the NARB web service returns an error 
	 */
	public FindPathResponseContent sendRequest(FindPathContent request) throws UnknownHostException, IOException, NARBFaultMessageException{
		NARBStub narb = new NARBStub(url);
		FindPath fp = new FindPath();
		fp.setFindPath(request);
		FindPathResponse response = narb.findPath(fp);
		
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
	 * Set the URL of the NARB web service server
	 * 
	 * @param url the URL of the NARB web service server
	 */
	public void setURL(String url) {
		this.url = url;
	}
}

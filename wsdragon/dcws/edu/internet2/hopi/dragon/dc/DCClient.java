package edu.internet2.hopi.dragon.dc;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import edu.internet2.hopi.dragon.dc.api.App2DCRequest;
import edu.internet2.hopi.dragon.dc.api.DCReply;

/**
 * Class used to issue requests on behalf of the application to a DC server. 
 * An example request is one asking for a path between two endpoints. 
 * The client does not currently support the DC-to-DC requests that are used for path authorization.
 *
 * @author Andrew Lake (alake@internet2.edu)
 */
public class DCClient {
	private String host;
	private int port;
	
	/**
	 * Constructor that accepts the hostname of the DC server and the TCP port number on which it is running
	 * @param host A String representation of either an IP address or DNS name used to identify the DC server
	 * @param port The remote TCP port on which the DC server is running
	 */
	public DCClient(String host, int port){
		this.host = host;
		this.port = port;
	}
	
	/**
	 * Submits a request from the application to the DC server and returns the DC server's response.
	 * @param request An object representation of the request parameters
	 * @return an DCReply object representation of the DC server's response
	 * @throws UnknownHostException thrown when unable to connect to the DC server becaus ethe host is not recognized
	 * @throws IOException thrown when there is an error reading or writing to the socket connected to the DC server
	 */
	public DCReply sendRequest(App2DCRequest request) throws UnknownHostException, IOException{
		Socket sock = new Socket(host, port);
		BufferedInputStream in = new BufferedInputStream(sock.getInputStream());
		BufferedOutputStream out = new BufferedOutputStream(sock.getOutputStream());
		
		out.write(request.toBytes(), 0, 44);
		out.flush();
		
		byte[] buf = new byte[10000];
		in.read(buf);
		
		DCReply reply = new DCReply(buf);
		
		sock.close();
		
		return reply;
	}

	/**
	 * Returns a string representation of the currently set host address of name 
	 * that represents the location of the DC server
	 * 
	 * @return a string representation of the DC server host name or address
	 */
	public String getHost() {
		return host;
	}

	/**
	 * Set IP Address or DNS name that represents the location of the DC server
	 * 
	 * @param host the IP Address or DNS name that represents the location of the DC server
	 */
	public void setHost(String host) {
		this.host = host;
	}

	/**
	 * Returns the currently set remote TCP port number that will be used to contact the DC server
	 * @return the currently set remote TCP port number that will be used to contact the DC server
	 */
	public int getPort() {
		return port;
	}

	/**
	 * Set the remote TCP port number on which the client will try to reach the DC server
	 * @param port the remote TCP port number on which the client will try to reach the DC server
	 */
	public void setPort(int port) {
		this.port = port;
	}
}

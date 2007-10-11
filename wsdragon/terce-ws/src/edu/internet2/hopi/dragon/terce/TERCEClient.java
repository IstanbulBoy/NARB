package edu.internet2.hopi.dragon.terce;

import edu.internet2.hopi.dragon.terce.api.App2TERCERequest;
import edu.internet2.hopi.dragon.terce.api.TERCEReply;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;

import java.net.Socket;
import java.net.UnknownHostException;


/**
 * Class used to issue requests on behalf of the application to a TERCE server.
 * An example request is one asking for a path between two endpoints.
 * The client does not currently support the TERCE-to-TERCE requests that are used for path authorization.
 *
 * @author Andrew Lake (alake@internet2.edu)
 */
public class TERCEClient {
    private String host;
    private int port;

    /**
     * Constructor that accepts the hostname of the TERCE server and the TCP port number on which it is running
     * @param host A String representation of either an IP address or DNS name used to identify the TERCE server
     * @param port The remote TCP port on which the TERCE server is running
     */
    public TERCEClient(String host, int port) {
        this.host = host;
        this.port = port;
    }

    /**
     * Submits a request from the application to the TERCE server and returns the TERCE server's response.
     * @param request An object representation of the request parameters
     * @return an TERCEReply object representation of the TERCE server's response
     * @throws UnknownHostException thrown when unable to connect to the TERCE server becaus ethe host is not recognized
     * @throws IOException thrown when there is an error reading or writing to the socket connected to the TERCE server
     */
    public TERCEReply sendRequest(App2TERCERequest request)
        throws UnknownHostException, IOException {
        Socket sock = new Socket(host, port);
        BufferedInputStream in = new BufferedInputStream(sock.getInputStream());
        BufferedOutputStream out = new BufferedOutputStream(sock.getOutputStream());

        out.write(request.toBytes(), 0, 44);
        out.flush();

        byte[] buf = new byte[10000];
        in.read(buf);

        TERCEReply reply = new TERCEReply(buf);

        sock.close();

        return reply;
    }

    /**
     * Returns a string representation of the currently set host address of name
     * that represents the location of the TERCE server
     *
     * @return a string representation of the TERCE server host name or address
     */
    public String getHost() {
        return host;
    }

    /**
     * Set IP Address or DNS name that represents the location of the TERCE server
     *
     * @param host the IP Address or DNS name that represents the location of the TERCE server
     */
    public void setHost(String host) {
        this.host = host;
    }

    /**
     * Returns the currently set remote TCP port number that will be used to contact the TERCE server
     * @return the currently set remote TCP port number that will be used to contact the TERCE server
     */
    public int getPort() {
        return port;
    }

    /**
     * Set the remote TCP port number on which the client will try to reach the TERCE server
     * @param port the remote TCP port number on which the client will try to reach the TERCE server
     */
    public void setPort(int port) {
        this.port = port;
    }
}

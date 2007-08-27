/*
 * TERCECore.java
 *
 * Created on August 13, 2007, 1:44 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce;

import edu.internet2.hopi.dragon.terce.api.TERCEMessage;
import edu.internet2.hopi.dragon.terce.api.TERCEMessageHeader;
import edu.internet2.hopi.dragon.terce.tedb.LSA;
import edu.internet2.hopi.dragon.terce.tedb.LSASet;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;

/**
 *
 * @author jflidr
 */
public class TERCECore implements Runnable {
    
    private ServerSocket ss; //server socket
    private Socket cs; //connected socket
    private Socket as; //async socket
    
    private int serverPort;
    private volatile boolean axisRunning = true;
    private volatile boolean forceReset;
    
    private InputStream is;
    private OutputStream os;
    
    private InetAddress narbIP = null;
    private int narbAsyncPort = 0;
    
    private final static int SERVER_TO = 2000;
    private boolean isInitialized = false;
    private boolean isAsyncConnected = false;
    
    private LSASet TEdb;
    
    /**
     * Creates a new instance of TERCECore
     */
    public TERCECore(int p) {
        serverPort = p;
        TEdb = new LSASet();
    }
    
    public void run() {
        TERCEMessage reply;
        TERCEMessageHeader mh;
        int l;
        int n;
        int ret;
        byte[] hbuf = new byte[TERCEMessageHeader.SIZE];
        
        try {
            ss = new ServerSocket(serverPort);
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        while(axisRunning) {
            try {
                cs = ss.accept();
                is = cs.getInputStream();
                os = cs.getOutputStream();
                
                /* main msg loop */
                forceReset = false;
                while(!forceReset) {
                    /* read the header */
                    l = hbuf.length;
                    n = 0;
                    while(n < l) {
                        ret = is.read(hbuf, n, l-n);
                        if(ret < 0) {
                            System.out.println("narb at " + cs.getInetAddress().toString() + " disconnected unexpectedly");
                            System.out.println("restarting server...");
                            forceReset = true;
                            break;
                        }
                        n += ret;
                    }
                    if(forceReset)
                        break;
                    
                    /* form the header object */
                    mh = new TERCEMessageHeader(hbuf);
//                    
//                    System.out.println("type: " + mh.getMessageType());
//                    System.out.println("length: " + mh.getLength());
//                    System.out.println("ucid: " + mh.getUcid());
//                    System.out.println("seqn: " + mh.getSequenceNumber());
//                    System.out.println("chksum: " + mh.getChksum());
//                    System.out.println("tag: " + mh.getTag());
                    if(!mh.isValid()) {
                        System.err.println("invalid message");
                        System.out.println("restarting server...");
                        break;
                    }
                    
                    /* read the data */
                    byte[] data = new byte[(int)mh.getLength()];
                    l = data.length;
                    if(l == 0) {
                        data = null;
                    }
                    n = 0;
                    while(n < l) {
                        ret = is.read(data, n, l-n);
                        if(ret < 0) {
                            System.out.println("narb at " + cs.getInetAddress().toString() + " disconnected unexpectedly");
                            System.out.println("restarting server...");
                            forceReset = true;
                            break;
                        }
                        n += ret;
                    }
                    if(forceReset)
                        break;
                    
                    processMessage(hbuf, data);
                }
                resetServer();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }
    
    private void processMessage(byte[] hdr, byte[] data) {
        byte[] h = null;
        byte[] d = null;
        Thread mrth;
        Thread mpth;
        
        h = hdr.clone();
        if(data != null)
            d = data.clone();
        
        mpth = new Thread(new TERCEMessageProcessor(this, h, d));
        mpth.start();
        
        mrth = new Thread(new TERCEResponder(this, h, d));
        mrth.start();
    }
    
    public void stopServer() {
        axisRunning = false;
        try {
            ss.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    
    private void resetServer() {
        if(!cs.isClosed()) {
            try {
                cs.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
        if(isAsyncConnected && !as.isClosed()) {
            try {
                as.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
        
        /* if narb's gone south clean up all the dynamic data structures */
        TEdb.removeAllElements();
        
        isInitialized = false;
        isAsyncConnected = false;
        forceReset = true;
    }
    
    public void openAsync(int port) {
        if(isAsyncConnected)
            return;
        if(cs.isClosed())
            return;
        if((port <= 0) || (port > 65535)) {
            System.err.println("Invalid Narb Async Port");
            System.out.println("Restarting server");
            resetServer();
            return;
        }
        narbIP = cs.getInetAddress();
        narbAsyncPort = port;
        
        if(narbIP != null) {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
            try {
                as = new Socket();
                as.connect(new InetSocketAddress(narbIP, narbAsyncPort), SERVER_TO);
            } catch (IOException ex) {
                System.err.println("Narb Async Socket Error");
                System.out.println("Restarting server");
                resetServer();
                return;
            }
            System.out.println("opened async side channel to narb @" + narbIP.toString() + ":" + narbAsyncPort);
            isAsyncConnected = true;
        }
    }
    
    public OutputStream getOutputStream() {
        return os;
    }
    
    void addLSA(LSA l) {
        TEdb.add(l);
        System.out.println(l.toString());
    }
}

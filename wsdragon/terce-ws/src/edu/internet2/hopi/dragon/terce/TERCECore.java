/*
 * TERCECore.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.api.TERCEMessage;
import edu.internet2.hopi.dragon.terce.api.TERCEMessageHeader;
import edu.internet2.hopi.dragon.terce.tedb.LSA;
import edu.internet2.hopi.dragon.terce.tedb.LSASet;
import edu.internet2.hopi.dragon.terce.ws.handlers.TERCEHandler;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Timer;
import java.util.TimerTask;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneTopologyContent;

/**
 *
 * @author jflidr
 */
public class TERCECore implements Runnable {
    private PropertyReader props = null;
    
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
    
    private LSASet teDB;
    private String topologyID = null;
    private String idcID = null;
    private boolean debugMode = false;
    private String xmlDumpFile = null;
    
    private final Object dumpLock = new Object();
    
    /**
     * Creates a new instance of TERCECore
     */
    public TERCECore(int p) {
        String tmp = null;
        serverPort = p;
        teDB = new LSASet();
        try {
            props = TERCEHandler.createPropertyReader();
            topologyID = props.getProperty("terce.topology.id");
            idcID = props.getProperty("terce.topology.idcid");
            TERCEGlobals.minRsvBW = props.getProperty("terce.topology.domain.node.port.minrsvbw");
            TERCEGlobals.granularity = props.getProperty("terce.topology.domain.node.port.granularity");
            TERCEGlobals.vlanMTU = props.getProperty("terce.topology.domain.node.port.link.vlan.mtu");
            tmp = props.getProperty("terce.debug");
            if(tmp != null)
                debugMode = (tmp.compareToIgnoreCase("yes") == 0);
            tmp = props.getProperty("terce.xml.dump");
            if(tmp != null)
                xmlDumpFile = tmp;
        } catch (IOException ex) {
            String msg = "";
            msg += "*******************************************************\n";
            msg += "* WARNING: the configuration file terce-ws.properties *\n";
            msg += "* was not found. Make sure that TERCEWS_HOME is set.  *\n";
            msg += "*******************************************************\n";
            System.out.println("WARNING: property file not found");
            new TERCEGlobals.delayedWarning(msg);
        }
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
        teDB.removeAllElements();
        
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
    
    void addLSA(LSA l) throws TERCELSAException {
        teDB.addLSA(l);
        if(debugMode)
            System.out.println(l.toString());
        if(xmlDumpFile == null)
            return;
        synchronized(dumpLock) {
            if(!TERCEGlobals.topologyDumpScheduled) {
                TERCEGlobals.topologyDumpScheduled = true;
                scheduleTopologyDump();
            }
        }
    }
    
    public CtrlPlaneTopologyContent getTopology() {
        CtrlPlaneTopologyContent topology = teDB.getTopology();
        
        if(topologyID != null)
            topology.setId(topologyID);
        else
            topology.setId("unknown");
        if(idcID != null)
            topology.setIdcId(idcID);
        else
            topology.setIdcId("unknown");
        
        return topology;
    }
    
    private void scheduleTopologyDump() {
        Timer tmr;
        tmr = new Timer();
        tmr.schedule(new TimerTask() {
            public void run() {
                TERCEUtilities.dumpLocalTopology(xmlDumpFile);
            }
        }, 3000);
    }
}

/*
 * LSASet.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce.tedb;

import edu.internet2.hopi.dragon.terce.TERCEGlobals;
import edu.internet2.hopi.dragon.terce.TERCELSAException;
import edu.internet2.hopi.dragon.terce.TERCEUtilities;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Vector;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwcapContent;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwitchingCapabilitySpecficInfo;
import org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneTopologyContent;

/**
 *
 * @author jflidr
 */
public class LSASet extends Vector<LSA> {
    
    private static final long UNKNOWN_DOMAIN_ID = -1;
    private static final long FOREIGN_DOMAIN_ID = -2;
    private static final long DANGLING_DOMAIN_ID = -3;
    
    private CtrlPlaneDomainContent domainContent = null;
    private nodeSet nodes = null;
    private portSet ports = null;
    
    /**
     * Creates a new instance of LSASet
     */
    public LSASet() {
        super(10, 10);
        domainContent = new CtrlPlaneDomainContent();
        nodes = new nodeSet();
        ports = new portSet();
    }
    
    public void addLSA(LSA lsa) throws TERCELSAException {
        TLV t = null;
        long l;
        float f;
        
        //LINK TLV
        if((t = lsa.getLinkTLV()) != null) {
            if(domainContent.getId() == null) {
                domainContent.setId(Long.toHexString(t.getDomainID()));
            } else {
                if(domainContent.getId().compareTo(Long.toHexString(t.getDomainID())) != 0)
                    throw new TERCELSAException("Mismatched Domain ID");
            }
            //build port and it's XML content
            port p = new port(lsa.getAdvRtr());
            
            l = lsa.getAdvRtr();
            p.setPortID(l);
            
            f = t.getMaxBW();
            p.setMaxBW(f);
            
            f = t.getMaxRsvBW();
            p.setMaxRsvBW(f);
            
            if(TERCEGlobals.minRsvBW != null) {
                try {
                    f = Float.valueOf(TERCEGlobals.minRsvBW);
                    p.setMinRsvBW(f);
                } catch (NumberFormatException ex) {
                    System.out.println("terce-ww.properties: malformed min. rsv. BW");
                }
            }
            
            f = t.getUnrsvBW();
            p.setUnrsvBW(f);
            
            if(TERCEGlobals.granularity != null) {
                try {
                    f = Float.valueOf(TERCEGlobals.granularity);
                    p.setGranularity(f);
                } catch (NumberFormatException ex) {
                    System.out.println("terce-ww.properties: malformed granularity");
                }
            }
            
            //build link and it's XML content
            l = t.getLinkID();
            p.setLinkID(l);
            p.setRmtLinkID(l);
            p.setRmtPortID(l);
            p.setRmtNodeID(l);
            
            //for this we will have to wait till all the LSAs are received (to
            //distinguish between inter and intra - domainContent links)
            p.setRmtDomainID(UNKNOWN_DOMAIN_ID);
            
            l = t.getMetric();
            p.setTEMetric(l);
            
            //build swcap and it's XML content
            l = t.getSCType();
            p.setSCType(l);
            
            l = t.getSCEnc();
            p.setSCEnc(l);
            
            //buid swcap specific info and it's XML content
            if(t.isVlan()) {
                p.initVlanType();
                
                if(TERCEGlobals.vlanMTU != null) {
                    l = Long.valueOf(TERCEGlobals.vlanMTU);
                    p.setVlanMTU(l);
                }
                
                Vector<int[]>vr = t.getAvailVlanRanges();
                p.setVlanR(vr);
                
            } else if(t.isTdm()) {
                throw new TERCELSAException("TDM switching capability unsupported by the schema");
            }else if(t.isPsc()) {
                throw new TERCELSAException("PSC switching capability unsupported by the schema");
            }
            
            ports.add(p);
        }
        //RTR ADDR TLV
        else if((t = lsa.getRtrAddrTLV()) != null) {
            node n = new node(lsa.getAdvRtr());
            
            l = t.getRtrAddr();
            n.setNodeID(l);
            n.setNodeAddr(l);
            
            nodes.add(n);
        }
        add(lsa);
    }
    
    public CtrlPlaneTopologyContent getTopology() {
        CtrlPlaneTopologyContent topology = new CtrlPlaneTopologyContent();
        //set the unitialized domain IDs
        ports.setRmtDomainID();
        // add the port XML content to nodes
        ports.combineWithNodes();
        
        for (int i = 0; i < nodes.size(); i++) {
            domainContent.addNode(nodes.get(i).nodeContent);
        }
        
        topology.addDomain(domainContent);
        return topology;
    }
    
    private String getNodeID(long addr) throws TERCELSAException {
        return getNodeAddr(addr);
    }
    
    private String getNodeAddr(long addr) throws TERCELSAException {
        InetAddress iAddr;
        
        if(addr == -1)
            throw new TERCELSAException("Missing Router Address");
        byte[] bAddr = {(byte)((addr>>24)&0xffL),
        (byte)((addr>>16)&0xffL),
        (byte)((addr>>8)&0xffL),
        (byte)((addr)&0xffL)};
        try {
            iAddr = InetAddress.getByAddress(bAddr);
        } catch (UnknownHostException ex) {
            throw new TERCELSAException("Malformed Router Address");
        }
        return iAddr.getHostAddress();
    }
    
    private class node {
        private CtrlPlaneNodeContent nodeContent = null;
        
        private long advRtr;
        
        private long rawNodeID;
        private long rawNodeAddr;
        
        private node(long rtr) {
            advRtr = rtr;
            nodeContent = new CtrlPlaneNodeContent();
        }
        
        private void setNodeID(long l) throws TERCELSAException {
            rawNodeID = l;
            nodeContent.setId(getNodeID(l)); // 192.168.1.8
        }
        
        private void setNodeAddr(long l) throws TERCELSAException {
            rawNodeAddr = l;
            nodeContent.setAddress(getNodeAddr(l)); // 192.168.1.8
        }
    }
    
    private class port {
        private CtrlPlanePortContent portContent = null;
        private CtrlPlaneLinkContent linkContent = null;
        private CtrlPlaneSwcapContent swcapContent = null;
        private CtrlPlaneSwitchingCapabilitySpecficInfo specInfoContent = null;
        
        private long advRtr;
        
        private long rawID;
        private float rawMaxBW;
        private float rawMaxRsvBW;
        private float rawMinRsvBW;
        private float rawUnrsvBW;
        private float rawGranularity;
        private long rawLinkID;
        private long rawRmtLinkID;
        private long rawRmtPortID;
        private long rawRmtNodeID;
        private long rawRmtDomainID;
        private long rawTEMetric;
        private long rawSCType;
        private long rawSCEnc;
        private long rawVlanMTU;
        private Vector<int[]> rawVlanRanges;
        
        private port(long rtr) {
            advRtr = rtr;
            portContent = new CtrlPlanePortContent();
            linkContent = new CtrlPlaneLinkContent();
            swcapContent = new CtrlPlaneSwcapContent();
            
            linkContent.setSwitchingCapabilityDescriptors(swcapContent);
            portContent.addLink(linkContent);
        }
        
        private void initVlanType() {
            specInfoContent = new CtrlPlaneSwitchingCapabilitySpecficInfo();
            swcapContent.setSwitchingCapabilitySpecficInfo(specInfoContent);
        }
        
        private void setPortID(long l) {
            rawID = l;
            portContent.setId("port-" + Long.toHexString(l)); // pC-c0a80108
        }
        
        private void setMaxBW(float f) {
            rawMaxBW = f;
            portContent.setCapacity(TERCEUtilities.toNetString(8*f)); // 1Gbps
        }
        
        private void setMaxRsvBW(float f) {
            rawMaxRsvBW = f;
            portContent.setMaximumReservableCapacity(TERCEUtilities.toNetString(8*f)); // 1Gbps
        }
        
        private void setMinRsvBW(float f) {
            rawMinRsvBW = f;
            portContent.setMinimumReservableCapacity(TERCEUtilities.toNetString(f*1000000)); // 100Mbps
        }
        
        private void setUnrsvBW(float f) {
            rawUnrsvBW = f;
            portContent.setUnreservedCapacity(TERCEUtilities.toNetString(8*f)); // 1Gbps
        }
        
        private void setGranularity(float f) {
            rawGranularity = f;
            portContent.setGranularity(TERCEUtilities.toNetString(f*1000000)); // 100Mbps
        }
        
        private void setLinkID(long l) {
            rawLinkID = l;
            linkContent.setId(Long.toHexString(advRtr) + "-" + Long.toHexString(l)); // c0a80108-c0a80204
        }
        
        private void setRmtLinkID(long l) {
            rawRmtLinkID = l;
            linkContent.setRemoteLinkId(Long.toHexString(l)); // c0a80204
        }
        
        private void setRmtPortID(long l) {
            rawRmtPortID = l;
            linkContent.setRemotePortId("port-" + Long.toHexString(l)); // port-c0a80204
        }
        
        private void setRmtNodeID(long l) throws TERCELSAException {
            rawRmtNodeID = l;
            linkContent.setRemoteNodeId(getNodeID(l)); // 192.168.2.4
        }
        
        private void setRmtDomainID(long l) {
            rawRmtDomainID = l;
            if(l == UNKNOWN_DOMAIN_ID)
                linkContent.setRemoteDomainId("unknown");
            else if(l == FOREIGN_DOMAIN_ID)
                linkContent.setRemoteDomainId("foreign");
            else if(l == DANGLING_DOMAIN_ID)
                linkContent.setRemoteDomainId("dangling");
            else
                linkContent.setRemoteDomainId(Long.toHexString(l)); // c0a80200
        }
        
        private void setRmtDomainID(String s) {
            Long l;
            try {
                l = Long.valueOf(s, 16);
            } catch (NumberFormatException ex) {
                l = FOREIGN_DOMAIN_ID;
            }
            setRmtDomainID(l);
        }
        
        private void setTEMetric(long l) {
            rawTEMetric = l;
            linkContent.setTrafficEngineeringMetric(Long.toString(l)); // 50
        }
        
        private void setSCType(long l) {
            rawSCType = l;
            swcapContent.setSwitchingcapType(TERCEGlobals.swcapStrDescs.getStr(l));
        }
        
        private void setSCEnc(long l) {
            rawSCEnc = l;
            swcapContent.setEncodingType(TERCEGlobals.encStrDescs.getStr(l));
        }
        
        private void setVlanMTU(long l) {
            rawVlanMTU = l;
            specInfoContent.setInterfaceMTU((int)l);
        }
        
        private void setVlanR(Vector<int[]> vr) {
            rawVlanRanges = vr;
            String s = "";
            for (int i = 0; i < vr.size(); i++) {
                s += vr.get(i)[0] + "-" + vr.get(i)[1];
                if(i < (vr.size()-1))
                    s += ",";
            }
            if(!s.equals(""))
                specInfoContent.setVlanRangeAvailability(s);
        }
    }
    
    private class nodeSet extends Vector<node> {
        private nodeSet() {
            super();
        }
        
        private boolean haveRtr(long l) {
            for (int i = 0; i < size(); i++) {
                if(get(i).rawNodeAddr == l)
                    return true;
            }
            return false;
        }
        
        private void addPortContents(port p) {
            for (int i = 0; i < size(); i++) {
                if(get(i).rawNodeAddr == p.advRtr) {
                    get(i).nodeContent.addPort(p.portContent);
                    return;
                }
            }
        }
    }
    
    private class portSet extends Vector<port> {
        private portSet() {
            super();
        }
        
        private void setRmtDomainID() {
            for (int i = 0; i < size(); i++) {
                if(nodes.haveRtr(get(i).rawRmtLinkID)) {
                    get(i).setRmtDomainID(domainContent.getId());
                } else {
                    get(i).setRmtDomainID(DANGLING_DOMAIN_ID);
                }
            }
        }
        
        private void combineWithNodes() {
            for (int i = 0; i < size(); i++) {
                nodes.addPortContents(get(i));
            }
        }
    }
}

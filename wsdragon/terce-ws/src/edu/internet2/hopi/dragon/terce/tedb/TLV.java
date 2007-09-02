/*
 * TLV.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce.tedb;

import edu.internet2.hopi.dragon.terce.TERCEGlobals;
import edu.internet2.hopi.dragon.terce.TERCELSAException;
import edu.internet2.hopi.dragon.terce.TERCENetworkDataException;
import edu.internet2.hopi.dragon.terce.TERCEUtilities;
import java.util.Vector;

/**
 *
 * @author jflidr
 */
public class TLV {
    /* opaque data */
    private long type;
    private long length;
    
    private final static int TLV_HDR_SIZE = 4;
    
    /* tlv header */
    private final int[] TYPE_F = {0, 2};
    private final int[] LENGTH_F = {2, 2};
    
    /* rtr ID */
    private final int[] RTRADDR_F = {0, 4};
    
    private final static int TE_TLV_ROUTER_ADDR = 1;
    private final static int TE_TLV_LINK = 2;
    
    private final static int TE_LINK_SUBTLV_LINK_TYPE = 1;
    private final static int TE_LINK_SUBTLV_LINK_ID = 2;
    private final static int TE_LINK_SUBTLV_LCLIF_IPADDR = 3; //Local Interface IP Address
    private final static int TE_LINK_SUBTLV_RMTIF_IPADDR = 4; //Remote Interface IP Address
    private final static int TE_LINK_SUBTLV_TE_METRIC = 5; //Traffic Engineering Metric
    private final static int TE_LINK_SUBTLV_MAX_BW = 6; //Maximum Bandwidth
    private final static int TE_LINK_SUBTLV_MAX_RSV_BW = 7; //Maximum Reservable Bandwidth
    private final static int TE_LINK_SUBTLV_UNRSV_BW = 8; //Unreserved Bandwidth
    private final static int TE_LINK_SUBTLV_RSC_CLSCLR = 9; //Administrative Group, a.k.a. Resource Class/Color
    private final static int TE_LINK_SUBTLV_LINK_LCRMT_ID = 11; //Link Local/Remote Identifiers
    private final static int TE_LINK_SUBTLV_LINK_IFSWCAP = 15;
    
    private final static int DRAGON_TLV_TYPE_BASE = 0x4000;
    private final static int TE_LINK_SUBTLV_RESV_SCHEDULE = DRAGON_TLV_TYPE_BASE + 1;
    private final static int TE_LINK_SUBTLV_LINK_IFADCAP = DRAGON_TLV_TYPE_BASE + 2;
    private final static int TE_LINK_SUBTLV_DOMAIN_ID= DRAGON_TLV_TYPE_BASE + 0x10;
    
    private Vector<TLV> stlvs = null;
    private long rtr_addr = -1;
    private TLV parent = null;
    
    private int dataOff;
    
    private long linkType;
    private long linkID;
    private long lclIFAddr;
    private long rmtIFAddr;
    private long teMetric;
    private float maxBW;
    private float maxRsvBW;
    private float[] unrsvBW = new float[8];
    private long rsrcClsClr;
    private long[] lclRmtID = new long[2];
    private IFSWCap ifSWCap = null;
    private Vector<Reservation> rsvSchedule = null;
    private long domainID;
    
    /** Creates a new instance of TLV */
    public TLV() {
        if(TERCEGlobals.tlvStrDescs == null) {
            TERCEGlobals.tlvStrDescs = new TERCEGlobals.strDesc();
            TERCEGlobals.tlvStrDescs.add("ROUTER_ADDR", TE_TLV_ROUTER_ADDR);
            TERCEGlobals.tlvStrDescs.add("LINK", TE_TLV_LINK);
        }
        if(TERCEGlobals.stlvStrDescs == null) {
            TERCEGlobals.stlvStrDescs = new TERCEGlobals.strDesc();
            TERCEGlobals.stlvStrDescs.add("LINK_TYPE", TE_LINK_SUBTLV_LINK_TYPE);
            TERCEGlobals.stlvStrDescs.add("LINK_ID", TE_LINK_SUBTLV_LINK_ID);
            TERCEGlobals.stlvStrDescs.add("LCLIF_IPADDR", TE_LINK_SUBTLV_LCLIF_IPADDR);
            TERCEGlobals.stlvStrDescs.add("RMTIF_IPADDR", TE_LINK_SUBTLV_RMTIF_IPADDR);
            TERCEGlobals.stlvStrDescs.add("TE_METRIC", TE_LINK_SUBTLV_TE_METRIC);
            TERCEGlobals.stlvStrDescs.add("MAX_BW", TE_LINK_SUBTLV_MAX_BW);
            TERCEGlobals.stlvStrDescs.add("MAX_RSV_BW", TE_LINK_SUBTLV_MAX_RSV_BW);
            TERCEGlobals.stlvStrDescs.add("UNRSV_BW", TE_LINK_SUBTLV_UNRSV_BW);
            TERCEGlobals.stlvStrDescs.add("RSC_CLSCLR", TE_LINK_SUBTLV_RSC_CLSCLR);
            TERCEGlobals.stlvStrDescs.add("LINK_LCRMT_ID", TE_LINK_SUBTLV_LINK_LCRMT_ID);
            TERCEGlobals.stlvStrDescs.add("LINK_IFSWCAP", TE_LINK_SUBTLV_LINK_IFSWCAP);
            TERCEGlobals.stlvStrDescs.add("RESV_SCHEDULE", TE_LINK_SUBTLV_RESV_SCHEDULE);
            TERCEGlobals.stlvStrDescs.add("LINK_IFADCAP", TE_LINK_SUBTLV_LINK_IFADCAP);
            TERCEGlobals.stlvStrDescs.add("DOMAIN_ID", TE_LINK_SUBTLV_DOMAIN_ID);
        }
    }
    
    /** Creates a new instance of TLV */
    public TLV(byte[] d, int o) throws TERCELSAException {
        this();
        dataOff = o;
        type = TERCEUtilities.nDatatoh(d, dataOff, TYPE_F);
        length = TERCEUtilities.nDatatoh(d, dataOff, LENGTH_F);
        if((type != TE_TLV_ROUTER_ADDR) && (type != TE_TLV_LINK))
            throw new TERCELSAException("Unknown TLV Type");
        
        dataOff += TLV_HDR_SIZE;
        
        /* get router ID TLV */
        if(type == TE_TLV_ROUTER_ADDR) {
            if(length != 4)
                throw new TERCELSAException("TE_TLV_ROUTER_ADDR incorrect length");
            
            rtr_addr = TERCEUtilities.nDatatoh(d, dataOff, RTRADDR_F);
            dataOff += 4;
            
            /* get TE Link TLV */
        } else if(type == TE_TLV_LINK) {
            if(length < 16)
                throw new TERCELSAException("TE_TLV_LINK too short");
            
            /* get the stlvs */
            stlvs = new Vector<TLV>();
        }
    }
    
    /** Creates a sub-TLV */
    public TLV(TLV t, byte[] d, int o) throws TERCELSAException {
        this();
        int x = 0;
        parent = t;
        dataOff = o;
        
        type = TERCEUtilities.nDatatoh(d, dataOff, TYPE_F);
        length = TERCEUtilities.nDatatoh(d, dataOff, LENGTH_F);
        dataOff += TLV_HDR_SIZE;
        if(type == TE_LINK_SUBTLV_LINK_TYPE) {
            x = parseLinkType(d);
        } else if(type == TE_LINK_SUBTLV_LINK_ID) {
            x = parseLinkID(d);
        } else if(type == TE_LINK_SUBTLV_LCLIF_IPADDR) {
            x = parseLclIFIPAddr(d);
        } else if(type == TE_LINK_SUBTLV_RMTIF_IPADDR) {
            x = parseRmtIFIPAddr(d);
        } else if(type == TE_LINK_SUBTLV_TE_METRIC) {
            x = parseTEMetric(d);
        } else if(type == TE_LINK_SUBTLV_MAX_BW) {
            x = parseMaxBW(d);
        } else if(type == TE_LINK_SUBTLV_MAX_RSV_BW) {
            x = parseMaxRsvBW(d);
        } else if(type == TE_LINK_SUBTLV_UNRSV_BW) {
            x = parseUnrsvBW(d);
        } else if(type == TE_LINK_SUBTLV_RSC_CLSCLR) {
            x = parseRscClsClr(d);
        } else if(type == TE_LINK_SUBTLV_LINK_LCRMT_ID) {
            x = parseLinkLclRmtID(d);
        } else if(type == TE_LINK_SUBTLV_LINK_IFSWCAP) {
            x = parseLinkIFSWCap(d);
        } else if(type == TE_LINK_SUBTLV_RESV_SCHEDULE) {
            x = parseResvSchedule(d);
        } else if(type == TE_LINK_SUBTLV_DOMAIN_ID) {
            x = parseDomainID(d);
        } else {
            throw new TERCELSAException("Unknown TE_LINK sub-TLV Type");
        }
        if(x<0) {
            throw new TERCELSAException("Sub TLV Parse Error");
        } else {
            dataOff += x;
        }
        
    }
    
    private int parseLinkType(byte[] d) {
        int[] f = {0, 1};
        linkType = TERCEUtilities.nDatatoh(d, dataOff, f);
        return 4;
    }
    
    private int parseLinkID(byte[] d) {
        int[] f = {0, 4};
        linkID = TERCEUtilities.nDatatoh(d, dataOff, f);
        return (f[0] + f[1]);
    }
    
    private int parseLclIFIPAddr(byte[] d) {
        int[] f = {0, 4};
        lclIFAddr = TERCEUtilities.nDatatoh(d, dataOff, f);
        return (f[0] + f[1]);
    }
    
    private int parseRmtIFIPAddr(byte[] d) {
        int[] f = {0, 4};
        rmtIFAddr = TERCEUtilities.nDatatoh(d, dataOff, f);
        return (f[0] + f[1]);
    }
    
    private int parseTEMetric(byte[] d) {
        int[] f = {0, 4};
        teMetric = TERCEUtilities.nDatatoh(d, dataOff, f);
        return (f[0] + f[1]);
    }
    
    private int parseMaxBW(byte[] d) {
        long tmp;
        int bits;
        int[] f = {0, 4};
        tmp = TERCEUtilities.nDatatoh(d, dataOff, f);
        bits = (int)(tmp & 0xffffffffL);
        maxBW = Float.intBitsToFloat(bits);
        return (f[0] + f[1]);
    }
    
    private int parseMaxRsvBW(byte[] d) {
        long tmp;
        int bits;
        int[] f = {0, 4};
        tmp = TERCEUtilities.nDatatoh(d, dataOff, f);
        bits = (int)(tmp & 0xffffffffL);
        maxRsvBW = Float.intBitsToFloat(bits);
        return (f[0] + f[1]);
    }
    
    private int parseUnrsvBW(byte[] d) {
        long tmp;
        int bits;
        int[] f = new int[2];
        f[1] = 4;
        for (int i = 0; i < 8; i++) {
            f[0] = i * 4;
            tmp = TERCEUtilities.nDatatoh(d, dataOff, f);
            bits = (int)(tmp & 0xffffffffL);
            unrsvBW[i] = Float.intBitsToFloat(bits);
        }
        return (f[0] + f[1]);
    }
    
    private int parseRscClsClr(byte[] d) {
        int[] f = {0, 4};
        rsrcClsClr = TERCEUtilities.nDatatoh(d, dataOff, f);
        return (f[0] + f[1]);
    }
    
    private int parseLinkLclRmtID(byte[] d) {
        int[] f = new int[2];
        f[1] = 4;
        for (int i = 0; i < 2; i++) {
            f[0] = i * 4;
            lclRmtID[i] = TERCEUtilities.nDatatoh(d, dataOff, f);
        }
        return (f[0] + f[1]);
    }
    
    private int parseLinkIFSWCap(byte[] d) {
        long enc = 0;
        int bits;
        long tmp;
        ifSWCap = new IFSWCap();
        int[] f = new int[2];
        
        f[0] = 0; f[1] = 1; //swcap
        ifSWCap.setSWCap(TERCEUtilities.nDatatoh(d, dataOff, f));
        f[0] = 1; f[1] = 1; //enc
        enc = TERCEUtilities.nDatatoh(d, dataOff, f);
        ifSWCap.setEnc(enc);
        
        
        f[0] = 4; f[1] = 4;
        for (int i = 0; i < 8; i++) {
            tmp = TERCEUtilities.nDatatoh(d, dataOff, f);
            bits = (int)(tmp & 0xffffffffL);
            ifSWCap.setMaxLspBW(Float.intBitsToFloat(bits), i);
            f[0] += 4;
        }
        
        /* parse the specific info union */
        if(ifSWCap.isPsc()) {
            f[1] = 4;
            tmp = TERCEUtilities.nDatatoh(d, dataOff, f);
            bits = (int)(tmp & 0xffffffffL);
            ifSWCap.setPscMinLspBW(Float.intBitsToFloat(bits));
            f[0] += 4; f[1] = 2;
            ifSWCap.setPscMtu(TERCEUtilities.nDatatoh(d, dataOff, f));
            return (f[0] + 4);
        } else if(ifSWCap.isTdm()) {
            f[1] = 4;
            tmp = TERCEUtilities.nDatatoh(d, dataOff, f);
            bits = (int)(tmp & 0xffffffffL);
            ifSWCap.setTdmMinLspBW(Float.intBitsToFloat(bits));
            f[0] += 4; f[1] = 1;
            ifSWCap.setTdmIndication(TERCEUtilities.nDatatoh(d, dataOff, f));
            return (f[0] + 4);
        } else if(ifSWCap.isVlan()) {
            f[1] = 2;
            ifSWCap.setVlanLength(TERCEUtilities.nDatatoh(d, dataOff, f));
            f[0] += 2; f[1] = 2;
            ifSWCap.setVlanVersion(TERCEUtilities.nDatatoh(d, dataOff, f));
            f[0] += 2;
            ifSWCap.setAvailVlans(d, dataOff, f);
            return (f[0] + (int)ifSWCap.getVlanLength() - 4);
        }
        return -1;
    }
    
    private int parseResvSchedule(byte[] d) {
        int bits;
        long tmp;
        Reservation rsv;
        rsvSchedule = new Vector<Reservation>();
        int[] f = new int[2];
        
        f[0] = 0; f[1] = 4;
        
        while(f[0] < length) {
            rsv = new Reservation();
            rsv.setDomainID(TERCEUtilities.nDatatoh(d, dataOff, f));
            f[0] += 4;
            
            rsv.setLspID(TERCEUtilities.nDatatoh(d, dataOff, f));
            f[0] += 4;
            
            rsv.setUptime(TERCEUtilities.nDatatoh(d, dataOff, f));
            f[0] += 4;
            
            rsv.setDuration(TERCEUtilities.nDatatoh(d, dataOff, f));
            f[0] += 4;
            
            tmp = TERCEUtilities.nDatatoh(d, dataOff, f);
            bits = (int)(tmp & 0xffffffffL);
            rsv.setBW(Float.intBitsToFloat(bits));
            f[0] += 4;
            
            rsv.setStatus(TERCEUtilities.nDatatoh(d, dataOff, f));
            f[0] += 4;
            rsvSchedule.add(rsv);
        }
        return (f[0]);
    }
    
    private int parseDomainID(byte[] d) {
        int[] f = {0, 4};
        domainID = TERCEUtilities.nDatatoh(d, dataOff, f);
        return (f[0] + f[1]);
    }
    
    public int getDataOff() {
        return dataOff;
    }
    
    public boolean isTELink() {
        return (type == TE_TLV_LINK);
    }
    
    public boolean isRtrAddr() {
        return (type == TE_TLV_ROUTER_ADDR);
    }
    
    public boolean isVlan() throws TERCELSAException {
        IFSWCap sw = getSubTLV(TE_LINK_SUBTLV_LINK_IFSWCAP).ifSWCap;
        return sw.isVlan();
    }
    
    public boolean isTdm() throws TERCELSAException {
        IFSWCap sw = getSubTLV(TE_LINK_SUBTLV_LINK_IFSWCAP).ifSWCap;
        return sw.isTdm();
    }
    
    public boolean isPsc() throws TERCELSAException {
        IFSWCap sw = getSubTLV(TE_LINK_SUBTLV_LINK_IFSWCAP).ifSWCap;
        return sw.isPsc();
    }
    
    public void addSTLV(TLV stlv) {
        if(stlvs != null)
            stlvs.add(stlv);
    }
    
    /**
     * This function, regardles where it is caled from always returns the type of the TLV
     * (never of the sub-TLV)
     * @return type of the TLV (if called on a sub-TLV, it returns the type of the enclosing TLV)
     */
    public long getType() {
        if(parent != null)
            return parent.type;
        else
            return type;
    }
    
    /**
     * This function, regardles where it is caled from always returns the length of the TLV
     * (never of the sub-TLV)
     * @return length of the TLV (if called on a sub-TLV, it returns the length of the enclosing TLV)
     */
    public long getLength() {
        if(parent != null)
            return parent.length;
        else
            return length;
    }
    
    public long getDomainID() throws TERCELSAException {
        return getSubTLV(TE_LINK_SUBTLV_DOMAIN_ID).domainID;
    }
    
    public long getRtrAddr() {
        if(parent != null)
            return parent.rtr_addr;
        else
            return rtr_addr;
    }
    
    public long getLinkID() throws TERCELSAException {
        return getSubTLV(TE_LINK_SUBTLV_LINK_ID).linkID;
    }
    
    public float getMaxBW() throws TERCELSAException {
        return getSubTLV(TE_LINK_SUBTLV_MAX_BW).maxBW;
    }
    
    public float getMaxRsvBW() throws TERCELSAException {
        return getSubTLV(TE_LINK_SUBTLV_MAX_RSV_BW).maxRsvBW;
    }
    
    public float getUnrsvBW() throws TERCELSAException {
        float[] fa = getSubTLV(TE_LINK_SUBTLV_UNRSV_BW).unrsvBW;
        float f = fa[0];
        for (int i = 1; i < fa.length; i++) {
            if(f > fa[i]) {
                f = fa[i];
            }
        }
        return f;
    }
    
    public long getMetric() throws TERCELSAException {
        return getSubTLV(TE_LINK_SUBTLV_TE_METRIC).teMetric;
    }
    
    public long getSCType() throws TERCELSAException {
        IFSWCap sw = getSubTLV(TE_LINK_SUBTLV_LINK_IFSWCAP).ifSWCap;
        return sw.getSWCap();
    }
    
    public long getSCEnc() throws TERCELSAException {
        IFSWCap sw = getSubTLV(TE_LINK_SUBTLV_LINK_IFSWCAP).ifSWCap;
        return sw.getEnc();
    }
    
    public Vector<int[]> getAvailVlanRanges() throws TERCELSAException {
        IFSWCap sw = getSubTLV(TE_LINK_SUBTLV_LINK_IFSWCAP).ifSWCap;
        return sw.getAvaiRanges();
    }
    
    private TLV getSubTLV(int t) throws TERCELSAException {
        if(type == t) {
            return this;
        } else if(parent != null) {
            return parent.getSubTLV(t);
        } else if(stlvs != null) {
            for (int i = 0; i < stlvs.size(); i++) {
                if(stlvs.get(i).getType() == t)
                    return stlvs.get(i);
            }
            throw new TERCELSAException(TERCEGlobals.stlvStrDescs.getStr(t) + ": missing tlv");
        } else {
            throw new TERCELSAException(TERCEGlobals.stlvStrDescs.getStr(t) + ": unknown tlv type");
        }
    }
    
    public String toString() {
        String s = "";
        if(parent != null) {
            s += "    SUB_TLV";
            s += "(" + TERCEGlobals.stlvStrDescs.getStr(type) + ")";
            if(type == TE_LINK_SUBTLV_LINK_TYPE) {
                s += " link type: " + linkType;
            } else if(type == TE_LINK_SUBTLV_LINK_ID) {
                s += " link ID: 0x" + Long.toHexString(linkID);
            } else if(type == TE_LINK_SUBTLV_LCLIF_IPADDR) {
                s += " lclif IP: 0x" + Long.toHexString(lclIFAddr);
            } else if(type == TE_LINK_SUBTLV_RMTIF_IPADDR) {
                s += " rmtif IP: 0x" + Long.toHexString(rmtIFAddr);
            } else if(type == TE_LINK_SUBTLV_TE_METRIC) {
                s += " metric: " + teMetric;
            } else if(type == TE_LINK_SUBTLV_MAX_BW) {
                s += " max BW: " + maxBW;
            } else if(type == TE_LINK_SUBTLV_MAX_RSV_BW) {
                s += " max rsv. BW: " + maxRsvBW;
            } else if(type == TE_LINK_SUBTLV_UNRSV_BW) {
                s += " unrsv. BW: (";
                for (int i = 0; i < 8; i++) {
                    s += unrsvBW[i];
                    if(i == 7)
                        s += ")";
                    else
                        s += ", ";
                }
            } else if(type == TE_LINK_SUBTLV_RSC_CLSCLR) {
                s += " class/color: " + rsrcClsClr;
            } else if(type == TE_LINK_SUBTLV_LINK_LCRMT_ID) {
                s += " lcl/rmt ID: (";
                for (int i = 0; i < 2; i++) {
                    s += lclRmtID[i];
                    if(i == 1)
                        s += ")";
                    else
                        s += ", ";
                }
                
            } else if(type == TE_LINK_SUBTLV_LINK_IFSWCAP) {
                s += " ifswcap: " + TERCEGlobals.swcapStrDescs.getStr(ifSWCap.getSWCap()) + ", ";
                s += TERCEGlobals.encStrDescs.getStr(ifSWCap.getEnc());
                s += " vlans: avail(";
                for (int i = 0; i < ifSWCap.getAvaiRanges().size(); i++) {
                    s += ifSWCap.getAvaiRanges().get(i)[0] + " - " + ifSWCap.getAvaiRanges().get(i)[1];
                    if(i < (ifSWCap.getAvaiRanges().size()-1)) s += ", ";
                }
                s += ")";
                s += " aloc(";
                for (int i = 0; i < ifSWCap.getAlocRanges().size(); i++) {
                    s += ifSWCap.getAlocRanges().get(i)[0] + " - " + ifSWCap.getAlocRanges().get(i)[1];
                    if(i < (ifSWCap.getAlocRanges().size()-1)) s += ", ";
                }
                s += ")";
            } else if(type == TE_LINK_SUBTLV_RESV_SCHEDULE) {
                s += " resv. schedule: (hidden)";
            } else if(type == TE_LINK_SUBTLV_DOMAIN_ID) {
                s += " domain ID: 0x" + Long.toHexString(domainID);
            }
            
            s += "\n";
        } else {
            s += "  TLV";
            s += "(" + TERCEGlobals.tlvStrDescs.getStr(type) + ")";
            if(type == TE_TLV_ROUTER_ADDR)
                s += " rtr_addr: 0x" + Long.toHexString(rtr_addr & 0xffffffffL);
            if(stlvs != null) {
                s += "\n";
                for (int i = 0; i < stlvs.size(); i++) {
                    s += stlvs.get(i).toString();
                }
            }
            s += "\n";
        }
        return s;
    }
}

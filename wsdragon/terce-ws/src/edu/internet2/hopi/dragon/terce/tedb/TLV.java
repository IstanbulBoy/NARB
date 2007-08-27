/*
 * TLV.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce.tedb;

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
    private float[] maxUnrsvBW = new float[8];
    private long rsrcClsClr;
    private long[] lclRmtID = new long[2];
    private IFSWCap ifSWCap = null;
    private Vector<Reservation> rsvSchedule = null;
    private long domainID;
    
    /** Creates a new instance of TLV */
    public TLV(byte[] d, int o) throws TERCELSAException {
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
    
    /** Creates a new instance of TLV */
    public TLV(TLV t, byte[] d, int o) throws TERCELSAException {
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
            maxUnrsvBW[i] = Float.intBitsToFloat(bits);
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
            ifSWCap.copyBitmask(d, dataOff, f);
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
    
    void addSTLV(TLV stlv) {
        if(stlvs != null)
            stlvs.add(stlv);
    }
    
    long getLength() {
        return length;
    }
}

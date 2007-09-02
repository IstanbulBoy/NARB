/*
 * IFSWCap.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce.tedb;

import edu.internet2.hopi.dragon.terce.TERCEGlobals;
import edu.internet2.hopi.dragon.terce.TERCELSAException;
import java.util.Vector;
import java.util.zip.DataFormatException;
import java.util.zip.Inflater;

/**
 *
 * @author jflidr
 */
public class IFSWCap {
    
    private final static int LINK_IFSWCAP_SUBTLV_SWCAP_PSC1 = 1;
    private final static int LINK_IFSWCAP_SUBTLV_SWCAP_PSC2 = 2;
    private final static int LINK_IFSWCAP_SUBTLV_SWCAP_PSC3 = 3;
    private final static int LINK_IFSWCAP_SUBTLV_SWCAP_PSC4 = 4;
    private final static int LINK_IFSWCAP_SUBTLV_SWCAP_L2SC = 51;
    private final static int LINK_IFSWCAP_SUBTLV_SWCAP_TDM = 100;
    private final static int LINK_IFSWCAP_SUBTLV_SWCAP_LSC = 150;
    private final static int LINK_IFSWCAP_SUBTLV_SWCAP_FSC = 200;
    
    private final static int LINK_IFSWCAP_SUBTLV_ENC_PKT = 1;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_ETH = 2;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_PDH = 3;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_RESV1 = 4;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_SONETSDH = 5;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_RESV2 = 6;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_DIGIWRAP = 7;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_LAMBDA = 8;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_FIBER = 9;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_RESV3 = 10;
    private final static int LINK_IFSWCAP_SUBTLV_ENC_FIBRCHNL = 11;
    
    /* vlan version */
    private static final int MAX_VLAN_NUM = 4096;
    private static final int IFSWCAP_SPECIFIC_VLAN_BASIC = 0x0002;
    private static final int IFSWCAP_SPECIFIC_VLAN_ALLOC = 0x0004;
    private static final int IFSWCAP_SPECIFIC_VLAN_COMPRESS_Z = 0x8000;
    
    private long swcap;
    private long enc;
    private float[] maxLspBW = new float[8];
    
    /* psc */
    private float pscMinLspBW;
    private long pscMtu;
    
    /* tdm */
    private float tdmMinLspBW;
    private long tdmIndication;
    
    /* vlan */
    private long vlanLength;
    private long vlanVersion;
    private byte[] vlanBitmask = null;
    private Vector<int[]> vlanAvailRanges = null;
    private Vector<int[]> vlanAlocRanges = null;
    
    /** Creates a new instance of IFSWCap */
    public IFSWCap() {
        if(TERCEGlobals.swcapStrDescs == null) {
            TERCEGlobals.swcapStrDescs = new TERCEGlobals.strDesc();
            TERCEGlobals.swcapStrDescs.add("psc-1", LINK_IFSWCAP_SUBTLV_SWCAP_PSC1);
            TERCEGlobals.swcapStrDescs.add("psc-2", LINK_IFSWCAP_SUBTLV_SWCAP_PSC2);
            TERCEGlobals.swcapStrDescs.add("psc-3", LINK_IFSWCAP_SUBTLV_SWCAP_PSC3);
            TERCEGlobals.swcapStrDescs.add("psc-4", LINK_IFSWCAP_SUBTLV_SWCAP_PSC4);
            TERCEGlobals.swcapStrDescs.add("l2sc", LINK_IFSWCAP_SUBTLV_SWCAP_L2SC);
            TERCEGlobals.swcapStrDescs.add("tdm", LINK_IFSWCAP_SUBTLV_SWCAP_TDM);
            TERCEGlobals.swcapStrDescs.add("lsc", LINK_IFSWCAP_SUBTLV_SWCAP_LSC);
            TERCEGlobals.swcapStrDescs.add("fsc", LINK_IFSWCAP_SUBTLV_SWCAP_FSC);
        }
        if(TERCEGlobals.encStrDescs == null) {
            TERCEGlobals.encStrDescs = new TERCEGlobals.strDesc();
            TERCEGlobals.encStrDescs.add("packet", LINK_IFSWCAP_SUBTLV_ENC_PKT);
            TERCEGlobals.encStrDescs.add("ethernet", LINK_IFSWCAP_SUBTLV_ENC_ETH);
            TERCEGlobals.encStrDescs.add("pdh", LINK_IFSWCAP_SUBTLV_ENC_PDH);
            TERCEGlobals.encStrDescs.add("resv1", LINK_IFSWCAP_SUBTLV_ENC_RESV1);
            TERCEGlobals.encStrDescs.add("sdh/sonet", LINK_IFSWCAP_SUBTLV_ENC_SONETSDH);
            TERCEGlobals.encStrDescs.add("resv2", LINK_IFSWCAP_SUBTLV_ENC_RESV2);
            TERCEGlobals.encStrDescs.add("digital wrapper", LINK_IFSWCAP_SUBTLV_ENC_DIGIWRAP);
            TERCEGlobals.encStrDescs.add("lambda", LINK_IFSWCAP_SUBTLV_ENC_LAMBDA);
            TERCEGlobals.encStrDescs.add("fiber", LINK_IFSWCAP_SUBTLV_ENC_FIBER);
            TERCEGlobals.encStrDescs.add("resv3", LINK_IFSWCAP_SUBTLV_ENC_RESV3);
            TERCEGlobals.encStrDescs.add("fiberchannel", LINK_IFSWCAP_SUBTLV_ENC_FIBRCHNL);
        }
    }
    
    public void setSWCap(long l) {
        swcap = l;
    }
    
    public long getSWCap() {
        return swcap;
    }
    
    public void setEnc(long l) {
        enc = l;
    }
    
    public long getEnc() {
        return enc;
    }
    
    public void setMaxLspBW(float f, int i) {
        maxLspBW[i] = f;
    }
    
    public boolean isPsc() {
        return ((enc == LINK_IFSWCAP_SUBTLV_ENC_PKT) &&
                ((swcap == LINK_IFSWCAP_SUBTLV_SWCAP_PSC4) ||
                (swcap == LINK_IFSWCAP_SUBTLV_SWCAP_PSC3) ||
                (swcap == LINK_IFSWCAP_SUBTLV_SWCAP_PSC2) ||
                (swcap == LINK_IFSWCAP_SUBTLV_SWCAP_PSC1)));
    }
    
    public boolean isTdm() {
        return ((swcap == LINK_IFSWCAP_SUBTLV_SWCAP_TDM) &&
                (enc == LINK_IFSWCAP_SUBTLV_ENC_SONETSDH));
    }
    
    public boolean isVlan() {
        return ((swcap == LINK_IFSWCAP_SUBTLV_SWCAP_L2SC) &&
                (enc == LINK_IFSWCAP_SUBTLV_ENC_ETH));
    }
    
    public void setPscMinLspBW(float f) {
        pscMinLspBW = f;
    }
    
    public void setPscMtu(long l) {
        pscMtu = l;
    }
    
    public void setTdmMinLspBW(float f) {
        tdmMinLspBW = f;
    }
    
    public void setTdmIndication(long l) {
        tdmIndication = l;
    }
    
    public void setVlanLength(long l) {
        vlanLength = l;
    }
    
    public long getVlanLength() {
        return vlanLength;
    }
    
    public void setVlanVersion(long l) {
        vlanVersion = l;
    }
    
    public void setAvailVlans(byte[] d, int doff, int[] f) {
        boolean inRange = false;
        int rangeStart = 0;
        Inflater inf;
        if(isCompressed()) {
            vlanBitmask = new byte[MAX_VLAN_NUM/4];
            inf = new Inflater();
            inf.setInput(d, doff + f[0], (int)vlanLength);
            try {
                inf.inflate(vlanBitmask);
            } catch (DataFormatException ex) {
                ex.printStackTrace();
            }
        } else {
            if(vlanLength > 4) {
                vlanBitmask = new byte[(int)vlanLength-4];
            }
            for (int i = 0; i < vlanBitmask.length; i++) {
                vlanBitmask[i] = d[doff + f[0] + i];
            }
        }
        vlanAvailRanges = new Vector<int[]>();
        vlanAlocRanges = new Vector<int[]>();
        for (int i = 0; i < vlanBitmask.length*4; i++) {
            if((vlanBitmask[i/8] & (0x80>>>(i%8))) != 0) {
                if(!inRange) {
                    rangeStart = i;
                    inRange = true;
                }
            } else {
                if(inRange) {
                    int[] r = {rangeStart+1, i};
                    inRange = false;
                    vlanAvailRanges.add(r);
                }
            }
        }
        inRange = false;
        for (int i = 0; i < vlanBitmask.length*4; i++) {
            if((vlanBitmask[vlanBitmask.length/2 + i/8] & (0x80>>>(i%8))) != 0) {
                if(!inRange) {
                    rangeStart = i;
                    inRange = true;
                }
            } else {
                if(inRange) {
                    int[] r = {rangeStart+1, i};
                    inRange = false;
                    vlanAlocRanges.add(r);
                }
            }
        }
    }
    
    private boolean isCompressed() {
        return ((vlanVersion & IFSWCAP_SPECIFIC_VLAN_COMPRESS_Z) != 0);
    }
    
    public Vector<int[]> getAvaiRanges() {
        return vlanAvailRanges;
    }
    
    public Vector<int[]> getAlocRanges() {
        return vlanAlocRanges;
    }
}

/*
 * IFSWCap.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce.tedb;

import edu.internet2.hopi.dragon.terce.TERCELSAException;

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
    
    /** Creates a new instance of IFSWCap */
    public IFSWCap() {
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
        if(l > 4) {
            vlanBitmask = new byte[(int)l-4];
        }
    }
    
    
    public long getVlanLength() {
        return vlanLength;
    }
    
    public void setVlanVersion(long l) {
        vlanVersion = l;
    }

    public void copyBitmask(byte[] d, int doff, int[] f) {
        for (int i = 0; i < vlanBitmask.length; i++) {
            vlanBitmask[i] = d[doff + f[0] + i];
        }
    }
}

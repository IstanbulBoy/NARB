/*
 * LSA.java
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
public class LSA {
    
    /* OSPF LSA Types */
    private final static int OSPF_ROUTER_LSA = 1;
    private final static int OSPF_NETWORK_LSA = 2;
    private final static int OSPF_SUMMARY_LSA = 3;
    private final static int OSPF_ASBR_SUMMARY_LSA = 4;
    private final static int OSPF_AS_EXTERNAL_LSA = 5;
    private final static int OSPF_AS_NSSA_LSA = 7;
    private final static int OSPF_OPAQUE_LINK_LSA = 9;
    private final static int OSPF_OPAQUE_AREA_LSA = 10;
    private final static int OSPF_OPAQUE_AS_LSA = 11;
    
    /* TE LSA Types */
    private final static int NOT_TE_LSA = 0;
    private final static int ROUTER_ID_TE_LSA = 1;
    private final static int LINK_TE_LSA = 2;
    private final static int LINK_LOCAL_TE_LSA = 3;
    
    /* Opaque LSA types */
    private final static int OPAQUE_TYPE_TE_LINKLOCAL_LSA = 1;
    private final static int OPAQUE_TYPE_TE_AREA_LSA = 1;
    private final static int OPAQUE_TYPE_SYCAMORE_OPTICAL_TOPOLOGY_DESC = 2;
    private final static int OPAQUE_TYPE_GRACE_LSA = 3;
    
    /* LSA header */
    private long age;
    private long options;
    private long type;
    private long opq_type;
    private long opq_id;
    private long adv_router;
    private long seqn;
    private long chksum;
    private long length;
    
    private final int[] AGE_FIELD = {0, 2};
    private final int[] OPTS_FIELD = {2, 1};
    private final int[] TYPE_FIELD = {3, 1};
    private final int[] ID_FIELD = {4, 4};
    private final int[] RTR_FIELD = {8, 4};
    private final int[] SEQN_FIELD = {12, 4};
    private final int[] CHKSUM_FIELD = {16, 2};
    private final int[] LENGTH_FIELD = {18, 2};
    
    public final static int HDR_SIZE = 20;
    
    private boolean isValid = false;
    
    private Vector<TLV> tlvs = new Vector<TLV>();
    
    /** Creates a new instance of LSA */
    public LSA(byte[] d) throws TERCELSAException {
        int to; //tlv offset
        long id;
        
        /* setup the lsa header */
        age = TERCEUtilities.hDatatoh(d, AGE_FIELD);
        options = TERCEUtilities.hDatatoh(d, OPTS_FIELD);
        type = TERCEUtilities.hDatatoh(d, TYPE_FIELD);
        id = TERCEUtilities.nDatatoh(d, ID_FIELD);
        opq_type = (id>>>24) & 0xffL;
        opq_id = id & 0xffffffL;
        adv_router = TERCEUtilities.hDatatoh(d, RTR_FIELD);
        seqn = TERCEUtilities.hDatatoh(d, SEQN_FIELD);
        chksum = TERCEUtilities.hDatatoh(d, CHKSUM_FIELD);
        length = TERCEUtilities.nDatatoh(d, LENGTH_FIELD);
        
        if(d.length != length)
            throw new TERCELSAException("incorrect LSA size");
        /* get tlvs */
        getParseTLVs(d, HDR_SIZE);
    }
    
    
    private void getParseTLVs(byte[] d, int o) throws TERCELSAException {
        int tlvDataStart;
        TLV stlv;
        while(o < length) {
            TLV tlv = new TLV(d, o);
            o = tlv.getDataOff();
            if(tlv.isTELink()) {
                tlvDataStart = o;
                while((o - tlvDataStart) < tlv.getLength()) {
                    stlv = new TLV(tlv, d, o);
                    tlv.addSTLV(stlv);
                    o = stlv.getDataOff();
                }
            }
            tlvs.add(tlv);
        }
    }
    
    public String toString() {
        String s = "";
        
        s += "LSA ";
        s += "age: 0x" + Long.toHexString(age);
        s += ", options: 0x" + Long.toHexString(options);
        s += ", type: 0x" + Long.toHexString(type);
        s += ", opaque type: 0x" + Long.toHexString(opq_type);
        s += ", opaque id: 0x" + Long.toHexString(opq_id);
        s += ", adv_router: 0x" + Long.toHexString(adv_router);
        s += ", seqn: 0x" + Long.toHexString(seqn);
        s += ", chksum: 0x" + Long.toHexString(chksum);
        s += ", length: 0x" + Long.toHexString(length);
        s += "\n";
        return s;
    }
}

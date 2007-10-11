package edu.internet2.hopi.dragon.terce.api;

import edu.internet2.hopi.dragon.terce.TERCENetworkDataException;
import edu.internet2.hopi.dragon.terce.TERCEUtilities;


public class TERCEMessageHeader extends Object {
    /* header description */
    private final static int[] TYPE_FIELD = { 0, 2 };
    private final static int[] LENGTH_FIELD = { 2, 2 };
    private final static int[] UCID_FIELD = { 4, 4 };
    private final static int[] SEQN_FIELD = { 8, 4 };
    private final static int[] CHKSUM_FIELD = { 12, 4 };
    private final static int[] OPTS_FIELD = { 16, 4 };
    private final static int[] TAG_FIELD = { 20, 4 };

    /* Path computation optsByte */
    public final static int OPT_STRICT = 0x00010000;
    public final static int OPT_PREFERRED = 0x00020000;
    public final static int OPT_MRN = 0x00040000;
    public final static int OPT_BIDIRECTIONAL = 0x00100000;
    public final static int OPT_E2E_VTAG = 0x00200000;
    public final static int OPT_VIA_MOVAZ = 0x00400000;
    public final static int OPT_VTAG_MASK = 0x00800000;
    public final static int OPT_REQ_ALL_VTAGS = 0x0002;
    public final static int OPT_REQ_ALL_WAVES = 0x0004;

    /* Layer exclusions optsByte */
    public final static int OPT_EXCLUDE_L1 = 0x0010;
    public final static int OPT_EXCLUDE_TDM = 0x0020;
    public final static int OPT_EXCLUDE_L2 = 0x0040;
    public final static int OPT_EXCLUDE_L3 = 0x0080;

    /* Tag values */
    public final static int VTAG_ANY = 0xffff;
    public final static int VTAG_NONE = 0x0000;

    /* Header size */
    public final static int SIZE = 24;

    /* Message types */
    public final static int TYPE_LSPQ = 0x01;
    public final static int TYPE_APP_REQUEST = 0x02;
    public final static int TYPE_APP_CONFIRM = 0x03;
    public final static int TYPE_APP_REMOVE = 0x04;
    public final static int TYPE_REPLY_ERO = 0x21;
    public final static int TYPE_REPLY_ERROR = 0x22;
    public final static int TYPE_REPLY_REMOVE_CONFIRM = 0x23;
    public final static int TYPE_PEER_REQUEST = 0x41;
    public final static int RCE_MSG_TERCE_TOPO_SYNC = 0x11;
    public final static int RCE_MSG_TERCE_TOPO_ASYNC = 0x12;

    /* actions */
    public final static int ACT_QUERY = 0x01;
    public final static int ACT_INSERT = 0x02;
    public final static int ACT_DELETE = 0x03;
    public final static int ACT_UPDATE = 0x04;
    public final static int ACT_ACK = 0x05;
    public final static int ACT_ACKDATA = 0x06;
    public final static int ACT_ERROR = 0x07;
    public final static int ACT_INIT = 0x0A;
    public final static int ACT_ALIVE = 0x0B;
    private byte[] headerBytes;
    private boolean isValid = false;
    private long type;
    private long bodyLength;
    private long ucid;
    private long seqn;
    private long chksum;
    private long opts;
    private long tag;

    public TERCEMessageHeader() {
        headerBytes = new byte[SIZE];

        for (int i = 0; i < headerBytes.length; i++) {
            headerBytes[i] = 0;
        }

        type = 0;
        bodyLength = 0;
        ucid = (((int) 't' & 0xff) + ((int) 'e' & 0xff)) << (8 +
            ((int) 'r' & 0xff)) << (16 + ((int) 'c' & 0xff)) << 24;

        TERCEUtilities.htonData(headerBytes, ucid, UCID_FIELD);

        seqn = 0;
        chksum = 0;
        opts = 0;
        tag = 0;
    }

    public TERCEMessageHeader(long t, long l) {
        this();
        type = t;
        bodyLength = l;

        TERCEUtilities.htonData(headerBytes, type, TYPE_FIELD);
        TERCEUtilities.htonData(headerBytes, bodyLength, LENGTH_FIELD);
    }

    public TERCEMessageHeader(long t, long l, long n) {
        this(t, l);
        seqn = n;

        TERCEUtilities.htonData(headerBytes, seqn, SEQN_FIELD);
    }

    /**
     * Constructor that generate an object of this class from a raw TERCE API data (including body of the message).
     * Useful when a TERCE API message is received and the values of the header need to be extracted.
     *
     * @param data a byte represenation of the entire TERCE API data (including the body of the message)
     */
    public TERCEMessageHeader(byte[] data) {
        headerBytes = data.clone();

        type = TERCEUtilities.nDatatoh(headerBytes, TYPE_FIELD);
        bodyLength = TERCEUtilities.nDatatoh(headerBytes, LENGTH_FIELD);
        ucid = TERCEUtilities.nDatatoh(headerBytes, UCID_FIELD);
        seqn = TERCEUtilities.nDatatoh(headerBytes, SEQN_FIELD);
        chksum = TERCEUtilities.hDatatoh(headerBytes, CHKSUM_FIELD);
        opts = TERCEUtilities.nDatatoh(headerBytes, OPTS_FIELD);
        tag = TERCEUtilities.nDatatoh(headerBytes, TAG_FIELD);

        //the only reasonable way to verify that a buffer is a valid message header is to verify the check sum
        isValid = (calculateHdrChkSum() == chksum);
    }

    public byte[] toBytes() {
        return headerBytes;
    }

    /**
     * Returns the lengthByte of the message body when in raw byte form
     *
     *
     * @return the lengthByte of the message body when in raw byte form
     */
    public long getLength() {
        return bodyLength;
    }

    /**
     * Returns the type of TERCE message. You may compare to TYPE_* static constants for convenience.
     * @return the type of TERCE message
     */
    public long getMessageType() {
        return type;
    }

    public void setOption(int o) {
        opts |= o;
        TERCEUtilities.htonData(headerBytes, opts, OPTS_FIELD);
    }

    public long getOptions() {
        return opts;
    }

    public long getSequenceNumber() {
        return seqn;
    }

    public void setTag(int t) {
        tag = t;
        TERCEUtilities.htonData(headerBytes, tag, TAG_FIELD);
    }

    public long getTag() {
        return tag;
    }

    public long getUcid() {
        return ucid;
    }

    public long getChksum() {
        return chksum;
    }

    public boolean isValid() {
        return isValid;
    }

    private long calculateHdrChkSum() {
        long c = 0;

        for (int i = 0; i < 3; i++) {
            c += TERCEUtilities.hDatatoh(headerBytes, (i * 4), 4);
        }

        return (c & 0xffffffffL);
    }

    public void insertChkSum() {
        chksum = calculateHdrChkSum();

        TERCEUtilities.htohData(headerBytes, chksum, CHKSUM_FIELD);
    }

    public boolean isSyncInit() {
        return (type == ((RCE_MSG_TERCE_TOPO_SYNC << 8) | ACT_INIT));
    }

    public boolean isSyncQuery() {
        return (type == ((RCE_MSG_TERCE_TOPO_SYNC << 8) | ACT_QUERY));
    }

    public boolean isSyncInsert() {
        return (type == ((RCE_MSG_TERCE_TOPO_SYNC << 8) | ACT_INSERT));
    }
}

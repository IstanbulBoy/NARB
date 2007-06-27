package edu.internet2.hopi.dragon.dc.api;

/**
 * Represents the standard header of a DC API message. 
 * This object can be generated by specifying each individual 
 * parameter or from the raw bytes of a DC API packet .
 * The former is useful when generating a DC API request 
 * and the latter is useful when a DC API request is received.
 * In addition, this class makes it convenient to go from an object representation of 
 * a DC API header to the raw bytes understood by a DC API server.
 * It's also useful to note that this class contains many static values important for setting 
 * options that control the meaning of a DC request.
 * 
 * @author Andrew Lake (alake@internet2.edu)
 *
 */
public class DCMessageHeader {
	private byte[] messageType;
	private byte[] length;
	private byte[] ucid;
	private byte[] sequenceNumber;
	private byte[] checksum;
	private byte[] options;
	private byte[] tag;
	
	/* Path computation options */
	public final static int OPT_STRICT = 0x00010000;
	public final static int OPT_PREFERRED = 0x00020000;
	public final static int OPT_MRN = 0x00040000;
	public final static int OPT_BIDIRECTIONAL= 0x00100000;
	public final static int OPT_E2E_VTAG = 0x00200000;
	public final static int OPT_VIA_MOVAZ = 0x00400000;
	public final static int OPT_VTAG_MASK = 0x00800000;
	public final static int OPT_REQ_ALL_VTAGS = 0x0002;
	public final static int OPT_REQ_ALL_WAVES = 0x0004;
	
	/* Layer exclusions options */
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
	public final static int TYPE_PEER_REQUEST= 0x41;
	
	/**
	 * Generate an object representation or a DC API header by setting specific values. Useful when 
	 * creating a new DC API request.
	 * 
	 * @param type the type of DC request to make. Refer to the TYPE_* constants for acceptable values.
	 * @param bodyLength the length of the body of this API message
	 * @param opt an array of options to set in this header. Refer to the OPT_* constants for acceptable values.
	 * @param vtag The VTAG returned or used by this request. It may be a number from 1-4096 
	 * or use the static VTAG_* constants for special cases (i.e. No VLAN or or application chooses VLAN)
	 */
	public DCMessageHeader(int type, int bodyLength, int[] opt, int vtag){
		/* Initialize values */
		initialize();
		
		/* Set message type */
		messageType[0] = (byte) ((type & 65280) >> 2);
		messageType[1] = (byte) (type & 255);
		
		/* Set length */
		length[0] = (byte) ((bodyLength & 65280) >> 2);
		length[1] = (byte) (bodyLength & 255);
		
		/* Set ucid */
		ucid[0] = 0;
		ucid[1] = 0;
		ucid[2] = 0;
		ucid[3] = 0;
		
		/* Set sequenceNumber */
		sequenceNumber[0] = 0;
		sequenceNumber[1] = 0;
		sequenceNumber[2] = 0;
		sequenceNumber[3] = 0;
		
		/* Calculate checksum */
		int chk = (type << 16) + bodyLength;
		checksum[0] = (byte) (chk >> 24);
		checksum[1] = (byte) ((chk & 16711680) >> 16);
		checksum[2] = (byte) ((chk & 65280) >> 8);
		checksum[3] = (byte) (chk & 255);
		
		/* Set options */
		if(opt != null){
			for(int i = 0; i < opt.length; i++){
				options[0] |= (byte) (opt[i] >> 24);
				options[1] |= (byte) ((opt[i] & 16711680) >> 16);
				options[2] |= (byte) ((opt[i] & 65280) >> 8);
				options[3] |= (byte) (opt[i] & 255);
			}
		}
		
		/* Set vtag */
		tag[0] = (byte) (vtag >> 24);
		tag[1] = (byte) ((vtag & 16711680) >> 16);
		tag[2] = (byte) ((vtag & 65280) >> 8);
		tag[3] = (byte) (vtag & 255);
		
	}
	
	/**
	 * Constructor that generate an object of this class from a raw DC API packet (including body of the message). 
	 * Useful when a DC API message is received and the values of the header need to be extracted.
	 * @param packet a byte represenation of the entire DC API packet (including the body of the message)
	 */
	public DCMessageHeader(byte[] packet){
		/* Initialize values */
		initialize();
		
		/* Extract header fields from packet */
		for(int i = 0; i < SIZE; i++){
			if(i >= 0 && i <= 1){
				messageType[i] = packet[i];
			}else if(i >= 2 && i <= 3){
				length[i-2] = packet[i];
			}else if(i >= 4 && i <= 7){
				ucid[i-4] = packet[i];
			}else if(i >= 8 && i <= 11){
				sequenceNumber[i-8] = packet[i];
			}else if(i >= 12 && i <= 15){
				checksum[i-12] = packet[i];
				//TODO: verify checksum
			}else if(i >= 16 && i <= 19){
				options[i-16] = packet[i];
			}else if(i >= 20 && i <= 23){
				tag[i-20] = packet[i];
			}
		}
	}
	
	/**
	 * Generates a byte representation of a DC API message header which can be used to create a valid DC API packet
	 * @return a raw byte representation of the DC API header that can be inserted into a DC API packet
	 */
	public byte[] toBytes(){
		byte[] header = new byte[SIZE];
		
		for(int i = 0; i < header.length; i++){
			if(i >= 0 && i <= 1){
				header[i] = messageType[i];
			}else if(i >= 2 && i <= 3){
				header[i] = length[i-2];
			}else if(i >= 4 && i <= 7){
				header[i] = ucid[i-4];
			}else if(i >= 8 && i <= 11){
				header[i] = sequenceNumber[i-8];
			}else if(i >= 12 && i <= 15){
				header[i] = checksum[i-12];
			}else if(i >= 16 && i <= 19){
				header[i] = options[i-16];
			}else if(i >= 20 && i <= 23){
				header[i] = tag[i-20];
			}
		}
		
		return header;
	}

	/**
	 * Returns the length of the message body when in raw byte form
	 * @return the length of the message body when in raw byte form
	 */
	public int getLength() {
		int l = (length[0] << 8);
		l += length[1];
		
		return l;
	}

	/**
	 * Sets the length of the message body when in raw byte form
	 * @param length the length of the message body when in raw byte form
	 */
	public void setLength(byte[] length) {
		this.length = length;
	}

	/**
	 * Returns the type of DC message. You may compare to TYPE_* static constants for convenience.
	 * @return the type of DC message
	 */
	public int getMessageType() {
		int m = (messageType[0] << 8);
		m += messageType[1];
		
		return m;
	}

	/**
	 * Sets the the type of DC message. You may use TYPE_* static constants for convenience.
	 * @param messageType type of DC message
	 */
	public void setMessageType(byte[] messageType) {
		this.messageType = messageType;
	}

	/**
	 * Returns the raw bytes of the options field in the header
	 * @return the raw bytes of the options field in the header
	 */
	public byte[] getOptions() {
		return options;
	}

	/**
	 * Set the options field in the header using a raw byte representation
	 * @param options a raw byte represention of the 32-bit options field
	 */
	public void setOptions(byte[] options) {
		this.options = options;
	}
	
	/**
	 * Returns the 32-bit sequence number in raw byte form
	 * @return the 32-bit sequence number in raw byte form
	 */
	public byte[] getSequenceNumber() {
		return sequenceNumber;
	}

	/**
	 * Sets the 32-bit sequence number using raw bytes
	 * @param sequenceNumber a 32-bit sequence number in raw byte form
	 */
	public void setSequenceNumber(byte[] sequenceNumber) {
		this.sequenceNumber = sequenceNumber;
	}

	/**
	 * Returns an int representing the VLAN tag in this header
	 * @return the tag an integer representing the VLAN tag in this header
	 */
	public int getTag() {
		int vtag = tag[0] << 24;
		vtag += (tag[1] << 16);
		vtag += (tag[2] << 8);
		vtag += (tag[3] & 255); //255 forces unsigned
		
		return vtag;
	}

	/**
	 * Set the VLAN tag in this header
	 * @param vtag an int representing the VLAN tag to use (SEE the VLAN_* conatsants)
	 */
	public void setTag(int vtag) {
		/* Set vtag */
		tag[0] = (byte) (vtag >> 24);
		tag[1] = (byte) ((vtag & 16711680) >> 16);
		tag[2] = (byte) ((vtag & 65280) >> 8);
		tag[3] = (byte) (vtag & 255);
	}

	/**
	 * Set the VLAN tag in this header using raw bytes
	 * @param vtag raw bytes representing the VLAN tag to use
	 */
	public void setTag(byte[] vtag) {
		tag = vtag;
	}
	
	/**
	 * Returns the UCID in raw byte form
	 * @return the UCID in raw byte form
	 */
	public byte[] getUcid() {
		return ucid;
	}

	/**
	 * Sets the UCID
	 * @param ucid the UCID in raw byte form
	 */
	public void setUcid(byte[] ucid) {
		this.ucid = ucid;
	}
	
	/**
	 * Initializes byte arrays
	 *
	 */
	private void initialize(){
		/* Initialize */
		messageType = new byte[2];
		length = new byte[2];
		ucid = new byte[4];
		sequenceNumber = new byte[4];
		checksum = new byte[4];
		options = new byte[4];
		tag = new byte[4];
	}
	
	public void addOption(int opt){
		options[0] |= (byte) (opt >> 24);
		options[1] |= (byte) ((opt & 16711680) >> 16);
		options[2] |= (byte) ((opt & 65280) >> 8);
		options[3] |= (byte) (opt & 255);
	}
}
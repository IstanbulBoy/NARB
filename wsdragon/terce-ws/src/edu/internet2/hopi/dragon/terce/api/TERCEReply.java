package edu.internet2.hopi.dragon.terce.api;

import edu.internet2.hopi.dragon.terce.TERCENetworkDataException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.zip.DataFormatException;

/**
 * Represents a reply from the TERCE server. It can be generated 
 * by passing specific values or from the raw bytes of a packet 
 * returned by a TERCE server. It also contains many usefule static 
 * constants related to fields found in TERCE reply messages.
 * 
 * @author Andrew Lake (alake@internet2.edu)
 *
 */
public class TERCEReply {
	private TERCEMessageHeader header;
	private int type;
	private int length;
	private ArrayList<EROSubobject> ero;
	private ArrayList<Integer> vtagList;
	private int errorCode;
	
	/* TLV types */
	public final static int TLV_TYPE_TERCE_REQUEST = 0x02;
	public final static int TLV_TYPE_TERCE_ERO = 0x03;
	public final static int TLV_TYPE_TERCE_ERROR_CODE = 0x04;
	public final static int TLV_TYPE_TERCE_REQUEST_VTAG_MASK = 0x05;
	
	/* Error codes */
	public final static int ERROR_NO_SRC = 1;
	public final static int ERROR_NO_DST = 2;
	public final static int ERROR_NO_ROUTE = 3;
	public final static int ERROR_INTERNAL = 4;
	public final static int ERROR_INVALID_REQUEST = 5;
	public final static int ERROR_JUST_HOLDON = 6;
	public final static int ERROR_EXCEED_MAX_RETRAN = 7;
	
	/**
	 * Constructor that creates a TERCEReply object from passwed parameters for individual fields
	 * 
	 * @param header a TERCEMessageHEader that represents the header of this new reply
	 * @param type the TLV type of the message body
	 * @param length the length of the message body excluding the length and type fields
	 * @param ero a list of EROSubobject that represent the explicite route object of the path
	 * @param vtagList a list of vtags derived from a vtag mask
	 * @param errorCode error code returned by server (0 if there is no error)
	 */
	public TERCEReply(TERCEMessageHeader header, int type, int length, ArrayList<EROSubobject> ero, ArrayList<Integer> vtagList, int errorCode) {
		this.header = header;
		this.type = type;
		this.length = length;
		this.ero = ero;
		this.vtagList = vtagList;
	}
	
	/**
	 * Constructor that creates a TERCEReply object from the raw bytes of a packet returned by a TERCE server
	 * @param packet the packet returned by the server in raw byte form
	 * @throws UnknownHostException thrown when ERO contains an IP address that is invalid
	 */
	public TERCEReply(byte[] packet) throws UnknownHostException{

            header = new TERCEMessageHeader(packet);
       	
		/* Set TLV type */
		type = packet[TERCEMessageHeader.SIZE] << 8;
		type += packet[TERCEMessageHeader.SIZE + 1];
		
		/* Set length */
		length = packet[TERCEMessageHeader.SIZE + 2] << 8;
		length += packet[TERCEMessageHeader.SIZE + 3];
		
		/* Parse TLV subobject */
		int msgType = (int)header.getMessageType();
		if(msgType == TERCEMessageHeader.TYPE_REPLY_ERO){
			/* read ERO TLVS */
			if(type == TLV_TYPE_TERCE_ERO){
				ero = EROFromBytes(packet);
				
				/* check for list of vlans */
				if(header.getLength() > (4 + length) ){//header + body type + body length field size + body length value
					vtagList = vtagMaskFromBytes(packet, (TERCEMessageHeader.SIZE + length + 4));
				}else{
					vtagList = new ArrayList<Integer>();
				}
			}else{
				ero = new ArrayList<EROSubobject>();//convenience so don't have to worry about null condition
			}
			errorCode = 0;
		}else if(msgType == TERCEMessageHeader.TYPE_REPLY_ERROR){
			if(type == TLV_TYPE_TERCE_ERROR_CODE){
				errorCode = (packet[TERCEMessageHeader.SIZE + 4] << 24);
				errorCode += (packet[TERCEMessageHeader.SIZE + 5] << 16);
				errorCode += (packet[TERCEMessageHeader.SIZE + 6] << 8);
				errorCode += packet[TERCEMessageHeader.SIZE + 7];
			}
			ero = new ArrayList<EROSubobject>();//convenience so don't have to worry about null condition
		}else{
			System.out.println("Unsupported Reply Type: " + msgType);
		}
	}

	/**
	 * Return the explicit route object contained within a packet returned by the TERCE API server
	 * @param packet the packet returned by the TERCE API server in byte form
	 * @return an array list of EROSubobjects that represents the ERO in the packet originally passed
	 * @throws UnknownHostException thrown when ERO contains invalid IP addresses
	 */
	private ArrayList<EROSubobject> EROFromBytes(byte[] packet) throws UnknownHostException{
		ArrayList<EROSubobject> localERO= new ArrayList<EROSubobject>();
		int bytesRead = 0;
		
		while((length - bytesRead) > 0){
			/* Get loose bit */
			byte looseVal = (byte) ((packet[TERCEMessageHeader.SIZE + 4 + bytesRead] & 128) >> 7);
			boolean loose = (looseVal == 1);
			
			/* get type */
			byte type = (byte) (packet[TERCEMessageHeader.SIZE + 4+ bytesRead] & 127);
			
			/* get length */
			byte tlvLength = packet[TERCEMessageHeader.SIZE + 5+ bytesRead];
			
			/* check type */
			if(type == EROSubobject.IPV4_PREFIX_SUBOBJ){
				/* get IPv4 Address*/
				byte[] rawIP4 = new byte[4];
				for(int i = 0; i < 4; i++){
					rawIP4[i] = packet[TERCEMessageHeader.SIZE + 6 + i + bytesRead];
				}
				InetAddress ip4 = InetAddress.getByAddress(rawIP4);
				
				/* Get address length */
				byte addressLength = packet[TERCEMessageHeader.SIZE + 10 + bytesRead];
				
				/* Get address padding */
				byte padding = packet[TERCEMessageHeader.SIZE + 11 + bytesRead];
				
				bytesRead += tlvLength;
				
				localERO.add(new IPv4PrefixSubobject(loose, tlvLength, ip4, addressLength, padding));
			}else if(type == EROSubobject.UNUM_IF_SUBOBJ){
				/* get reserved bits */
				byte[] reserved = new byte[2];
				reserved[0] = packet[TERCEMessageHeader.SIZE + 6 + bytesRead];
				reserved[1] = packet[TERCEMessageHeader.SIZE + 7 + bytesRead];
				
				/* get IP address  */
				byte[] rawIP4 = new byte[4];
				for(int i = 0; i < 4; i++){
					rawIP4[i] = packet[TERCEMessageHeader.SIZE + 8 + i + bytesRead];
				}
				InetAddress ip4 = InetAddress.getByAddress(rawIP4);
				
				/* get Interface id */
				int interfaceId = (packet[TERCEMessageHeader.SIZE + 12 + bytesRead] << 24);
				interfaceId += (packet[TERCEMessageHeader.SIZE + 13 + bytesRead] << 16);
				interfaceId += (packet[TERCEMessageHeader.SIZE + 14 + bytesRead] << 8);
				interfaceId += (packet[TERCEMessageHeader.SIZE + 15 + bytesRead]);
				
				bytesRead += tlvLength;
				
				localERO.add(new UnumInterfaceSubobject(loose, tlvLength, reserved, ip4, interfaceId));
			}
		}
		
		return localERO;
	}
	
	/**
	 * Extracts available VLAN tags from a VLAN tag mask
	 * 
	 * @param packet the raw packet returned by the server
	 * @param offset where to start reading the VLAN tag mask
	 * @return
	 */
	private ArrayList<Integer> vtagMaskFromBytes(byte[] packet, int offset) {
		ArrayList<Integer> vtags = new ArrayList<Integer>();
		
		/* check type */
		int type = (packet[offset] << 8);
		type += packet[offset + 1];
		
		/* check length */
		int length = packet[offset + 2] << 8;
		length += packet[offset + 3];
		
		/* Read bitmask */
		if(type == TLV_TYPE_TERCE_REQUEST_VTAG_MASK){
			for(int i = 0; i < length; i++){
				for(int j = 0; j < 8; j++){
					if((packet[offset + 4 + i] & (int)Math.pow(2, (7-j))) > 0){
						vtags.add(new Integer(i*8 + j + 1));//+ 1 because VLAN tags start at 1
					}
				}
			}
		}
		
		return vtags;
	}
	
	/**
	 * Returns the ERO of the reply
	 * @return the ERO(explicit route object)
	 */
	public ArrayList<EROSubobject> getERO() {
		return ero;
	}

	/**
	 * Set the ERO
	 * @param ero the ERO to set
	 */
	public void setERO(ArrayList<EROSubobject> ero) {
		this.ero = ero;
	}

	/**
	 * Returns the header of this reply
	 * @return the header of this reply
	 */
	public TERCEMessageHeader getHeader() {
		return header;
	}

	/**
	 * Sets the header of this reply
	 * @param header the header to set for this reply
	 */
	public void setHeader(TERCEMessageHeader header) {
		this.header = header;
	}

	/**
	 * Returns the length of the message body excluding this field and the TLV type field
	 * @return the length
	 */
	public int getLength() {
		return length;
	}

	/**
	 * Returns the TLV type of this reply
	 * @return the TLV type of this reply
	 */
	public int getType() {
		return type;
	}

	/**
	 * Set the TLV type of this reply
	 * @param type the type of this reply to set
	 */
	public void setType(int type) {
		this.type = type;
	}

	/**
	 * Returns the error code contained within this reply.
	 * @return the error code contained within this reply (0 if no error)
	 */
	public int getErrorCode() {
		return errorCode;
	}

	/**
	 * Set the error code of this reply
	 * @param errorCode the error code of this reply to set
	 */
	public void setErrorCode(int errorCode) {
		this.errorCode = errorCode;
	}

	/**
	 * Returns the list of all available VLAN tags
	 * @return the list of all available VLAN tags
	 */
	public ArrayList<Integer> getVtagList() {
		return vtagList;
	}

	/**
	 * Sets the list of available VLAN tags
	 * @param vtagList the list of available VLAN tags to set
	 */
	public void setVtagList(ArrayList<Integer> vtagList) {
		this.vtagList = vtagList;
	}
}

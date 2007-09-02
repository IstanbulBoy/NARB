/*
 * TERCEUtilities.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package edu.internet2.hopi.dragon.terce;

import java.util.zip.DataFormatException;

/**
 * This class provides some utility functions for manipulating primitive byte arrays (<CODE>byte[]</CODE>) received from the network while carefully avoiding auto-conversions, type promotions and other java-related delicatessen.
 * To prevent autoconversion of large integers to their 2's complement (negative value) all the types a treated as <CODE>long</CODE>.
 * It is important to understand that the <I>host</I> order actually means little-endian. This was done to circumvent certain idiosyncrasies in the narb/rce code which assumes that the peer knows the byte-architecture the code is running on.
 * @author jflidr
 */
public class TERCEUtilities {
    
    /**
     * Writes the data to the byte array in the network byte order
     * @param b target byte array
     * @param d data (can be 1, 2 or 4 bytes long)
     * @param f structure field descriptor: int[] f = {offset, size}
     */
    public static void htonData(byte[] b, long d, int[] f) {
        //if((b == null) || ((f[0] + f[1]) > b.length) || !((f[1] == 1) || (f[1] == 2) || (f[1] == 4)) || (((f[0]%2) != 0) && (f[1] != 1)))
        for (int i = 0; i < f[1]; i++) {
            b[f[0] + i] = (byte)((d >>> (8*(f[1]-1-i))) & 0xff);
        }
    }
    
    /**
     * Writes the data to the offset byte array in the network byte order. The offset
     * can be used to traverse the primitive array without need to change the field
     * descriptors.
     * @param b target byte array
     * @param o offset from the start of the array
     * @param d data (can be 1, 2 or 4 bytes long)
     * @param f structure field descriptor: int[] f = {offset, size}
     */
    public static void htonData(byte[] b, int o, long d, int[] f) {
        int[] ff = {f[0], f[1]};
        ff[0] += o;
        htonData(b, d, ff);
    }
    
    /**
     * Reads the data from the byte array in the network byte order
     * @param b target byte array
     * @param f structure field descriptor: int[] f = {offset, size}
     * @return value (always long) in the host order
     */
    public static long nDatatoh(byte[] b, int[] f) {
        long ret = 0;
        for (int i = 0; i < f[1]; i++) {
            ret += (((long)b[f[0] + i] & 0xffL) << (8*(f[1]-1-i)));
        }
        return ret;
    }
    
    /**
     * Reads the data from the byte array in the network byte order. The offset
     * can be used to traverse the primitive array without need to change the field
     * descriptors.
     * @param b target byte array
     * @param o offset from the start of the array
     * @param f structure field descriptor: int[] f = {offset, size}
     * @return value (always long) in the host order
     */
    public static long nDatatoh(byte[] b, int o, int[] f) {
        int[] ff = {f[0], f[1]};
        ff[0] += o;
        return nDatatoh(b, ff);
    }
    
    /**
     * Writes the data to the byte array in the host byte order
     * @param b target byte array
     * @param d data (can be 1, 2 or 4 bytes long)
     * @param f structure field descriptor: int[] f = {offset, size}
     */
    public static void htohData(byte[] b, long d, int[] f) {
        for (int i = 0; i < f[1]; i++) {
            b[f[0] + i] = (byte)((d >>> (8*i)) & 0xff);
        }
    }
    
    /**
     * Writes the data to the offset byte array in the host byte order. The offset
     * can be used to traverse the primitive array without need to change the field
     * descriptors.
     * @param b target byte array
     * @param o offset from the start of the array
     * @param d data (can be 1, 2 or 4 bytes long)
     * @param f structure field descriptor: int[] f = {offset, size}
     */
    public static void htohData(byte[] b, int o, long d, int[] f) {
        int[] ff = {f[0], f[1]};
        ff[0] += o;
        htohData(b, d, ff);
    }
    
    /**
     * Reads the data from the byte array in the host byte order
     * @param b target byte array
     * @param f structure field descriptor: int[] f = {offset, size}
     * @return value (always long) in the host order
     */
    public static long hDatatoh(byte[] b, int[] f) {
        long ret = 0;
        for (int i = 0; i < f[1]; i++) {
            ret += (((long)b[f[0] + i] & 0xffL) << (8*i));
        }
        return ret;
    }
    
    /**
     * Reads the data from the byte array in the host byte order. The offset
     * can be used to traverse the primitive array without need to change the field
     * descriptors.
     * @param b target byte array
     * @param o offset from the start of the array
     * @param f structure field descriptor: int[] f = {offset, size}
     * @return value (always long) in the host order
     */
    public static long hDatatoh(byte[] b, int o, int[] f) {
        int[] ff = {f[0], f[1]};
        ff[0] += o;
        return hDatatoh(b, ff);
    }
    
    /**
     * Reads the data from the byte array in the host byte order. It allows to
     * read the data without the prior binary structure definition (useful for
     * checksumming)
     * @param b target byte array
     * @param o offset from the start of the array
     * @param s size of the read data in bytes
     * @return value (always long) in the host order
     */
    public static long hDatatoh(byte[] b, int o, int s) {
        int[] f = {o, s};
        return hDatatoh(b, f);
    }
    
    
    public static String toNetString(float f) {
        String suff = "";

        if(f >= 1000000000) {
            suff = "Gbps";
            f /= 1000000000;
        } else if(f >= 1000000) {
            suff = "Mbps";
            f /= 1000000;
        } else if(f >= 1000) {
            suff = "kbps";
            f /= 1000;
        }
        return Float.toString(f) + suff;
    }
}

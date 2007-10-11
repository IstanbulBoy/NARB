/*
 * Reservation.java
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package edu.internet2.hopi.dragon.terce.tedb;


/**
 *
 * @author jflidr
 */
public class Reservation {
    private long domainID;
    private long lspID;
    private long uptime;
    private long duration;
    private float bandwidth;
    private long status;

    /** Creates a new instance of Reservation */
    public Reservation() {
    }

    void setDomainID(long l) {
        domainID = l;
    }

    void setLspID(long l) {
        lspID = l;
    }

    void setUptime(long l) {
        uptime = l;
    }

    void setDuration(long l) {
        duration = l;
    }

    void setBW(float f) {
        bandwidth = f;
    }

    void setStatus(long l) {
        status = l;
    }
}

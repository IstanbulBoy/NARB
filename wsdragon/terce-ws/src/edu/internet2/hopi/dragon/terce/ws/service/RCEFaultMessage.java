
/**
 * RCEFaultMessage.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.3-RC2  Built on : Jul 20, 2007 (04:21:49 LKT)
 */

package edu.internet2.hopi.dragon.terce.ws.service;

public class RCEFaultMessage extends java.lang.Exception{
    
    private edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault faultMessage;
    
    public RCEFaultMessage() {
        super("RCEFaultMessage");
    }
           
    public RCEFaultMessage(java.lang.String s) {
       super(s);
    }
    
    public RCEFaultMessage(java.lang.String s, java.lang.Throwable ex) {
      super(s, ex);
    }
    
    public void setFaultMessage(edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault msg){
       faultMessage = msg;
    }
    
    public edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault getFaultMessage(){
       return faultMessage;
    }
}
    
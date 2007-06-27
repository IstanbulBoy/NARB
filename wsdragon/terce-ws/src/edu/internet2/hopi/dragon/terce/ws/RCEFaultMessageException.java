
/**
 * RCEFaultMessageException.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.2 Apr 27, 2007 (04:35:37 IST)
 */
package edu.internet2.hopi.dragon.terce.ws;

public class RCEFaultMessageException extends java.lang.Exception{
    
    private edu.internet2.hopi.dragon.terce.rce.TerceRceFault faultMessage;
    
    public RCEFaultMessageException() {
        super("RCEFaultMessageException");
    }
           
    public RCEFaultMessageException(java.lang.String s) {
       super(s);
    }
    
    public RCEFaultMessageException(java.lang.String s, java.lang.Throwable ex) {
      super(s, ex);
    }
    
    public void setFaultMessage(edu.internet2.hopi.dragon.terce.rce.TerceRceFault msg){
       faultMessage = msg;
    }
    
    public edu.internet2.hopi.dragon.terce.rce.TerceRceFault getFaultMessage(){
       return faultMessage;
    }
}
    
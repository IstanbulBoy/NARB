
/**
 * TERCEFaultMessageException.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.1.1 Jan 09, 2007 (06:20:51 LKT)
 */
package edu.internet2.hopi.dragon.terce.ws.client;

public class TERCEFaultMessageException extends java.lang.Exception{
    
    private edu.internet2.hopi.dragon.terce.ws.client.TERCEStub.TERCEFault faultMessage;
    
    public TERCEFaultMessageException() {
        super("TERCEFaultMessageException");
    }
           
    public TERCEFaultMessageException(java.lang.String s) {
       super(s);
    }
    
    public TERCEFaultMessageException(java.lang.String s, java.lang.Throwable ex) {
      super(s, ex);
    }
    
    public void setFaultMessage(edu.internet2.hopi.dragon.terce.ws.client.TERCEStub.TERCEFault msg){
       faultMessage = msg;
    }
    
    public edu.internet2.hopi.dragon.terce.ws.client.TERCEStub.TERCEFault getFaultMessage(){
       return faultMessage;
    }
}
    

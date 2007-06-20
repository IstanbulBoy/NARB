
/**
 * NARBFaultMessageException.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.1.1 Jan 09, 2007 (06:20:51 LKT)
 */
package edu.internet2.hopi.dragon.narb.ws.client;

public class NARBFaultMessageException extends java.lang.Exception{
    
    private edu.internet2.hopi.dragon.narb.ws.client.NARBStub.NARBFault faultMessage;
    
    public NARBFaultMessageException() {
        super("NARBFaultMessageException");
    }
           
    public NARBFaultMessageException(java.lang.String s) {
       super(s);
    }
    
    public NARBFaultMessageException(java.lang.String s, java.lang.Throwable ex) {
      super(s, ex);
    }
    
    public void setFaultMessage(edu.internet2.hopi.dragon.narb.ws.client.NARBStub.NARBFault msg){
       faultMessage = msg;
    }
    
    public edu.internet2.hopi.dragon.narb.ws.client.NARBStub.NARBFault getFaultMessage(){
       return faultMessage;
    }
}
    

/**
 * DCFaultMessageException.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.1.1 Jan 09, 2007 (06:20:51 LKT)
 */
package edu.internet2.hopi.dragon.dc.ws.client;

public class DCFaultMessageException extends java.lang.Exception{
    
    private edu.internet2.hopi.dragon.dc.ws.client.DCStub.DCFault faultMessage;
    
    public DCFaultMessageException() {
        super("DCFaultMessageException");
    }
           
    public DCFaultMessageException(java.lang.String s) {
       super(s);
    }
    
    public DCFaultMessageException(java.lang.String s, java.lang.Throwable ex) {
      super(s, ex);
    }
    
    public void setFaultMessage(edu.internet2.hopi.dragon.dc.ws.client.DCStub.DCFault msg){
       faultMessage = msg;
    }
    
    public edu.internet2.hopi.dragon.dc.ws.client.DCStub.DCFault getFaultMessage(){
       return faultMessage;
    }
}
    


/**
 * TEDBFaultMessageException.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.2 Apr 27, 2007 (04:35:37 IST)
 */
package edu.internet2.hopi.dragon.terce.ws.service;

public class TEDBFaultMessageException extends java.lang.Exception{
    
    private edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault faultMessage;
    
    public TEDBFaultMessageException() {
        super("TEDBFaultMessageException");
    }
           
    public TEDBFaultMessageException(java.lang.String s) {
       super(s);
    }
    
    public TEDBFaultMessageException(java.lang.String s, java.lang.Throwable ex) {
      super(s, ex);
    }
    
    public void setFaultMessage(edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault msg){
       faultMessage = msg;
    }
    
    public edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault getFaultMessage(){
       return faultMessage;
    }
}
    
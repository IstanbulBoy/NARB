
/**
 * TEDBFaultMessage.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.3-RC2  Built on : Jul 20, 2007 (04:21:49 LKT)
 */

package edu.internet2.hopi.dragon.terce.ws.service;

public class TEDBFaultMessage extends java.lang.Exception{
    
    private edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault faultMessage;
    
    public TEDBFaultMessage() {
        super("TEDBFaultMessage");
    }
           
    public TEDBFaultMessage(java.lang.String s) {
       super(s);
    }
    
    public TEDBFaultMessage(java.lang.String s, java.lang.Throwable ex) {
      super(s, ex);
    }
    
    public void setFaultMessage(edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault msg){
       faultMessage = msg;
    }
    
    public edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault getFaultMessage(){
       return faultMessage;
    }
}
    


        /**
        * DCMessageReceiverInOut.java
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2 version: 1.1.1 Jan 09, 2007 (06:20:51 LKT)
        */
        package edu.internet2.hopi.dragon.dc.ws.server;

        /**
        *  DCMessageReceiverInOut message receiver
        */

        public class DCMessageReceiverInOut extends org.apache.axis2.receivers.AbstractInOutSyncMessageReceiver{


        public void invokeBusinessLogic(org.apache.axis2.context.MessageContext msgContext, org.apache.axis2.context.MessageContext newMsgContext)
        throws org.apache.axis2.AxisFault{

        try {

        // get the implementation class for the Web Service
        Object obj = getTheImplementationObject(msgContext);

        DCSkeleton skel = (DCSkeleton)obj;
        //Out Envelop
        org.apache.axiom.soap.SOAPEnvelope envelope = null;
        //Find the axisOperation that has been set by the Dispatch phase.
        org.apache.axis2.description.AxisOperation op = msgContext.getOperationContext().getAxisOperation();
        if (op == null) {
        throw new org.apache.axis2.AxisFault("Operation is not located, if this is doclit style the SOAP-ACTION should specified via the SOAP Action to use the RawXMLProvider");
        }

        java.lang.String methodName;
        if(op.getName() != null & (methodName = org.apache.axis2.util.JavaUtils.xmlNameToJava(op.getName().getLocalPart())) != null){

        

            if("findPath".equals(methodName)){

            
            edu.internet2.hopi.dragon.dc.ws.FindPathResponse param3 = null;
                    
                            //doc style
                            edu.internet2.hopi.dragon.dc.ws.FindPath wrappedParam =
                                                 (edu.internet2.hopi.dragon.dc.ws.FindPath)fromOM(
                        msgContext.getEnvelope().getBody().getFirstElement(),
                        edu.internet2.hopi.dragon.dc.ws.FindPath.class,
                        getEnvelopeNamespaces(msgContext.getEnvelope()));
                                    
                                   param3 =
                                             skel.findPath(wrappedParam) ;
                                        
                                    envelope = toEnvelope(getSOAPFactory(msgContext), param3, false);
                                

            }
        

        newMsgContext.setEnvelope(envelope);
        }
        }catch (DCFaultMessageException e) {

            org.apache.axis2.AxisFault f = createAxisFault(e);

            f.setDetail(toOM(e.getFaultMessage(),false));

            throw f;
            }
        
        catch (Exception e) {
        throw org.apache.axis2.AxisFault.makeFault(e);
        }
        }
        
        //
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.dc.ws.FindPath param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.dc.ws.FindPath.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.dc.ws.FindPathResponse param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.dc.ws.FindPathResponse.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.dc.ws.DCFault param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.dc.ws.DCFault.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
                    private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, edu.internet2.hopi.dragon.dc.ws.FindPathResponse param, boolean optimizeContent){
                      org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();
                       
                                emptyEnvelope.getBody().addChild(param.getOMElement(edu.internet2.hopi.dragon.dc.ws.FindPathResponse.MY_QNAME,factory));
                            

                     return emptyEnvelope;
                    }
                    


        /**
        *  get the default envelope
        */
        private org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory){
        return factory.getDefaultEnvelope();
        }


        private  java.lang.Object fromOM(
        org.apache.axiom.om.OMElement param,
        java.lang.Class type,
        java.util.Map extraNamespaces){

        try {
        
                if (edu.internet2.hopi.dragon.dc.ws.FindPath.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.dc.ws.FindPath.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.dc.ws.FindPathResponse.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.dc.ws.FindPathResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.dc.ws.DCFault.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.dc.ws.DCFault.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
        } catch (Exception e) {
        throw new RuntimeException(e);
        }
           return null;
        }



    

        /**
        *  A utility method that copies the namepaces from the SOAPEnvelope
        */
        private java.util.Map getEnvelopeNamespaces(org.apache.axiom.soap.SOAPEnvelope env){
        java.util.Map returnMap = new java.util.HashMap();
        java.util.Iterator namespaceIterator = env.getAllDeclaredNamespaces();
        while (namespaceIterator.hasNext()) {
        org.apache.axiom.om.OMNamespace ns = (org.apache.axiom.om.OMNamespace) namespaceIterator.next();
        returnMap.put(ns.getPrefix(),ns.getNamespaceURI());
        }
        return returnMap;
        }

        private org.apache.axis2.AxisFault createAxisFault(java.lang.Exception e) {
        org.apache.axis2.AxisFault f;
        Throwable cause = e.getCause();
        if (cause != null) {
            f = new org.apache.axis2.AxisFault(e.getMessage(), cause);
        } else {
            f = new org.apache.axis2.AxisFault(e.getMessage());
        }

        return f;
    }

        }//end of class
    

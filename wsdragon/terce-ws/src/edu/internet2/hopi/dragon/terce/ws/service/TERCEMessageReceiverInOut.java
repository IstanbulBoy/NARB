
/**
 * TERCEMessageReceiverInOut.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.3-RC2  Built on : Jul 20, 2007 (04:21:49 LKT)
 */
        package edu.internet2.hopi.dragon.terce.ws.service;

        /**
        *  TERCEMessageReceiverInOut message receiver
        */

        public class TERCEMessageReceiverInOut extends org.apache.axis2.receivers.AbstractInOutSyncMessageReceiver{


        public void invokeBusinessLogic(org.apache.axis2.context.MessageContext msgContext, org.apache.axis2.context.MessageContext newMsgContext)
        throws org.apache.axis2.AxisFault{

        try {

        // get the implementation class for the Web Service
        Object obj = getTheImplementationObject(msgContext);

        TERCESkeleton skel = (TERCESkeleton)obj;
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
                
                edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse findPathResponse7 = null;
                        edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath wrappedParam =
                                                             (edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath)fromOM(
                                    msgContext.getEnvelope().getBody().getFirstElement(),
                                    edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath.class,
                                    getEnvelopeNamespaces(msgContext.getEnvelope()));
                                                
                                               findPathResponse7 =
                                                   
                                                   
                                                         skel.findPath(wrappedParam)
                                                    ;
                                            
                                        envelope = toEnvelope(getSOAPFactory(msgContext), findPathResponse7, false);
                                    } else 

            if("selectNetworkTopology".equals(methodName)){
                
                edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse selectNetworkTopologyResponse9 = null;
                        edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology wrappedParam =
                                                             (edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology)fromOM(
                                    msgContext.getEnvelope().getBody().getFirstElement(),
                                    edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology.class,
                                    getEnvelopeNamespaces(msgContext.getEnvelope()));
                                                
                                               selectNetworkTopologyResponse9 =
                                                   
                                                   
                                                         skel.selectNetworkTopology(wrappedParam)
                                                    ;
                                            
                                        envelope = toEnvelope(getSOAPFactory(msgContext), selectNetworkTopologyResponse9, false);
                                    } else 

            if("insertNetworkTopology".equals(methodName)){
                
                edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse insertNetworkTopologyResponse11 = null;
                        edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology wrappedParam =
                                                             (edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology)fromOM(
                                    msgContext.getEnvelope().getBody().getFirstElement(),
                                    edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology.class,
                                    getEnvelopeNamespaces(msgContext.getEnvelope()));
                                                
                                               insertNetworkTopologyResponse11 =
                                                   
                                                   
                                                         skel.insertNetworkTopology(wrappedParam)
                                                    ;
                                            
                                        envelope = toEnvelope(getSOAPFactory(msgContext), insertNetworkTopologyResponse11, false);
                                    
            } else {
              throw new java.lang.RuntimeException("method not found");
            }
        

        newMsgContext.setEnvelope(envelope);
        }
        } catch (TEDBFaultMessage e) {

            msgContext.setProperty(org.apache.axis2.Constants.FAULT_NAME,"TerceTedbFault");
            org.apache.axis2.AxisFault f = createAxisFault(e);
            if (e.getFaultMessage() != null){
                f.setDetail(toOM(e.getFaultMessage(),false));
            }
            throw f;
            }
         catch (RCEFaultMessage e) {

            msgContext.setProperty(org.apache.axis2.Constants.FAULT_NAME,"TerceRceFault");
            org.apache.axis2.AxisFault f = createAxisFault(e);
            if (e.getFaultMessage() != null){
                f.setDetail(toOM(e.getFaultMessage(),false));
            }
            throw f;
            }
        
        catch (java.lang.Exception e) {
        throw org.apache.axis2.AxisFault.makeFault(e);
        }
        }
        
        //
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology param, boolean optimizeContent)
            throws org.apache.axis2.AxisFault {

            
                        try{
                             return param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology.MY_QNAME,
                                          org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                        } catch(org.apache.axis2.databinding.ADBException e){
                            throw org.apache.axis2.AxisFault.makeFault(e);
                        }
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse param, boolean optimizeContent)
            throws org.apache.axis2.AxisFault {

            
                        try{
                             return param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse.MY_QNAME,
                                          org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                        } catch(org.apache.axis2.databinding.ADBException e){
                            throw org.apache.axis2.AxisFault.makeFault(e);
                        }
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault param, boolean optimizeContent)
            throws org.apache.axis2.AxisFault {

            
                        try{
                             return param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault.MY_QNAME,
                                          org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                        } catch(org.apache.axis2.databinding.ADBException e){
                            throw org.apache.axis2.AxisFault.makeFault(e);
                        }
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath param, boolean optimizeContent)
            throws org.apache.axis2.AxisFault {

            
                        try{
                             return param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath.MY_QNAME,
                                          org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                        } catch(org.apache.axis2.databinding.ADBException e){
                            throw org.apache.axis2.AxisFault.makeFault(e);
                        }
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse param, boolean optimizeContent)
            throws org.apache.axis2.AxisFault {

            
                        try{
                             return param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse.MY_QNAME,
                                          org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                        } catch(org.apache.axis2.databinding.ADBException e){
                            throw org.apache.axis2.AxisFault.makeFault(e);
                        }
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault param, boolean optimizeContent)
            throws org.apache.axis2.AxisFault {

            
                        try{
                             return param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault.MY_QNAME,
                                          org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                        } catch(org.apache.axis2.databinding.ADBException e){
                            throw org.apache.axis2.AxisFault.makeFault(e);
                        }
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology param, boolean optimizeContent)
            throws org.apache.axis2.AxisFault {

            
                        try{
                             return param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology.MY_QNAME,
                                          org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                        } catch(org.apache.axis2.databinding.ADBException e){
                            throw org.apache.axis2.AxisFault.makeFault(e);
                        }
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse param, boolean optimizeContent)
            throws org.apache.axis2.AxisFault {

            
                        try{
                             return param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse.MY_QNAME,
                                          org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                        } catch(org.apache.axis2.databinding.ADBException e){
                            throw org.apache.axis2.AxisFault.makeFault(e);
                        }
                    

            }
        
                    private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse param, boolean optimizeContent)
                        throws org.apache.axis2.AxisFault{
                      try{
                          org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();
                           
                                    emptyEnvelope.getBody().addChild(param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse.MY_QNAME,factory));
                                

                         return emptyEnvelope;
                    } catch(org.apache.axis2.databinding.ADBException e){
                        throw org.apache.axis2.AxisFault.makeFault(e);
                    }
                    }
                    
                    private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse param, boolean optimizeContent)
                        throws org.apache.axis2.AxisFault{
                      try{
                          org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();
                           
                                    emptyEnvelope.getBody().addChild(param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse.MY_QNAME,factory));
                                

                         return emptyEnvelope;
                    } catch(org.apache.axis2.databinding.ADBException e){
                        throw org.apache.axis2.AxisFault.makeFault(e);
                    }
                    }
                    
                    private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse param, boolean optimizeContent)
                        throws org.apache.axis2.AxisFault{
                      try{
                          org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();
                           
                                    emptyEnvelope.getBody().addChild(param.getOMElement(edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse.MY_QNAME,factory));
                                

                         return emptyEnvelope;
                    } catch(org.apache.axis2.databinding.ADBException e){
                        throw org.apache.axis2.AxisFault.makeFault(e);
                    }
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
        java.util.Map extraNamespaces) throws org.apache.axis2.AxisFault{

        try {
        
                if (edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopology.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.ws.types.rce.FindPath.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopology.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
        } catch (java.lang.Exception e) {
        throw org.apache.axis2.AxisFault.makeFault(e);
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
    
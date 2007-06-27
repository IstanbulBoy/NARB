
        /**
        * TERCEStub.java
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2 version: 1.2 Apr 27, 2007 (04:35:37 IST)
        */
        package edu.internet2.hopi.dragon.terce.ws;

        

        /*
        *  TERCEStub java implementation
        */

        
        public class TERCEStub extends org.apache.axis2.client.Stub
        {
        protected org.apache.axis2.description.AxisOperation[] _operations;

        //hashmaps to keep the fault mapping
        private java.util.HashMap faultExceptionNameMap = new java.util.HashMap();
        private java.util.HashMap faultExceptionClassNameMap = new java.util.HashMap();
        private java.util.HashMap faultMessageMap = new java.util.HashMap();

    
    private void populateAxisService() throws org.apache.axis2.AxisFault {

     //creating the Service with a unique name
     _service = new org.apache.axis2.description.AxisService("TERCE" + this.hashCode());
     
    

        //creating the operations
        org.apache.axis2.description.AxisOperation __operation;
    


        _operations = new org.apache.axis2.description.AxisOperation[3];
        
                   __operation = new org.apache.axis2.description.OutInAxisOperation();
                

            __operation.setName(new javax.xml.namespace.QName("", "findPath"));
	    _service.addOperation(__operation);
	    
	    
	    
            _operations[0]=__operation;
            
        
                   __operation = new org.apache.axis2.description.OutInAxisOperation();
                

            __operation.setName(new javax.xml.namespace.QName("", "selectNetworkTopology"));
	    _service.addOperation(__operation);
	    
	    
	    
            _operations[1]=__operation;
            
        
                   __operation = new org.apache.axis2.description.OutInAxisOperation();
                

            __operation.setName(new javax.xml.namespace.QName("", "insertNetworkTopology"));
	    _service.addOperation(__operation);
	    
	    
	    
            _operations[2]=__operation;
            
        
        }

    //populates the faults
    private void populateFaults(){
         
              faultExceptionNameMap.put( new javax.xml.namespace.QName(
                 "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                 "TerceRceFault"),
                 "edu.internet2.hopi.dragon.terce.ws.RCEFaultMessageException"
               );
              faultExceptionClassNameMap.put(new javax.xml.namespace.QName(
                "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                "TerceRceFault"),
                "edu.internet2.hopi.dragon.terce.ws.RCEFaultMessageException");
               faultMessageMap.put( new javax.xml.namespace.QName(
                 "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                 "TerceRceFault"),
                 "edu.internet2.hopi.dragon.terce.rce.TerceRceFault"
               );
           
              faultExceptionNameMap.put( new javax.xml.namespace.QName(
                 "http://hopi.internet2.edu/DRAGON/TERCE/TEDB",
                 "TerceTedbFault"),
                 "edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException"
               );
              faultExceptionClassNameMap.put(new javax.xml.namespace.QName(
                "http://hopi.internet2.edu/DRAGON/TERCE/TEDB",
                "TerceTedbFault"),
                "edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException");
               faultMessageMap.put( new javax.xml.namespace.QName(
                 "http://hopi.internet2.edu/DRAGON/TERCE/TEDB",
                 "TerceTedbFault"),
                 "edu.internet2.hopi.dragon.terce.tedb.TerceTedbFault"
               );
           
              faultExceptionNameMap.put( new javax.xml.namespace.QName(
                 "http://hopi.internet2.edu/DRAGON/TERCE/TEDB",
                 "TerceTedbFault"),
                 "edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException"
               );
              faultExceptionClassNameMap.put(new javax.xml.namespace.QName(
                "http://hopi.internet2.edu/DRAGON/TERCE/TEDB",
                "TerceTedbFault"),
                "edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException");
               faultMessageMap.put( new javax.xml.namespace.QName(
                 "http://hopi.internet2.edu/DRAGON/TERCE/TEDB",
                 "TerceTedbFault"),
                 "edu.internet2.hopi.dragon.terce.tedb.TerceTedbFault"
               );
           


    }

   /**
    Constructor that takes in a configContext
    */
   public TERCEStub(org.apache.axis2.context.ConfigurationContext configurationContext,
        java.lang.String targetEndpoint)
        throws org.apache.axis2.AxisFault {
         //To populate AxisService
         populateAxisService();
         populateFaults();

        _serviceClient = new org.apache.axis2.client.ServiceClient(configurationContext,_service);
        
	
        configurationContext = _serviceClient.getServiceContext().getConfigurationContext();

        _serviceClient.getOptions().setTo(new org.apache.axis2.addressing.EndpointReference(
                targetEndpoint));
        
            //Set the soap version
            _serviceClient.getOptions().setSoapVersionURI(org.apache.axiom.soap.SOAP12Constants.SOAP_ENVELOPE_NAMESPACE_URI);
        
    
    }

    /**
     * Default Constructor
     */
    public TERCEStub() throws org.apache.axis2.AxisFault {
        
                    this("https://hopi.internet2.edu/axis2/services/DRAGON/TERCE" );
                
    }

    /**
     * Constructor taking the target endpoint
     */
    public TERCEStub(java.lang.String targetEndpoint) throws org.apache.axis2.AxisFault {
        this(null,targetEndpoint);
    }



        
                    /**
                    * Auto generated method signature
                    * @see edu.internet2.hopi.dragon.terce.ws.TERCE#findPath
                        * @param findPath36
                    
                    */

                    
                            public  edu.internet2.hopi.dragon.terce.rce.FindPathResponse findPath(

                            edu.internet2.hopi.dragon.terce.rce.FindPath findPath36)
                        

                    throws java.rmi.RemoteException
                    
                    
                        ,edu.internet2.hopi.dragon.terce.ws.RCEFaultMessageException{

              try{
               org.apache.axis2.client.OperationClient _operationClient = _serviceClient.createClient(_operations[0].getName());
              _operationClient.getOptions().setAction("http://hopi.internet2.edu/DRAGON/TERCE/WS/findPath");
              _operationClient.getOptions().setExceptionToBeThrownOnSOAPFault(true);

              
              
                  addPropertyToOperationClient(_operationClient,org.apache.axis2.description.WSDL2Constants.ATTR_WHTTP_QUERY_PARAMETER_SEPARATOR,"&");
              

              // create a message context
              org.apache.axis2.context.MessageContext _messageContext = new org.apache.axis2.context.MessageContext();

              

              // create SOAP envelope with that payload
              org.apache.axiom.soap.SOAPEnvelope env = null;
                    
                                                    
                                                    env = toEnvelope(getFactory(_operationClient.getOptions().getSoapVersionURI()),
                                                    findPath36,
                                                    optimizeContent(new javax.xml.namespace.QName("",
                                                    "findPath")));
                                                
        //adding SOAP soap_headers
         _serviceClient.addHeadersToEnvelope(env);
        // set the message context with that soap envelope
        _messageContext.setEnvelope(env);

        // add the message contxt to the operation client
        _operationClient.addMessageContext(_messageContext);

        //execute the operation client
        _operationClient.execute(true);

         
               org.apache.axis2.context.MessageContext _returnMessageContext = _operationClient.getMessageContext(
                                           org.apache.axis2.wsdl.WSDLConstants.MESSAGE_LABEL_IN_VALUE);
                org.apache.axiom.soap.SOAPEnvelope _returnEnv = _returnMessageContext.getEnvelope();
                
                
                           java.lang.Object object = fromOM(
                                        _returnEnv.getBody().getFirstElement() ,
                                        edu.internet2.hopi.dragon.terce.rce.FindPathResponse.class,
                                         getEnvelopeNamespaces(_returnEnv));
                           _messageContext.getTransportOut().getSender().cleanup(_messageContext);
                          
                                   return (edu.internet2.hopi.dragon.terce.rce.FindPathResponse)object;
                              
         }catch(org.apache.axis2.AxisFault f){

            org.apache.axiom.om.OMElement faultElt = f.getDetail();
            if (faultElt!=null){
                if (faultExceptionNameMap.containsKey(faultElt.getQName())){
                    //make the fault by reflection
                    try{
                        java.lang.String exceptionClassName = (java.lang.String)faultExceptionClassNameMap.get(faultElt.getQName());
                        java.lang.Class exceptionClass = java.lang.Class.forName(exceptionClassName);
                        java.lang.Exception ex=
                                (java.lang.Exception) exceptionClass.newInstance();
                        //message class
                        java.lang.String messageClassName = (java.lang.String)faultMessageMap.get(faultElt.getQName());
                        java.lang.Class messageClass = java.lang.Class.forName(messageClassName);
                        java.lang.Object messageObject = fromOM(faultElt,messageClass,null);
                        java.lang.reflect.Method m = exceptionClass.getMethod("setFaultMessage",
                                   new java.lang.Class[]{messageClass});
                        m.invoke(ex,new java.lang.Object[]{messageObject});
                        
                        if (ex instanceof edu.internet2.hopi.dragon.terce.ws.RCEFaultMessageException){
                          throw (edu.internet2.hopi.dragon.terce.ws.RCEFaultMessageException)ex;
                        }
                        

                        throw new java.rmi.RemoteException(ex.getMessage(), ex);
                    }catch(java.lang.ClassCastException e){
                       // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    } catch (java.lang.ClassNotFoundException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }catch (java.lang.NoSuchMethodException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    } catch (java.lang.reflect.InvocationTargetException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }  catch (java.lang.IllegalAccessException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }   catch (java.lang.InstantiationException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }
                }else{
                    throw f;
                }
            }else{
                throw f;
            }
        }
        }
            
                /**
                * Auto generated method signature for Asynchronous Invocations
                * @see edu.internet2.hopi.dragon.terce.ws.TERCE#startfindPath
                    * @param findPath36
                
                */
                public  void startfindPath(

                 edu.internet2.hopi.dragon.terce.rce.FindPath findPath36,

                  final edu.internet2.hopi.dragon.terce.ws.TERCECallbackHandler callback)

                throws java.rmi.RemoteException{

              org.apache.axis2.client.OperationClient _operationClient = _serviceClient.createClient(_operations[0].getName());
             _operationClient.getOptions().setAction("http://hopi.internet2.edu/DRAGON/TERCE/WS/findPath");
             _operationClient.getOptions().setExceptionToBeThrownOnSOAPFault(true);

              
              
                  addPropertyToOperationClient(_operationClient,org.apache.axis2.description.WSDL2Constants.ATTR_WHTTP_QUERY_PARAMETER_SEPARATOR,"&");
              

          

              // create SOAP envelope with that payload
              org.apache.axiom.soap.SOAPEnvelope env=null;
              org.apache.axis2.context.MessageContext _messageContext = new org.apache.axis2.context.MessageContext();

                    
                                    //Style is Doc.
                                    
                                                    
                                                    env = toEnvelope(getFactory(_operationClient.getOptions().getSoapVersionURI()),
                                                    findPath36,
                                                    optimizeContent(new javax.xml.namespace.QName("",
                                                    "findPath")));
                                                
        //adding SOAP soap_headers
         _serviceClient.addHeadersToEnvelope(env);
        // create message context with that soap envelope
        _messageContext.setEnvelope(env);

        // add the message contxt to the operation client
        _operationClient.addMessageContext(_messageContext);


                    
                           _operationClient.setCallback(new org.apache.axis2.client.async.Callback() {
                    public void onComplete(
                            org.apache.axis2.client.async.AsyncResult result) {
                        java.lang.Object object = fromOM(result.getResponseEnvelope().getBody().getFirstElement(),
                               edu.internet2.hopi.dragon.terce.rce.FindPathResponse.class,
                               getEnvelopeNamespaces(result.getResponseEnvelope())
                            );
                        callback.receiveResultfindPath(
                            
                                   (edu.internet2.hopi.dragon.terce.rce.FindPathResponse)object);
                                

                    }

                    public void onError(java.lang.Exception e) {
                        callback.receiveErrorfindPath(e);
                    }
                });
                        

          org.apache.axis2.util.CallbackReceiver _callbackReceiver = null;
        if ( _operations[0].getMessageReceiver()==null &&  _operationClient.getOptions().isUseSeparateListener()) {
           _callbackReceiver = new org.apache.axis2.util.CallbackReceiver();
          _operations[0].setMessageReceiver(
                    _callbackReceiver);
        }

           //execute the operation client
           _operationClient.execute(false);

                    }
                
                    /**
                    * Auto generated method signature
                    * @see edu.internet2.hopi.dragon.terce.ws.TERCE#selectNetworkTopology
                        * @param selectNetworkTopology38
                    
                    */

                    
                            public  edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse selectNetworkTopology(

                            edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopology selectNetworkTopology38)
                        

                    throws java.rmi.RemoteException
                    
                    
                        ,edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException{

              try{
               org.apache.axis2.client.OperationClient _operationClient = _serviceClient.createClient(_operations[1].getName());
              _operationClient.getOptions().setAction("http://hopi.internet2.edu/DRAGON/TERCE/WS/selectNetworkTopology");
              _operationClient.getOptions().setExceptionToBeThrownOnSOAPFault(true);

              
              
                  addPropertyToOperationClient(_operationClient,org.apache.axis2.description.WSDL2Constants.ATTR_WHTTP_QUERY_PARAMETER_SEPARATOR,"&");
              

              // create a message context
              org.apache.axis2.context.MessageContext _messageContext = new org.apache.axis2.context.MessageContext();

              

              // create SOAP envelope with that payload
              org.apache.axiom.soap.SOAPEnvelope env = null;
                    
                                                    
                                                    env = toEnvelope(getFactory(_operationClient.getOptions().getSoapVersionURI()),
                                                    selectNetworkTopology38,
                                                    optimizeContent(new javax.xml.namespace.QName("",
                                                    "selectNetworkTopology")));
                                                
        //adding SOAP soap_headers
         _serviceClient.addHeadersToEnvelope(env);
        // set the message context with that soap envelope
        _messageContext.setEnvelope(env);

        // add the message contxt to the operation client
        _operationClient.addMessageContext(_messageContext);

        //execute the operation client
        _operationClient.execute(true);

         
               org.apache.axis2.context.MessageContext _returnMessageContext = _operationClient.getMessageContext(
                                           org.apache.axis2.wsdl.WSDLConstants.MESSAGE_LABEL_IN_VALUE);
                org.apache.axiom.soap.SOAPEnvelope _returnEnv = _returnMessageContext.getEnvelope();
                
                
                           java.lang.Object object = fromOM(
                                        _returnEnv.getBody().getFirstElement() ,
                                        edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse.class,
                                         getEnvelopeNamespaces(_returnEnv));
                           _messageContext.getTransportOut().getSender().cleanup(_messageContext);
                          
                                   return (edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse)object;
                              
         }catch(org.apache.axis2.AxisFault f){

            org.apache.axiom.om.OMElement faultElt = f.getDetail();
            if (faultElt!=null){
                if (faultExceptionNameMap.containsKey(faultElt.getQName())){
                    //make the fault by reflection
                    try{
                        java.lang.String exceptionClassName = (java.lang.String)faultExceptionClassNameMap.get(faultElt.getQName());
                        java.lang.Class exceptionClass = java.lang.Class.forName(exceptionClassName);
                        java.lang.Exception ex=
                                (java.lang.Exception) exceptionClass.newInstance();
                        //message class
                        java.lang.String messageClassName = (java.lang.String)faultMessageMap.get(faultElt.getQName());
                        java.lang.Class messageClass = java.lang.Class.forName(messageClassName);
                        java.lang.Object messageObject = fromOM(faultElt,messageClass,null);
                        java.lang.reflect.Method m = exceptionClass.getMethod("setFaultMessage",
                                   new java.lang.Class[]{messageClass});
                        m.invoke(ex,new java.lang.Object[]{messageObject});
                        
                        if (ex instanceof edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException){
                          throw (edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException)ex;
                        }
                        

                        throw new java.rmi.RemoteException(ex.getMessage(), ex);
                    }catch(java.lang.ClassCastException e){
                       // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    } catch (java.lang.ClassNotFoundException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }catch (java.lang.NoSuchMethodException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    } catch (java.lang.reflect.InvocationTargetException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }  catch (java.lang.IllegalAccessException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }   catch (java.lang.InstantiationException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }
                }else{
                    throw f;
                }
            }else{
                throw f;
            }
        }
        }
            
                /**
                * Auto generated method signature for Asynchronous Invocations
                * @see edu.internet2.hopi.dragon.terce.ws.TERCE#startselectNetworkTopology
                    * @param selectNetworkTopology38
                
                */
                public  void startselectNetworkTopology(

                 edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopology selectNetworkTopology38,

                  final edu.internet2.hopi.dragon.terce.ws.TERCECallbackHandler callback)

                throws java.rmi.RemoteException{

              org.apache.axis2.client.OperationClient _operationClient = _serviceClient.createClient(_operations[1].getName());
             _operationClient.getOptions().setAction("http://hopi.internet2.edu/DRAGON/TERCE/WS/selectNetworkTopology");
             _operationClient.getOptions().setExceptionToBeThrownOnSOAPFault(true);

              
              
                  addPropertyToOperationClient(_operationClient,org.apache.axis2.description.WSDL2Constants.ATTR_WHTTP_QUERY_PARAMETER_SEPARATOR,"&");
              

          

              // create SOAP envelope with that payload
              org.apache.axiom.soap.SOAPEnvelope env=null;
              org.apache.axis2.context.MessageContext _messageContext = new org.apache.axis2.context.MessageContext();

                    
                                    //Style is Doc.
                                    
                                                    
                                                    env = toEnvelope(getFactory(_operationClient.getOptions().getSoapVersionURI()),
                                                    selectNetworkTopology38,
                                                    optimizeContent(new javax.xml.namespace.QName("",
                                                    "selectNetworkTopology")));
                                                
        //adding SOAP soap_headers
         _serviceClient.addHeadersToEnvelope(env);
        // create message context with that soap envelope
        _messageContext.setEnvelope(env);

        // add the message contxt to the operation client
        _operationClient.addMessageContext(_messageContext);


                    
                           _operationClient.setCallback(new org.apache.axis2.client.async.Callback() {
                    public void onComplete(
                            org.apache.axis2.client.async.AsyncResult result) {
                        java.lang.Object object = fromOM(result.getResponseEnvelope().getBody().getFirstElement(),
                               edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse.class,
                               getEnvelopeNamespaces(result.getResponseEnvelope())
                            );
                        callback.receiveResultselectNetworkTopology(
                            
                                   (edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse)object);
                                

                    }

                    public void onError(java.lang.Exception e) {
                        callback.receiveErrorselectNetworkTopology(e);
                    }
                });
                        

          org.apache.axis2.util.CallbackReceiver _callbackReceiver = null;
        if ( _operations[1].getMessageReceiver()==null &&  _operationClient.getOptions().isUseSeparateListener()) {
           _callbackReceiver = new org.apache.axis2.util.CallbackReceiver();
          _operations[1].setMessageReceiver(
                    _callbackReceiver);
        }

           //execute the operation client
           _operationClient.execute(false);

                    }
                
                    /**
                    * Auto generated method signature
                    * @see edu.internet2.hopi.dragon.terce.ws.TERCE#insertNetworkTopology
                        * @param insertNetworkTopology40
                    
                    */

                    
                            public  edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse insertNetworkTopology(

                            edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopology insertNetworkTopology40)
                        

                    throws java.rmi.RemoteException
                    
                    
                        ,edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException{

              try{
               org.apache.axis2.client.OperationClient _operationClient = _serviceClient.createClient(_operations[2].getName());
              _operationClient.getOptions().setAction("http://hopi.internet2.edu/DRAGON/TERCE/WS/insertNetworkTopology");
              _operationClient.getOptions().setExceptionToBeThrownOnSOAPFault(true);

              
              
                  addPropertyToOperationClient(_operationClient,org.apache.axis2.description.WSDL2Constants.ATTR_WHTTP_QUERY_PARAMETER_SEPARATOR,"&");
              

              // create a message context
              org.apache.axis2.context.MessageContext _messageContext = new org.apache.axis2.context.MessageContext();

              

              // create SOAP envelope with that payload
              org.apache.axiom.soap.SOAPEnvelope env = null;
                    
                                                    
                                                    env = toEnvelope(getFactory(_operationClient.getOptions().getSoapVersionURI()),
                                                    insertNetworkTopology40,
                                                    optimizeContent(new javax.xml.namespace.QName("",
                                                    "insertNetworkTopology")));
                                                
        //adding SOAP soap_headers
         _serviceClient.addHeadersToEnvelope(env);
        // set the message context with that soap envelope
        _messageContext.setEnvelope(env);

        // add the message contxt to the operation client
        _operationClient.addMessageContext(_messageContext);

        //execute the operation client
        _operationClient.execute(true);

         
               org.apache.axis2.context.MessageContext _returnMessageContext = _operationClient.getMessageContext(
                                           org.apache.axis2.wsdl.WSDLConstants.MESSAGE_LABEL_IN_VALUE);
                org.apache.axiom.soap.SOAPEnvelope _returnEnv = _returnMessageContext.getEnvelope();
                
                
                           java.lang.Object object = fromOM(
                                        _returnEnv.getBody().getFirstElement() ,
                                        edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse.class,
                                         getEnvelopeNamespaces(_returnEnv));
                           _messageContext.getTransportOut().getSender().cleanup(_messageContext);
                          
                                   return (edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse)object;
                              
         }catch(org.apache.axis2.AxisFault f){

            org.apache.axiom.om.OMElement faultElt = f.getDetail();
            if (faultElt!=null){
                if (faultExceptionNameMap.containsKey(faultElt.getQName())){
                    //make the fault by reflection
                    try{
                        java.lang.String exceptionClassName = (java.lang.String)faultExceptionClassNameMap.get(faultElt.getQName());
                        java.lang.Class exceptionClass = java.lang.Class.forName(exceptionClassName);
                        java.lang.Exception ex=
                                (java.lang.Exception) exceptionClass.newInstance();
                        //message class
                        java.lang.String messageClassName = (java.lang.String)faultMessageMap.get(faultElt.getQName());
                        java.lang.Class messageClass = java.lang.Class.forName(messageClassName);
                        java.lang.Object messageObject = fromOM(faultElt,messageClass,null);
                        java.lang.reflect.Method m = exceptionClass.getMethod("setFaultMessage",
                                   new java.lang.Class[]{messageClass});
                        m.invoke(ex,new java.lang.Object[]{messageObject});
                        
                        if (ex instanceof edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException){
                          throw (edu.internet2.hopi.dragon.terce.ws.TEDBFaultMessageException)ex;
                        }
                        

                        throw new java.rmi.RemoteException(ex.getMessage(), ex);
                    }catch(java.lang.ClassCastException e){
                       // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    } catch (java.lang.ClassNotFoundException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }catch (java.lang.NoSuchMethodException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    } catch (java.lang.reflect.InvocationTargetException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }  catch (java.lang.IllegalAccessException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }   catch (java.lang.InstantiationException e) {
                        // we cannot intantiate the class - throw the original Axis fault
                        throw f;
                    }
                }else{
                    throw f;
                }
            }else{
                throw f;
            }
        }
        }
            
                /**
                * Auto generated method signature for Asynchronous Invocations
                * @see edu.internet2.hopi.dragon.terce.ws.TERCE#startinsertNetworkTopology
                    * @param insertNetworkTopology40
                
                */
                public  void startinsertNetworkTopology(

                 edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopology insertNetworkTopology40,

                  final edu.internet2.hopi.dragon.terce.ws.TERCECallbackHandler callback)

                throws java.rmi.RemoteException{

              org.apache.axis2.client.OperationClient _operationClient = _serviceClient.createClient(_operations[2].getName());
             _operationClient.getOptions().setAction("http://hopi.internet2.edu/DRAGON/TERCE/WS/insertNetworkTopology");
             _operationClient.getOptions().setExceptionToBeThrownOnSOAPFault(true);

              
              
                  addPropertyToOperationClient(_operationClient,org.apache.axis2.description.WSDL2Constants.ATTR_WHTTP_QUERY_PARAMETER_SEPARATOR,"&");
              

          

              // create SOAP envelope with that payload
              org.apache.axiom.soap.SOAPEnvelope env=null;
              org.apache.axis2.context.MessageContext _messageContext = new org.apache.axis2.context.MessageContext();

                    
                                    //Style is Doc.
                                    
                                                    
                                                    env = toEnvelope(getFactory(_operationClient.getOptions().getSoapVersionURI()),
                                                    insertNetworkTopology40,
                                                    optimizeContent(new javax.xml.namespace.QName("",
                                                    "insertNetworkTopology")));
                                                
        //adding SOAP soap_headers
         _serviceClient.addHeadersToEnvelope(env);
        // create message context with that soap envelope
        _messageContext.setEnvelope(env);

        // add the message contxt to the operation client
        _operationClient.addMessageContext(_messageContext);


                    
                           _operationClient.setCallback(new org.apache.axis2.client.async.Callback() {
                    public void onComplete(
                            org.apache.axis2.client.async.AsyncResult result) {
                        java.lang.Object object = fromOM(result.getResponseEnvelope().getBody().getFirstElement(),
                               edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse.class,
                               getEnvelopeNamespaces(result.getResponseEnvelope())
                            );
                        callback.receiveResultinsertNetworkTopology(
                            
                                   (edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse)object);
                                

                    }

                    public void onError(java.lang.Exception e) {
                        callback.receiveErrorinsertNetworkTopology(e);
                    }
                });
                        

          org.apache.axis2.util.CallbackReceiver _callbackReceiver = null;
        if ( _operations[2].getMessageReceiver()==null &&  _operationClient.getOptions().isUseSeparateListener()) {
           _callbackReceiver = new org.apache.axis2.util.CallbackReceiver();
          _operations[2].setMessageReceiver(
                    _callbackReceiver);
        }

           //execute the operation client
           _operationClient.execute(false);

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

    
    
    private javax.xml.namespace.QName[] opNameArray = null;
    private boolean optimizeContent(javax.xml.namespace.QName opName) {
        

        if (opNameArray == null) {
            return false;
        }
        for (int i = 0; i < opNameArray.length; i++) {
            if (opName.equals(opNameArray[i])) {
                return true;   
            }
        }
        return false;
    }
     //https://hopi.internet2.edu/axis2/services/DRAGON/TERCE
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.rce.FindPath param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.terce.rce.FindPath.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.rce.FindPathResponse param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.terce.rce.FindPathResponse.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.rce.TerceRceFault param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.terce.rce.TerceRceFault.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopology param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopology.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.tedb.TerceTedbFault param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.terce.tedb.TerceTedbFault.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopology param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopology.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
            private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse param, boolean optimizeContent){
            
                     return param.getOMElement(edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse.MY_QNAME,
                                  org.apache.axiom.om.OMAbstractFactory.getOMFactory());
                    

            }
        
                            
                            private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, edu.internet2.hopi.dragon.terce.rce.FindPath param, boolean optimizeContent){
                            org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();
                                 
                                        emptyEnvelope.getBody().addChild(param.getOMElement(edu.internet2.hopi.dragon.terce.rce.FindPath.MY_QNAME,factory));
                                    
                             return emptyEnvelope;
                            }

                             
                             /* methods to provide back word compatibility */

                             
                            
                            private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopology param, boolean optimizeContent){
                            org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();
                                 
                                        emptyEnvelope.getBody().addChild(param.getOMElement(edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopology.MY_QNAME,factory));
                                    
                             return emptyEnvelope;
                            }

                             
                             /* methods to provide back word compatibility */

                             
                            
                            private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopology param, boolean optimizeContent){
                            org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();
                                 
                                        emptyEnvelope.getBody().addChild(param.getOMElement(edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopology.MY_QNAME,factory));
                                    
                             return emptyEnvelope;
                            }

                             
                             /* methods to provide back word compatibility */

                             


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
        
                if (edu.internet2.hopi.dragon.terce.rce.FindPath.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.rce.FindPath.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.rce.FindPathResponse.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.rce.FindPathResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.rce.TerceRceFault.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.rce.TerceRceFault.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopology.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopology.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.tedb.TerceTedbFault.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.tedb.TerceTedbFault.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopology.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopology.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
                if (edu.internet2.hopi.dragon.terce.tedb.TerceTedbFault.class.equals(type)){
                
                           return edu.internet2.hopi.dragon.terce.tedb.TerceTedbFault.Factory.parse(param.getXMLStreamReaderWithoutCaching());
                    

                }
           
        } catch (Exception e) {
        throw new RuntimeException(e);
        }
           return null;
        }



    
            private void setOpNameArray(){
            opNameArray = null;
            }
           
   }
   
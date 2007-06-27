
    /**
     * TERCECallbackHandler.java
     *
     * This file was auto-generated from WSDL
     * by the Apache Axis2 version: 1.2 Apr 27, 2007 (04:35:37 IST)
     */
    package edu.internet2.hopi.dragon.terce.ws;

    /**
     *  TERCECallbackHandler Callback class, Users can extend this class and implement
     *  their own receiveResult and receiveError methods.
     */
    public abstract class TERCECallbackHandler{



    protected Object clientData;

    /**
    * User can pass in any object that needs to be accessed once the NonBlocking
    * Web service call is finished and appropriate method of this CallBack is called.
    * @param clientData Object mechanism by which the user can pass in user data
    * that will be avilable at the time this callback is called.
    */
    public TERCECallbackHandler(Object clientData){
        this.clientData = clientData;
    }

    /**
    * Please use this constructor if you don't want to set any clientData
    */
    public TERCECallbackHandler(){
        this.clientData = null;
    }

    /**
     * Get the client data
     */

     public Object getClientData() {
        return clientData;
     }

        
           /**
            * auto generated Axis2 call back method for findPath method
            * override this method for handling normal response from findPath operation
            */
           public void receiveResultfindPath(
                    edu.internet2.hopi.dragon.terce.rce.FindPathResponse result
                        ) {
           }

          /**
           * auto generated Axis2 Error handler
           * override this method for handling error response from findPath operation
           */
            public void receiveErrorfindPath(java.lang.Exception e) {
            }
                
           /**
            * auto generated Axis2 call back method for selectNetworkTopology method
            * override this method for handling normal response from selectNetworkTopology operation
            */
           public void receiveResultselectNetworkTopology(
                    edu.internet2.hopi.dragon.terce.tedb.SelectNetworkTopologyResponse result
                        ) {
           }

          /**
           * auto generated Axis2 Error handler
           * override this method for handling error response from selectNetworkTopology operation
           */
            public void receiveErrorselectNetworkTopology(java.lang.Exception e) {
            }
                
           /**
            * auto generated Axis2 call back method for insertNetworkTopology method
            * override this method for handling normal response from insertNetworkTopology operation
            */
           public void receiveResultinsertNetworkTopology(
                    edu.internet2.hopi.dragon.terce.tedb.InsertNetworkTopologyResponse result
                        ) {
           }

          /**
           * auto generated Axis2 Error handler
           * override this method for handling error response from insertNetworkTopology operation
           */
            public void receiveErrorinsertNetworkTopology(java.lang.Exception e) {
            }
                


    }
    

            /**
            * CtrlPlaneLinkContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package org.ogf.schema.network.topology.ctrlplane._20070626;
            /**
            *  CtrlPlaneLinkContent bean class
            */
        

        public  class CtrlPlaneLinkContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = CtrlPlaneLinkContent
                Namespace URI = http://ogf.org/schema/network/topology/ctrlPlane/20070626/
                Namespace Prefix = ns1
                */
            

                        /**
                        * field for LocalPortId
                        */

                        
                            protected org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent localLocalPortId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLocalPortIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent
                           */
                           public  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent getLocalPortId(){
                               return localLocalPortId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param LocalPortId
                               */
                               public void setLocalPortId(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLocalPortIdTracker = true;
                                       } else {
                                          localLocalPortIdTracker = false;
                                              
                                       }
                                   
                                            this.localLocalPortId=param;
                                    

                               }
                            

                        /**
                        * field for RemotePortId
                        */

                        
                            protected org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent localRemotePortId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localRemotePortIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent
                           */
                           public  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent getRemotePortId(){
                               return localRemotePortId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param RemotePortId
                               */
                               public void setRemotePortId(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localRemotePortIdTracker = true;
                                       } else {
                                          localRemotePortIdTracker = false;
                                              
                                       }
                                   
                                            this.localRemotePortId=param;
                                    

                               }
                            

                        /**
                        * field for RemoteNodeId
                        */

                        
                            protected org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent localRemoteNodeId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localRemoteNodeIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent
                           */
                           public  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent getRemoteNodeId(){
                               return localRemoteNodeId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param RemoteNodeId
                               */
                               public void setRemoteNodeId(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localRemoteNodeIdTracker = true;
                                       } else {
                                          localRemoteNodeIdTracker = false;
                                              
                                       }
                                   
                                            this.localRemoteNodeId=param;
                                    

                               }
                            

                        /**
                        * field for RemoteDomainId
                        */

                        
                            protected org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent localRemoteDomainId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localRemoteDomainIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent
                           */
                           public  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent getRemoteDomainId(){
                               return localRemoteDomainId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param RemoteDomainId
                               */
                               public void setRemoteDomainId(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localRemoteDomainIdTracker = true;
                                       } else {
                                          localRemoteDomainIdTracker = false;
                                              
                                       }
                                   
                                            this.localRemoteDomainId=param;
                                    

                               }
                            

                        /**
                        * field for TrafficEngineeringMetric
                        */

                        
                            protected java.lang.String localTrafficEngineeringMetric ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localTrafficEngineeringMetricTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getTrafficEngineeringMetric(){
                               return localTrafficEngineeringMetric;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param TrafficEngineeringMetric
                               */
                               public void setTrafficEngineeringMetric(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localTrafficEngineeringMetricTracker = true;
                                       } else {
                                          localTrafficEngineeringMetricTracker = false;
                                              
                                       }
                                   
                                            this.localTrafficEngineeringMetric=param;
                                    

                               }
                            

                        /**
                        * field for LinkProtectionTypes
                        */

                        
                            protected java.lang.String localLinkProtectionTypes ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinkProtectionTypesTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getLinkProtectionTypes(){
                               return localLinkProtectionTypes;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param LinkProtectionTypes
                               */
                               public void setLinkProtectionTypes(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLinkProtectionTypesTracker = true;
                                       } else {
                                          localLinkProtectionTypesTracker = false;
                                              
                                       }
                                   
                                            this.localLinkProtectionTypes=param;
                                    

                               }
                            

                        /**
                        * field for Capacity
                        */

                        
                            protected java.lang.String localCapacity ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localCapacityTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getCapacity(){
                               return localCapacity;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Capacity
                               */
                               public void setCapacity(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localCapacityTracker = true;
                                       } else {
                                          localCapacityTracker = false;
                                              
                                       }
                                   
                                            this.localCapacity=param;
                                    

                               }
                            

                        /**
                        * field for MaximumReservableCapacity
                        */

                        
                            protected java.lang.String localMaximumReservableCapacity ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localMaximumReservableCapacityTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getMaximumReservableCapacity(){
                               return localMaximumReservableCapacity;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param MaximumReservableCapacity
                               */
                               public void setMaximumReservableCapacity(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localMaximumReservableCapacityTracker = true;
                                       } else {
                                          localMaximumReservableCapacityTracker = false;
                                              
                                       }
                                   
                                            this.localMaximumReservableCapacity=param;
                                    

                               }
                            

                        /**
                        * field for MinimumReservableCapacity
                        */

                        
                            protected java.lang.String localMinimumReservableCapacity ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localMinimumReservableCapacityTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getMinimumReservableCapacity(){
                               return localMinimumReservableCapacity;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param MinimumReservableCapacity
                               */
                               public void setMinimumReservableCapacity(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localMinimumReservableCapacityTracker = true;
                                       } else {
                                          localMinimumReservableCapacityTracker = false;
                                              
                                       }
                                   
                                            this.localMinimumReservableCapacity=param;
                                    

                               }
                            

                        /**
                        * field for Granularity
                        */

                        
                            protected java.lang.String localGranularity ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localGranularityTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getGranularity(){
                               return localGranularity;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Granularity
                               */
                               public void setGranularity(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localGranularityTracker = true;
                                       } else {
                                          localGranularityTracker = false;
                                              
                                       }
                                   
                                            this.localGranularity=param;
                                    

                               }
                            

                        /**
                        * field for UnreservedCapacity
                        */

                        
                            protected java.lang.String localUnreservedCapacity ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localUnreservedCapacityTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getUnreservedCapacity(){
                               return localUnreservedCapacity;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param UnreservedCapacity
                               */
                               public void setUnreservedCapacity(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localUnreservedCapacityTracker = true;
                                       } else {
                                          localUnreservedCapacityTracker = false;
                                              
                                       }
                                   
                                            this.localUnreservedCapacity=param;
                                    

                               }
                            

                        /**
                        * field for AdministrativeGroups
                        * This was an Array!
                        */

                        
                            protected org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup[] localAdministrativeGroups ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localAdministrativeGroupsTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup[]
                           */
                           public  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup[] getAdministrativeGroups(){
                               return localAdministrativeGroups;
                           }

                           
                        


                               
                              /**
                               * validate the array for AdministrativeGroups
                               */
                              protected void validateAdministrativeGroups(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup[] param){
                             
                              }


                             /**
                              * Auto generated setter method
                              * @param param AdministrativeGroups
                              */
                              public void setAdministrativeGroups(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup[] param){
                              
                                   validateAdministrativeGroups(param);

                               
                                          if (param != null){
                                             //update the setting tracker
                                             localAdministrativeGroupsTracker = true;
                                          } else {
                                             localAdministrativeGroupsTracker = false;
                                                 
                                          }
                                      
                                      this.localAdministrativeGroups=param;
                              }

                               
                             
                             /**
                             * Auto generated add method for the array for convenience
                             * @param param org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup
                             */
                             public void addAdministrativeGroups(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup param){
                                   if (localAdministrativeGroups == null){
                                   localAdministrativeGroups = new org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup[]{};
                                   }

                            
                                 //update the setting tracker
                                localAdministrativeGroupsTracker = true;
                            

                               java.util.List list =
                            org.apache.axis2.databinding.utils.ConverterUtil.toList(localAdministrativeGroups);
                               list.add(param);
                               this.localAdministrativeGroups =
                             (org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup[])list.toArray(
                            new org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup[list.size()]);

                             }
                             

                        /**
                        * field for SwitchingCapabilityDescriptors
                        */

                        
                            protected org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwcapContent localSwitchingCapabilityDescriptors ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localSwitchingCapabilityDescriptorsTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwcapContent
                           */
                           public  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwcapContent getSwitchingCapabilityDescriptors(){
                               return localSwitchingCapabilityDescriptors;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param SwitchingCapabilityDescriptors
                               */
                               public void setSwitchingCapabilityDescriptors(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwcapContent param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localSwitchingCapabilityDescriptorsTracker = true;
                                       } else {
                                          localSwitchingCapabilityDescriptorsTracker = false;
                                              
                                       }
                                   
                                            this.localSwitchingCapabilityDescriptors=param;
                                    

                               }
                            

                        /**
                        * field for Id
                        * This was an Attribute!
                        */

                        
                            protected java.lang.String localId ;
                        

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getId(){
                               return localId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Id
                               */
                               public void setId(java.lang.String param){
                            
                                            this.localId=param;
                                    

                               }
                            

     /**
     * isReaderMTOMAware
     * @return true if the reader supports MTOM
     */
   public static boolean isReaderMTOMAware(javax.xml.stream.XMLStreamReader reader) {
        boolean isReaderMTOMAware = false;
        
        try{
          isReaderMTOMAware = java.lang.Boolean.TRUE.equals(reader.getProperty(org.apache.axiom.om.OMConstants.IS_DATA_HANDLERS_AWARE));
        }catch(java.lang.IllegalArgumentException e){
          isReaderMTOMAware = false;
        }
        return isReaderMTOMAware;
   }
     
     
        /**
        *
        * @param parentQName
        * @param factory
        * @return org.apache.axiom.om.OMElement
        */
       public org.apache.axiom.om.OMElement getOMElement(
               final javax.xml.namespace.QName parentQName,
               final org.apache.axiom.om.OMFactory factory){


        
               org.apache.axiom.om.OMDataSource dataSource =
                       new org.apache.axis2.databinding.ADBDataSource(this,parentQName){

                 public void serialize(javax.xml.stream.XMLStreamWriter xmlWriter) throws javax.xml.stream.XMLStreamException {
                       CtrlPlaneLinkContent.this.serialize(parentQName,factory,xmlWriter);
                 }
               };
               return new org.apache.axiom.om.impl.llom.OMSourcedElementImpl(
               parentQName,factory,dataSource);
            
       }



         public void serialize(final javax.xml.namespace.QName parentQName,
                               final org.apache.axiom.om.OMFactory factory,
                               javax.xml.stream.XMLStreamWriter xmlWriter) throws javax.xml.stream.XMLStreamException {
            
                

                java.lang.String prefix = parentQName.getPrefix();
                java.lang.String namespace = parentQName.getNamespaceURI();

                if (namespace != null) {
                    java.lang.String writerPrefix = xmlWriter.getPrefix(namespace);
                    if (writerPrefix != null) {
                        xmlWriter.writeStartElement(namespace, parentQName.getLocalPart());
                    } else {
                        if (prefix == null) {
                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();
                        }

                        xmlWriter.writeStartElement(prefix, parentQName.getLocalPart(), namespace);
                        xmlWriter.writeNamespace(prefix, namespace);
                        xmlWriter.setPrefix(prefix, namespace);
                    }
                } else {
                    xmlWriter.writeStartElement(parentQName.getLocalPart());
                }

                
               
                                            if (localId != null){
                                        
                                        writeAttribute("",
                                                  "id",
                                                  org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localId), xmlWriter);
                                    }
                                    
                                      else {
                                          throw new RuntimeException("required attribute localId is null");
                                      }
                                     if (localLocalPortIdTracker){
                                    if (localLocalPortId==null){
                                         throw new RuntimeException("localPortId cannot be null!!");
                                    }
                                   localLocalPortId.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","localPortId"),
                                       factory,xmlWriter);
                                } if (localRemotePortIdTracker){
                                    if (localRemotePortId==null){
                                         throw new RuntimeException("remotePortId cannot be null!!");
                                    }
                                   localRemotePortId.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","remotePortId"),
                                       factory,xmlWriter);
                                } if (localRemoteNodeIdTracker){
                                    if (localRemoteNodeId==null){
                                         throw new RuntimeException("remoteNodeId cannot be null!!");
                                    }
                                   localRemoteNodeId.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","remoteNodeId"),
                                       factory,xmlWriter);
                                } if (localRemoteDomainIdTracker){
                                    if (localRemoteDomainId==null){
                                         throw new RuntimeException("remoteDomainId cannot be null!!");
                                    }
                                   localRemoteDomainId.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","remoteDomainId"),
                                       factory,xmlWriter);
                                } if (localTrafficEngineeringMetricTracker){
                                    namespace = "http://ogf.org/schema/network/topology/ctrlPlane/20070626/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"trafficEngineeringMetric", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"trafficEngineeringMetric");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("trafficEngineeringMetric");
                                    }
                                

                                          if (localTrafficEngineeringMetric==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("trafficEngineeringMetric cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localTrafficEngineeringMetric);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localLinkProtectionTypesTracker){
                                    namespace = "http://ogf.org/schema/network/topology/ctrlPlane/20070626/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"linkProtectionTypes", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"linkProtectionTypes");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("linkProtectionTypes");
                                    }
                                

                                          if (localLinkProtectionTypes==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("linkProtectionTypes cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localLinkProtectionTypes);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localCapacityTracker){
                                    namespace = "http://ogf.org/schema/network/topology/ctrlPlane/20070626/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"capacity", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"capacity");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("capacity");
                                    }
                                

                                          if (localCapacity==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("capacity cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localCapacity);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localMaximumReservableCapacityTracker){
                                    namespace = "http://ogf.org/schema/network/topology/ctrlPlane/20070626/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"maximumReservableCapacity", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"maximumReservableCapacity");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("maximumReservableCapacity");
                                    }
                                

                                          if (localMaximumReservableCapacity==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("maximumReservableCapacity cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localMaximumReservableCapacity);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localMinimumReservableCapacityTracker){
                                    namespace = "http://ogf.org/schema/network/topology/ctrlPlane/20070626/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"minimumReservableCapacity", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"minimumReservableCapacity");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("minimumReservableCapacity");
                                    }
                                

                                          if (localMinimumReservableCapacity==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("minimumReservableCapacity cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localMinimumReservableCapacity);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localGranularityTracker){
                                    namespace = "http://ogf.org/schema/network/topology/ctrlPlane/20070626/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"granularity", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"granularity");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("granularity");
                                    }
                                

                                          if (localGranularity==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("granularity cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localGranularity);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localUnreservedCapacityTracker){
                                    namespace = "http://ogf.org/schema/network/topology/ctrlPlane/20070626/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"unreservedCapacity", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"unreservedCapacity");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("unreservedCapacity");
                                    }
                                

                                          if (localUnreservedCapacity==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("unreservedCapacity cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localUnreservedCapacity);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localAdministrativeGroupsTracker){
                             if (localAdministrativeGroups!=null){
                                    for (int i = 0;i < localAdministrativeGroups.length;i++){
                                        if (localAdministrativeGroups[i] != null){
                                         localAdministrativeGroups[i].serialize(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","administrativeGroups"),
                                                   factory,xmlWriter);
                                        } else {
                                           
                                                // we don't have to do any thing since minOccures is zero
                                            
                                        }

                                    }
                             } else {
                                
                                       throw new RuntimeException("administrativeGroups cannot be null!!");
                                
                            }
                        } if (localSwitchingCapabilityDescriptorsTracker){
                                    if (localSwitchingCapabilityDescriptors==null){
                                         throw new RuntimeException("SwitchingCapabilityDescriptors cannot be null!!");
                                    }
                                   localSwitchingCapabilityDescriptors.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","SwitchingCapabilityDescriptors"),
                                       factory,xmlWriter);
                                }
                   
               xmlWriter.writeEndElement();
            
            

        }

         /**
          * Util method to write an attribute with the ns prefix
          */
          private void writeAttribute(java.lang.String prefix,java.lang.String namespace,java.lang.String attName,
                                      java.lang.String attValue,javax.xml.stream.XMLStreamWriter xmlWriter) throws javax.xml.stream.XMLStreamException{
              if (xmlWriter.getPrefix(namespace) == null) {
                       xmlWriter.writeNamespace(prefix, namespace);
                       xmlWriter.setPrefix(prefix, namespace);

              }

              xmlWriter.writeAttribute(namespace,attName,attValue);

         }

         /**
          * Util method to write an attribute without the ns prefix
          */
          private void writeAttribute(java.lang.String namespace,java.lang.String attName,
                                      java.lang.String attValue,javax.xml.stream.XMLStreamWriter xmlWriter) throws javax.xml.stream.XMLStreamException{
    	  	  if (namespace.equals(""))
        	  {
        		  xmlWriter.writeAttribute(attName,attValue);
        	  }
        	  else
        	  {
                  registerPrefix(xmlWriter, namespace);
                  xmlWriter.writeAttribute(namespace,attName,attValue);
              }
          }

        /**
         *  method to handle Qnames
         */

        private void writeQName(javax.xml.namespace.QName qname,
                                javax.xml.stream.XMLStreamWriter xmlWriter) throws javax.xml.stream.XMLStreamException {
            java.lang.String namespaceURI = qname.getNamespaceURI();
            if (namespaceURI != null) {
                java.lang.String prefix = xmlWriter.getPrefix(namespaceURI);
                if (prefix == null) {
                    prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();
                    xmlWriter.writeNamespace(prefix, namespaceURI);
                    xmlWriter.setPrefix(prefix,namespaceURI);
                }
                xmlWriter.writeCharacters(prefix + ":" + org.apache.axis2.databinding.utils.ConverterUtil.convertToString(qname));
            } else {
                xmlWriter.writeCharacters(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(qname));
            }
        }

        private void writeQNames(javax.xml.namespace.QName[] qnames,
                                 javax.xml.stream.XMLStreamWriter xmlWriter) throws javax.xml.stream.XMLStreamException {

            if (qnames != null) {
                // we have to store this data until last moment since it is not possible to write any
                // namespace data after writing the charactor data
                java.lang.StringBuffer stringToWrite = new java.lang.StringBuffer();
                java.lang.String namespaceURI = null;
                java.lang.String prefix = null;

                for (int i = 0; i < qnames.length; i++) {
                    if (i > 0) {
                        stringToWrite.append(" ");
                    }
                    namespaceURI = qnames[i].getNamespaceURI();
                    if (namespaceURI != null) {
                        prefix = xmlWriter.getPrefix(namespaceURI);
                        if ((prefix == null) || (prefix.length() == 0)) {
                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();
                            xmlWriter.writeNamespace(prefix, namespaceURI);
                            xmlWriter.setPrefix(prefix,namespaceURI);
                        }
                        stringToWrite.append(prefix).append(":").append(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(qnames[i]));
                    } else {
                        stringToWrite.append(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(qnames[i]));
                    }
                }
                xmlWriter.writeCharacters(stringToWrite.toString());
            }

        }


         /**
         * Register a namespace prefix
         */
         private java.lang.String registerPrefix(javax.xml.stream.XMLStreamWriter xmlWriter, java.lang.String namespace) throws javax.xml.stream.XMLStreamException {
                java.lang.String prefix = xmlWriter.getPrefix(namespace);

                if (prefix == null) {
                    prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                    while (xmlWriter.getNamespaceContext().getNamespaceURI(prefix) != null) {
                        prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();
                    }

                    xmlWriter.writeNamespace(prefix, namespace);
                    xmlWriter.setPrefix(prefix, namespace);
                }

                return prefix;
            }


  
        /**
        * databinding method to get an XML representation of this object
        *
        */
        public javax.xml.stream.XMLStreamReader getPullParser(javax.xml.namespace.QName qName){


        
                 java.util.ArrayList elementList = new java.util.ArrayList();
                 java.util.ArrayList attribList = new java.util.ArrayList();

                 if (localLocalPortIdTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "localPortId"));
                            
                            
                                    if (localLocalPortId==null){
                                         throw new RuntimeException("localPortId cannot be null!!");
                                    }
                                    elementList.add(localLocalPortId);
                                } if (localRemotePortIdTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "remotePortId"));
                            
                            
                                    if (localRemotePortId==null){
                                         throw new RuntimeException("remotePortId cannot be null!!");
                                    }
                                    elementList.add(localRemotePortId);
                                } if (localRemoteNodeIdTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "remoteNodeId"));
                            
                            
                                    if (localRemoteNodeId==null){
                                         throw new RuntimeException("remoteNodeId cannot be null!!");
                                    }
                                    elementList.add(localRemoteNodeId);
                                } if (localRemoteDomainIdTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "remoteDomainId"));
                            
                            
                                    if (localRemoteDomainId==null){
                                         throw new RuntimeException("remoteDomainId cannot be null!!");
                                    }
                                    elementList.add(localRemoteDomainId);
                                } if (localTrafficEngineeringMetricTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "trafficEngineeringMetric"));
                            
                                        if (localTrafficEngineeringMetric != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localTrafficEngineeringMetric));
                                        } else {
                                           throw new RuntimeException("trafficEngineeringMetric cannot be null!!");
                                        }
                                    } if (localLinkProtectionTypesTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "linkProtectionTypes"));
                            
                                        if (localLinkProtectionTypes != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLinkProtectionTypes));
                                        } else {
                                           throw new RuntimeException("linkProtectionTypes cannot be null!!");
                                        }
                                    } if (localCapacityTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "capacity"));
                            
                                        if (localCapacity != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localCapacity));
                                        } else {
                                           throw new RuntimeException("capacity cannot be null!!");
                                        }
                                    } if (localMaximumReservableCapacityTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "maximumReservableCapacity"));
                            
                                        if (localMaximumReservableCapacity != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localMaximumReservableCapacity));
                                        } else {
                                           throw new RuntimeException("maximumReservableCapacity cannot be null!!");
                                        }
                                    } if (localMinimumReservableCapacityTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "minimumReservableCapacity"));
                            
                                        if (localMinimumReservableCapacity != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localMinimumReservableCapacity));
                                        } else {
                                           throw new RuntimeException("minimumReservableCapacity cannot be null!!");
                                        }
                                    } if (localGranularityTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "granularity"));
                            
                                        if (localGranularity != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localGranularity));
                                        } else {
                                           throw new RuntimeException("granularity cannot be null!!");
                                        }
                                    } if (localUnreservedCapacityTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "unreservedCapacity"));
                            
                                        if (localUnreservedCapacity != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localUnreservedCapacity));
                                        } else {
                                           throw new RuntimeException("unreservedCapacity cannot be null!!");
                                        }
                                    } if (localAdministrativeGroupsTracker){
                             if (localAdministrativeGroups!=null) {
                                 for (int i = 0;i < localAdministrativeGroups.length;i++){

                                    if (localAdministrativeGroups[i] != null){
                                         elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                          "administrativeGroups"));
                                         elementList.add(localAdministrativeGroups[i]);
                                    } else {
                                        
                                                // nothing to do
                                            
                                    }

                                 }
                             } else {
                                 
                                        throw new RuntimeException("administrativeGroups cannot be null!!");
                                    
                             }

                        } if (localSwitchingCapabilityDescriptorsTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "SwitchingCapabilityDescriptors"));
                            
                            
                                    if (localSwitchingCapabilityDescriptors==null){
                                         throw new RuntimeException("SwitchingCapabilityDescriptors cannot be null!!");
                                    }
                                    elementList.add(localSwitchingCapabilityDescriptors);
                                }
                            attribList.add(
                            new javax.xml.namespace.QName("","id"));
                            
                                      attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localId));
                                

                return new org.apache.axis2.databinding.utils.reader.ADBXMLStreamReaderImpl(qName, elementList.toArray(), attribList.toArray());
            
            

        }

  

     /**
      *  Factory class that keeps the parse method
      */
    public static class Factory{

        
        

        /**
        * static method to create the object
        * Precondition:  If this object is an element, the current or next start element starts this object and any intervening reader events are ignorable
        *                If this object is not an element, it is a complex type and the reader is at the event just after the outer start element
        * Postcondition: If this object is an element, the reader is positioned at its end element
        *                If this object is a complex type, the reader is positioned at the end element of its outer element
        */
        public static CtrlPlaneLinkContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
            CtrlPlaneLinkContent object = new CtrlPlaneLinkContent();

            int event;
            java.lang.String nillableValue = null;
            java.lang.String prefix ="";
            java.lang.String namespaceuri ="";
            try {
                
                while (!reader.isStartElement() && !reader.isEndElement())
                    reader.next();

                
                if (reader.getAttributeValue("http://www.w3.org/2001/XMLSchema-instance","type")!=null){
                  java.lang.String fullTypeName = reader.getAttributeValue("http://www.w3.org/2001/XMLSchema-instance",
                        "type");
                  if (fullTypeName!=null){
                    java.lang.String nsPrefix = null;
                    if (fullTypeName.indexOf(":") > -1){
                        nsPrefix = fullTypeName.substring(0,fullTypeName.indexOf(":"));
                    }
                    nsPrefix = nsPrefix==null?"":nsPrefix;

                    java.lang.String type = fullTypeName.substring(fullTypeName.indexOf(":")+1);
                    
                            if (!"CtrlPlaneLinkContent".equals(type)){
                                //find namespace for the prefix
                                java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
                                return (CtrlPlaneLinkContent)net.es.oscars.oscars.ExtensionMapper.getTypeObject(
                                     nsUri,type,reader);
                              }
                        

                  }

                }
                

                
                // Note all attributes that were handled. Used to differ normal attributes
                // from anyAttributes.
                java.util.Vector handledAttributes = new java.util.Vector();
                

                 
                    // handle attribute "id"
                    java.lang.String tempAttribid =
                      reader.getAttributeValue("","id");
                   if (tempAttribid!=null){
                         java.lang.String content = tempAttribid;
                        
                                                 object.setId(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(tempAttribid));
                                            
                    } else {
                       
                               throw new RuntimeException("Required attribute id is missing");
                           
                    }
                    handledAttributes.add("id");
                    
                    
                    reader.next();
                
                        java.util.ArrayList list12 = new java.util.ArrayList();
                    
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","localPortId").equals(reader.getName())){
                                
                                        object.setLocalPortId(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","remotePortId").equals(reader.getName())){
                                
                                        object.setRemotePortId(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","remoteNodeId").equals(reader.getName())){
                                
                                        object.setRemoteNodeId(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","remoteDomainId").equals(reader.getName())){
                                
                                        object.setRemoteDomainId(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","trafficEngineeringMetric").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setTrafficEngineeringMetric(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","linkProtectionTypes").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setLinkProtectionTypes(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","capacity").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setCapacity(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","maximumReservableCapacity").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setMaximumReservableCapacity(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","minimumReservableCapacity").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setMinimumReservableCapacity(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","granularity").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setGranularity(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","unreservedCapacity").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setUnreservedCapacity(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","administrativeGroups").equals(reader.getName())){
                                
                                    
                                    
                                    // Process the array and step past its final element's end.
                                    list12.add(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup.Factory.parse(reader));
                                            
                                            //loop until we find a start element that is not part of this array
                                            boolean loopDone12 = false;
                                            while(!loopDone12){
                                                // We should be at the end element, but make sure
                                                while (!reader.isEndElement())
                                                    reader.next();
                                                // Step out of this element
                                                reader.next();
                                                // Step to next element event.
                                                while (!reader.isStartElement() && !reader.isEndElement())
                                                    reader.next();
                                                if (reader.isEndElement()){
                                                    //two continuous end elements means we are exiting the xml structure
                                                    loopDone12 = true;
                                                } else {
                                                    if (new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","administrativeGroups").equals(reader.getName())){
                                                        list12.add(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup.Factory.parse(reader));
                                                        
                                                    }else{
                                                        loopDone12 = true;
                                                    }
                                                }
                                            }
                                            // call the converter utility  to convert and set the array
                                            
                                            object.setAdministrativeGroups((org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup[])
                                                org.apache.axis2.databinding.utils.ConverterUtil.convertToArray(
                                                    org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup.class,
                                                    list12));
                                                
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","SwitchingCapabilityDescriptors").equals(reader.getName())){
                                
                                        object.setSwitchingCapabilityDescriptors(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwcapContent.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                  
                            while (!reader.isStartElement() && !reader.isEndElement())
                                reader.next();
                            if (reader.isStartElement())
                                // A start element we are not expecting indicates a trailing invalid property
                                throw new java.lang.RuntimeException("Unexpected subelement " + reader.getLocalName());
                        



            } catch (javax.xml.stream.XMLStreamException e) {
                throw new java.lang.Exception(e);
            }

            return object;
        }

        }//end of factory class

        

        }
           
          
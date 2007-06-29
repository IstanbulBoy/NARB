
            /**
            * CtrlPlaneLinkContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package org.ogf.schema.nm.topology.ctrlplane._20070611;
            /**
            *  CtrlPlaneLinkContent bean class
            */
        

        public  class CtrlPlaneLinkContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = CtrlPlaneLinkContent
                Namespace URI = http://ogf.org/schema/nm/topology/CtrlPlane/20070611/
                Namespace Prefix = ns5
                */
            
            /** Whenever a new property is set ensure all others are unset
             *  There can be only one choice and the last one wins
             */
            private void clearAllSettingTrackers() {
            
                   localLinklocalIdTracker = false;
                
                   localLinkremotenodeIdTracker = false;
                
                   localLinkremoteportIdTracker = false;
                
                   localLinkremotedomainIdTracker = false;
                
                   localTrafficengineeringmetricTracker = false;
                
                   localLinkProtectionTypesTracker = false;
                
                   localAdministrativeGroupsTracker = false;
                
                   localInterfaceSwitchingCapabilityDescriptorsTracker = false;
                
                   localIdTracker = false;
                
            }
        

                        /**
                        * field for LinklocalId
                        */

                        
                            protected org.ogf.schema.nm.topology.ctrlplane._20070611.LinklocalId_type16 localLinklocalId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinklocalIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.nm.topology.ctrlplane._20070611.LinklocalId_type16
                           */
                           public  org.ogf.schema.nm.topology.ctrlplane._20070611.LinklocalId_type16 getLinklocalId(){
                               return localLinklocalId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param LinklocalId
                               */
                               public void setLinklocalId(org.ogf.schema.nm.topology.ctrlplane._20070611.LinklocalId_type16 param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLinklocalIdTracker = true;
                                       } else {
                                          localLinklocalIdTracker = false;
                                              
                                       }
                                   
                                            this.localLinklocalId=param;
                                    

                               }
                            

                        /**
                        * field for LinkremotenodeId
                        */

                        
                            protected org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotenodeId_type17 localLinkremotenodeId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinkremotenodeIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotenodeId_type17
                           */
                           public  org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotenodeId_type17 getLinkremotenodeId(){
                               return localLinkremotenodeId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param LinkremotenodeId
                               */
                               public void setLinkremotenodeId(org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotenodeId_type17 param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLinkremotenodeIdTracker = true;
                                       } else {
                                          localLinkremotenodeIdTracker = false;
                                              
                                       }
                                   
                                            this.localLinkremotenodeId=param;
                                    

                               }
                            

                        /**
                        * field for LinkremoteportId
                        */

                        
                            protected org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremoteportId_type18 localLinkremoteportId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinkremoteportIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremoteportId_type18
                           */
                           public  org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremoteportId_type18 getLinkremoteportId(){
                               return localLinkremoteportId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param LinkremoteportId
                               */
                               public void setLinkremoteportId(org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremoteportId_type18 param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLinkremoteportIdTracker = true;
                                       } else {
                                          localLinkremoteportIdTracker = false;
                                              
                                       }
                                   
                                            this.localLinkremoteportId=param;
                                    

                               }
                            

                        /**
                        * field for LinkremotedomainId
                        */

                        
                            protected org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotedomainId_type19 localLinkremotedomainId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinkremotedomainIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotedomainId_type19
                           */
                           public  org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotedomainId_type19 getLinkremotedomainId(){
                               return localLinkremotedomainId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param LinkremotedomainId
                               */
                               public void setLinkremotedomainId(org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotedomainId_type19 param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLinkremotedomainIdTracker = true;
                                       } else {
                                          localLinkremotedomainIdTracker = false;
                                              
                                       }
                                   
                                            this.localLinkremotedomainId=param;
                                    

                               }
                            

                        /**
                        * field for Trafficengineeringmetric
                        */

                        
                            protected java.lang.String localTrafficengineeringmetric ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localTrafficengineeringmetricTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getTrafficengineeringmetric(){
                               return localTrafficengineeringmetric;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Trafficengineeringmetric
                               */
                               public void setTrafficengineeringmetric(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localTrafficengineeringmetricTracker = true;
                                       } else {
                                          localTrafficengineeringmetricTracker = false;
                                              
                                       }
                                   
                                            this.localTrafficengineeringmetric=param;
                                    

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
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLinkProtectionTypesTracker = true;
                                       } else {
                                          localLinkProtectionTypesTracker = false;
                                              
                                       }
                                   
                                            this.localLinkProtectionTypes=param;
                                    

                               }
                            

                        /**
                        * field for AdministrativeGroups
                        */

                        
                            protected org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneAdministrativeGroup localAdministrativeGroups ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localAdministrativeGroupsTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneAdministrativeGroup
                           */
                           public  org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneAdministrativeGroup getAdministrativeGroups(){
                               return localAdministrativeGroups;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param AdministrativeGroups
                               */
                               public void setAdministrativeGroups(org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneAdministrativeGroup param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localAdministrativeGroupsTracker = true;
                                       } else {
                                          localAdministrativeGroupsTracker = false;
                                              
                                       }
                                   
                                            this.localAdministrativeGroups=param;
                                    

                               }
                            

                        /**
                        * field for InterfaceSwitchingCapabilityDescriptors
                        */

                        
                            protected org.ogf.schema.nm.topology.ctrlplane._20070611.InterfaceSwitchingCapabilityDescriptors_type20 localInterfaceSwitchingCapabilityDescriptors ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localInterfaceSwitchingCapabilityDescriptorsTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.nm.topology.ctrlplane._20070611.InterfaceSwitchingCapabilityDescriptors_type20
                           */
                           public  org.ogf.schema.nm.topology.ctrlplane._20070611.InterfaceSwitchingCapabilityDescriptors_type20 getInterfaceSwitchingCapabilityDescriptors(){
                               return localInterfaceSwitchingCapabilityDescriptors;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param InterfaceSwitchingCapabilityDescriptors
                               */
                               public void setInterfaceSwitchingCapabilityDescriptors(org.ogf.schema.nm.topology.ctrlplane._20070611.InterfaceSwitchingCapabilityDescriptors_type20 param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localInterfaceSwitchingCapabilityDescriptorsTracker = true;
                                       } else {
                                          localInterfaceSwitchingCapabilityDescriptorsTracker = false;
                                              
                                       }
                                   
                                            this.localInterfaceSwitchingCapabilityDescriptors=param;
                                    

                               }
                            

                        /**
                        * field for Id
                        * This was an Attribute!
                        */

                        
                            protected java.lang.String localId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localIdTracker = false ;
                           

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
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localIdTracker = true;
                                       } else {
                                          localIdTracker = false;
                                              
                                       }
                                   
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
                                     if (localLinklocalIdTracker){
                                    if (localLinklocalId==null){
                                         throw new RuntimeException("linklocalId cannot be null!!");
                                    }
                                   localLinklocalId.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","linklocalId"),
                                       factory,xmlWriter);
                                } if (localLinkremotenodeIdTracker){
                                    if (localLinkremotenodeId==null){
                                         throw new RuntimeException("linkremotenodeId cannot be null!!");
                                    }
                                   localLinkremotenodeId.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","linkremotenodeId"),
                                       factory,xmlWriter);
                                } if (localLinkremoteportIdTracker){
                                    if (localLinkremoteportId==null){
                                         throw new RuntimeException("linkremoteportId cannot be null!!");
                                    }
                                   localLinkremoteportId.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","linkremoteportId"),
                                       factory,xmlWriter);
                                } if (localLinkremotedomainIdTracker){
                                    if (localLinkremotedomainId==null){
                                         throw new RuntimeException("linkremotedomainId cannot be null!!");
                                    }
                                   localLinkremotedomainId.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","linkremotedomainId"),
                                       factory,xmlWriter);
                                } if (localTrafficengineeringmetricTracker){
                                    namespace = "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"trafficengineeringmetric", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"trafficengineeringmetric");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("trafficengineeringmetric");
                                    }
                                

                                          if (localTrafficengineeringmetric==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("trafficengineeringmetric cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localTrafficengineeringmetric);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localLinkProtectionTypesTracker){
                                    namespace = "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/";
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
                             } if (localAdministrativeGroupsTracker){
                                    if (localAdministrativeGroups==null){
                                         throw new RuntimeException("administrativeGroups cannot be null!!");
                                    }
                                   localAdministrativeGroups.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","administrativeGroups"),
                                       factory,xmlWriter);
                                } if (localInterfaceSwitchingCapabilityDescriptorsTracker){
                                    if (localInterfaceSwitchingCapabilityDescriptors==null){
                                         throw new RuntimeException("interfaceSwitchingCapabilityDescriptors cannot be null!!");
                                    }
                                   localInterfaceSwitchingCapabilityDescriptors.serialize(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","interfaceSwitchingCapabilityDescriptors"),
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

                 if (localLinklocalIdTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/",
                                                                      "linklocalId"));
                            
                            
                                    if (localLinklocalId==null){
                                         throw new RuntimeException("linklocalId cannot be null!!");
                                    }
                                    elementList.add(localLinklocalId);
                                } if (localLinkremotenodeIdTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/",
                                                                      "linkremotenodeId"));
                            
                            
                                    if (localLinkremotenodeId==null){
                                         throw new RuntimeException("linkremotenodeId cannot be null!!");
                                    }
                                    elementList.add(localLinkremotenodeId);
                                } if (localLinkremoteportIdTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/",
                                                                      "linkremoteportId"));
                            
                            
                                    if (localLinkremoteportId==null){
                                         throw new RuntimeException("linkremoteportId cannot be null!!");
                                    }
                                    elementList.add(localLinkremoteportId);
                                } if (localLinkremotedomainIdTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/",
                                                                      "linkremotedomainId"));
                            
                            
                                    if (localLinkremotedomainId==null){
                                         throw new RuntimeException("linkremotedomainId cannot be null!!");
                                    }
                                    elementList.add(localLinkremotedomainId);
                                } if (localTrafficengineeringmetricTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/",
                                                                      "trafficengineeringmetric"));
                            
                                        if (localTrafficengineeringmetric != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localTrafficengineeringmetric));
                                        } else {
                                           throw new RuntimeException("trafficengineeringmetric cannot be null!!");
                                        }
                                    } if (localLinkProtectionTypesTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/",
                                                                      "linkProtectionTypes"));
                            
                                        if (localLinkProtectionTypes != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLinkProtectionTypes));
                                        } else {
                                           throw new RuntimeException("linkProtectionTypes cannot be null!!");
                                        }
                                    } if (localAdministrativeGroupsTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/",
                                                                      "administrativeGroups"));
                            
                            
                                    if (localAdministrativeGroups==null){
                                         throw new RuntimeException("administrativeGroups cannot be null!!");
                                    }
                                    elementList.add(localAdministrativeGroups);
                                } if (localInterfaceSwitchingCapabilityDescriptorsTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/",
                                                                      "interfaceSwitchingCapabilityDescriptors"));
                            
                            
                                    if (localInterfaceSwitchingCapabilityDescriptors==null){
                                         throw new RuntimeException("interfaceSwitchingCapabilityDescriptors cannot be null!!");
                                    }
                                    elementList.add(localInterfaceSwitchingCapabilityDescriptors);
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
                                return (CtrlPlaneLinkContent)edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(
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
                   
                while(!reader.isEndElement()) {
                    if (reader.isStartElement() ){
                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","linklocalId").equals(reader.getName())){
                                
                                        object.setLinklocalId(org.ogf.schema.nm.topology.ctrlplane._20070611.LinklocalId_type16.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","linkremotenodeId").equals(reader.getName())){
                                
                                        object.setLinkremotenodeId(org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotenodeId_type17.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","linkremoteportId").equals(reader.getName())){
                                
                                        object.setLinkremoteportId(org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremoteportId_type18.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","linkremotedomainId").equals(reader.getName())){
                                
                                        object.setLinkremotedomainId(org.ogf.schema.nm.topology.ctrlplane._20070611.LinkremotedomainId_type19.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","trafficengineeringmetric").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setTrafficengineeringmetric(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","linkProtectionTypes").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setLinkProtectionTypes(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","administrativeGroups").equals(reader.getName())){
                                
                                        object.setAdministrativeGroups(org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneAdministrativeGroup.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/nm/topology/CtrlPlane/20070611/","interfaceSwitchingCapabilityDescriptors").equals(reader.getName())){
                                
                                        object.setInterfaceSwitchingCapabilityDescriptors(org.ogf.schema.nm.topology.ctrlplane._20070611.InterfaceSwitchingCapabilityDescriptors_type20.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                             else{
                                        // A start element we are not expecting indicates an invalid parameter was passed
                                        throw new java.lang.RuntimeException("Unexpected subelement " + reader.getLocalName());
                             }
                          
                             } else reader.next();  
                            }  // end of while loop
                        



            } catch (javax.xml.stream.XMLStreamException e) {
                throw new java.lang.Exception(e);
            }

            return object;
        }

        }//end of factory class

        

        }
           
          
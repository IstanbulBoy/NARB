
            /**
            * BaseInterfaceContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package org.ggf.ns.nmwg.topology.base._3_0;
            /**
            *  BaseInterfaceContent bean class
            */
        

        public  class BaseInterfaceContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = BaseInterfaceContent
                Namespace URI = http://ggf.org/ns/nmwg/topology/base/3.0/
                Namespace Prefix = ns3
                */
            
            /** Whenever a new property is set ensure all others are unset
             *  There can be only one choice and the last one wins
             */
            private void clearAllSettingTrackers() {
            
                   localTypeTracker = false;
                
                   localHostNameTracker = false;
                
                   localIfNameTracker = false;
                
                   localIfDescriptionTracker = false;
                
                   localIfIndexTracker = false;
                
                   localCapacityTracker = false;
                
                   localNameTracker = false;
                
                   localInterfaceIdRefTracker = false;
                
                   localNodeIdRefTracker = false;
                
            }
        

                        /**
                        * field for Type
                        */

                        
                            protected java.lang.String localType ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localTypeTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getType(){
                               return localType;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Type
                               */
                               public void setType(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localTypeTracker = true;
                                       } else {
                                          localTypeTracker = false;
                                              
                                       }
                                   
                                            this.localType=param;
                                    

                               }
                            

                        /**
                        * field for HostName
                        */

                        
                            protected java.lang.String localHostName ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localHostNameTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getHostName(){
                               return localHostName;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param HostName
                               */
                               public void setHostName(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localHostNameTracker = true;
                                       } else {
                                          localHostNameTracker = false;
                                              
                                       }
                                   
                                            this.localHostName=param;
                                    

                               }
                            

                        /**
                        * field for IfName
                        */

                        
                            protected java.lang.String localIfName ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localIfNameTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getIfName(){
                               return localIfName;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param IfName
                               */
                               public void setIfName(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localIfNameTracker = true;
                                       } else {
                                          localIfNameTracker = false;
                                              
                                       }
                                   
                                            this.localIfName=param;
                                    

                               }
                            

                        /**
                        * field for IfDescription
                        */

                        
                            protected java.lang.String localIfDescription ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localIfDescriptionTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getIfDescription(){
                               return localIfDescription;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param IfDescription
                               */
                               public void setIfDescription(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localIfDescriptionTracker = true;
                                       } else {
                                          localIfDescriptionTracker = false;
                                              
                                       }
                                   
                                            this.localIfDescription=param;
                                    

                               }
                            

                        /**
                        * field for IfIndex
                        */

                        
                            protected java.lang.String localIfIndex ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localIfIndexTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getIfIndex(){
                               return localIfIndex;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param IfIndex
                               */
                               public void setIfIndex(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localIfIndexTracker = true;
                                       } else {
                                          localIfIndexTracker = false;
                                              
                                       }
                                   
                                            this.localIfIndex=param;
                                    

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
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localCapacityTracker = true;
                                       } else {
                                          localCapacityTracker = false;
                                              
                                       }
                                   
                                            this.localCapacity=param;
                                    

                               }
                            

                        /**
                        * field for Name
                        */

                        
                            protected org.ggf.ns.nmwg.topology.base._3_0.Name_type6 localName ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localNameTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ggf.ns.nmwg.topology.base._3_0.Name_type6
                           */
                           public  org.ggf.ns.nmwg.topology.base._3_0.Name_type6 getName(){
                               return localName;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Name
                               */
                               public void setName(org.ggf.ns.nmwg.topology.base._3_0.Name_type6 param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localNameTracker = true;
                                       } else {
                                          localNameTracker = false;
                                              
                                       }
                                   
                                            this.localName=param;
                                    

                               }
                            

                        /**
                        * field for InterfaceIdRef
                        * This was an Attribute!
                        */

                        
                            protected java.lang.String localInterfaceIdRef ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localInterfaceIdRefTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getInterfaceIdRef(){
                               return localInterfaceIdRef;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param InterfaceIdRef
                               */
                               public void setInterfaceIdRef(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localInterfaceIdRefTracker = true;
                                       } else {
                                          localInterfaceIdRefTracker = false;
                                              
                                       }
                                   
                                            this.localInterfaceIdRef=param;
                                    

                               }
                            

                        /**
                        * field for NodeIdRef
                        * This was an Attribute!
                        */

                        
                            protected java.lang.String localNodeIdRef ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localNodeIdRefTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getNodeIdRef(){
                               return localNodeIdRef;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param NodeIdRef
                               */
                               public void setNodeIdRef(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localNodeIdRefTracker = true;
                                       } else {
                                          localNodeIdRefTracker = false;
                                              
                                       }
                                   
                                            this.localNodeIdRef=param;
                                    

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
                       BaseInterfaceContent.this.serialize(parentQName,factory,xmlWriter);
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

                
               
                                            if (localInterfaceIdRef != null){
                                        
                                        writeAttribute("",
                                                  "interfaceIdRef",
                                                  org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localInterfaceIdRef), xmlWriter);
                                    }
                                    
                                            if (localNodeIdRef != null){
                                        
                                        writeAttribute("",
                                                  "nodeIdRef",
                                                  org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localNodeIdRef), xmlWriter);
                                    }
                                     if (localTypeTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"type", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"type");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("type");
                                    }
                                

                                          if (localType==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("type cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localType);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localHostNameTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"hostName", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"hostName");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("hostName");
                                    }
                                

                                          if (localHostName==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("hostName cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localHostName);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localIfNameTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"ifName", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"ifName");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("ifName");
                                    }
                                

                                          if (localIfName==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("ifName cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localIfName);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localIfDescriptionTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"ifDescription", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"ifDescription");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("ifDescription");
                                    }
                                

                                          if (localIfDescription==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("ifDescription cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localIfDescription);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localIfIndexTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"ifIndex", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"ifIndex");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("ifIndex");
                                    }
                                

                                          if (localIfIndex==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("ifIndex cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localIfIndex);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localCapacityTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
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
                             } if (localNameTracker){
                                    if (localName==null){
                                         throw new RuntimeException("name cannot be null!!");
                                    }
                                   localName.serialize(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","name"),
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

                 if (localTypeTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "type"));
                            
                                        if (localType != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localType));
                                        } else {
                                           throw new RuntimeException("type cannot be null!!");
                                        }
                                    } if (localHostNameTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "hostName"));
                            
                                        if (localHostName != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localHostName));
                                        } else {
                                           throw new RuntimeException("hostName cannot be null!!");
                                        }
                                    } if (localIfNameTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "ifName"));
                            
                                        if (localIfName != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localIfName));
                                        } else {
                                           throw new RuntimeException("ifName cannot be null!!");
                                        }
                                    } if (localIfDescriptionTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "ifDescription"));
                            
                                        if (localIfDescription != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localIfDescription));
                                        } else {
                                           throw new RuntimeException("ifDescription cannot be null!!");
                                        }
                                    } if (localIfIndexTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "ifIndex"));
                            
                                        if (localIfIndex != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localIfIndex));
                                        } else {
                                           throw new RuntimeException("ifIndex cannot be null!!");
                                        }
                                    } if (localCapacityTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "capacity"));
                            
                                        if (localCapacity != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localCapacity));
                                        } else {
                                           throw new RuntimeException("capacity cannot be null!!");
                                        }
                                    } if (localNameTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "name"));
                            
                            
                                    if (localName==null){
                                         throw new RuntimeException("name cannot be null!!");
                                    }
                                    elementList.add(localName);
                                }
                            attribList.add(
                            new javax.xml.namespace.QName("","interfaceIdRef"));
                            
                                      attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localInterfaceIdRef));
                                
                            attribList.add(
                            new javax.xml.namespace.QName("","nodeIdRef"));
                            
                                      attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localNodeIdRef));
                                

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
        public static BaseInterfaceContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
            BaseInterfaceContent object = new BaseInterfaceContent();

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
                    
                            if (!"BaseInterfaceContent".equals(type)){
                                //find namespace for the prefix
                                java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
                                return (BaseInterfaceContent)edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(
                                     nsUri,type,reader);
                              }
                        

                  }

                }
                

                
                // Note all attributes that were handled. Used to differ normal attributes
                // from anyAttributes.
                java.util.Vector handledAttributes = new java.util.Vector();
                

                 
                    // handle attribute "interfaceIdRef"
                    java.lang.String tempAttribinterfaceIdRef =
                      reader.getAttributeValue("","interfaceIdRef");
                   if (tempAttribinterfaceIdRef!=null){
                         java.lang.String content = tempAttribinterfaceIdRef;
                        
                                                 object.setInterfaceIdRef(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(tempAttribinterfaceIdRef));
                                            
                    } else {
                       
                    }
                    handledAttributes.add("interfaceIdRef");
                    
                    // handle attribute "nodeIdRef"
                    java.lang.String tempAttribnodeIdRef =
                      reader.getAttributeValue("","nodeIdRef");
                   if (tempAttribnodeIdRef!=null){
                         java.lang.String content = tempAttribnodeIdRef;
                        
                                                 object.setNodeIdRef(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(tempAttribnodeIdRef));
                                            
                    } else {
                       
                    }
                    handledAttributes.add("nodeIdRef");
                    
                    
                    reader.next();
                   
                while(!reader.isEndElement()) {
                    if (reader.isStartElement() ){
                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","type").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setType(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","hostName").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setHostName(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","ifName").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setIfName(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","ifDescription").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setIfDescription(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","ifIndex").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setIfIndex(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","capacity").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setCapacity(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","name").equals(reader.getName())){
                                
                                        object.setName(org.ggf.ns.nmwg.topology.base._3_0.Name_type6.Factory.parse(reader));
                                      
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
           
          
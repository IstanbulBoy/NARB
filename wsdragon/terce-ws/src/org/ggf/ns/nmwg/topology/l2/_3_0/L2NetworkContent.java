
            /**
            * L2NetworkContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package org.ggf.ns.nmwg.topology.l2._3_0;
            /**
            *  L2NetworkContent bean class
            */
        

        public  class L2NetworkContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = L2NetworkContent
                Namespace URI = http://ggf.org/ns/nmwg/topology/l2/3.0/
                Namespace Prefix = ns4
                */
            
            /** Whenever a new property is set ensure all others are unset
             *  There can be only one choice and the last one wins
             */
            private void clearAllSettingTrackers() {
            
                   localNameTracker = false;
                
                   localTypeTracker = false;
                
                   localVlanTracker = false;
                
                   local_interfaceTracker = false;
                
                   localLinkTracker = false;
                
                   localNodeTracker = false;
                
                   localNetworkIdRefTracker = false;
                
            }
        

                        /**
                        * field for Name
                        */

                        
                            protected org.ggf.ns.nmwg.topology.l2._3_0.Name_type7 localName ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localNameTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ggf.ns.nmwg.topology.l2._3_0.Name_type7
                           */
                           public  org.ggf.ns.nmwg.topology.l2._3_0.Name_type7 getName(){
                               return localName;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Name
                               */
                               public void setName(org.ggf.ns.nmwg.topology.l2._3_0.Name_type7 param){
                            
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
                        * field for Vlan
                        */

                        
                            protected java.lang.String localVlan ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localVlanTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getVlan(){
                               return localVlan;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Vlan
                               */
                               public void setVlan(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localVlanTracker = true;
                                       } else {
                                          localVlanTracker = false;
                                              
                                       }
                                   
                                            this.localVlan=param;
                                    

                               }
                            

                        /**
                        * field for _interface
                        */

                        
                            protected org.ggf.ns.nmwg.topology.l2._3_0.L2InterfaceContent local_interface ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean local_interfaceTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ggf.ns.nmwg.topology.l2._3_0.L2InterfaceContent
                           */
                           public  org.ggf.ns.nmwg.topology.l2._3_0.L2InterfaceContent get_interface(){
                               return local_interface;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param _interface
                               */
                               public void set_interface(org.ggf.ns.nmwg.topology.l2._3_0.L2InterfaceContent param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          local_interfaceTracker = true;
                                       } else {
                                          local_interfaceTracker = false;
                                              
                                       }
                                   
                                            this.local_interface=param;
                                    

                               }
                            

                        /**
                        * field for Link
                        */

                        
                            protected org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent localLink ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinkTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent
                           */
                           public  org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent getLink(){
                               return localLink;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Link
                               */
                               public void setLink(org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLinkTracker = true;
                                       } else {
                                          localLinkTracker = false;
                                              
                                       }
                                   
                                            this.localLink=param;
                                    

                               }
                            

                        /**
                        * field for Node
                        */

                        
                            protected org.ggf.ns.nmwg.topology.base._3_0.NodeContent localNode ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localNodeTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ggf.ns.nmwg.topology.base._3_0.NodeContent
                           */
                           public  org.ggf.ns.nmwg.topology.base._3_0.NodeContent getNode(){
                               return localNode;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Node
                               */
                               public void setNode(org.ggf.ns.nmwg.topology.base._3_0.NodeContent param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localNodeTracker = true;
                                       } else {
                                          localNodeTracker = false;
                                              
                                       }
                                   
                                            this.localNode=param;
                                    

                               }
                            

                        /**
                        * field for NetworkIdRef
                        * This was an Attribute!
                        */

                        
                            protected java.lang.String localNetworkIdRef ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localNetworkIdRefTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getNetworkIdRef(){
                               return localNetworkIdRef;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param NetworkIdRef
                               */
                               public void setNetworkIdRef(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localNetworkIdRefTracker = true;
                                       } else {
                                          localNetworkIdRefTracker = false;
                                              
                                       }
                                   
                                            this.localNetworkIdRef=param;
                                    

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
                       L2NetworkContent.this.serialize(parentQName,factory,xmlWriter);
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

                
               
                                            if (localNetworkIdRef != null){
                                        
                                        writeAttribute("",
                                                  "networkIdRef",
                                                  org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localNetworkIdRef), xmlWriter);
                                    }
                                     if (localNameTracker){
                                    if (localName==null){
                                         throw new RuntimeException("name cannot be null!!");
                                    }
                                   localName.serialize(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","name"),
                                       factory,xmlWriter);
                                } if (localTypeTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/l2/3.0/";
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
                             } if (localVlanTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/l2/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"vlan", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"vlan");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("vlan");
                                    }
                                

                                          if (localVlan==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("vlan cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localVlan);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (local_interfaceTracker){
                                    if (local_interface==null){
                                         throw new RuntimeException("interface cannot be null!!");
                                    }
                                   local_interface.serialize(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","interface"),
                                       factory,xmlWriter);
                                } if (localLinkTracker){
                                    if (localLink==null){
                                         throw new RuntimeException("link cannot be null!!");
                                    }
                                   localLink.serialize(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","link"),
                                       factory,xmlWriter);
                                } if (localNodeTracker){
                                    if (localNode==null){
                                         throw new RuntimeException("node cannot be null!!");
                                    }
                                   localNode.serialize(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","node"),
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

                 if (localNameTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/",
                                                                      "name"));
                            
                            
                                    if (localName==null){
                                         throw new RuntimeException("name cannot be null!!");
                                    }
                                    elementList.add(localName);
                                } if (localTypeTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/",
                                                                      "type"));
                            
                                        if (localType != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localType));
                                        } else {
                                           throw new RuntimeException("type cannot be null!!");
                                        }
                                    } if (localVlanTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/",
                                                                      "vlan"));
                            
                                        if (localVlan != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localVlan));
                                        } else {
                                           throw new RuntimeException("vlan cannot be null!!");
                                        }
                                    } if (local_interfaceTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/",
                                                                      "interface"));
                            
                            
                                    if (local_interface==null){
                                         throw new RuntimeException("interface cannot be null!!");
                                    }
                                    elementList.add(local_interface);
                                } if (localLinkTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/",
                                                                      "link"));
                            
                            
                                    if (localLink==null){
                                         throw new RuntimeException("link cannot be null!!");
                                    }
                                    elementList.add(localLink);
                                } if (localNodeTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "node"));
                            
                            
                                    if (localNode==null){
                                         throw new RuntimeException("node cannot be null!!");
                                    }
                                    elementList.add(localNode);
                                }
                            attribList.add(
                            new javax.xml.namespace.QName("","networkIdRef"));
                            
                                      attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localNetworkIdRef));
                                

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
        public static L2NetworkContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
            L2NetworkContent object = new L2NetworkContent();

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
                    
                            if (!"L2NetworkContent".equals(type)){
                                //find namespace for the prefix
                                java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
                                return (L2NetworkContent)edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(
                                     nsUri,type,reader);
                              }
                        

                  }

                }
                

                
                // Note all attributes that were handled. Used to differ normal attributes
                // from anyAttributes.
                java.util.Vector handledAttributes = new java.util.Vector();
                

                 
                    // handle attribute "networkIdRef"
                    java.lang.String tempAttribnetworkIdRef =
                      reader.getAttributeValue("","networkIdRef");
                   if (tempAttribnetworkIdRef!=null){
                         java.lang.String content = tempAttribnetworkIdRef;
                        
                                                 object.setNetworkIdRef(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(tempAttribnetworkIdRef));
                                            
                    } else {
                       
                    }
                    handledAttributes.add("networkIdRef");
                    
                    
                    reader.next();
                   
                while(!reader.isEndElement()) {
                    if (reader.isStartElement() ){
                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","name").equals(reader.getName())){
                                
                                        object.setName(org.ggf.ns.nmwg.topology.l2._3_0.Name_type7.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","type").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setType(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","vlan").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setVlan(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","interface").equals(reader.getName())){
                                
                                        object.set_interface(org.ggf.ns.nmwg.topology.l2._3_0.L2InterfaceContent.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","link").equals(reader.getName())){
                                
                                        object.setLink(org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","node").equals(reader.getName())){
                                
                                        object.setNode(org.ggf.ns.nmwg.topology.base._3_0.NodeContent.Factory.parse(reader));
                                      
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
           
          
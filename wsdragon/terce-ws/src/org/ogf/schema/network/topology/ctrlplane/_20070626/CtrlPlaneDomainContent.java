
            /**
            * CtrlPlaneDomainContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package org.ogf.schema.network.topology.ctrlplane._20070626;
            /**
            *  CtrlPlaneDomainContent bean class
            */
        

        public  class CtrlPlaneDomainContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = CtrlPlaneDomainContent
                Namespace URI = http://ogf.org/schema/network/topology/ctrlPlane/20070626/
                Namespace Prefix = ns1
                */
            

                        /**
                        * field for DomainId
                        */

                        
                            protected java.lang.String localDomainId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localDomainIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getDomainId(){
                               return localDomainId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param DomainId
                               */
                               public void setDomainId(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localDomainIdTracker = true;
                                       } else {
                                          localDomainIdTracker = false;
                                              
                                       }
                                   
                                            this.localDomainId=param;
                                    

                               }
                            

                        /**
                        * field for Node
                        * This was an Array!
                        */

                        
                            protected org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent[] localNode ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localNodeTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent[]
                           */
                           public  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent[] getNode(){
                               return localNode;
                           }

                           
                        


                               
                              /**
                               * validate the array for Node
                               */
                              protected void validateNode(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent[] param){
                             
                              }


                             /**
                              * Auto generated setter method
                              * @param param Node
                              */
                              public void setNode(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent[] param){
                              
                                   validateNode(param);

                               
                                          if (param != null){
                                             //update the setting tracker
                                             localNodeTracker = true;
                                          } else {
                                             localNodeTracker = false;
                                                 
                                          }
                                      
                                      this.localNode=param;
                              }

                               
                             
                             /**
                             * Auto generated add method for the array for convenience
                             * @param param org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent
                             */
                             public void addNode(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent param){
                                   if (localNode == null){
                                   localNode = new org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent[]{};
                                   }

                            
                                 //update the setting tracker
                                localNodeTracker = true;
                            

                               java.util.List list =
                            org.apache.axis2.databinding.utils.ConverterUtil.toList(localNode);
                               list.add(param);
                               this.localNode =
                             (org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent[])list.toArray(
                            new org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent[list.size()]);

                             }
                             

                        /**
                        * field for Port
                        * This was an Array!
                        */

                        
                            protected org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent[] localPort ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localPortTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent[]
                           */
                           public  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent[] getPort(){
                               return localPort;
                           }

                           
                        


                               
                              /**
                               * validate the array for Port
                               */
                              protected void validatePort(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent[] param){
                             
                              }


                             /**
                              * Auto generated setter method
                              * @param param Port
                              */
                              public void setPort(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent[] param){
                              
                                   validatePort(param);

                               
                                          if (param != null){
                                             //update the setting tracker
                                             localPortTracker = true;
                                          } else {
                                             localPortTracker = false;
                                                 
                                          }
                                      
                                      this.localPort=param;
                              }

                               
                             
                             /**
                             * Auto generated add method for the array for convenience
                             * @param param org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent
                             */
                             public void addPort(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent param){
                                   if (localPort == null){
                                   localPort = new org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent[]{};
                                   }

                            
                                 //update the setting tracker
                                localPortTracker = true;
                            

                               java.util.List list =
                            org.apache.axis2.databinding.utils.ConverterUtil.toList(localPort);
                               list.add(param);
                               this.localPort =
                             (org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent[])list.toArray(
                            new org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent[list.size()]);

                             }
                             

                        /**
                        * field for Link
                        * This was an Array!
                        */

                        
                            protected org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent[] localLink ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinkTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent[]
                           */
                           public  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent[] getLink(){
                               return localLink;
                           }

                           
                        


                               
                              /**
                               * validate the array for Link
                               */
                              protected void validateLink(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent[] param){
                             
                              }


                             /**
                              * Auto generated setter method
                              * @param param Link
                              */
                              public void setLink(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent[] param){
                              
                                   validateLink(param);

                               
                                          if (param != null){
                                             //update the setting tracker
                                             localLinkTracker = true;
                                          } else {
                                             localLinkTracker = false;
                                                 
                                          }
                                      
                                      this.localLink=param;
                              }

                               
                             
                             /**
                             * Auto generated add method for the array for convenience
                             * @param param org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent
                             */
                             public void addLink(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent param){
                                   if (localLink == null){
                                   localLink = new org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent[]{};
                                   }

                            
                                 //update the setting tracker
                                localLinkTracker = true;
                            

                               java.util.List list =
                            org.apache.axis2.databinding.utils.ConverterUtil.toList(localLink);
                               list.add(param);
                               this.localLink =
                             (org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent[])list.toArray(
                            new org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent[list.size()]);

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
                       CtrlPlaneDomainContent.this.serialize(parentQName,factory,xmlWriter);
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
                                     if (localDomainIdTracker){
                                    namespace = "http://ogf.org/schema/network/topology/ctrlPlane/20070626/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"domainId", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"domainId");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("domainId");
                                    }
                                

                                          if (localDomainId==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("domainId cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localDomainId);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localNodeTracker){
                             if (localNode!=null){
                                    for (int i = 0;i < localNode.length;i++){
                                        if (localNode[i] != null){
                                         localNode[i].serialize(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","node"),
                                                   factory,xmlWriter);
                                        } else {
                                           
                                                // we don't have to do any thing since minOccures is zero
                                            
                                        }

                                    }
                             } else {
                                
                                       throw new RuntimeException("node cannot be null!!");
                                
                            }
                        } if (localPortTracker){
                             if (localPort!=null){
                                    for (int i = 0;i < localPort.length;i++){
                                        if (localPort[i] != null){
                                         localPort[i].serialize(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","port"),
                                                   factory,xmlWriter);
                                        } else {
                                           
                                                // we don't have to do any thing since minOccures is zero
                                            
                                        }

                                    }
                             } else {
                                
                                       throw new RuntimeException("port cannot be null!!");
                                
                            }
                        } if (localLinkTracker){
                             if (localLink!=null){
                                    for (int i = 0;i < localLink.length;i++){
                                        if (localLink[i] != null){
                                         localLink[i].serialize(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","link"),
                                                   factory,xmlWriter);
                                        } else {
                                           
                                                // we don't have to do any thing since minOccures is zero
                                            
                                        }

                                    }
                             } else {
                                
                                       throw new RuntimeException("link cannot be null!!");
                                
                            }
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

                 if (localDomainIdTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                      "domainId"));
                            
                                        if (localDomainId != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localDomainId));
                                        } else {
                                           throw new RuntimeException("domainId cannot be null!!");
                                        }
                                    } if (localNodeTracker){
                             if (localNode!=null) {
                                 for (int i = 0;i < localNode.length;i++){

                                    if (localNode[i] != null){
                                         elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                          "node"));
                                         elementList.add(localNode[i]);
                                    } else {
                                        
                                                // nothing to do
                                            
                                    }

                                 }
                             } else {
                                 
                                        throw new RuntimeException("node cannot be null!!");
                                    
                             }

                        } if (localPortTracker){
                             if (localPort!=null) {
                                 for (int i = 0;i < localPort.length;i++){

                                    if (localPort[i] != null){
                                         elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                          "port"));
                                         elementList.add(localPort[i]);
                                    } else {
                                        
                                                // nothing to do
                                            
                                    }

                                 }
                             } else {
                                 
                                        throw new RuntimeException("port cannot be null!!");
                                    
                             }

                        } if (localLinkTracker){
                             if (localLink!=null) {
                                 for (int i = 0;i < localLink.length;i++){

                                    if (localLink[i] != null){
                                         elementList.add(new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/",
                                                                          "link"));
                                         elementList.add(localLink[i]);
                                    } else {
                                        
                                                // nothing to do
                                            
                                    }

                                 }
                             } else {
                                 
                                        throw new RuntimeException("link cannot be null!!");
                                    
                             }

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
        public static CtrlPlaneDomainContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
            CtrlPlaneDomainContent object = new CtrlPlaneDomainContent();

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
                    
                            if (!"CtrlPlaneDomainContent".equals(type)){
                                //find namespace for the prefix
                                java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
                                return (CtrlPlaneDomainContent)edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(
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
                
                        java.util.ArrayList list2 = new java.util.ArrayList();
                    
                        java.util.ArrayList list3 = new java.util.ArrayList();
                    
                        java.util.ArrayList list4 = new java.util.ArrayList();
                    
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","domainId").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setDomainId(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","node").equals(reader.getName())){
                                
                                    
                                    
                                    // Process the array and step past its final element's end.
                                    list2.add(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent.Factory.parse(reader));
                                            
                                            //loop until we find a start element that is not part of this array
                                            boolean loopDone2 = false;
                                            while(!loopDone2){
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
                                                    loopDone2 = true;
                                                } else {
                                                    if (new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","node").equals(reader.getName())){
                                                        list2.add(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent.Factory.parse(reader));
                                                        
                                                    }else{
                                                        loopDone2 = true;
                                                    }
                                                }
                                            }
                                            // call the converter utility  to convert and set the array
                                            
                                            object.setNode((org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent[])
                                                org.apache.axis2.databinding.utils.ConverterUtil.convertToArray(
                                                    org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent.class,
                                                    list2));
                                                
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","port").equals(reader.getName())){
                                
                                    
                                    
                                    // Process the array and step past its final element's end.
                                    list3.add(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent.Factory.parse(reader));
                                            
                                            //loop until we find a start element that is not part of this array
                                            boolean loopDone3 = false;
                                            while(!loopDone3){
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
                                                    loopDone3 = true;
                                                } else {
                                                    if (new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","port").equals(reader.getName())){
                                                        list3.add(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent.Factory.parse(reader));
                                                        
                                                    }else{
                                                        loopDone3 = true;
                                                    }
                                                }
                                            }
                                            // call the converter utility  to convert and set the array
                                            
                                            object.setPort((org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent[])
                                                org.apache.axis2.databinding.utils.ConverterUtil.convertToArray(
                                                    org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent.class,
                                                    list3));
                                                
                              }  // End of if for expected property start element
                                
                                    else {
                                        
                                    }
                                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","link").equals(reader.getName())){
                                
                                    
                                    
                                    // Process the array and step past its final element's end.
                                    list4.add(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent.Factory.parse(reader));
                                            
                                            //loop until we find a start element that is not part of this array
                                            boolean loopDone4 = false;
                                            while(!loopDone4){
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
                                                    loopDone4 = true;
                                                } else {
                                                    if (new javax.xml.namespace.QName("http://ogf.org/schema/network/topology/ctrlPlane/20070626/","link").equals(reader.getName())){
                                                        list4.add(org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent.Factory.parse(reader));
                                                        
                                                    }else{
                                                        loopDone4 = true;
                                                    }
                                                }
                                            }
                                            // call the converter utility  to convert and set the array
                                            
                                            object.setLink((org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent[])
                                                org.apache.axis2.databinding.utils.ConverterUtil.convertToArray(
                                                    org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent.class,
                                                    list4));
                                                
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
           
          
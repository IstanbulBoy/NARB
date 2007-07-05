
            /**
            * EndpointContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package edu.internet2.hopi.dragon.terce.ws.types.rce;
            /**
            *  EndpointContent bean class
            */
        

        public  class EndpointContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = endpointContent
                Namespace URI = http://hopi.internet2.edu/DRAGON/TERCE/RCE
                Namespace Prefix = ns2
                */
            

                        /**
                        * field for DomainId
                        */

                        
                            protected java.lang.String localDomainId ;
                        

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
                            
                                            this.localDomainId=param;
                                    

                               }
                            

                        /**
                        * field for NodeId
                        */

                        
                            protected java.lang.String localNodeId ;
                        

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getNodeId(){
                               return localNodeId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param NodeId
                               */
                               public void setNodeId(java.lang.String param){
                            
                                            this.localNodeId=param;
                                    

                               }
                            

                        /**
                        * field for PortId
                        */

                        
                            protected java.lang.String localPortId ;
                        

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getPortId(){
                               return localPortId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param PortId
                               */
                               public void setPortId(java.lang.String param){
                            
                                            this.localPortId=param;
                                    

                               }
                            

                        /**
                        * field for LinkId
                        */

                        
                            protected java.lang.String localLinkId ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinkIdTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getLinkId(){
                               return localLinkId;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param LinkId
                               */
                               public void setLinkId(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLinkIdTracker = true;
                                       } else {
                                          localLinkIdTracker = false;
                                              
                                       }
                                   
                                            this.localLinkId=param;
                                    

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
                       EndpointContent.this.serialize(parentQName,factory,xmlWriter);
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

                
               
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";
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
                             
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"nodeId", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"nodeId");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("nodeId");
                                    }
                                

                                          if (localNodeId==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("nodeId cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localNodeId);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"portId", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"portId");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("portId");
                                    }
                                

                                          if (localPortId==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("portId cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localPortId);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                              if (localLinkIdTracker){
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"linkId", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"linkId");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("linkId");
                                    }
                                

                                          if (localLinkId==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("linkId cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localLinkId);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
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

                
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                                                      "domainId"));
                            
                                        if (localDomainId != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localDomainId));
                                        } else {
                                           throw new RuntimeException("domainId cannot be null!!");
                                        }
                                    
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                                                      "nodeId"));
                            
                                        if (localNodeId != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localNodeId));
                                        } else {
                                           throw new RuntimeException("nodeId cannot be null!!");
                                        }
                                    
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                                                      "portId"));
                            
                                        if (localPortId != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localPortId));
                                        } else {
                                           throw new RuntimeException("portId cannot be null!!");
                                        }
                                     if (localLinkIdTracker){
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                                                      "linkId"));
                            
                                        if (localLinkId != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLinkId));
                                        } else {
                                           throw new RuntimeException("linkId cannot be null!!");
                                        }
                                    }

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
        public static EndpointContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
            EndpointContent object = new EndpointContent();

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
                    
                            if (!"endpointContent".equals(type)){
                                //find namespace for the prefix
                                java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
                                return (EndpointContent)edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(
                                     nsUri,type,reader);
                              }
                        

                  }

                }
                

                
                // Note all attributes that were handled. Used to differ normal attributes
                // from anyAttributes.
                java.util.Vector handledAttributes = new java.util.Vector();
                

                 
                    
                    reader.next();
                
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/RCE","domainId").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setDomainId(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                else{
                                    // A start element we are not expecting indicates an invalid parameter was passed
                                    throw new java.lang.RuntimeException("Unexpected subelement " + reader.getLocalName());
                                }
                            
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/RCE","nodeId").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setNodeId(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                else{
                                    // A start element we are not expecting indicates an invalid parameter was passed
                                    throw new java.lang.RuntimeException("Unexpected subelement " + reader.getLocalName());
                                }
                            
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/RCE","portId").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setPortId(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                else{
                                    // A start element we are not expecting indicates an invalid parameter was passed
                                    throw new java.lang.RuntimeException("Unexpected subelement " + reader.getLocalName());
                                }
                            
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/RCE","linkId").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setLinkId(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
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
           
          
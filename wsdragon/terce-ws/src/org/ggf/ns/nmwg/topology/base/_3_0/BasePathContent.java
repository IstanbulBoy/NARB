
            /**
            * BasePathContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package org.ggf.ns.nmwg.topology.base._3_0;
            /**
            *  BasePathContent bean class
            */
        

        public  class BasePathContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = BasePathContent
                Namespace URI = http://ggf.org/ns/nmwg/topology/base/3.0/
                Namespace Prefix = ns3
                */
            
            /** Whenever a new property is set ensure all others are unset
             *  There can be only one choice and the last one wins
             */
            private void clearAllSettingTrackers() {
            
                   localLinkTracker = false;
                
                   localLink15Tracker = false;
                
                   localLink16Tracker = false;
                
                   localPathIdRefTracker = false;
                
            }
        

                        /**
                        * field for Link
                        */

                        
                            protected org.ggf.ns.nmwg.topology.base._3_0.BaseLinkContent localLink ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinkTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ggf.ns.nmwg.topology.base._3_0.BaseLinkContent
                           */
                           public  org.ggf.ns.nmwg.topology.base._3_0.BaseLinkContent getLink(){
                               return localLink;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Link
                               */
                               public void setLink(org.ggf.ns.nmwg.topology.base._3_0.BaseLinkContent param){
                            
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
                        * field for Link15
                        */

                        
                            protected org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent localLink15 ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLink15Tracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent
                           */
                           public  org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent getLink15(){
                               return localLink15;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Link15
                               */
                               public void setLink15(org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLink15Tracker = true;
                                       } else {
                                          localLink15Tracker = false;
                                              
                                       }
                                   
                                            this.localLink15=param;
                                    

                               }
                            

                        /**
                        * field for Link16
                        */

                        
                            protected org.ggf.ns.nmwg.topology.l3._3_0.L3LinkContent localLink16 ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLink16Tracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ggf.ns.nmwg.topology.l3._3_0.L3LinkContent
                           */
                           public  org.ggf.ns.nmwg.topology.l3._3_0.L3LinkContent getLink16(){
                               return localLink16;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Link16
                               */
                               public void setLink16(org.ggf.ns.nmwg.topology.l3._3_0.L3LinkContent param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localLink16Tracker = true;
                                       } else {
                                          localLink16Tracker = false;
                                              
                                       }
                                   
                                            this.localLink16=param;
                                    

                               }
                            

                        /**
                        * field for PathIdRef
                        * This was an Attribute!
                        */

                        
                            protected java.lang.String localPathIdRef ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localPathIdRefTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getPathIdRef(){
                               return localPathIdRef;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param PathIdRef
                               */
                               public void setPathIdRef(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localPathIdRefTracker = true;
                                       } else {
                                          localPathIdRefTracker = false;
                                              
                                       }
                                   
                                            this.localPathIdRef=param;
                                    

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
                       BasePathContent.this.serialize(parentQName,factory,xmlWriter);
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

                
               
                                            if (localPathIdRef != null){
                                        
                                        writeAttribute("",
                                                  "pathIdRef",
                                                  org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localPathIdRef), xmlWriter);
                                    }
                                     if (localLinkTracker){
                                    if (localLink==null){
                                         throw new RuntimeException("link cannot be null!!");
                                    }
                                   localLink.serialize(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","link"),
                                       factory,xmlWriter);
                                } if (localLink15Tracker){
                                    if (localLink15==null){
                                         throw new RuntimeException("link cannot be null!!");
                                    }
                                   localLink15.serialize(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","link"),
                                       factory,xmlWriter);
                                } if (localLink16Tracker){
                                    if (localLink16==null){
                                         throw new RuntimeException("link cannot be null!!");
                                    }
                                   localLink16.serialize(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l3/3.0/","link"),
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

                 if (localLinkTracker){
                            elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "link"));
                            
                            
                                    if (localLink==null){
                                         throw new RuntimeException("link cannot be null!!");
                                    }
                                    elementList.add(localLink);
                                } if (localLink15Tracker){
                            elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/",
                                                                      "link"));
                            
                            
                                    if (localLink15==null){
                                         throw new RuntimeException("link cannot be null!!");
                                    }
                                    elementList.add(localLink15);
                                } if (localLink16Tracker){
                            elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l3/3.0/",
                                                                      "link"));
                            
                            
                                    if (localLink16==null){
                                         throw new RuntimeException("link cannot be null!!");
                                    }
                                    elementList.add(localLink16);
                                }
                            attribList.add(
                            new javax.xml.namespace.QName("","pathIdRef"));
                            
                                      attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localPathIdRef));
                                

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
        public static BasePathContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
            BasePathContent object = new BasePathContent();

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
                    
                            if (!"BasePathContent".equals(type)){
                                //find namespace for the prefix
                                java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
                                return (BasePathContent)edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(
                                     nsUri,type,reader);
                              }
                        

                  }

                }
                

                
                // Note all attributes that were handled. Used to differ normal attributes
                // from anyAttributes.
                java.util.Vector handledAttributes = new java.util.Vector();
                

                 
                    // handle attribute "pathIdRef"
                    java.lang.String tempAttribpathIdRef =
                      reader.getAttributeValue("","pathIdRef");
                   if (tempAttribpathIdRef!=null){
                         java.lang.String content = tempAttribpathIdRef;
                        
                                                 object.setPathIdRef(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(tempAttribpathIdRef));
                                            
                    } else {
                       
                    }
                    handledAttributes.add("pathIdRef");
                    
                    
                    reader.next();
                   
                while(!reader.isEndElement()) {
                    if (reader.isStartElement() ){
                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","link").equals(reader.getName())){
                                
                                        object.setLink(org.ggf.ns.nmwg.topology.base._3_0.BaseLinkContent.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","link").equals(reader.getName())){
                                
                                        object.setLink15(org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent.Factory.parse(reader));
                                      
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l3/3.0/","link").equals(reader.getName())){
                                
                                        object.setLink16(org.ggf.ns.nmwg.topology.l3._3_0.L3LinkContent.Factory.parse(reader));
                                      
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
           
          
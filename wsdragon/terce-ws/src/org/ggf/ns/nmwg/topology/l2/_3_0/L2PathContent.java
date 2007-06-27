
            /**
            * L2PathContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package org.ggf.ns.nmwg.topology.l2._3_0;
            /**
            *  L2PathContent bean class
            */
        

        public  class L2PathContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = L2PathContent
                Namespace URI = http://ggf.org/ns/nmwg/topology/l2/3.0/
                Namespace Prefix = ns4
                */
            

                        /**
                        * field for Link
                        * This was an Array!
                        */

                        
                            protected org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent[] localLink ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLinkTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent[]
                           */
                           public  org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent[] getLink(){
                               return localLink;
                           }

                           
                        


                               
                              /**
                               * validate the array for Link
                               */
                              protected void validateLink(org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent[] param){
                             
                              }


                             /**
                              * Auto generated setter method
                              * @param param Link
                              */
                              public void setLink(org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent[] param){
                              
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
                             * @param param org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent
                             */
                             public void addLink(org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent param){
                                   if (localLink == null){
                                   localLink = new org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent[]{};
                                   }

                            
                                 //update the setting tracker
                                localLinkTracker = true;
                            

                               java.util.List list =
                            org.apache.axis2.databinding.utils.ConverterUtil.toList(localLink);
                               list.add(param);
                               this.localLink =
                             (org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent[])list.toArray(
                            new org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent[list.size()]);

                             }
                             

                        /**
                        * field for PathIdRef
                        * This was an Attribute!
                        */

                        
                            protected java.lang.String localPathIdRef ;
                        

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
                       L2PathContent.this.serialize(parentQName,factory,xmlWriter);
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
                             if (localLink!=null){
                                    for (int i = 0;i < localLink.length;i++){
                                        if (localLink[i] != null){
                                         localLink[i].serialize(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","link"),
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

                 if (localLinkTracker){
                             if (localLink!=null) {
                                 for (int i = 0;i < localLink.length;i++){

                                    if (localLink[i] != null){
                                         elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/",
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
        public static L2PathContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
            L2PathContent object = new L2PathContent();

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
                    
                            if (!"L2PathContent".equals(type)){
                                //find namespace for the prefix
                                java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
                                return (L2PathContent)edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(
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
                
                        java.util.ArrayList list1 = new java.util.ArrayList();
                    
                                    
                                    while (!reader.isStartElement() && !reader.isEndElement()) reader.next();
                                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","link").equals(reader.getName())){
                                
                                    
                                    
                                    // Process the array and step past its final element's end.
                                    list1.add(org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent.Factory.parse(reader));
                                            
                                            //loop until we find a start element that is not part of this array
                                            boolean loopDone1 = false;
                                            while(!loopDone1){
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
                                                    loopDone1 = true;
                                                } else {
                                                    if (new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/l2/3.0/","link").equals(reader.getName())){
                                                        list1.add(org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent.Factory.parse(reader));
                                                        
                                                    }else{
                                                        loopDone1 = true;
                                                    }
                                                }
                                            }
                                            // call the converter utility  to convert and set the array
                                            
                                            object.setLink((org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent[])
                                                org.apache.axis2.databinding.utils.ConverterUtil.convertToArray(
                                                    org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent.class,
                                                    list1));
                                                
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
           
          
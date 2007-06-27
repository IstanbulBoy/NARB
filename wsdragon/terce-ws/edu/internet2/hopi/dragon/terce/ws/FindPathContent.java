
            /**
            * FindPathContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package edu.internet2.hopi.dragon.terce.ws;
            /**
            *  FindPathContent bean class
            */
        
        public  class FindPathContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = findPathContent
                Namespace URI = http://hopi.internet2.edu/DRAGON/TERCE/WS
                Namespace Prefix = ns1
                */
            

                        /**
                        * field for SrcHost
                        */

                        protected java.lang.String localSrcHost ;
                        

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getSrcHost(){
                               return localSrcHost;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param SrcHost
                               */
                               public void setSrcHost(java.lang.String param){
                            
                                    this.localSrcHost=param;
                            

                               }
                            

                        /**
                        * field for DstHost
                        */

                        protected java.lang.String localDstHost ;
                        

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getDstHost(){
                               return localDstHost;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param DstHost
                               */
                               public void setDstHost(java.lang.String param){
                            
                                    this.localDstHost=param;
                            

                               }
                            

                        /**
                        * field for Bandwidth
                        */

                        protected int localBandwidth ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localBandwidthTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return int
                           */
                           public  int getBandwidth(){
                               return localBandwidth;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Bandwidth
                               */
                               public void setBandwidth(int param){
                            
                                       // setting primitive attribute tracker to true
                                       localBandwidthTracker = true;
                                   
                                    this.localBandwidth=param;
                            

                               }
                            

                        /**
                        * field for Encoding
                        */

                        protected java.lang.String localEncoding ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localEncodingTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getEncoding(){
                               return localEncoding;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Encoding
                               */
                               public void setEncoding(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localEncodingTracker = true;
                                       } else {
                                          localEncodingTracker = false;
                                              
                                       }
                                   
                                    this.localEncoding=param;
                            

                               }
                            

                        /**
                        * field for Swcap
                        */

                        protected java.lang.String localSwcap ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localSwcapTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getSwcap(){
                               return localSwcap;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Swcap
                               */
                               public void setSwcap(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localSwcapTracker = true;
                                       } else {
                                          localSwcapTracker = false;
                                              
                                       }
                                   
                                    this.localSwcap=param;
                            

                               }
                            

                        /**
                        * field for Gpid
                        */

                        protected java.lang.String localGpid ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localGpidTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getGpid(){
                               return localGpid;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Gpid
                               */
                               public void setGpid(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localGpidTracker = true;
                                       } else {
                                          localGpidTracker = false;
                                              
                                       }
                                   
                                    this.localGpid=param;
                            

                               }
                            

                        /**
                        * field for Vtag
                        */

                        protected java.lang.String localVtag ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localVtagTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getVtag(){
                               return localVtag;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Vtag
                               */
                               public void setVtag(java.lang.String param){
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localVtagTracker = true;
                                       } else {
                                          localVtagTracker = false;
                                              
                                       }
                                   
                                    this.localVtag=param;
                            

                               }
                            

                        /**
                        * field for Exclude
                        * This was an Array!
                        */

                        protected edu.internet2.hopi.dragon.terce.ws.Exclude[] localExclude ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localExcludeTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return edu.internet2.hopi.dragon.terce.ws.Exclude[]
                           */
                           public  edu.internet2.hopi.dragon.terce.ws.Exclude[] getExclude(){
                               return localExclude;
                           }

                           
                        


                               
                              /**
                               * validate the array for Exclude
                               */
                              protected void validateExclude(edu.internet2.hopi.dragon.terce.ws.Exclude[] param){
                             
                              }


                             /**
                              * Auto generated setter method
                              * @param param Exclude
                              */
                              public void setExclude(edu.internet2.hopi.dragon.terce.ws.Exclude[] param){
                              
                                   validateExclude(param);

                               
                                          if (param != null){
                                             //update the setting tracker
                                             localExcludeTracker = true;
                                          } else {
                                             localExcludeTracker = false;
                                                 
                                          }
                                      
                                      this.localExclude=param;
                              }

                               
                             
                             /**
                             * Auto generated add method for the array for convenience
                             * @param param edu.internet2.hopi.dragon.terce.ws.Exclude
                             */
                             public void addExclude(edu.internet2.hopi.dragon.terce.ws.Exclude param){
                                   if (localExclude == null){
                                   localExclude = new edu.internet2.hopi.dragon.terce.ws.Exclude[]{};
                                   }

                            
                                 //update the setting tracker
                                localExcludeTracker = true;
                            

                               java.util.List list =
                            org.apache.axis2.databinding.utils.ConverterUtil.toList(localExclude);
                               list.add(param);
                               this.localExclude =
                             (edu.internet2.hopi.dragon.terce.ws.Exclude[])list.toArray(
                            new edu.internet2.hopi.dragon.terce.ws.Exclude[list.size()]);

                             }
                             

                        /**
                        * field for Bidirectional
                        * This was an Attribute!
                        */

                        protected boolean localBidirectional ;
                        

                           /**
                           * Auto generated getter method
                           * @return boolean
                           */
                           public  boolean getBidirectional(){
                               return localBidirectional;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Bidirectional
                               */
                               public void setBidirectional(boolean param){
                            
                                    this.localBidirectional=param;
                            

                               }
                            

                        /**
                        * field for Strict
                        * This was an Attribute!
                        */

                        protected boolean localStrict ;
                        

                           /**
                           * Auto generated getter method
                           * @return boolean
                           */
                           public  boolean getStrict(){
                               return localStrict;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Strict
                               */
                               public void setStrict(boolean param){
                            
                                    this.localStrict=param;
                            

                               }
                            

                        /**
                        * field for Preferred
                        * This was an Attribute!
                        */

                        protected boolean localPreferred ;
                        

                           /**
                           * Auto generated getter method
                           * @return boolean
                           */
                           public  boolean getPreferred(){
                               return localPreferred;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Preferred
                               */
                               public void setPreferred(boolean param){
                            
                                    this.localPreferred=param;
                            

                               }
                            

                        /**
                        * field for Mrn
                        * This was an Attribute!
                        */

                        protected boolean localMrn ;
                        

                           /**
                           * Auto generated getter method
                           * @return boolean
                           */
                           public  boolean getMrn(){
                               return localMrn;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Mrn
                               */
                               public void setMrn(boolean param){
                            
                                    this.localMrn=param;
                            

                               }
                            

                        /**
                        * field for Viamovaz
                        * This was an Attribute!
                        */

                        protected boolean localViamovaz ;
                        

                           /**
                           * Auto generated getter method
                           * @return boolean
                           */
                           public  boolean getViamovaz(){
                               return localViamovaz;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Viamovaz
                               */
                               public void setViamovaz(boolean param){
                            
                                    this.localViamovaz=param;
                            

                               }
                            

                        /**
                        * field for Allvtags
                        * This was an Attribute!
                        */

                        protected boolean localAllvtags ;
                        

                           /**
                           * Auto generated getter method
                           * @return boolean
                           */
                           public  boolean getAllvtags(){
                               return localAllvtags;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Allvtags
                               */
                               public void setAllvtags(boolean param){
                            
                                    this.localAllvtags=param;
                            

                               }
                            

                        /**
                        * field for Allwaves
                        * This was an Attribute!
                        */

                        protected boolean localAllwaves ;
                        

                           /**
                           * Auto generated getter method
                           * @return boolean
                           */
                           public  boolean getAllwaves(){
                               return localAllwaves;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Allwaves
                               */
                               public void setAllwaves(boolean param){
                            
                                    this.localAllwaves=param;
                            

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

        org.apache.axiom.om.OMDataSource dataSource = getOMDataSource(parentQName, factory);

        
               return new org.apache.axiom.om.impl.llom.OMSourcedElementImpl(
               parentQName,factory,dataSource);
            
       }

     /**
     *
     * @param parentQName
     * @param factory
     * @return org.apache.axiom.om.OMElement
     */
    public org.apache.axiom.om.OMDataSource getOMDataSource(
            final javax.xml.namespace.QName parentQName,
            final org.apache.axiom.om.OMFactory factory){


        org.apache.axiom.om.OMDataSource dataSource =
                       new org.apache.axis2.databinding.ADBDataSource(this,parentQName){

         public void serialize(
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

                
               
                             // optional attribute bidirectional
                             try {
                                writeAttribute("",
                                               "bidirectional",
                                               org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localBidirectional), xmlWriter);
                            } catch (NullPointerException e) {
                                // If localBidirectional was null
                                // it can not be serialized.
                            }
                        
                             // optional attribute strict
                             try {
                                writeAttribute("",
                                               "strict",
                                               org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localStrict), xmlWriter);
                            } catch (NullPointerException e) {
                                // If localStrict was null
                                // it can not be serialized.
                            }
                        
                             // optional attribute preferred
                             try {
                                writeAttribute("",
                                               "preferred",
                                               org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localPreferred), xmlWriter);
                            } catch (NullPointerException e) {
                                // If localPreferred was null
                                // it can not be serialized.
                            }
                        
                             // optional attribute mrn
                             try {
                                writeAttribute("",
                                               "mrn",
                                               org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localMrn), xmlWriter);
                            } catch (NullPointerException e) {
                                // If localMrn was null
                                // it can not be serialized.
                            }
                        
                             // optional attribute viamovaz
                             try {
                                writeAttribute("",
                                               "viamovaz",
                                               org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localViamovaz), xmlWriter);
                            } catch (NullPointerException e) {
                                // If localViamovaz was null
                                // it can not be serialized.
                            }
                        
                             // optional attribute allvtags
                             try {
                                writeAttribute("",
                                               "allvtags",
                                               org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localAllvtags), xmlWriter);
                            } catch (NullPointerException e) {
                                // If localAllvtags was null
                                // it can not be serialized.
                            }
                        
                             // optional attribute allwaves
                             try {
                                writeAttribute("",
                                               "allwaves",
                                               org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localAllwaves), xmlWriter);
                            } catch (NullPointerException e) {
                                // If localAllwaves was null
                                // it can not be serialized.
                            }
                        
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/WS";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"srcHost", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"srcHost");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("srcHost");
                                    }
                                

                                          if (localSrcHost==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("srcHost cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localSrcHost);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/WS";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"dstHost", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"dstHost");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("dstHost");
                                    }
                                

                                          if (localDstHost==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("dstHost cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localDstHost);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                              if (localBandwidthTracker){
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/WS";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"bandwidth", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"bandwidth");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("bandwidth");
                                    }
                                
                                       xmlWriter.writeCharacters(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localBandwidth));
                                    
                                   xmlWriter.writeEndElement();
                             } if (localEncodingTracker){
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/WS";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"encoding", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"encoding");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("encoding");
                                    }
                                

                                          if (localEncoding==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("encoding cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localEncoding);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localSwcapTracker){
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/WS";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"swcap", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"swcap");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("swcap");
                                    }
                                

                                          if (localSwcap==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("swcap cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localSwcap);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localGpidTracker){
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/WS";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"gpid", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"gpid");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("gpid");
                                    }
                                

                                          if (localGpid==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("gpid cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localGpid);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localVtagTracker){
                                    namespace = "http://hopi.internet2.edu/DRAGON/TERCE/WS";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"vtag", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"vtag");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("vtag");
                                    }
                                

                                          if (localVtag==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("vtag cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localVtag);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localExcludeTracker){
                             if (localExclude!=null){
                                    for (int i = 0;i < localExclude.length;i++){
                                        if (localExclude[i] != null){
                                         localExclude[i].getOMDataSource(
                                                   new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","exclude"),
                                                   factory).serialize(xmlWriter);
                                        } else {
                                           
                                                // we don't have to do any thing since minOccures is zero
                                            
                                        }

                                    }
                             } else {
                                
                                       throw new RuntimeException("exclude cannot be null!!");
                                
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
         * Register a namespace prefix
         */
         private java.lang.String registerPrefix(javax.xml.stream.XMLStreamWriter xmlWriter, java.lang.String namespace) throws javax.xml.stream.XMLStreamException {
                java.lang.String prefix = xmlWriter.getPrefix(namespace);

                if (prefix == null) {
                    prefix = createPrefix();

                    while (xmlWriter.getNamespaceContext().getNamespaceURI(prefix) != null) {
                        prefix = createPrefix();
                    }

                    xmlWriter.writeNamespace(prefix, namespace);
                    xmlWriter.setPrefix(prefix, namespace);
                }

                return prefix;
            }

         /**
          * Create a prefix
          */
          private java.lang.String createPrefix() {
                return "ns" + (int)Math.random();
          }
        };

        return dataSource;
    }

  
        /**
        * databinding method to get an XML representation of this object
        *
        */
        public javax.xml.stream.XMLStreamReader getPullParser(javax.xml.namespace.QName qName){


        
                 java.util.ArrayList elementList = new java.util.ArrayList();
                 java.util.ArrayList attribList = new java.util.ArrayList();

                
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS",
                                                                      "srcHost"));
                            
                                        if (localSrcHost != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localSrcHost));
                                        } else {
                                           throw new RuntimeException("srcHost cannot be null!!");
                                        }
                                    
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS",
                                                                      "dstHost"));
                            
                                        if (localDstHost != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localDstHost));
                                        } else {
                                           throw new RuntimeException("dstHost cannot be null!!");
                                        }
                                     if (localBandwidthTracker){
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS",
                                                                      "bandwidth"));
                            
                                elementList.add(
                                   org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localBandwidth));
                            } if (localEncodingTracker){
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS",
                                                                      "encoding"));
                            
                                        if (localEncoding != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localEncoding));
                                        } else {
                                           throw new RuntimeException("encoding cannot be null!!");
                                        }
                                    } if (localSwcapTracker){
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS",
                                                                      "swcap"));
                            
                                        if (localSwcap != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localSwcap));
                                        } else {
                                           throw new RuntimeException("swcap cannot be null!!");
                                        }
                                    } if (localGpidTracker){
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS",
                                                                      "gpid"));
                            
                                        if (localGpid != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localGpid));
                                        } else {
                                           throw new RuntimeException("gpid cannot be null!!");
                                        }
                                    } if (localVtagTracker){
                             elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS",
                                                                      "vtag"));
                            
                                        if (localVtag != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localVtag));
                                        } else {
                                           throw new RuntimeException("vtag cannot be null!!");
                                        }
                                    } if (localExcludeTracker){
                             if (localExclude!=null) {
                                 for (int i = 0;i < localExclude.length;i++){

                                    if (localExclude[i] != null){
                                         elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS",
                                                                          "exclude"));
                                         elementList.add(localExclude[i]);
                                    } else {
                                        
                                                // nothing to do
                                            
                                    }

                                 }
                             } else {
                                 
                                        throw new RuntimeException("exclude cannot be null!!");
                                    
                             }

                        }
                            attribList.add(
                            new javax.xml.namespace.QName("","bidirectional"));
                            attribList.add(
                            org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localBidirectional));
                        
                            attribList.add(
                            new javax.xml.namespace.QName("","strict"));
                            attribList.add(
                            org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localStrict));
                        
                            attribList.add(
                            new javax.xml.namespace.QName("","preferred"));
                            attribList.add(
                            org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localPreferred));
                        
                            attribList.add(
                            new javax.xml.namespace.QName("","mrn"));
                            attribList.add(
                            org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localMrn));
                        
                            attribList.add(
                            new javax.xml.namespace.QName("","viamovaz"));
                            attribList.add(
                            org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localViamovaz));
                        
                            attribList.add(
                            new javax.xml.namespace.QName("","allvtags"));
                            attribList.add(
                            org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localAllvtags));
                        
                            attribList.add(
                            new javax.xml.namespace.QName("","allwaves"));
                            attribList.add(
                            org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localAllwaves));
                        

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
        public static FindPathContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
            FindPathContent object = new FindPathContent();
            int event;
            try {
                
                while (!reader.isStartElement() && !reader.isEndElement())
                    reader.next();

                
                if (reader.getAttributeValue("http://www.w3.org/2001/XMLSchema-instance","type")!=null){
                  java.lang.String fullTypeName = reader.getAttributeValue("http://www.w3.org/2001/XMLSchema-instance",
                        "type");
                  if (fullTypeName!=null){
                    java.lang.String nsPrefix = fullTypeName.substring(0,fullTypeName.indexOf(":"));
                    nsPrefix = nsPrefix==null?"":nsPrefix;

                    java.lang.String type = fullTypeName.substring(fullTypeName.indexOf(":")+1);
                    if (!"findPathContent".equals(type)){
                        //find namespace for the prefix
                        java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
                        return (FindPathContent)edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(
                             nsUri,type,reader);
                      }

                  }

                }
                

                
                // Note all attributes that were handled. Used to differ normal attributes
                // from anyAttributes.
                java.util.Vector handledAttributes = new java.util.Vector();
                
                    // handle attribute "bidirectional"
                    java.lang.String tempAttribbidirectional =
                      reader.getAttributeValue("","bidirectional");
                   if (tempAttribbidirectional!=null){
                         java.lang.String content = tempAttribbidirectional;
                        
                         object.setBidirectional(
                           org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                                tempAttribbidirectional));
                            
                    }
                    handledAttributes.add("bidirectional");
                    
                    // handle attribute "strict"
                    java.lang.String tempAttribstrict =
                      reader.getAttributeValue("","strict");
                   if (tempAttribstrict!=null){
                         java.lang.String content = tempAttribstrict;
                        
                         object.setStrict(
                           org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                                tempAttribstrict));
                            
                    }
                    handledAttributes.add("strict");
                    
                    // handle attribute "preferred"
                    java.lang.String tempAttribpreferred =
                      reader.getAttributeValue("","preferred");
                   if (tempAttribpreferred!=null){
                         java.lang.String content = tempAttribpreferred;
                        
                         object.setPreferred(
                           org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                                tempAttribpreferred));
                            
                    }
                    handledAttributes.add("preferred");
                    
                    // handle attribute "mrn"
                    java.lang.String tempAttribmrn =
                      reader.getAttributeValue("","mrn");
                   if (tempAttribmrn!=null){
                         java.lang.String content = tempAttribmrn;
                        
                         object.setMrn(
                           org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                                tempAttribmrn));
                            
                    }
                    handledAttributes.add("mrn");
                    
                    // handle attribute "viamovaz"
                    java.lang.String tempAttribviamovaz =
                      reader.getAttributeValue("","viamovaz");
                   if (tempAttribviamovaz!=null){
                         java.lang.String content = tempAttribviamovaz;
                        
                         object.setViamovaz(
                           org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                                tempAttribviamovaz));
                            
                    }
                    handledAttributes.add("viamovaz");
                    
                    // handle attribute "allvtags"
                    java.lang.String tempAttriballvtags =
                      reader.getAttributeValue("","allvtags");
                   if (tempAttriballvtags!=null){
                         java.lang.String content = tempAttriballvtags;
                        
                         object.setAllvtags(
                           org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                                tempAttriballvtags));
                            
                    }
                    handledAttributes.add("allvtags");
                    
                    // handle attribute "allwaves"
                    java.lang.String tempAttriballwaves =
                      reader.getAttributeValue("","allwaves");
                   if (tempAttriballwaves!=null){
                         java.lang.String content = tempAttriballwaves;
                        
                         object.setAllwaves(
                           org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                                tempAttriballwaves));
                            
                    }
                    handledAttributes.add("allwaves");
                    
                    
                    reader.next();
                
                        java.util.ArrayList list8 = new java.util.ArrayList();
                       
                while(!reader.isEndElement()) {
                    if (reader.isStartElement() ){
                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","srcHost").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setSrcHost(
                                        org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                            
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","dstHost").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setDstHost(
                                        org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                            
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","bandwidth").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setBandwidth(
                                        org.apache.axis2.databinding.utils.ConverterUtil.convertToInt(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                            
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","encoding").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setEncoding(
                                        org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                            
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","swcap").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setSwcap(
                                        org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                            
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","gpid").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setGpid(
                                        org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                            
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","vtag").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setVtag(
                                        org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                            
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","exclude").equals(reader.getName())){
                                
                                    
                                    
                                    // Process the array and step past its final element's end.
                                    list8.add(edu.internet2.hopi.dragon.terce.ws.Exclude.Factory.parse(reader));
                                            
                                            //loop until we find a start element that is not part of this array
                                            boolean loopDone8 = false;
                                            while(!loopDone8){
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
                                                    loopDone8 = true;
                                                } else {
                                                    if (new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/TERCE/WS","exclude").equals(reader.getName())){
                                                        list8.add(edu.internet2.hopi.dragon.terce.ws.Exclude.Factory.parse(reader));
                                                        
                                                    }else{
                                                        loopDone8 = true;
                                                    }
                                                }
                                            }
                                            // call the converter utility  to convert and set the array
                                            
                                                    object.setExclude((edu.internet2.hopi.dragon.terce.ws.Exclude[])
                                                        org.apache.axis2.databinding.utils.ConverterUtil.convertToArray(
                                                            edu.internet2.hopi.dragon.terce.ws.Exclude.class,
                                                            list8));
                                                
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
           
          

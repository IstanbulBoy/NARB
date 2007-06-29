
            /**
            * LocationContent.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package org.ggf.ns.nmwg.topology.base._3_0;
            /**
            *  LocationContent bean class
            */
        

        public  class LocationContent
        implements org.apache.axis2.databinding.ADBBean{
        /* This type was generated from the piece of schema that had
                name = LocationContent
                Namespace URI = http://ggf.org/ns/nmwg/topology/base/3.0/
                Namespace Prefix = ns3
                */
            
            /** Whenever a new property is set ensure all others are unset
             *  There can be only one choice and the last one wins
             */
            private void clearAllSettingTrackers() {
            
                   localInstitutionTracker = false;
                
                   localCountryTracker = false;
                
                   localZipcodeTracker = false;
                
                   localStateTracker = false;
                
                   localCityTracker = false;
                
                   localStreetAddressTracker = false;
                
                   localFloorTracker = false;
                
                   localRoomTracker = false;
                
                   localCageTracker = false;
                
                   localRackTracker = false;
                
                   localShelfTracker = false;
                
                   localLatitudeTracker = false;
                
                   localLongitudeTracker = false;
                
            }
        

                        /**
                        * field for Institution
                        */

                        
                            protected java.lang.String localInstitution ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localInstitutionTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getInstitution(){
                               return localInstitution;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Institution
                               */
                               public void setInstitution(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localInstitutionTracker = true;
                                       } else {
                                          localInstitutionTracker = false;
                                              
                                       }
                                   
                                            this.localInstitution=param;
                                    

                               }
                            

                        /**
                        * field for Country
                        */

                        
                            protected java.lang.String localCountry ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localCountryTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getCountry(){
                               return localCountry;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Country
                               */
                               public void setCountry(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localCountryTracker = true;
                                       } else {
                                          localCountryTracker = false;
                                              
                                       }
                                   
                                            this.localCountry=param;
                                    

                               }
                            

                        /**
                        * field for Zipcode
                        */

                        
                            protected java.math.BigInteger localZipcode ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localZipcodeTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.math.BigInteger
                           */
                           public  java.math.BigInteger getZipcode(){
                               return localZipcode;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Zipcode
                               */
                               public void setZipcode(java.math.BigInteger param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localZipcodeTracker = true;
                                       } else {
                                          localZipcodeTracker = false;
                                              
                                       }
                                   
                                            this.localZipcode=param;
                                    

                               }
                            

                        /**
                        * field for State
                        */

                        
                            protected java.lang.String localState ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localStateTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getState(){
                               return localState;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param State
                               */
                               public void setState(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localStateTracker = true;
                                       } else {
                                          localStateTracker = false;
                                              
                                       }
                                   
                                            this.localState=param;
                                    

                               }
                            

                        /**
                        * field for City
                        */

                        
                            protected java.lang.String localCity ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localCityTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getCity(){
                               return localCity;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param City
                               */
                               public void setCity(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localCityTracker = true;
                                       } else {
                                          localCityTracker = false;
                                              
                                       }
                                   
                                            this.localCity=param;
                                    

                               }
                            

                        /**
                        * field for StreetAddress
                        */

                        
                            protected java.lang.String localStreetAddress ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localStreetAddressTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getStreetAddress(){
                               return localStreetAddress;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param StreetAddress
                               */
                               public void setStreetAddress(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localStreetAddressTracker = true;
                                       } else {
                                          localStreetAddressTracker = false;
                                              
                                       }
                                   
                                            this.localStreetAddress=param;
                                    

                               }
                            

                        /**
                        * field for Floor
                        */

                        
                            protected java.lang.String localFloor ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localFloorTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getFloor(){
                               return localFloor;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Floor
                               */
                               public void setFloor(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localFloorTracker = true;
                                       } else {
                                          localFloorTracker = false;
                                              
                                       }
                                   
                                            this.localFloor=param;
                                    

                               }
                            

                        /**
                        * field for Room
                        */

                        
                            protected java.lang.String localRoom ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localRoomTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getRoom(){
                               return localRoom;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Room
                               */
                               public void setRoom(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localRoomTracker = true;
                                       } else {
                                          localRoomTracker = false;
                                              
                                       }
                                   
                                            this.localRoom=param;
                                    

                               }
                            

                        /**
                        * field for Cage
                        */

                        
                            protected java.lang.String localCage ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localCageTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getCage(){
                               return localCage;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Cage
                               */
                               public void setCage(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localCageTracker = true;
                                       } else {
                                          localCageTracker = false;
                                              
                                       }
                                   
                                            this.localCage=param;
                                    

                               }
                            

                        /**
                        * field for Rack
                        */

                        
                            protected java.lang.String localRack ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localRackTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getRack(){
                               return localRack;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Rack
                               */
                               public void setRack(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localRackTracker = true;
                                       } else {
                                          localRackTracker = false;
                                              
                                       }
                                   
                                            this.localRack=param;
                                    

                               }
                            

                        /**
                        * field for Shelf
                        */

                        
                            protected java.lang.String localShelf ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localShelfTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return java.lang.String
                           */
                           public  java.lang.String getShelf(){
                               return localShelf;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Shelf
                               */
                               public void setShelf(java.lang.String param){
                            
                                clearAllSettingTrackers();
                            
                                       if (param != null){
                                          //update the setting tracker
                                          localShelfTracker = true;
                                       } else {
                                          localShelfTracker = false;
                                              
                                       }
                                   
                                            this.localShelf=param;
                                    

                               }
                            

                        /**
                        * field for Latitude
                        */

                        
                            protected float localLatitude ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLatitudeTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return float
                           */
                           public  float getLatitude(){
                               return localLatitude;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Latitude
                               */
                               public void setLatitude(float param){
                            
                                clearAllSettingTrackers();
                            
                                       // setting primitive attribute tracker to true
                                       
                                               if (param==java.lang.Float.NaN) {
                                           localLatitudeTracker = false;
                                              
                                       } else {
                                          localLatitudeTracker = true;
                                       }
                                   
                                            this.localLatitude=param;
                                    

                               }
                            

                        /**
                        * field for Longitude
                        */

                        
                            protected float localLongitude ;
                        
                           /*  This tracker boolean wil be used to detect whether the user called the set method
                          *   for this attribute. It will be used to determine whether to include this field
                           *   in the serialized XML
                           */
                           protected boolean localLongitudeTracker = false ;
                           

                           /**
                           * Auto generated getter method
                           * @return float
                           */
                           public  float getLongitude(){
                               return localLongitude;
                           }

                           
                        
                            /**
                               * Auto generated setter method
                               * @param param Longitude
                               */
                               public void setLongitude(float param){
                            
                                clearAllSettingTrackers();
                            
                                       // setting primitive attribute tracker to true
                                       
                                               if (param==java.lang.Float.NaN) {
                                           localLongitudeTracker = false;
                                              
                                       } else {
                                          localLongitudeTracker = true;
                                       }
                                   
                                            this.localLongitude=param;
                                    

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
                       LocationContent.this.serialize(parentQName,factory,xmlWriter);
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

                
                if (localInstitutionTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"institution", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"institution");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("institution");
                                    }
                                

                                          if (localInstitution==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("institution cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localInstitution);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localCountryTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"country", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"country");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("country");
                                    }
                                

                                          if (localCountry==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("country cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localCountry);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localZipcodeTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"zipcode", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"zipcode");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("zipcode");
                                    }
                                

                                          if (localZipcode==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("zipcode cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localZipcode));
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localStateTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"state", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"state");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("state");
                                    }
                                

                                          if (localState==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("state cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localState);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localCityTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"city", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"city");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("city");
                                    }
                                

                                          if (localCity==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("city cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localCity);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localStreetAddressTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"streetAddress", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"streetAddress");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("streetAddress");
                                    }
                                

                                          if (localStreetAddress==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("streetAddress cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localStreetAddress);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localFloorTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"floor", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"floor");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("floor");
                                    }
                                

                                          if (localFloor==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("floor cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localFloor);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localRoomTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"room", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"room");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("room");
                                    }
                                

                                          if (localRoom==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("room cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localRoom);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localCageTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"cage", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"cage");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("cage");
                                    }
                                

                                          if (localCage==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("cage cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localCage);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localRackTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"rack", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"rack");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("rack");
                                    }
                                

                                          if (localRack==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("rack cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localRack);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localShelfTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"shelf", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"shelf");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("shelf");
                                    }
                                

                                          if (localShelf==null){
                                              // write the nil attribute
                                              
                                                     throw new RuntimeException("shelf cannot be null!!");
                                                  
                                          }else{

                                        
                                                   xmlWriter.writeCharacters(localShelf);
                                            
                                          }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localLatitudeTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"latitude", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"latitude");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("latitude");
                                    }
                                
                                               if (localLatitude==java.lang.Float.NaN) {
                                           
                                                         throw new RuntimeException("latitude cannot be null!!");
                                                      
                                               } else {
                                                    xmlWriter.writeCharacters(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLatitude));
                                               }
                                    
                                   xmlWriter.writeEndElement();
                             } if (localLongitudeTracker){
                                    namespace = "http://ggf.org/ns/nmwg/topology/base/3.0/";
                                    if (! namespace.equals("")) {
                                        prefix = xmlWriter.getPrefix(namespace);

                                        if (prefix == null) {
                                            prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

                                            xmlWriter.writeStartElement(prefix,"longitude", namespace);
                                            xmlWriter.writeNamespace(prefix, namespace);
                                            xmlWriter.setPrefix(prefix, namespace);

                                        } else {
                                            xmlWriter.writeStartElement(namespace,"longitude");
                                        }

                                    } else {
                                        xmlWriter.writeStartElement("longitude");
                                    }
                                
                                               if (localLongitude==java.lang.Float.NaN) {
                                           
                                                         throw new RuntimeException("longitude cannot be null!!");
                                                      
                                               } else {
                                                    xmlWriter.writeCharacters(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLongitude));
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

                 if (localInstitutionTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "institution"));
                            
                                        if (localInstitution != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localInstitution));
                                        } else {
                                           throw new RuntimeException("institution cannot be null!!");
                                        }
                                    } if (localCountryTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "country"));
                            
                                        if (localCountry != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localCountry));
                                        } else {
                                           throw new RuntimeException("country cannot be null!!");
                                        }
                                    } if (localZipcodeTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "zipcode"));
                            
                                        if (localZipcode != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localZipcode));
                                        } else {
                                           throw new RuntimeException("zipcode cannot be null!!");
                                        }
                                    } if (localStateTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "state"));
                            
                                        if (localState != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localState));
                                        } else {
                                           throw new RuntimeException("state cannot be null!!");
                                        }
                                    } if (localCityTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "city"));
                            
                                        if (localCity != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localCity));
                                        } else {
                                           throw new RuntimeException("city cannot be null!!");
                                        }
                                    } if (localStreetAddressTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "streetAddress"));
                            
                                        if (localStreetAddress != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localStreetAddress));
                                        } else {
                                           throw new RuntimeException("streetAddress cannot be null!!");
                                        }
                                    } if (localFloorTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "floor"));
                            
                                        if (localFloor != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localFloor));
                                        } else {
                                           throw new RuntimeException("floor cannot be null!!");
                                        }
                                    } if (localRoomTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "room"));
                            
                                        if (localRoom != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localRoom));
                                        } else {
                                           throw new RuntimeException("room cannot be null!!");
                                        }
                                    } if (localCageTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "cage"));
                            
                                        if (localCage != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localCage));
                                        } else {
                                           throw new RuntimeException("cage cannot be null!!");
                                        }
                                    } if (localRackTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "rack"));
                            
                                        if (localRack != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localRack));
                                        } else {
                                           throw new RuntimeException("rack cannot be null!!");
                                        }
                                    } if (localShelfTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "shelf"));
                            
                                        if (localShelf != null){
                                            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localShelf));
                                        } else {
                                           throw new RuntimeException("shelf cannot be null!!");
                                        }
                                    } if (localLatitudeTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "latitude"));
                            
                                elementList.add(
                                   org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLatitude));
                            } if (localLongitudeTracker){
                             elementList.add(new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/",
                                                                      "longitude"));
                            
                                elementList.add(
                                   org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLongitude));
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
        public static LocationContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
            LocationContent object = new LocationContent();

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
                    
                            if (!"LocationContent".equals(type)){
                                //find namespace for the prefix
                                java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
                                return (LocationContent)edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(
                                     nsUri,type,reader);
                              }
                        

                  }

                }
                

                
                // Note all attributes that were handled. Used to differ normal attributes
                // from anyAttributes.
                java.util.Vector handledAttributes = new java.util.Vector();
                

                 
                    
                    reader.next();
                   
                while(!reader.isEndElement()) {
                    if (reader.isStartElement() ){
                
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","institution").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setInstitution(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","country").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setCountry(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","zipcode").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setZipcode(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToInteger(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","state").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setState(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","city").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setCity(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","streetAddress").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setStreetAddress(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","floor").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setFloor(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","room").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setRoom(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","cage").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setCage(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","rack").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setRack(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","shelf").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setShelf(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","latitude").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setLatitude(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToFloat(content));
                                              
                                        reader.next();
                                    
                              }  // End of if for expected property start element
                                
                                        else
                                    
                                    if (reader.isStartElement() && new javax.xml.namespace.QName("http://ggf.org/ns/nmwg/topology/base/3.0/","longitude").equals(reader.getName())){
                                
                                    java.lang.String content = reader.getElementText();
                                    
                                              object.setLongitude(
                                                    org.apache.axis2.databinding.utils.ConverterUtil.convertToFloat(content));
                                              
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
           
          
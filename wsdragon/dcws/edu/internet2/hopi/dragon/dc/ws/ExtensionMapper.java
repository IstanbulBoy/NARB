
            /**
            * ExtensionMapper.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package edu.internet2.hopi.dragon.dc.ws;
            /**
            *  ExtensionMapper class
            */
        
        public  class ExtensionMapper{

          public static java.lang.Object getTypeObject(java.lang.String namespaceURI,
                                                       java.lang.String typeName,
                                                       javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/DC/WS".equals(namespaceURI) &&
                  "findPathContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.dc.ws.FindPathContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/DC/WS".equals(namespaceURI) &&
                  "vtagList".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.dc.ws.VtagList.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/DC/WS".equals(namespaceURI) &&
                  "path".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.dc.ws.Path.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/DC/WS".equals(namespaceURI) &&
                  "findPathResponseContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.dc.ws.FindPathResponseContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/DC/WS".equals(namespaceURI) &&
                  "hop".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.dc.ws.Hop.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/DC/WS".equals(namespaceURI) &&
                  "exclude".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.dc.ws.Exclude.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/DC/WS".equals(namespaceURI) &&
                  "hopList".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.dc.ws.HopList.Factory.parse(reader);
                        

                  }

              
             throw new java.lang.RuntimeException("Unsupported type " + namespaceURI + " " + typeName);
          }

        }
    

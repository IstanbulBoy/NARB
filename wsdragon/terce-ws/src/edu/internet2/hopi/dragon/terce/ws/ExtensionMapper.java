
            /**
            * ExtensionMapper.java
            *
            * This file was auto-generated from WSDL
            * by the Apache Axis2 version: #axisVersion# #today#
            */

            package edu.internet2.hopi.dragon.terce.ws;
            /**
            *  ExtensionMapper class
            */
        
        public  class ExtensionMapper{

          public static java.lang.Object getTypeObject(java.lang.String namespaceURI,
                                                       java.lang.String typeName,
                                                       javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/RCE".equals(namespaceURI) &&
                  "findPathContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneDomainContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneSwcapContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwcapContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "insertNetworkTopologyResponseContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponseContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlanePathContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePathContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneDomainSignatureContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneDomainSignatureContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneHopContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneHopContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "selectNetworkTopologyResponseContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponseContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneLinkContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneLinkContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneAddressContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAddressContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneAdministrativeGroup".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneAdministrativeGroup.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "selectNetworkTopologyContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "insertNetworkTopologyContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneNodeContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneNodeContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneTopologyContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneTopologyContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlanePortContent".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlanePortContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/RCE".equals(namespaceURI) &&
                  "exclude".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "selectTypes".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectTypes.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/network/topology/ctrlPlane/20070626/".equals(namespaceURI) &&
                  "CtrlPlaneSwitchingCapabilitySpecficInfo".equals(typeName)){
                   
                            return  org.ogf.schema.network.topology.ctrlplane._20070626.CtrlPlaneSwitchingCapabilitySpecficInfo.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/RCE".equals(namespaceURI) &&
                  "findPathResponseContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponseContent.Factory.parse(reader);
                        

                  }

              
             throw new java.lang.RuntimeException("Unsupported type " + namespaceURI + " " + typeName);
          }

        }
    
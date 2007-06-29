
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
                  "http://hopi.internet2.edu/DRAGON/TERCE/RCE".equals(namespaceURI) &&
                  "vtagList".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.rce.VtagList.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l3/3.0/".equals(namespaceURI) &&
                  "L3NetworkContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l3._3_0.L3NetworkContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l3/3.0/".equals(namespaceURI) &&
                  "L3InterfaceContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l3._3_0.L3InterfaceContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/".equals(namespaceURI) &&
                  "CtrlPlanePortContent".equals(typeName)){
                   
                            return  org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlanePortContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l4/3.0/".equals(namespaceURI) &&
                  "PathContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l4._3_0.PathContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l4/3.0/".equals(namespaceURI) &&
                  "role_type0".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l4._3_0.Role_type0.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/".equals(namespaceURI) &&
                  "CtrlPlaneLinkContent".equals(typeName)){
                   
                            return  org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneLinkContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/".equals(namespaceURI) &&
                  "CtrlPlaneDomainSignatureContent".equals(typeName)){
                   
                            return  org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneDomainSignatureContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/RCE".equals(namespaceURI) &&
                  "hopList".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.rce.HopList.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l2/3.0/".equals(namespaceURI) &&
                  "L2LinkContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l2._3_0.L2LinkContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/".equals(namespaceURI) &&
                  "CtrlPlaneAdministrativeGroup".equals(typeName)){
                   
                            return  org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneAdministrativeGroup.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/".equals(namespaceURI) &&
                  "CtrlPlaneDomainContent".equals(typeName)){
                   
                            return  org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneDomainContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/base/3.0/".equals(namespaceURI) &&
                  "NodeContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.base._3_0.NodeContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/RCE".equals(namespaceURI) &&
                  "findPathResponseContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.rce.FindPathResponseContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "insertNetworkTopologyResponseContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyResponseContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l4/3.0/".equals(namespaceURI) &&
                  "endPoint_type1".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l4._3_0.EndPoint_type1.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/base/3.0/".equals(namespaceURI) &&
                  "BaseNetworkContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.base._3_0.BaseNetworkContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "selectNetworkTopologyContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/".equals(namespaceURI) &&
                  "CtrlPlaneSwitchingCapabilitySpecficInfo".equals(typeName)){
                   
                            return  org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneSwitchingCapabilitySpecficInfo.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l4/3.0/".equals(namespaceURI) &&
                  "EndpointContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l4._3_0.EndpointContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "selectTypes".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectTypes.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l4/3.0/".equals(namespaceURI) &&
                  "EndpointPairContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l4._3_0.EndpointPairContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/base/3.0/".equals(namespaceURI) &&
                  "LocationContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.base._3_0.LocationContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l3/3.0/".equals(namespaceURI) &&
                  "L3LinkContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l3._3_0.L3LinkContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/RCE".equals(namespaceURI) &&
                  "path".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.rce.Path33.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l2/3.0/".equals(namespaceURI) &&
                  "L2InterfaceContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l2._3_0.L2InterfaceContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "selectNetworkTopologyResponseContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.SelectNetworkTopologyResponseContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l3/3.0/".equals(namespaceURI) &&
                  "L3PathContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l3._3_0.L3PathContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/".equals(namespaceURI) &&
                  "CtrlPlaneNodeContent".equals(typeName)){
                   
                            return  org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneNodeContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l4/3.0/".equals(namespaceURI) &&
                  "role_type2".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l4._3_0.Role_type2.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/base/3.0/".equals(namespaceURI) &&
                  "BaseInterfaceContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.base._3_0.BaseInterfaceContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/TEDB".equals(namespaceURI) &&
                  "insertNetworkTopologyContent".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.tedb.InsertNetworkTopologyContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l2/3.0/".equals(namespaceURI) &&
                  "L2PathContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l2._3_0.L2PathContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l2/3.0/".equals(namespaceURI) &&
                  "L2NetworkContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l2._3_0.L2NetworkContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ogf.org/schema/nm/topology/CtrlPlane/20070611/".equals(namespaceURI) &&
                  "CtrlPlaneTopologyContent".equals(typeName)){
                   
                            return  org.ogf.schema.nm.topology.ctrlplane._20070611.CtrlPlaneTopologyContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/base/3.0/".equals(namespaceURI) &&
                  "BasePathContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.base._3_0.BasePathContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://hopi.internet2.edu/DRAGON/TERCE/RCE".equals(namespaceURI) &&
                  "exclude".equals(typeName)){
                   
                            return  edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/base/3.0/".equals(namespaceURI) &&
                  "BaseLinkContent".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.base._3_0.BaseLinkContent.Factory.parse(reader);
                        

                  }

              
                  if (
                  "http://ggf.org/ns/nmwg/topology/l4/3.0/".equals(namespaceURI) &&
                  "endPoint_type3".equals(typeName)){
                   
                            return  org.ggf.ns.nmwg.topology.l4._3_0.EndPoint_type3.Factory.parse(reader);
                        

                  }

              
             throw new java.lang.RuntimeException("Unsupported type " + namespaceURI + " " + typeName);
          }

        }
    
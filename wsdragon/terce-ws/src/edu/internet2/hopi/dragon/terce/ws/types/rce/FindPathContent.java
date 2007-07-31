/**
 * FindPathContent.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.3-RC2  Built on : Jul 20, 2007 (04:22:00 LKT)
 */
package edu.internet2.hopi.dragon.terce.ws.types.rce;

/**
 * FindPathContent bean class
 */
public class FindPathContent implements org.apache.axis2.databinding.ADBBean {
    /** field for SrcEndpoint */
    protected java.lang.String localSrcEndpoint;

    /** field for DestEndpoint */
    protected java.lang.String localDestEndpoint;

    /** field for Bandwidth */
    protected int localBandwidth;

    /*  This tracker boolean wil be used to detect whether the user called the set method
     *   for this attribute. It will be used to determine whether to include this field
     *   in the serialized XML
     */
    protected boolean localBandwidthTracker = false;

    /** field for Encoding */
    protected java.lang.String localEncoding;

    /*  This tracker boolean wil be used to detect whether the user called the set method
     *   for this attribute. It will be used to determine whether to include this field
     *   in the serialized XML
     */
    protected boolean localEncodingTracker = false;

    /** field for Swcap */
    protected java.lang.String localSwcap;

    /*  This tracker boolean wil be used to detect whether the user called the set method
     *   for this attribute. It will be used to determine whether to include this field
     *   in the serialized XML
     */
    protected boolean localSwcapTracker = false;

    /** field for Gpid */
    protected java.lang.String localGpid;

    /*  This tracker boolean wil be used to detect whether the user called the set method
     *   for this attribute. It will be used to determine whether to include this field
     *   in the serialized XML
     */
    protected boolean localGpidTracker = false;

    /** field for Vtag */
    protected java.lang.String localVtag;

    /*  This tracker boolean wil be used to detect whether the user called the set method
     *   for this attribute. It will be used to determine whether to include this field
     *   in the serialized XML
     */
    protected boolean localVtagTracker = false;

    /** field for Exclude This was an Array! */
    protected edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude[] localExclude;

    /*  This tracker boolean wil be used to detect whether the user called the set method
     *   for this attribute. It will be used to determine whether to include this field
     *   in the serialized XML
     */
    protected boolean localExcludeTracker = false;

    /** field for Bidirectional This was an Attribute! */
    protected boolean localBidirectional;

    /** field for Strict This was an Attribute! */
    protected boolean localStrict;

    /** field for Preferred This was an Attribute! */
    protected boolean localPreferred;

    /** field for Mrn This was an Attribute! */
    protected boolean localMrn;

    /** field for Viamovaz This was an Attribute! */
    protected boolean localViamovaz;

    /** field for Allvtags This was an Attribute! */
    protected boolean localAllvtags;

    /** field for Allwaves This was an Attribute! */
    protected boolean localAllwaves;

    /* This type was generated from the piece of schema that had
       name = findPathContent
       Namespace URI = http://hopi.internet2.edu/DRAGON/TERCE/RCE
       Namespace Prefix = ns2
     */
    private static java.lang.String generatePrefix(java.lang.String namespace) {
        if (namespace.equals("http://hopi.internet2.edu/DRAGON/TERCE/RCE")) {
            return "ns2";
        }

        return org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();
    }

    /**
     * Auto generated getter method
     *
     * @return java.lang.String
     */
    public java.lang.String getSrcEndpoint() {
        return localSrcEndpoint;
    }

    /**
     * Auto generated setter method
     *
     * @param param SrcEndpoint
     */
    public void setSrcEndpoint(java.lang.String param) {
        this.localSrcEndpoint = param;
    }

    /**
     * Auto generated getter method
     *
     * @return java.lang.String
     */
    public java.lang.String getDestEndpoint() {
        return localDestEndpoint;
    }

    /**
     * Auto generated setter method
     *
     * @param param DestEndpoint
     */
    public void setDestEndpoint(java.lang.String param) {
        this.localDestEndpoint = param;
    }

    /**
     * Auto generated getter method
     *
     * @return int
     */
    public int getBandwidth() {
        return localBandwidth;
    }

    /**
     * Auto generated setter method
     *
     * @param param Bandwidth
     */
    public void setBandwidth(int param) {
        // setting primitive attribute tracker to true
        if (param == java.lang.Integer.MIN_VALUE) {
            localBandwidthTracker = false;
        } else {
            localBandwidthTracker = true;
        }

        this.localBandwidth = param;
    }

    /**
     * Auto generated getter method
     *
     * @return java.lang.String
     */
    public java.lang.String getEncoding() {
        return localEncoding;
    }

    /**
     * Auto generated setter method
     *
     * @param param Encoding
     */
    public void setEncoding(java.lang.String param) {
        if (param != null) {
            //update the setting tracker
            localEncodingTracker = true;
        } else {
            localEncodingTracker = false;
        }

        this.localEncoding = param;
    }

    /**
     * Auto generated getter method
     *
     * @return java.lang.String
     */
    public java.lang.String getSwcap() {
        return localSwcap;
    }

    /**
     * Auto generated setter method
     *
     * @param param Swcap
     */
    public void setSwcap(java.lang.String param) {
        if (param != null) {
            //update the setting tracker
            localSwcapTracker = true;
        } else {
            localSwcapTracker = false;
        }

        this.localSwcap = param;
    }

    /**
     * Auto generated getter method
     *
     * @return java.lang.String
     */
    public java.lang.String getGpid() {
        return localGpid;
    }

    /**
     * Auto generated setter method
     *
     * @param param Gpid
     */
    public void setGpid(java.lang.String param) {
        if (param != null) {
            //update the setting tracker
            localGpidTracker = true;
        } else {
            localGpidTracker = false;
        }

        this.localGpid = param;
    }

    /**
     * Auto generated getter method
     *
     * @return java.lang.String
     */
    public java.lang.String getVtag() {
        return localVtag;
    }

    /**
     * Auto generated setter method
     *
     * @param param Vtag
     */
    public void setVtag(java.lang.String param) {
        if (param != null) {
            //update the setting tracker
            localVtagTracker = true;
        } else {
            localVtagTracker = false;
        }

        this.localVtag = param;
    }

    /**
     * Auto generated getter method
     *
     * @return edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude[]
     */
    public edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude[] getExclude() {
        return localExclude;
    }

    /**
     * validate the array for Exclude
     */
    protected void validateExclude(
        edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude[] param) {
    }

    /**
     * Auto generated setter method
     *
     * @param param Exclude
     */
    public void setExclude(
        edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude[] param) {
        validateExclude(param);

        if (param != null) {
            //update the setting tracker
            localExcludeTracker = true;
        } else {
            localExcludeTracker = false;
        }

        this.localExclude = param;
    }

    /**
     * Auto generated add method for the array for convenience
     *
     * @param param edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude
     */
    public void addExclude(
        edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude param) {
        if (localExclude == null) {
            localExclude = new edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude[] {
                    
                };
        }

        //update the setting tracker
        localExcludeTracker = true;

        java.util.List list = org.apache.axis2.databinding.utils.ConverterUtil.toList(localExclude);
        list.add(param);
        this.localExclude = (edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude[]) list.toArray(new edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude[list.size()]);
    }

    /**
     * Auto generated getter method
     *
     * @return boolean
     */
    public boolean getBidirectional() {
        return localBidirectional;
    }

    /**
     * Auto generated setter method
     *
     * @param param Bidirectional
     */
    public void setBidirectional(boolean param) {
        this.localBidirectional = param;
    }

    /**
     * Auto generated getter method
     *
     * @return boolean
     */
    public boolean getStrict() {
        return localStrict;
    }

    /**
     * Auto generated setter method
     *
     * @param param Strict
     */
    public void setStrict(boolean param) {
        this.localStrict = param;
    }

    /**
     * Auto generated getter method
     *
     * @return boolean
     */
    public boolean getPreferred() {
        return localPreferred;
    }

    /**
     * Auto generated setter method
     *
     * @param param Preferred
     */
    public void setPreferred(boolean param) {
        this.localPreferred = param;
    }

    /**
     * Auto generated getter method
     *
     * @return boolean
     */
    public boolean getMrn() {
        return localMrn;
    }

    /**
     * Auto generated setter method
     *
     * @param param Mrn
     */
    public void setMrn(boolean param) {
        this.localMrn = param;
    }

    /**
     * Auto generated getter method
     *
     * @return boolean
     */
    public boolean getViamovaz() {
        return localViamovaz;
    }

    /**
     * Auto generated setter method
     *
     * @param param Viamovaz
     */
    public void setViamovaz(boolean param) {
        this.localViamovaz = param;
    }

    /**
     * Auto generated getter method
     *
     * @return boolean
     */
    public boolean getAllvtags() {
        return localAllvtags;
    }

    /**
     * Auto generated setter method
     *
     * @param param Allvtags
     */
    public void setAllvtags(boolean param) {
        this.localAllvtags = param;
    }

    /**
     * Auto generated getter method
     *
     * @return boolean
     */
    public boolean getAllwaves() {
        return localAllwaves;
    }

    /**
     * Auto generated setter method
     *
     * @param param Allwaves
     */
    public void setAllwaves(boolean param) {
        this.localAllwaves = param;
    }

    /**
     * isReaderMTOMAware
     *
     * @return true if the reader supports MTOM
     */
    public static boolean isReaderMTOMAware(
        javax.xml.stream.XMLStreamReader reader) {
        boolean isReaderMTOMAware = false;

        try {
            isReaderMTOMAware = java.lang.Boolean.TRUE.equals(reader.getProperty(
                        org.apache.axiom.om.OMConstants.IS_DATA_HANDLERS_AWARE));
        } catch (java.lang.IllegalArgumentException e) {
            isReaderMTOMAware = false;
        }

        return isReaderMTOMAware;
    }

    /**
     * 
     * @param parentQName
     * @param factory
     *
     * @return org.apache.axiom.om.OMElement
     */
    public org.apache.axiom.om.OMElement getOMElement(
        final javax.xml.namespace.QName parentQName,
        final org.apache.axiom.om.OMFactory factory)
        throws org.apache.axis2.databinding.ADBException {
        org.apache.axiom.om.OMDataSource dataSource = new org.apache.axis2.databinding.ADBDataSource(this,
                parentQName) {
                public void serialize(
                    javax.xml.stream.XMLStreamWriter xmlWriter)
                    throws javax.xml.stream.XMLStreamException {
                    FindPathContent.this.serialize(parentQName, factory,
                        xmlWriter);
                }
            };

        return new org.apache.axiom.om.impl.llom.OMSourcedElementImpl(parentQName,
            factory, dataSource);
    }

    public void serialize(final javax.xml.namespace.QName parentQName,
        final org.apache.axiom.om.OMFactory factory,
        javax.xml.stream.XMLStreamWriter xmlWriter)
        throws javax.xml.stream.XMLStreamException,
            org.apache.axis2.databinding.ADBException {
        java.lang.String prefix = null;
        java.lang.String namespace = null;

        prefix = parentQName.getPrefix();
        namespace = parentQName.getNamespaceURI();

        if (namespace != null) {
            java.lang.String writerPrefix = xmlWriter.getPrefix(namespace);

            if (writerPrefix != null) {
                xmlWriter.writeStartElement(namespace,
                    parentQName.getLocalPart());
            } else {
                if (prefix == null) {
                    prefix = generatePrefix(namespace);
                }

                xmlWriter.writeStartElement(prefix, parentQName.getLocalPart(),
                    namespace);
                xmlWriter.writeNamespace(prefix, namespace);
                xmlWriter.setPrefix(prefix, namespace);
            }
        } else {
            xmlWriter.writeStartElement(parentQName.getLocalPart());
        }

        if (true) {
            writeAttribute("", "bidirectional",
                org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localBidirectional), xmlWriter);
        }

        if (true) {
            writeAttribute("", "strict",
                org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localStrict), xmlWriter);
        }

        if (true) {
            writeAttribute("", "preferred",
                org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localPreferred), xmlWriter);
        }

        if (true) {
            writeAttribute("", "mrn",
                org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localMrn), xmlWriter);
        }

        if (true) {
            writeAttribute("", "viamovaz",
                org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localViamovaz), xmlWriter);
        }

        if (true) {
            writeAttribute("", "allvtags",
                org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localAllvtags), xmlWriter);
        }

        if (true) {
            writeAttribute("", "allwaves",
                org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localAllwaves), xmlWriter);
        }

        namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";

        if (!namespace.equals("")) {
            prefix = xmlWriter.getPrefix(namespace);

            if (prefix == null) {
                prefix = generatePrefix(namespace);

                xmlWriter.writeStartElement(prefix, "srcEndpoint", namespace);
                xmlWriter.writeNamespace(prefix, namespace);
                xmlWriter.setPrefix(prefix, namespace);
            } else {
                xmlWriter.writeStartElement(namespace, "srcEndpoint");
            }
        } else {
            xmlWriter.writeStartElement("srcEndpoint");
        }

        if (localSrcEndpoint == null) {
            // write the nil attribute
            throw new org.apache.axis2.databinding.ADBException(
                "srcEndpoint cannot be null!!");
        } else {
            xmlWriter.writeCharacters(localSrcEndpoint);
        }

        xmlWriter.writeEndElement();

        namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";

        if (!namespace.equals("")) {
            prefix = xmlWriter.getPrefix(namespace);

            if (prefix == null) {
                prefix = generatePrefix(namespace);

                xmlWriter.writeStartElement(prefix, "destEndpoint", namespace);
                xmlWriter.writeNamespace(prefix, namespace);
                xmlWriter.setPrefix(prefix, namespace);
            } else {
                xmlWriter.writeStartElement(namespace, "destEndpoint");
            }
        } else {
            xmlWriter.writeStartElement("destEndpoint");
        }

        if (localDestEndpoint == null) {
            // write the nil attribute
            throw new org.apache.axis2.databinding.ADBException(
                "destEndpoint cannot be null!!");
        } else {
            xmlWriter.writeCharacters(localDestEndpoint);
        }

        xmlWriter.writeEndElement();

        if (localBandwidthTracker) {
            namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";

            if (!namespace.equals("")) {
                prefix = xmlWriter.getPrefix(namespace);

                if (prefix == null) {
                    prefix = generatePrefix(namespace);

                    xmlWriter.writeStartElement(prefix, "bandwidth", namespace);
                    xmlWriter.writeNamespace(prefix, namespace);
                    xmlWriter.setPrefix(prefix, namespace);
                } else {
                    xmlWriter.writeStartElement(namespace, "bandwidth");
                }
            } else {
                xmlWriter.writeStartElement("bandwidth");
            }

            if (localBandwidth == java.lang.Integer.MIN_VALUE) {
                throw new org.apache.axis2.databinding.ADBException(
                    "bandwidth cannot be null!!");
            } else {
                xmlWriter.writeCharacters(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                        localBandwidth));
            }

            xmlWriter.writeEndElement();
        }

        if (localEncodingTracker) {
            namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";

            if (!namespace.equals("")) {
                prefix = xmlWriter.getPrefix(namespace);

                if (prefix == null) {
                    prefix = generatePrefix(namespace);

                    xmlWriter.writeStartElement(prefix, "encoding", namespace);
                    xmlWriter.writeNamespace(prefix, namespace);
                    xmlWriter.setPrefix(prefix, namespace);
                } else {
                    xmlWriter.writeStartElement(namespace, "encoding");
                }
            } else {
                xmlWriter.writeStartElement("encoding");
            }

            if (localEncoding == null) {
                // write the nil attribute
                throw new org.apache.axis2.databinding.ADBException(
                    "encoding cannot be null!!");
            } else {
                xmlWriter.writeCharacters(localEncoding);
            }

            xmlWriter.writeEndElement();
        }

        if (localSwcapTracker) {
            namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";

            if (!namespace.equals("")) {
                prefix = xmlWriter.getPrefix(namespace);

                if (prefix == null) {
                    prefix = generatePrefix(namespace);

                    xmlWriter.writeStartElement(prefix, "swcap", namespace);
                    xmlWriter.writeNamespace(prefix, namespace);
                    xmlWriter.setPrefix(prefix, namespace);
                } else {
                    xmlWriter.writeStartElement(namespace, "swcap");
                }
            } else {
                xmlWriter.writeStartElement("swcap");
            }

            if (localSwcap == null) {
                // write the nil attribute
                throw new org.apache.axis2.databinding.ADBException(
                    "swcap cannot be null!!");
            } else {
                xmlWriter.writeCharacters(localSwcap);
            }

            xmlWriter.writeEndElement();
        }

        if (localGpidTracker) {
            namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";

            if (!namespace.equals("")) {
                prefix = xmlWriter.getPrefix(namespace);

                if (prefix == null) {
                    prefix = generatePrefix(namespace);

                    xmlWriter.writeStartElement(prefix, "gpid", namespace);
                    xmlWriter.writeNamespace(prefix, namespace);
                    xmlWriter.setPrefix(prefix, namespace);
                } else {
                    xmlWriter.writeStartElement(namespace, "gpid");
                }
            } else {
                xmlWriter.writeStartElement("gpid");
            }

            if (localGpid == null) {
                // write the nil attribute
                throw new org.apache.axis2.databinding.ADBException(
                    "gpid cannot be null!!");
            } else {
                xmlWriter.writeCharacters(localGpid);
            }

            xmlWriter.writeEndElement();
        }

        if (localVtagTracker) {
            namespace = "http://hopi.internet2.edu/DRAGON/TERCE/RCE";

            if (!namespace.equals("")) {
                prefix = xmlWriter.getPrefix(namespace);

                if (prefix == null) {
                    prefix = generatePrefix(namespace);

                    xmlWriter.writeStartElement(prefix, "vtag", namespace);
                    xmlWriter.writeNamespace(prefix, namespace);
                    xmlWriter.setPrefix(prefix, namespace);
                } else {
                    xmlWriter.writeStartElement(namespace, "vtag");
                }
            } else {
                xmlWriter.writeStartElement("vtag");
            }

            if (localVtag == null) {
                // write the nil attribute
                throw new org.apache.axis2.databinding.ADBException(
                    "vtag cannot be null!!");
            } else {
                xmlWriter.writeCharacters(localVtag);
            }

            xmlWriter.writeEndElement();
        }

        if (localExcludeTracker) {
            if (localExclude != null) {
                for (int i = 0; i < localExclude.length; i++) {
                    if (localExclude[i] != null) {
                        localExclude[i].serialize(new javax.xml.namespace.QName(
                                "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                "exclude"), factory, xmlWriter);
                    } else {
                        // we don't have to do any thing since minOccures is zero
                    }
                }
            } else {
                throw new org.apache.axis2.databinding.ADBException(
                    "exclude cannot be null!!");
            }
        }

        xmlWriter.writeEndElement();
    }

    /**
     * Util method to write an attribute with the ns prefix
     */
    private void writeAttribute(java.lang.String prefix,
        java.lang.String namespace, java.lang.String attName,
        java.lang.String attValue, javax.xml.stream.XMLStreamWriter xmlWriter)
        throws javax.xml.stream.XMLStreamException {
        if (xmlWriter.getPrefix(namespace) == null) {
            xmlWriter.writeNamespace(prefix, namespace);
            xmlWriter.setPrefix(prefix, namespace);
        }

        xmlWriter.writeAttribute(namespace, attName, attValue);
    }

    /**
     * Util method to write an attribute without the ns prefix
     */
    private void writeAttribute(java.lang.String namespace,
        java.lang.String attName, java.lang.String attValue,
        javax.xml.stream.XMLStreamWriter xmlWriter)
        throws javax.xml.stream.XMLStreamException {
        if (namespace.equals("")) {
            xmlWriter.writeAttribute(attName, attValue);
        } else {
            registerPrefix(xmlWriter, namespace);
            xmlWriter.writeAttribute(namespace, attName, attValue);
        }
    }

    /**
     * Util method to write an attribute without the ns prefix
     */
    private void writeQNameAttribute(java.lang.String namespace,
        java.lang.String attName, javax.xml.namespace.QName qname,
        javax.xml.stream.XMLStreamWriter xmlWriter)
        throws javax.xml.stream.XMLStreamException {
        java.lang.String attributeNamespace = qname.getNamespaceURI();
        java.lang.String attributePrefix = xmlWriter.getPrefix(attributeNamespace);

        if (attributePrefix == null) {
            attributePrefix = registerPrefix(xmlWriter, attributeNamespace);
        }

        java.lang.String attributeValue;

        if (attributePrefix.trim().length() > 0) {
            attributeValue = attributePrefix + ":" + qname.getLocalPart();
        } else {
            attributeValue = qname.getLocalPart();
        }

        if (namespace.equals("")) {
            xmlWriter.writeAttribute(attName, attributeValue);
        } else {
            registerPrefix(xmlWriter, namespace);
            xmlWriter.writeAttribute(namespace, attName, attributeValue);
        }
    }

    /**
     * method to handle Qnames
     */
    private void writeQName(javax.xml.namespace.QName qname,
        javax.xml.stream.XMLStreamWriter xmlWriter)
        throws javax.xml.stream.XMLStreamException {
        java.lang.String namespaceURI = qname.getNamespaceURI();

        if (namespaceURI != null) {
            java.lang.String prefix = xmlWriter.getPrefix(namespaceURI);

            if (prefix == null) {
                prefix = generatePrefix(namespaceURI);
                xmlWriter.writeNamespace(prefix, namespaceURI);
                xmlWriter.setPrefix(prefix, namespaceURI);
            }

            if (prefix.trim().length() > 0) {
                xmlWriter.writeCharacters(prefix + ":" +
                    org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                        qname));
            } else {
                // i.e this is the default namespace
                xmlWriter.writeCharacters(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                        qname));
            }
        } else {
            xmlWriter.writeCharacters(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    qname));
        }
    }

    private void writeQNames(javax.xml.namespace.QName[] qnames,
        javax.xml.stream.XMLStreamWriter xmlWriter)
        throws javax.xml.stream.XMLStreamException {
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
                        prefix = generatePrefix(namespaceURI);
                        xmlWriter.writeNamespace(prefix, namespaceURI);
                        xmlWriter.setPrefix(prefix, namespaceURI);
                    }

                    if (prefix.trim().length() > 0) {
                        stringToWrite.append(prefix).append(":")
                                     .append(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                                qnames[i]));
                    } else {
                        stringToWrite.append(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                                qnames[i]));
                    }
                } else {
                    stringToWrite.append(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                            qnames[i]));
                }
            }

            xmlWriter.writeCharacters(stringToWrite.toString());
        }
    }

    /**
     * Register a namespace prefix
     */
    private java.lang.String registerPrefix(
        javax.xml.stream.XMLStreamWriter xmlWriter, java.lang.String namespace)
        throws javax.xml.stream.XMLStreamException {
        java.lang.String prefix = xmlWriter.getPrefix(namespace);

        if (prefix == null) {
            prefix = generatePrefix(namespace);

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
     */
    public javax.xml.stream.XMLStreamReader getPullParser(
        javax.xml.namespace.QName qName)
        throws org.apache.axis2.databinding.ADBException {
        java.util.ArrayList elementList = new java.util.ArrayList();
        java.util.ArrayList attribList = new java.util.ArrayList();

        elementList.add(new javax.xml.namespace.QName(
                "http://hopi.internet2.edu/DRAGON/TERCE/RCE", "srcEndpoint"));

        if (localSrcEndpoint != null) {
            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localSrcEndpoint));
        } else {
            throw new org.apache.axis2.databinding.ADBException(
                "srcEndpoint cannot be null!!");
        }

        elementList.add(new javax.xml.namespace.QName(
                "http://hopi.internet2.edu/DRAGON/TERCE/RCE", "destEndpoint"));

        if (localDestEndpoint != null) {
            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localDestEndpoint));
        } else {
            throw new org.apache.axis2.databinding.ADBException(
                "destEndpoint cannot be null!!");
        }

        if (localBandwidthTracker) {
            elementList.add(new javax.xml.namespace.QName(
                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE", "bandwidth"));

            elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                    localBandwidth));
        }

        if (localEncodingTracker) {
            elementList.add(new javax.xml.namespace.QName(
                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE", "encoding"));

            if (localEncoding != null) {
                elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                        localEncoding));
            } else {
                throw new org.apache.axis2.databinding.ADBException(
                    "encoding cannot be null!!");
            }
        }

        if (localSwcapTracker) {
            elementList.add(new javax.xml.namespace.QName(
                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE", "swcap"));

            if (localSwcap != null) {
                elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                        localSwcap));
            } else {
                throw new org.apache.axis2.databinding.ADBException(
                    "swcap cannot be null!!");
            }
        }

        if (localGpidTracker) {
            elementList.add(new javax.xml.namespace.QName(
                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE", "gpid"));

            if (localGpid != null) {
                elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                        localGpid));
            } else {
                throw new org.apache.axis2.databinding.ADBException(
                    "gpid cannot be null!!");
            }
        }

        if (localVtagTracker) {
            elementList.add(new javax.xml.namespace.QName(
                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE", "vtag"));

            if (localVtag != null) {
                elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                        localVtag));
            } else {
                throw new org.apache.axis2.databinding.ADBException(
                    "vtag cannot be null!!");
            }
        }

        if (localExcludeTracker) {
            if (localExclude != null) {
                for (int i = 0; i < localExclude.length; i++) {
                    if (localExclude[i] != null) {
                        elementList.add(new javax.xml.namespace.QName(
                                "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                "exclude"));
                        elementList.add(localExclude[i]);
                    } else {
                        // nothing to do
                    }
                }
            } else {
                throw new org.apache.axis2.databinding.ADBException(
                    "exclude cannot be null!!");
            }
        }

        attribList.add(new javax.xml.namespace.QName("", "bidirectional"));

        attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                localBidirectional));

        attribList.add(new javax.xml.namespace.QName("", "strict"));

        attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                localStrict));

        attribList.add(new javax.xml.namespace.QName("", "preferred"));

        attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                localPreferred));

        attribList.add(new javax.xml.namespace.QName("", "mrn"));

        attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                localMrn));

        attribList.add(new javax.xml.namespace.QName("", "viamovaz"));

        attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                localViamovaz));

        attribList.add(new javax.xml.namespace.QName("", "allvtags"));

        attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                localAllvtags));

        attribList.add(new javax.xml.namespace.QName("", "allwaves"));

        attribList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                localAllwaves));

        return new org.apache.axis2.databinding.utils.reader.ADBXMLStreamReaderImpl(qName,
            elementList.toArray(), attribList.toArray());
    }

    /**
     * Factory class that keeps the parse method
     */
    public static class Factory {
        /**
         * static method to create the object Precondition:  If
         * this object is an element, the current or next start element starts
         * this object and any intervening reader events are ignorable If this
         * object is not an element, it is a complex type and the reader is at
         * the event just after the outer start element Postcondition: If this
         * object is an element, the reader is positioned at its end element
         * If this object is a complex type, the reader is positioned at the
         * end element of its outer element
         */
        public static FindPathContent parse(
            javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception {
            FindPathContent object = new FindPathContent();

            int event;
            java.lang.String nillableValue = null;
            java.lang.String prefix = "";
            java.lang.String namespaceuri = "";

            try {
                while (!reader.isStartElement() && !reader.isEndElement())
                    reader.next();

                if (reader.getAttributeValue(
                            "http://www.w3.org/2001/XMLSchema-instance", "type") != null) {
                    java.lang.String fullTypeName = reader.getAttributeValue("http://www.w3.org/2001/XMLSchema-instance",
                            "type");

                    if (fullTypeName != null) {
                        java.lang.String nsPrefix = null;

                        if (fullTypeName.indexOf(":") > -1) {
                            nsPrefix = fullTypeName.substring(0,
                                    fullTypeName.indexOf(":"));
                        }

                        nsPrefix = (nsPrefix == null) ? "" : nsPrefix;

                        java.lang.String type = fullTypeName.substring(fullTypeName.indexOf(
                                    ":") + 1);

                        if (!"findPathContent".equals(type)) {
                            //find namespace for the prefix
                            java.lang.String nsUri = reader.getNamespaceContext()
                                                           .getNamespaceURI(nsPrefix);

                            return (FindPathContent) edu.internet2.hopi.dragon.terce.ws.ExtensionMapper.getTypeObject(nsUri,
                                type, reader);
                        }
                    }
                }

                // Note all attributes that were handled. Used to differ normal attributes
                // from anyAttributes.
                java.util.Vector handledAttributes = new java.util.Vector();

                // handle attribute "bidirectional"
                java.lang.String tempAttribBidirectional = reader.getAttributeValue(null,
                        "bidirectional");

                if (tempAttribBidirectional != null) {
                    java.lang.String content = tempAttribBidirectional;

                    object.setBidirectional(org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                            tempAttribBidirectional));
                } else {
                }

                handledAttributes.add("bidirectional");

                // handle attribute "strict"
                java.lang.String tempAttribStrict = reader.getAttributeValue(null,
                        "strict");

                if (tempAttribStrict != null) {
                    java.lang.String content = tempAttribStrict;

                    object.setStrict(org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                            tempAttribStrict));
                } else {
                }

                handledAttributes.add("strict");

                // handle attribute "preferred"
                java.lang.String tempAttribPreferred = reader.getAttributeValue(null,
                        "preferred");

                if (tempAttribPreferred != null) {
                    java.lang.String content = tempAttribPreferred;

                    object.setPreferred(org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                            tempAttribPreferred));
                } else {
                }

                handledAttributes.add("preferred");

                // handle attribute "mrn"
                java.lang.String tempAttribMrn = reader.getAttributeValue(null,
                        "mrn");

                if (tempAttribMrn != null) {
                    java.lang.String content = tempAttribMrn;

                    object.setMrn(org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                            tempAttribMrn));
                } else {
                }

                handledAttributes.add("mrn");

                // handle attribute "viamovaz"
                java.lang.String tempAttribViamovaz = reader.getAttributeValue(null,
                        "viamovaz");

                if (tempAttribViamovaz != null) {
                    java.lang.String content = tempAttribViamovaz;

                    object.setViamovaz(org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                            tempAttribViamovaz));
                } else {
                }

                handledAttributes.add("viamovaz");

                // handle attribute "allvtags"
                java.lang.String tempAttribAllvtags = reader.getAttributeValue(null,
                        "allvtags");

                if (tempAttribAllvtags != null) {
                    java.lang.String content = tempAttribAllvtags;

                    object.setAllvtags(org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                            tempAttribAllvtags));
                } else {
                }

                handledAttributes.add("allvtags");

                // handle attribute "allwaves"
                java.lang.String tempAttribAllwaves = reader.getAttributeValue(null,
                        "allwaves");

                if (tempAttribAllwaves != null) {
                    java.lang.String content = tempAttribAllwaves;

                    object.setAllwaves(org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
                            tempAttribAllwaves));
                } else {
                }

                handledAttributes.add("allwaves");

                reader.next();

                java.util.ArrayList list8 = new java.util.ArrayList();

                while (!reader.isEndElement()) {
                    if (reader.isStartElement()) {
                        if (reader.isStartElement() &&
                                new javax.xml.namespace.QName(
                                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                    "srcEndpoint").equals(reader.getName())) {
                            java.lang.String content = reader.getElementText();

                            object.setSrcEndpoint(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                                    content));

                            reader.next();
                        } // End of if for expected property start element

                        else
                         if (reader.isStartElement() &&
                                new javax.xml.namespace.QName(
                                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                    "destEndpoint").equals(reader.getName())) {
                            java.lang.String content = reader.getElementText();

                            object.setDestEndpoint(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                                    content));

                            reader.next();
                        } // End of if for expected property start element

                        else
                         if (reader.isStartElement() &&
                                new javax.xml.namespace.QName(
                                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                    "bandwidth").equals(reader.getName())) {
                            java.lang.String content = reader.getElementText();

                            object.setBandwidth(org.apache.axis2.databinding.utils.ConverterUtil.convertToInt(
                                    content));

                            reader.next();
                        } // End of if for expected property start element

                        else
                         if (reader.isStartElement() &&
                                new javax.xml.namespace.QName(
                                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                    "encoding").equals(reader.getName())) {
                            java.lang.String content = reader.getElementText();

                            object.setEncoding(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                                    content));

                            reader.next();
                        } // End of if for expected property start element

                        else
                         if (reader.isStartElement() &&
                                new javax.xml.namespace.QName(
                                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                    "swcap").equals(reader.getName())) {
                            java.lang.String content = reader.getElementText();

                            object.setSwcap(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                                    content));

                            reader.next();
                        } // End of if for expected property start element

                        else
                         if (reader.isStartElement() &&
                                new javax.xml.namespace.QName(
                                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                    "gpid").equals(reader.getName())) {
                            java.lang.String content = reader.getElementText();

                            object.setGpid(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                                    content));

                            reader.next();
                        } // End of if for expected property start element

                        else
                         if (reader.isStartElement() &&
                                new javax.xml.namespace.QName(
                                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                    "vtag").equals(reader.getName())) {
                            java.lang.String content = reader.getElementText();

                            object.setVtag(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
                                    content));

                            reader.next();
                        } // End of if for expected property start element

                        else
                         if (reader.isStartElement() &&
                                new javax.xml.namespace.QName(
                                    "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                    "exclude").equals(reader.getName())) {
                            // Process the array and step past its final element's end.
                            list8.add(edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude.Factory.parse(
                                    reader));

                            //loop until we find a start element that is not part of this array
                            boolean loopDone8 = false;

                            while (!loopDone8) {
                                // We should be at the end element, but make sure
                                while (!reader.isEndElement())
                                    reader.next();

                                // Step out of this element
                                reader.next();

                                // Step to next element event.
                                while (!reader.isStartElement() &&
                                        !reader.isEndElement())
                                    reader.next();

                                if (reader.isEndElement()) {
                                    //two continuous end elements means we are exiting the xml structure
                                    loopDone8 = true;
                                } else {
                                    if (new javax.xml.namespace.QName(
                                                "http://hopi.internet2.edu/DRAGON/TERCE/RCE",
                                                "exclude").equals(
                                                reader.getName())) {
                                        list8.add(edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude.Factory.parse(
                                                reader));
                                    } else {
                                        loopDone8 = true;
                                    }
                                }
                            }

                            // call the converter utility  to convert and set the array
                            object.setExclude((edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude[]) org.apache.axis2.databinding.utils.ConverterUtil.convertToArray(
                                    edu.internet2.hopi.dragon.terce.ws.types.rce.Exclude.class,
                                    list8));
                        } // End of if for expected property start element

                        else {
                            // A start element we are not expecting indicates an invalid parameter was passed
                            throw new org.apache.axis2.databinding.ADBException(
                                "Unexpected subelement " +
                                reader.getLocalName());
                        }
                    } else {
                        reader.next();
                    }
                } // end of while loop
            } catch (javax.xml.stream.XMLStreamException e) {
                throw new java.lang.Exception(e);
            }

            return object;
        }
    } //end of factory class
}

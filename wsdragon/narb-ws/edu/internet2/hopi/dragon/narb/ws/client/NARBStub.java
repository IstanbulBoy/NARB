
/**
 * NARBStub.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis2 version: 1.1.1 Jan 09, 2007 (06:20:51 LKT)
 */
package edu.internet2.hopi.dragon.narb.ws.client;



/*
 *  NARBStub java implementation
 */


public class NARBStub extends org.apache.axis2.client.Stub
{
	protected org.apache.axis2.description.AxisOperation[] _operations;

	//hashmaps to keep the fault mapping
	private java.util.HashMap faultExeptionNameMap = new java.util.HashMap();
	private java.util.HashMap faultExeptionClassNameMap = new java.util.HashMap();
	private java.util.HashMap faultMessageMap = new java.util.HashMap();


	private void populateAxisService() throws org.apache.axis2.AxisFault {

		//creating the Service with a unique name
		_service = new org.apache.axis2.description.AxisService("NARB" + this.hashCode());



		//creating the operations
		org.apache.axis2.description.AxisOperation __operation;



		_operations = new org.apache.axis2.description.AxisOperation[1];

		__operation = new org.apache.axis2.description.OutInAxisOperation();


		__operation.setName(new javax.xml.namespace.QName("", "findPath"));
		_service.addOperation(__operation);



		_operations[0]=__operation;


	}

	//populates the faults
	private void populateFaults(){

		faultExeptionNameMap.put( new javax.xml.namespace.QName(
				"http://hopi.internet2.edu/DRAGON/NARB/WS",
		"NARBFault"),
		"edu.internet2.hopi.dragon.narb.ws.client.NARBFaultMessageException"
		);
		faultExeptionClassNameMap.put(new javax.xml.namespace.QName(
				"http://hopi.internet2.edu/DRAGON/NARB/WS",
		"NARBFault"),
		"edu.internet2.hopi.dragon.narb.ws.client.NARBFaultMessageException");
		faultMessageMap.put( new javax.xml.namespace.QName(
				"http://hopi.internet2.edu/DRAGON/NARB/WS",
		"NARBFault"),
		"edu.internet2.hopi.dragon.narb.ws.client.NARBStub$NARBFault"
		);



	}

	/**
    Constructor that takes in a configContext
	 */
	public NARBStub(org.apache.axis2.context.ConfigurationContext configurationContext,
			java.lang.String targetEndpoint)
	throws org.apache.axis2.AxisFault {
		//To populate AxisService
		populateAxisService();
		populateFaults();

		_serviceClient = new org.apache.axis2.client.ServiceClient(configurationContext,_service);


		configurationContext = _serviceClient.getServiceContext().getConfigurationContext();

		_serviceClient.getOptions().setTo(new org.apache.axis2.addressing.EndpointReference(
				targetEndpoint));

		//Set the soap version
		_serviceClient.getOptions().setSoapVersionURI(org.apache.axiom.soap.SOAP12Constants.SOAP_ENVELOPE_NAMESPACE_URI);


	}

	/**
	 * Default Constructor
	 */
	public NARBStub() throws org.apache.axis2.AxisFault {

		this("https://hopibruw.internet2.edu/axis2/services/DRAGON/NARB" );

	}

	/**
	 * Constructor taking the target endpoint
	 */
	public NARBStub(java.lang.String targetEndpoint) throws org.apache.axis2.AxisFault {
		this(null,targetEndpoint);
	}




	/**
	 * Auto generated method signature

	 * @param param0

	 */
	public edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponse findPath(

			edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPath param0)
	throws java.rmi.RemoteException


	,edu.internet2.hopi.dragon.narb.ws.client.NARBFaultMessageException{
		try{
			org.apache.axis2.client.OperationClient _operationClient = _serviceClient.createClient(_operations[0].getName());
			_operationClient.getOptions().setAction("http://hopi.internet2.edu/DRAGON/NARB/WS/findPath");
			_operationClient.getOptions().setExceptionToBeThrownOnSOAPFault(true);



			// create SOAP envelope with that payload
			org.apache.axiom.soap.SOAPEnvelope env = null;

			//Style is Doc.


			env = toEnvelope(getFactory(_operationClient.getOptions().getSoapVersionURI()),
					param0,
					optimizeContent(new javax.xml.namespace.QName("",
							"findPath")));

			//adding SOAP headers
			_serviceClient.addHeadersToEnvelope(env);
			// create message context with that soap envelope
			org.apache.axis2.context.MessageContext _messageContext = new org.apache.axis2.context.MessageContext() ;
			_messageContext.setEnvelope(env);

			// add the message contxt to the operation client
			_operationClient.addMessageContext(_messageContext);

			//execute the operation client
			_operationClient.execute(true);


			org.apache.axis2.context.MessageContext _returnMessageContext = _operationClient.getMessageContext(
					org.apache.axis2.wsdl.WSDLConstants.MESSAGE_LABEL_IN_VALUE);
			org.apache.axiom.soap.SOAPEnvelope _returnEnv = _returnMessageContext.getEnvelope();


			java.lang.Object object = fromOM(
					_returnEnv.getBody().getFirstElement() ,
					edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponse.class,
					getEnvelopeNamespaces(_returnEnv));
			_messageContext.getTransportOut().getSender().cleanup(_messageContext);
			return (edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponse)object;

		}catch(org.apache.axis2.AxisFault f){
			org.apache.axiom.om.OMElement faultElt = f.getDetail();
			if (faultElt!=null){
				if (faultExeptionNameMap.containsKey(faultElt.getQName())){
					//make the fault by reflection
					try{
						java.lang.String exceptionClassName = (java.lang.String)faultExeptionClassNameMap.get(faultElt.getQName());
						java.lang.Class exceptionClass = java.lang.Class.forName(exceptionClassName);
						java.lang.Exception ex=
							(java.lang.Exception) exceptionClass.newInstance();
						//message class
						java.lang.String messageClassName = (java.lang.String)faultMessageMap.get(faultElt.getQName());
						java.lang.Class messageClass = java.lang.Class.forName(messageClassName);
						java.lang.Object messageObject = fromOM(faultElt,messageClass,null);
						java.lang.reflect.Method m = exceptionClass.getMethod("setFaultMessage",
								new java.lang.Class[]{messageClass});
						m.invoke(ex,new java.lang.Object[]{messageObject});

						if (ex instanceof edu.internet2.hopi.dragon.narb.ws.client.NARBFaultMessageException){
							throw (edu.internet2.hopi.dragon.narb.ws.client.NARBFaultMessageException)ex;
						}


						throw new java.rmi.RemoteException(ex.getMessage(), ex);
					}catch(java.lang.ClassCastException e){
						// we cannot intantiate the class - throw the original Axis fault
						throw f;
					} catch (java.lang.ClassNotFoundException e) {
						// we cannot intantiate the class - throw the original Axis fault
						throw f;
					}catch (java.lang.NoSuchMethodException e) {
						// we cannot intantiate the class - throw the original Axis fault
						throw f;
					} catch (java.lang.reflect.InvocationTargetException e) {
						// we cannot intantiate the class - throw the original Axis fault
						throw f;
					}  catch (java.lang.IllegalAccessException e) {
						// we cannot intantiate the class - throw the original Axis fault
						throw f;
					}   catch (java.lang.InstantiationException e) {
						// we cannot intantiate the class - throw the original Axis fault
						throw f;
					}
				}else{
					throw f;
				}
			}else{
				throw f;
			}
		}
	}


	/**
	 *  A utility method that copies the namepaces from the SOAPEnvelope
	 */
	private java.util.Map getEnvelopeNamespaces(org.apache.axiom.soap.SOAPEnvelope env){
		java.util.Map returnMap = new java.util.HashMap();
		java.util.Iterator namespaceIterator = env.getAllDeclaredNamespaces();
		while (namespaceIterator.hasNext()) {
			org.apache.axiom.om.OMNamespace ns = (org.apache.axiom.om.OMNamespace) namespaceIterator.next();
			returnMap.put(ns.getPrefix(),ns.getNamespaceURI());
		}
		return returnMap;
	}



	private javax.xml.namespace.QName[] opNameArray = null;
	private boolean optimizeContent(javax.xml.namespace.QName opName) {


		if (opNameArray == null) {
			return false;
		}
		for (int i = 0; i < opNameArray.length; i++) {
			if (opName.equals(opNameArray[i])) {
				return true;   
			}
		}
		return false;
	}
	//https://hopibruw.internet2.edu/axis2/services/DRAGON/NARB
	public static class Path
	implements org.apache.axis2.databinding.ADBBean{
		/* This type was generated from the piece of schema that had
                name = path
                Namespace URI = http://hopi.internet2.edu/DRAGON/NARB/WS
                Namespace Prefix = ns1
		 */


		/**
		 * field for Hops
		 */

		protected HopList localHops ;


		/**
		 * Auto generated getter method
		 * @return HopList
		 */
		public  HopList getHops(){
			return localHops;
		}



		/**
		 * Auto generated setter method
		 * @param param Hops
		 */
		public void setHops(HopList param){

			this.localHops=param;


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
		 * field for AvailableVtags
		 */

		protected VtagList localAvailableVtags ;

		/*  This tracker boolean wil be used to detect whether the user called the set method
		 *   for this attribute. It will be used to determine whether to include this field
		 *   in the serialized XML
		 */
		protected boolean localAvailableVtagsTracker = false ;


		/**
		 * Auto generated getter method
		 * @return VtagList
		 */
		public  VtagList getAvailableVtags(){
			return localAvailableVtags;
		}



		/**
		 * Auto generated setter method
		 * @param param AvailableVtags
		 */
		public void setAvailableVtags(VtagList param){

			if (param != null){
				//update the setting tracker
				localAvailableVtagsTracker = true;
			} else {
				localAvailableVtagsTracker = false;

			}

			this.localAvailableVtags=param;


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



					if (localHops==null){
						throw new RuntimeException("hops cannot be null!!");
					}
					localHops.getOMDataSource(
							new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","hops"),
							factory).serialize(xmlWriter);
					if (localVtagTracker){
						namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
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
					} if (localAvailableVtagsTracker){
						if (localAvailableVtags==null){
							throw new RuntimeException("availableVtags cannot be null!!");
						}
						localAvailableVtags.getOMDataSource(
								new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","availableVtags"),
								factory).serialize(xmlWriter);
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


			elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
			"hops"));


			if (localHops==null){
				throw new RuntimeException("hops cannot be null!!");
			}
			elementList.add(localHops);
			if (localVtagTracker){
				elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
				"vtag"));

				if (localVtag != null){
					elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localVtag));
				} else {
					throw new RuntimeException("vtag cannot be null!!");
				}
			} if (localAvailableVtagsTracker){
				elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
				"availableVtags"));


				if (localAvailableVtags==null){
					throw new RuntimeException("availableVtags cannot be null!!");
				}
				elementList.add(localAvailableVtags);
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
			public static Path parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
				Path object = new Path();
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
							if (!"path".equals(type)){
								//find namespace for the prefix
								java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
								return (Path)ExtensionMapper.getTypeObject(
										nsUri,type,reader);
							}

						}

					}



					// Note all attributes that were handled. Used to differ normal attributes
					// from anyAttributes.
					java.util.Vector handledAttributes = new java.util.Vector();


					reader.next();


					while (!reader.isStartElement() && !reader.isEndElement()) reader.next();

					if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","hops").equals(reader.getName())){

						object.setHops(HopList.Factory.parse(reader));

						reader.next();

					}  // End of if for expected property start element

					else{
						// A start element we are not expecting indicates an invalid parameter was passed
						throw new java.lang.RuntimeException("Unexpected subelement " + reader.getLocalName());
					}


					while (!reader.isStartElement() && !reader.isEndElement()) reader.next();

					if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","vtag").equals(reader.getName())){

						java.lang.String content = reader.getElementText();

						object.setVtag(
								org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));

						reader.next();

					}  // End of if for expected property start element


					while (!reader.isStartElement() && !reader.isEndElement()) reader.next();

					if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","availableVtags").equals(reader.getName())){

						object.setAvailableVtags(VtagList.Factory.parse(reader));

						reader.next();

					}  // End of if for expected property start element

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


	public static class ExtensionMapper{

		public static java.lang.Object getTypeObject(java.lang.String namespaceURI,
				java.lang.String typeName,
				javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{


			if (
					"http://hopi.internet2.edu/DRAGON/NARB/WS".equals(namespaceURI) &&
					"findPathContent".equals(typeName)){

				return  FindPathContent.Factory.parse(reader);


			}


			if (
					"http://hopi.internet2.edu/DRAGON/NARB/WS".equals(namespaceURI) &&
					"vtagList".equals(typeName)){

				return  VtagList.Factory.parse(reader);


			}


			if (
					"http://hopi.internet2.edu/DRAGON/NARB/WS".equals(namespaceURI) &&
					"path".equals(typeName)){

				return  Path.Factory.parse(reader);


			}


			if (
					"http://hopi.internet2.edu/DRAGON/NARB/WS".equals(namespaceURI) &&
					"findPathResponseContent".equals(typeName)){

				return  FindPathResponseContent.Factory.parse(reader);


			}


			if (
					"http://hopi.internet2.edu/DRAGON/NARB/WS".equals(namespaceURI) &&
					"hop".equals(typeName)){

				return  Hop.Factory.parse(reader);


			}


			if (
					"http://hopi.internet2.edu/DRAGON/NARB/WS".equals(namespaceURI) &&
					"exclude".equals(typeName)){

				return  Exclude.Factory.parse(reader);


			}


			if (
					"http://hopi.internet2.edu/DRAGON/NARB/WS".equals(namespaceURI) &&
					"hopList".equals(typeName)){

				return  HopList.Factory.parse(reader);


			}


			throw new java.lang.RuntimeException("Unsupported type " + namespaceURI + " " + typeName);
		}

	}

	public static class NARBFault
	implements org.apache.axis2.databinding.ADBBean{

		public static final javax.xml.namespace.QName MY_QNAME = new javax.xml.namespace.QName(
				"http://hopi.internet2.edu/DRAGON/NARB/WS",
				"NARBFault",
		"ns1");



		/**
		 * field for Msg
		 */

		protected java.lang.String localMsg ;


		/**
		 * Auto generated getter method
		 * @return java.lang.String
		 */
		public  java.lang.String getMsg(){
			return localMsg;
		}



		/**
		 * Auto generated setter method
		 * @param param Msg
		 */
		public void setMsg(java.lang.String param){

			this.localMsg=param;


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
					MY_QNAME,factory,dataSource);

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



					namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
					if (! namespace.equals("")) {
						prefix = xmlWriter.getPrefix(namespace);

						if (prefix == null) {
							prefix = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

							xmlWriter.writeStartElement(prefix,"msg", namespace);
							xmlWriter.writeNamespace(prefix, namespace);
							xmlWriter.setPrefix(prefix, namespace);

						} else {
							xmlWriter.writeStartElement(namespace,"msg");
						}

					} else {
						xmlWriter.writeStartElement("msg");
					}


					if (localMsg==null){
						// write the nil attribute

						throw new RuntimeException("msg cannot be null!!");

					}else{


						xmlWriter.writeCharacters(localMsg);

					}

					xmlWriter.writeEndElement();


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


			elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
			"msg"));

			if (localMsg != null){
				elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localMsg));
			} else {
				throw new RuntimeException("msg cannot be null!!");
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
			public static NARBFault parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
				NARBFault object = new NARBFault();
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
							if (!"NARBFault".equals(type)){
								//find namespace for the prefix
								java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
								return (NARBFault)ExtensionMapper.getTypeObject(
										nsUri,type,reader);
							}

						}

					}



					// Note all attributes that were handled. Used to differ normal attributes
					// from anyAttributes.
					java.util.Vector handledAttributes = new java.util.Vector();


					reader.next();


					while (!reader.isStartElement() && !reader.isEndElement()) reader.next();

					if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","msg").equals(reader.getName())){

						java.lang.String content = reader.getElementText();

						object.setMsg(
								org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));

						reader.next();

					}  // End of if for expected property start element

					else{
						// A start element we are not expecting indicates an invalid parameter was passed
						throw new java.lang.RuntimeException("Unexpected subelement " + reader.getLocalName());
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


	public static class Hop
	implements org.apache.axis2.databinding.ADBBean{
		/* This type was generated from the piece of schema that had
                name = hop
                Namespace URI = http://hopi.internet2.edu/DRAGON/NARB/WS
                Namespace Prefix = ns1
		 */


		/**
		 * field for String
		 */

		protected java.lang.String localString ;


		/**
		 * Auto generated getter method
		 * @return java.lang.String
		 */
		public  java.lang.String getString(){
			return localString;
		}



		/**
		 * Auto generated setter method
		 * @param param String
		 */
		public void setString(java.lang.String param){

			this.localString=param;


		}


		/**
		 * field for Loose
		 * This was an Attribute!
		 */

		protected boolean localLoose ;


		/**
		 * Auto generated getter method
		 * @return boolean
		 */
		public  boolean getLoose(){
			return localLoose;
		}



		/**
		 * Auto generated setter method
		 * @param param Loose
		 */
		public void setLoose(boolean param){

			this.localLoose=param;


		}


		/**
		 * field for Type
		 * This was an Attribute!
		 */

		protected java.lang.String localType ;


		/**
		 * Auto generated getter method
		 * @return java.lang.String
		 */
		public  java.lang.String getType(){
			return localType;
		}



		/**
		 * Auto generated setter method
		 * @param param Type
		 */
		public void setType(java.lang.String param){

			this.localType=param;


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



					// optional attribute loose
					try {
						writeAttribute("",
								"loose",
								org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLoose), xmlWriter);
					} catch (NullPointerException e) {
						// If localLoose was null
						// it can not be serialized.
					}

					// optional attribute type
					try {
						writeAttribute("",
								"type",
								org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localType), xmlWriter);
					} catch (NullPointerException e) {
						// If localType was null
						// it can not be serialized.
					}


					if (localString==null){
						// write the nil attribute

						throw new RuntimeException("string cannot be null!!");

					}else{


						xmlWriter.writeCharacters(localString);

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


			elementList.add(new javax.xml.namespace.QName("http://www.w3.org/2001/XMLSchema",
			"string"));

			if (localString != null){
				elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localString));
			} else {
				throw new RuntimeException("string cannot be null!!");
			}

			attribList.add(
					new javax.xml.namespace.QName("","loose"));
			attribList.add(
					org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLoose));

			attribList.add(
					new javax.xml.namespace.QName("","type"));
			attribList.add(
					org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localType));


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
			public static Hop parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
				Hop object = new Hop();
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
							if (!"hop".equals(type)){
								//find namespace for the prefix
								java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
								return (Hop)ExtensionMapper.getTypeObject(
										nsUri,type,reader);
							}

						}

					}



					// Note all attributes that were handled. Used to differ normal attributes
					// from anyAttributes.
					java.util.Vector handledAttributes = new java.util.Vector();

					// handle attribute "loose"
					java.lang.String tempAttribloose =
						reader.getAttributeValue("","loose");
					if (tempAttribloose!=null){
						java.lang.String content = tempAttribloose;

						object.setLoose(
								org.apache.axis2.databinding.utils.ConverterUtil.convertToBoolean(
										tempAttribloose));

					}
					handledAttributes.add("loose");

					// handle attribute "type"
					java.lang.String tempAttribtype =
						reader.getAttributeValue("","type");
					if (tempAttribtype!=null){
						java.lang.String content = tempAttribtype;

						object.setType(
								org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
										tempAttribtype));

					}
					handledAttributes.add("type");

					while(!reader.isEndElement()) {
						if (reader.isStartElement()  || reader.hasText()){

							if (reader.isStartElement()  || reader.hasText()){

								java.lang.String content = reader.getElementText();

								object.setString(
										org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));

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


	public static class Exclude
	implements org.apache.axis2.databinding.ADBBean{
		/* This type was generated from the piece of schema that had
                name = exclude
                Namespace URI = http://hopi.internet2.edu/DRAGON/NARB/WS
                Namespace Prefix = ns1
		 */


		/**
		 * field for Layer
		 * This was an Attribute!
		 */

		protected java.lang.String localLayer ;


		/**
		 * Auto generated getter method
		 * @return java.lang.String
		 */
		public  java.lang.String getLayer(){
			return localLayer;
		}



		/**
		 * Auto generated setter method
		 * @param param Layer
		 */
		public void setLayer(java.lang.String param){

			this.localLayer=param;


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



					writeAttribute("",
							"layer",
							org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLayer),
							xmlWriter);


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


			attribList.add(
					new javax.xml.namespace.QName("","layer"));
			attribList.add(
					org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localLayer));


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
			public static Exclude parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
				Exclude object = new Exclude();
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
							if (!"exclude".equals(type)){
								//find namespace for the prefix
								java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
								return (Exclude)ExtensionMapper.getTypeObject(
										nsUri,type,reader);
							}

						}

					}



					// Note all attributes that were handled. Used to differ normal attributes
					// from anyAttributes.
					java.util.Vector handledAttributes = new java.util.Vector();

					// handle attribute "layer"
					java.lang.String tempAttriblayer =
						reader.getAttributeValue("","layer");
					if (tempAttriblayer!=null){
						java.lang.String content = tempAttriblayer;

						object.setLayer(
								org.apache.axis2.databinding.utils.ConverterUtil.convertToString(
										tempAttriblayer));

					}
					handledAttributes.add("layer");


					reader.next();



				} catch (javax.xml.stream.XMLStreamException e) {
					throw new java.lang.Exception(e);
				}

				return object;
			}

		}//end of factory class



	}


	public static class FindPathResponse
	implements org.apache.axis2.databinding.ADBBean{

		public static final javax.xml.namespace.QName MY_QNAME = new javax.xml.namespace.QName(
				"http://hopi.internet2.edu/DRAGON/NARB/WS",
				"findPathResponse",
		"ns1");



		/**
		 * field for FindPathResponse
		 */

		protected FindPathResponseContent localFindPathResponse ;


		/**
		 * Auto generated getter method
		 * @return FindPathResponseContent
		 */
		public  FindPathResponseContent getFindPathResponse(){
			return localFindPathResponse;
		}



		/**
		 * Auto generated setter method
		 * @param param FindPathResponse
		 */
		public void setFindPathResponse(FindPathResponseContent param){

			this.localFindPathResponse=param;


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
					MY_QNAME,factory,dataSource);

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


					//We can safely assume an element has only one type associated with it

					if (localFindPathResponse==null){
						throw new RuntimeException("Property cannot be null!");
					}
					localFindPathResponse.getOMDataSource(
							MY_QNAME,
							factory).serialize(xmlWriter);


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




			//We can safely assume an element has only one type associated with it
			return localFindPathResponse.getPullParser(MY_QNAME);

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
			public static FindPathResponse parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
				FindPathResponse object = new FindPathResponse();
				int event;
				try {

					while (!reader.isStartElement() && !reader.isEndElement())
						reader.next();




					// Note all attributes that were handled. Used to differ normal attributes
					// from anyAttributes.
					java.util.Vector handledAttributes = new java.util.Vector();

					while(!reader.isEndElement()) {
						if (reader.isStartElement() ){

							if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","findPathResponse").equals(reader.getName())){

								object.setFindPathResponse(FindPathResponseContent.Factory.parse(reader));

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


	public static class VtagList
	implements org.apache.axis2.databinding.ADBBean{
		/* This type was generated from the piece of schema that had
                name = vtagList
                Namespace URI = http://hopi.internet2.edu/DRAGON/NARB/WS
                Namespace Prefix = ns1
		 */


		/**
		 * field for Vtag
		 * This was an Array!
		 */

		protected java.lang.String[] localVtag ;


		/**
		 * Auto generated getter method
		 * @return java.lang.String[]
		 */
		public  java.lang.String[] getVtag(){
			return localVtag;
		}






		/**
		 * validate the array for Vtag
		 */
		protected void validateVtag(java.lang.String[] param){

			if ((param != null) && (param.length < 1)){
				throw new java.lang.RuntimeException();
			}

		}


		/**
		 * Auto generated setter method
		 * @param param Vtag
		 */
		public void setVtag(java.lang.String[] param){

			validateVtag(param);


			this.localVtag=param;
		}



		/**
		 * Auto generated add method for the array for convenience
		 * @param param java.lang.String
		 */
		public void addVtag(java.lang.String param){
			if (localVtag == null){
				localVtag = new java.lang.String[]{};
			}



			java.util.List list =
				org.apache.axis2.databinding.utils.ConverterUtil.toList(localVtag);
			list.add(param);
			this.localVtag =
				(java.lang.String[])list.toArray(
						new java.lang.String[list.size()]);

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



					if (localVtag!=null) {
						namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
						boolean emptyNamespace = namespace == null || namespace.length() == 0;
						prefix =  emptyNamespace ? null : xmlWriter.getPrefix(namespace);
						for (int i = 0;i < localVtag.length;i++){

							if (localVtag[i] != null){
								if (!emptyNamespace) {
									if (prefix == null) {
										String prefix2 = org.apache.axis2.databinding.utils.BeanUtil.getUniquePrefix();

										xmlWriter.writeStartElement(prefix2,"vtag", namespace);
										xmlWriter.writeNamespace(prefix2, namespace);
										xmlWriter.setPrefix(prefix2, namespace);

									} else {
										xmlWriter.writeStartElement(namespace,"vtag");
									}

								} else {
									xmlWriter.writeStartElement("vtag");
								}

								xmlWriter.writeCharacters(localVtag[i]);

								xmlWriter.writeEndElement();

							} else {

								throw new RuntimeException("vtag cannot be null!!");

							}

						}
					} else {

						throw new RuntimeException("vtag cannot be null!!");

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


			if (localVtag!=null){
				for (int i = 0;i < localVtag.length;i++){

					if (localVtag[i] != null){
						elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
								"vtag"));
						elementList.add(
								org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localVtag[i]));
					} else {

						throw new RuntimeException("vtag cannot be null!!");

					}


				}
			} else {

				throw new RuntimeException("vtag cannot be null!!");

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
			public static VtagList parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
				VtagList object = new VtagList();
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
							if (!"vtagList".equals(type)){
								//find namespace for the prefix
								java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
								return (VtagList)ExtensionMapper.getTypeObject(
										nsUri,type,reader);
							}

						}

					}



					// Note all attributes that were handled. Used to differ normal attributes
					// from anyAttributes.
					java.util.Vector handledAttributes = new java.util.Vector();


					reader.next();

					java.util.ArrayList list1 = new java.util.ArrayList();


					while (!reader.isStartElement() && !reader.isEndElement()) reader.next();

					if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","vtag").equals(reader.getName())){



						// Process the array and step past its final element's end.
						list1.add(reader.getElementText());

						//loop until we find a start element that is not part of this array
						boolean loopDone1 = false;
						while(!loopDone1){
							// Ensure we are at the EndElement
							while (!reader.isEndElement()){
								reader.next();
							}
							// Step out of this element
							reader.next();
							// Step to next element event.
							while (!reader.isStartElement() && !reader.isEndElement())
								reader.next();
							if (reader.isEndElement()){
								//two continuous end elements means we are exiting the xml structure
								loopDone1 = true;
							} else {
								if (new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","vtag").equals(reader.getName())){
									list1.add(reader.getElementText());

								}else{
									loopDone1 = true;
								}
							}
						}
						// call the converter utility  to convert and set the array

						object.setVtag((java.lang.String[])
								list1.toArray(new java.lang.String[list1.size()]));

					}  // End of if for expected property start element

					else{
						// A start element we are not expecting indicates an invalid parameter was passed
						throw new java.lang.RuntimeException("Unexpected subelement " + reader.getLocalName());
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


	public static class FindPathContent
	implements org.apache.axis2.databinding.ADBBean{
		/* This type was generated from the piece of schema that had
                name = findPathContent
                Namespace URI = http://hopi.internet2.edu/DRAGON/NARB/WS
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

		protected Exclude[] localExclude ;

		/*  This tracker boolean wil be used to detect whether the user called the set method
		 *   for this attribute. It will be used to determine whether to include this field
		 *   in the serialized XML
		 */
		protected boolean localExcludeTracker = false ;


		/**
		 * Auto generated getter method
		 * @return Exclude[]
		 */
		public  Exclude[] getExclude(){
			return localExclude;
		}






		/**
		 * validate the array for Exclude
		 */
		protected void validateExclude(Exclude[] param){

		}


		/**
		 * Auto generated setter method
		 * @param param Exclude
		 */
		public void setExclude(Exclude[] param){

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
		 * @param param Exclude
		 */
		public void addExclude(Exclude param){
			if (localExclude == null){
				localExclude = new Exclude[]{};
			}


			//update the setting tracker
			localExcludeTracker = true;


			java.util.List list =
				org.apache.axis2.databinding.utils.ConverterUtil.toList(localExclude);
			list.add(param);
			this.localExclude =
				(Exclude[])list.toArray(
						new Exclude[list.size()]);

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

					namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
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

					namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
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
						namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
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
						namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
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
						namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
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
						namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
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
						namespace = "http://hopi.internet2.edu/DRAGON/NARB/WS";
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
											new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","exclude"),
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


			elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
			"srcHost"));

			if (localSrcHost != null){
				elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localSrcHost));
			} else {
				throw new RuntimeException("srcHost cannot be null!!");
			}

			elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
			"dstHost"));

			if (localDstHost != null){
				elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localDstHost));
			} else {
				throw new RuntimeException("dstHost cannot be null!!");
			}
			if (localBandwidthTracker){
				elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
				"bandwidth"));

				elementList.add(
						org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localBandwidth));
			} if (localEncodingTracker){
				elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
				"encoding"));

				if (localEncoding != null){
					elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localEncoding));
				} else {
					throw new RuntimeException("encoding cannot be null!!");
				}
			} if (localSwcapTracker){
				elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
				"swcap"));

				if (localSwcap != null){
					elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localSwcap));
				} else {
					throw new RuntimeException("swcap cannot be null!!");
				}
			} if (localGpidTracker){
				elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
				"gpid"));

				if (localGpid != null){
					elementList.add(org.apache.axis2.databinding.utils.ConverterUtil.convertToString(localGpid));
				} else {
					throw new RuntimeException("gpid cannot be null!!");
				}
			} if (localVtagTracker){
				elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
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
							elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
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
								return (FindPathContent)ExtensionMapper.getTypeObject(
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

							if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","srcHost").equals(reader.getName())){

								java.lang.String content = reader.getElementText();

								object.setSrcHost(
										org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));

								reader.next();

							}  // End of if for expected property start element

							else

								if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","dstHost").equals(reader.getName())){

									java.lang.String content = reader.getElementText();

									object.setDstHost(
											org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));

									reader.next();

								}  // End of if for expected property start element

								else

									if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","bandwidth").equals(reader.getName())){

										java.lang.String content = reader.getElementText();

										object.setBandwidth(
												org.apache.axis2.databinding.utils.ConverterUtil.convertToInt(content));

										reader.next();

									}  // End of if for expected property start element

									else

										if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","encoding").equals(reader.getName())){

											java.lang.String content = reader.getElementText();

											object.setEncoding(
													org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));

											reader.next();

										}  // End of if for expected property start element

										else

											if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","swcap").equals(reader.getName())){

												java.lang.String content = reader.getElementText();

												object.setSwcap(
														org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));

												reader.next();

											}  // End of if for expected property start element

											else

												if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","gpid").equals(reader.getName())){

													java.lang.String content = reader.getElementText();

													object.setGpid(
															org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));

													reader.next();

												}  // End of if for expected property start element

												else

													if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","vtag").equals(reader.getName())){

														java.lang.String content = reader.getElementText();

														object.setVtag(
																org.apache.axis2.databinding.utils.ConverterUtil.convertToString(content));

														reader.next();

													}  // End of if for expected property start element

													else

														if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","exclude").equals(reader.getName())){



															// Process the array and step past its final element's end.
															list8.add(Exclude.Factory.parse(reader));

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
																	if (new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","exclude").equals(reader.getName())){
																		list8.add(Exclude.Factory.parse(reader));

																	}else{
																		loopDone8 = true;
																	}
																}
															}
															// call the converter utility  to convert and set the array

															object.setExclude((Exclude[])
																	org.apache.axis2.databinding.utils.ConverterUtil.convertToArray(
																			Exclude.class,
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


	public static class FindPath
	implements org.apache.axis2.databinding.ADBBean{

		public static final javax.xml.namespace.QName MY_QNAME = new javax.xml.namespace.QName(
				"http://hopi.internet2.edu/DRAGON/NARB/WS",
				"findPath",
		"ns1");



		/**
		 * field for FindPath
		 */

		protected FindPathContent localFindPath ;


		/**
		 * Auto generated getter method
		 * @return FindPathContent
		 */
		public  FindPathContent getFindPath(){
			return localFindPath;
		}



		/**
		 * Auto generated setter method
		 * @param param FindPath
		 */
		public void setFindPath(FindPathContent param){

			this.localFindPath=param;


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
					MY_QNAME,factory,dataSource);

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


					//We can safely assume an element has only one type associated with it

					if (localFindPath==null){
						throw new RuntimeException("Property cannot be null!");
					}
					localFindPath.getOMDataSource(
							MY_QNAME,
							factory).serialize(xmlWriter);


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




			//We can safely assume an element has only one type associated with it
			return localFindPath.getPullParser(MY_QNAME);

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
			public static FindPath parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
				FindPath object = new FindPath();
				int event;
				try {

					while (!reader.isStartElement() && !reader.isEndElement())
						reader.next();




					// Note all attributes that were handled. Used to differ normal attributes
					// from anyAttributes.
					java.util.Vector handledAttributes = new java.util.Vector();

					while(!reader.isEndElement()) {
						if (reader.isStartElement() ){

							if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","findPath").equals(reader.getName())){

								object.setFindPath(FindPathContent.Factory.parse(reader));

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


	public static class FindPathResponseContent
	implements org.apache.axis2.databinding.ADBBean{
		/* This type was generated from the piece of schema that had
                name = findPathResponseContent
                Namespace URI = http://hopi.internet2.edu/DRAGON/NARB/WS
                Namespace Prefix = ns1
		 */


		/**
		 * field for Path
		 */

		protected Path localPath ;


		/**
		 * Auto generated getter method
		 * @return Path
		 */
		public  Path getPath(){
			return localPath;
		}



		/**
		 * Auto generated setter method
		 * @param param Path
		 */
		public void setPath(Path param){

			this.localPath=param;


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



					if (localPath==null){
						throw new RuntimeException("path cannot be null!!");
					}
					localPath.getOMDataSource(
							new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","path"),
							factory).serialize(xmlWriter);


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


			elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
			"path"));


			if (localPath==null){
				throw new RuntimeException("path cannot be null!!");
			}
			elementList.add(localPath);


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
			public static FindPathResponseContent parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
				FindPathResponseContent object = new FindPathResponseContent();
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
							if (!"findPathResponseContent".equals(type)){
								//find namespace for the prefix
								java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
								return (FindPathResponseContent)ExtensionMapper.getTypeObject(
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

							if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","path").equals(reader.getName())){

								object.setPath(Path.Factory.parse(reader));

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


	public static class HopList
	implements org.apache.axis2.databinding.ADBBean{
		/* This type was generated from the piece of schema that had
                name = hopList
                Namespace URI = http://hopi.internet2.edu/DRAGON/NARB/WS
                Namespace Prefix = ns1
		 */


		/**
		 * field for Hop
		 * This was an Array!
		 */

		protected Hop[] localHop ;


		/**
		 * Auto generated getter method
		 * @return Hop[]
		 */
		public  Hop[] getHop(){
			return localHop;
		}






		/**
		 * validate the array for Hop
		 */
		protected void validateHop(Hop[] param){

			if ((param != null) && (param.length < 1)){
				throw new java.lang.RuntimeException();
			}

		}


		/**
		 * Auto generated setter method
		 * @param param Hop
		 */
		public void setHop(Hop[] param){

			validateHop(param);


			this.localHop=param;
		}



		/**
		 * Auto generated add method for the array for convenience
		 * @param param Hop
		 */
		public void addHop(Hop param){
			if (localHop == null){
				localHop = new Hop[]{};
			}



			java.util.List list =
				org.apache.axis2.databinding.utils.ConverterUtil.toList(localHop);
			list.add(param);
			this.localHop =
				(Hop[])list.toArray(
						new Hop[list.size()]);

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



					if (localHop!=null){
						for (int i = 0;i < localHop.length;i++){
							if (localHop[i] != null){
								localHop[i].getOMDataSource(
										new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","hop"),
										factory).serialize(xmlWriter);
							} else {

								throw new RuntimeException("hop cannot be null!!");

							}

						}
					} else {

						throw new RuntimeException("hop cannot be null!!");

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


			if (localHop!=null) {
				for (int i = 0;i < localHop.length;i++){

					if (localHop[i] != null){
						elementList.add(new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS",
								"hop"));
						elementList.add(localHop[i]);
					} else {

						throw new RuntimeException("hop cannot be null !!");

					}

				}
			} else {

				throw new RuntimeException("hop cannot be null!!");

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
			public static HopList parse(javax.xml.stream.XMLStreamReader reader) throws java.lang.Exception{
				HopList object = new HopList();
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
							if (!"hopList".equals(type)){
								//find namespace for the prefix
								java.lang.String nsUri = reader.getNamespaceContext().getNamespaceURI(nsPrefix);
								return (HopList)ExtensionMapper.getTypeObject(
										nsUri,type,reader);
							}

						}

					}



					// Note all attributes that were handled. Used to differ normal attributes
					// from anyAttributes.
					java.util.Vector handledAttributes = new java.util.Vector();


					reader.next();

					java.util.ArrayList list1 = new java.util.ArrayList();


					while (!reader.isStartElement() && !reader.isEndElement()) reader.next();

					if (reader.isStartElement() && new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","hop").equals(reader.getName())){



						// Process the array and step past its final element's end.
						list1.add(Hop.Factory.parse(reader));

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
								if (new javax.xml.namespace.QName("http://hopi.internet2.edu/DRAGON/NARB/WS","hop").equals(reader.getName())){
									list1.add(Hop.Factory.parse(reader));

								}else{
									loopDone1 = true;
								}
							}
						}
						// call the converter utility  to convert and set the array

						object.setHop((Hop[])
								org.apache.axis2.databinding.utils.ConverterUtil.convertToArray(
										Hop.class,
										list1));

					}  // End of if for expected property start element

					else{
						// A start element we are not expecting indicates an invalid parameter was passed
						throw new java.lang.RuntimeException("Unexpected subelement " + reader.getLocalName());
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


	private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPath param, boolean optimizeContent){

		return param.getOMElement(edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPath.MY_QNAME,
				org.apache.axiom.om.OMAbstractFactory.getOMFactory());


	}

	private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponse param, boolean optimizeContent){

		return param.getOMElement(edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponse.MY_QNAME,
				org.apache.axiom.om.OMAbstractFactory.getOMFactory());


	}

	private  org.apache.axiom.om.OMElement  toOM(edu.internet2.hopi.dragon.narb.ws.client.NARBStub.NARBFault param, boolean optimizeContent){

		return param.getOMElement(edu.internet2.hopi.dragon.narb.ws.client.NARBStub.NARBFault.MY_QNAME,
				org.apache.axiom.om.OMAbstractFactory.getOMFactory());


	}



	private  org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory, edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPath param, boolean optimizeContent){
		org.apache.axiom.soap.SOAPEnvelope emptyEnvelope = factory.getDefaultEnvelope();

		emptyEnvelope.getBody().addChild(param.getOMElement(edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPath.MY_QNAME,factory));

		return emptyEnvelope;
	}




	/**
	 *  get the default envelope
	 */
	 private org.apache.axiom.soap.SOAPEnvelope toEnvelope(org.apache.axiom.soap.SOAPFactory factory){
		return factory.getDefaultEnvelope();
	}


	private  java.lang.Object fromOM(
			org.apache.axiom.om.OMElement param,
			java.lang.Class type,
			java.util.Map extraNamespaces){

		try {

			if (edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPath.class.equals(type)){

				return edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPath.Factory.parse(param.getXMLStreamReaderWithoutCaching());


			}

			if (edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponse.class.equals(type)){

				return edu.internet2.hopi.dragon.narb.ws.client.NARBStub.FindPathResponse.Factory.parse(param.getXMLStreamReaderWithoutCaching());


			}

			if (edu.internet2.hopi.dragon.narb.ws.client.NARBStub.NARBFault.class.equals(type)){

				return edu.internet2.hopi.dragon.narb.ws.client.NARBStub.NARBFault.Factory.parse(param.getXMLStreamReaderWithoutCaching());


			}

		} catch (Exception e) {
			throw new RuntimeException(e);
		}
		return null;
	}




	private void setOpNameArray(){
		opNameArray = null;
	}

}

package edu.internet2.hopi.dragon.terce.ws.handlers;

import java.io.IOException;

import edu.internet2.hopi.dragon.PropertyReader;
import edu.internet2.hopi.dragon.terce.ws.service.RCEFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.service.TEDBFaultMessageException;
import edu.internet2.hopi.dragon.terce.ws.types.rce.TerceRceFault;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault;

public class TERCEHandler {
	/**
	 * Reads the properties file at $TERCEWS_HOME/terce-ws.properties
	 * @return PropertyReader object with properties from file
	 */
	public static PropertyReader createPropertyReader() throws TEDBFaultMessageException{
		PropertyReader props = null;
		try {
			props = new PropertyReader(System.getenv("TERCEWS_HOME") + "/terce-ws.properties");	
		} catch (IOException e) {
			throw TERCEHandler.generateTEDBException("IO Exception reading properties file: " + e.getMessage());
		}
		
		return props;
	}
	
	/**
	 * Sets fault message and returns an RCEFaultMessageException
	 * @param msg fault message to set in exception
	 * @return the generated exception
	 */
	public static RCEFaultMessageException generateRCEException(String msg){
   	 TerceRceFault fault = new TerceRceFault();
   	 fault.setMsg(msg);
   	 RCEFaultMessageException e = new RCEFaultMessageException();
   	 e.setFaultMessage(fault);
			
   	 return e;
    }
    
	/**
	 * Sets fault message and returns an TEDBFaultMessageException
	 * @param msg fault message to set in exception
	 * @return the generated exception
	 */
	public static TEDBFaultMessageException generateTEDBException(String msg){
   	 TerceTedbFault fault = new TerceTedbFault();
   	 fault.setMsg(msg);
   	 TEDBFaultMessageException e = new TEDBFaultMessageException();
   	 e.setFaultMessage(fault);
			
   	 return e;
    }
}

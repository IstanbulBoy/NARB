package edu.internet2.hopi.dragon.terce.ws.handlers.tedb;

import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.dynamicdb.DynamicTEDB;
import edu.internet2.hopi.dragon.terce.ws.handlers.tedb.staticdb.StaticTEDB;
import edu.internet2.hopi.dragon.terce.ws.service.TEDBFaultMessageException;

/**
 * Factory that generates classes which implement TEDBInterface. Can be extended to 
 * support multiple types of TEDBs.
 * 
 * @author Andrew Lake(alake@internet2.edu)
 *
 */	
public class TEDBFactory {
	/**
	 * Generates a class that implements TEDBInterface based on the type passed.
	 * @param tedbType the type of the TEDB module to generate
	 * @return a class that implements TEDBInterface
	 * @throws TEDBFaultMessageException
	 */
	public TEDBInterface createTEDB(String tedbType) throws TEDBFaultMessageException{
		if(tedbType.equals("static")){
			return new StaticTEDB();
		}else if(tedbType.equals("dynamic")){
			return new DynamicTEDB();
		}else{
			throw new TEDBFaultMessageException("Unsupported RCE type configured");
		}
	}

}

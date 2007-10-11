package edu.internet2.hopi.dragon.terce.ws.handlers.tedb;

import edu.internet2.hopi.dragon.terce.ws.service.TEDBFaultMessage;
import edu.internet2.hopi.dragon.terce.ws.types.tedb.TerceTedbFault;


/**
 * Class extended to create frontend to an underlying traffic engineering database.
 * Should be combined with TEDBInterface
 *
 * @author Andrew Lake(alake@internet2.edu)
 *
 */
public abstract class TEDB {
    /**
     * Sets fault message and returns an TEDBFaultMessage
     * @param msg fault message to set in exception
     * @return the generated exception
     */
    protected TEDBFaultMessage generateTEDBException(String msg) {
        TerceTedbFault fault = new TerceTedbFault();
        fault.setMsg(msg);

        TEDBFaultMessage e = new TEDBFaultMessage();
        e.setFaultMessage(fault);

        return e;
    }
}

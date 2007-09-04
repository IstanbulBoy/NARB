#!/bin/sh
sh /usr/local/dragon/axis2-1.3-RC2/bin/wsdl2java.sh -ss -sd -d adb -g -ns2p http://hopi.internet2.edu/DRAGON/TERCE/RCE=edu.internet2.hopi.dragon.terce.ws.types.rce,http://hopi.internet2.edu/DRAGON/TERCE/TEDB=edu.internet2.hopi.dragon.terce.ws.types.tedb  -p edu.internet2.hopi.dragon.terce.ws.service -uri resources/TERCE.wsdl

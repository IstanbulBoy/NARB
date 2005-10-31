## Automake.am for NARB server

INCLUDES = -I.

sbin_PROGRAMS = narb narb_test

##lib_LIBRARIES = libxxx.a
##libxxx_a_SOURCES = narb_xxx.c

narb_SOURCES = \
	event.cc log.cc resource.cc apiserver.cc apiclient.cc rce_apiclient.cc narb_apiserver.cc narb_apiclient.cc lsa.cc toolbox.cc lsp_broker.cc zebra_ospfclient.cc clicmd.cc cmdtree.cc cli.cc narb_config.cc dts.cc narb_main.cc

narb_test_SOURCES = \
	event.cc log.cc resource.cc apiserver.cc apiclient.cc rce_apiclient.cc narb_apiserver.cc narb_apiclient.cc lsa.cc toolbox.cc lsp_broker.cc zebra_ospfclient.cc clicmd.cc cmdtree.cc cli.cc narb_config.cc dts.cc narb_test.cc

narb_LDADD = -lc -lstdc++ 
narb_test_LDADD = -lc -lstdc++

clean-local:
	-rm -rf *.log

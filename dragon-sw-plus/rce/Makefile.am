## Automake.am for RCE server

INCLUDES = -I.

sbin_PROGRAMS = rce rce_test

##lib_LIBRARIES = libxxx.a
##libxxx_a_SOURCES = rce_xxx.c

rce_SOURCES = \
	rce_main.cc event.cc rce_log.cc resource.cc rce_api.cc rce_apiserver.cc rce_lsa.cc rce_config.cc toolbox.cc rce_pcen.cc rce_lsp.cc rce_filter.cc zebra_ospfclient.cc rce_schema.cc rce_cli.cc rce_clicmd.cc cmdtree.cc pcen_ksp.cc pcen_mrn.cc pcen_mrn_cg.cc

rce_test_SOURCES = \
	rce_test.cc event.cc rce_log.cc resource.cc rce_api.cc rce_apiserver.cc rce_lsa.cc rce_config.cc toolbox.cc rce_pcen.cc rce_lsp.cc rce_filter.cc zebra_ospfclient.cc rce_schema.cc rce_cli.cc rce_clicmd.cc cmdtree.cc pcen_ksp.cc pcen_mrn.cc pcen_mrn_cg.cc

## $(libxxx_a_SOURCES)

rce_LDADD = -lc -lstdc++
rce_test_LDADD = -lc -lstdc++

clean-local:
	-rm -rf *.log

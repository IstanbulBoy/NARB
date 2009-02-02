## Automake.am for RCE server

INCLUDES = -I.

DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"

INSTALL_SDATA=@INSTALL@ -m 600

sysconf_DATA = schema_combo.rsd.sample

sbin_PROGRAMS = rce rce_test

##lib_LIBRARIES = libxxx.a
##libxxx_a_SOURCES = rce_xxx.c

rce_SOURCES = \
	rce_main.cc event.cc rce_version.cc rce_log.cc resource.cc rce_api.cc rce_apiserver.cc rce_lsa.cc rce_config.cc toolbox.cc rce_pcen.cc rce_lsp.cc rce_filter.cc zebra_ospfclient.cc rce_schema.cc rce_cli.cc rce_clicmd.cc cmdtree.cc pcen_ksp.cc pcen_mrn.cc pcen_mrn_cg.cc pcen_dcn.cc pcen_test.cc rce_subnet.cc terce_apiclient.cc

rce_test_SOURCES = \
	rce_test.cc event.cc rce_log.cc resource.cc rce_api.cc rce_apiserver.cc rce_lsa.cc rce_config.cc toolbox.cc rce_pcen.cc rce_lsp.cc rce_filter.cc zebra_ospfclient.cc rce_schema.cc rce_cli.cc rce_clicmd.cc cmdtree.cc pcen_ksp.cc pcen_mrn.cc pcen_mrn_cg.cc pcen_dcn.cc pcen_test.cc rce_subnet.cc terce_apiclient.cc

## $(libxxx_a_SOURCES)

rce_LDADD = -lc -lstdc++ -lz -L/usr/lib -L/usr/local/lib
rce_test_LDADD = -lc -lstdc++ -lz -L/usr/lib -L/usr/local/lib

install-sysconfDATA: $(sysconf_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)
	echo " $(INSTALL_SDATA) $(sysconf_DATA) $(DESTDIR)$(sysconfdir)/$(sysconf_DATA)";
	@$(INSTALL_SDATA) $(sysconf_DATA) $(DESTDIR)$(sysconfdir)/$(sysconf_DATA);

clean-local:
	-rm -rf *.log

## Automake.am for NARB server


INCLUDES = -I. -I/usr/include -I/usr/local/include -I/usr/include/libxml2 -I/usr/local/include/libxml2

DEFS = @DEFS@ -DSYSCONFDIR=\"$(sysconfdir)/\"

INSTALL_SDATA=@INSTALL@ -m 600

sbin_PROGRAMS = narb narb_test

##lib_LIBRARIES = libxxx.a
##libxxx_a_SOURCES = narb_xxx.c

narb_SOURCES = \
	event.cc log.cc resource.cc apiserver.cc apiclient.cc rce_apiclient.cc narb_apiserver.cc narb_apiclient.cc lsa.cc toolbox.cc lsp_broker.cc zebra_ospfclient.cc clicmd.cc cmdtree.cc cli.cc narb_config.cc dts.cc narb_xmlserver.cc terce_apiclient.cc narb_main.cc

narb_test_SOURCES = \
	event.cc log.cc resource.cc apiserver.cc apiclient.cc rce_apiclient.cc narb_apiserver.cc narb_apiclient.cc lsa.cc toolbox.cc lsp_broker.cc zebra_ospfclient.cc clicmd.cc cmdtree.cc cli.cc narb_config.cc dts.cc terce_apiclient.cc narb_test.cc

narb_LDADD = -lc -lstdc++ -lxml2 -lz
narb_test_LDADD = -lc -lstdc++ -lxml2 -lz

sysconf_DATA = narb.conf.sample ospfd-intra.conf.sample ospfd-inter.conf.sample
RC_SCRIPT = run_narb.sh

install-sysconfDATA: $(sysconf_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)
	@list='$(sysconf_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_SDATA) $(srcdir)/$$p $(DESTDIR)$(sysconfdir)/$$p"; \
	    $(INSTALL_SDATA) $(srcdir)/$$p $(DESTDIR)$(sysconfdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_SDATA) $$p $(DESTDIR)$(sysconfdir)/$$p"; \
	    $(INSTALL_SDATA) $$p $(DESTDIR)$(sysconfdir)/$$p; \
	  fi; fi; \
	done
	echo "$(INSTALL) $(RC_SCRIPT) $(DESTDIR)$(bindir)/$(RC_SCRIPT)";
	@$(INSTALL) $(RC_SCRIPT) $(DESTDIR)$(bindir)/$(RC_SCRIPT);

clean-local:
	-rm -rf *.log

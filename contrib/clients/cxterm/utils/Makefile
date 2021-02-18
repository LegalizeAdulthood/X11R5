SUBDIRS = celvis cless hz2ps b2g et2hkubig5
CURRENT_DIR = ./utils

all:
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
	(cd $$i ; echo "making" all "in $(CURRENT_DIR)/$$i..."; \
	$(MAKE) $(MFLAGS)  all); \
	done

install:
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
	(cd $$i ; echo "installing" "in $(CURRENT_DIR)/$$i..."; \
	$(MAKE) $(MFLAGS) DESTDIR='$(DESTDIR)' install); \
	done

install.man:
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
	(cd $$i ; echo "installing man pages" "in $(CURRENT_DIR)/$$i..."; \
	$(MAKE) $(MFLAGS) DESTDIR='$(DESTDIR)' install.man); \
	done

clean:
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
	(cd $$i ; echo "cleaning" "in $(CURRENT_DIR)/$$i..."; \
	$(MAKE) $(MFLAGS) RM_CMD='$(RM_CMD)' clean); \
	done

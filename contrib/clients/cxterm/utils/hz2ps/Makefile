SUBDIRS = hzbdf hz2ps-2.0
CURRENT_DIR = ./utils/hz2ps

all:
	(cd hzbdf; echo "making in $(CURRENT_DIR)/hzbdf ..."; \
	$(MAKE) $(MFLAGS))
	(cd hz2ps-2.0; echo "making in $(CURRENT_DIR)/hz2ps-2.0 ..."; \
	$(MAKE) $(MFLAGS))

install:
	@echo "Goto sub-directory { $(SUBDIRS) } and do it manually"

install.man:
	@echo "Goto sub-directory { $(SUBDIRS) } and do it manually"

clean:
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
	(cd $$i ; echo "cleaning" "in $(CURRENT_DIR)/$$i..."; \
	$(MAKE) $(MFLAGS) RM_CMD='$(RM_CMD)' clean); \
	done


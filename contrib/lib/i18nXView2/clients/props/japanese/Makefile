WHOAMI			= japanese
SPECIFIC_LOCALE_DIR	= $(DESTDIR)/$(LOCALE_DIR)/$(WHOAMI)
LOCALE_FILES		= C \
			  basic_setting \
			  japanese

PATH_TO_SUBROOT		= ../../..

include $(PATH_TO_SUBROOT)/Makefile.master

install_locale: $(LOCALE_FILES)
	install -d -m 755 $(SPECIFIC_LOCALE_DIR)
	for i in $(LOCALE_FILES); do \
		(install -m 644 $$i $(SPECIFIC_LOCALE_DIR)); \
	done

SUBDIRS = depackpc packpc

all clean install::
	@dirs="$(SUBDIRS)"; \
	for i in $$dirs; do \
		if (cd $$i && $(MAKE) $@); \
		then true; else exit 1; fi; \
	done

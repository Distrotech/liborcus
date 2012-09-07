SUBDIRS = src include
ACLOCAL_AMFLAGS = -I m4

pcfiles = liborcus-@ORCUS_API_VERSION@.pc
liborcus-@ORCUS_API_VERSION@.pc: liborcus.pc
	@cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

EXTRA_DIST = \
	liborcus.pc.in

dist-hook:
	@if test -d "$(srcdir)/.git"; \
	then \
		git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog \
	else \
		echo "A git clone is required to generate a ChangeLog" >&2; \
	fi


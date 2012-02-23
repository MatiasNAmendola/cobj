all:: html

.PHONY: help build clean

SVNROOT	= http://svn.python.org/projects

SPHINXOPTS = -b $(BUILDER) -d build/doctrees -D latex_paper_size= . build/$(BUILDER)

help:
	@echo "use \`make <target>\` where <target> is one of"
	@echo "	help"
	@echo "	html"
	@echo "	clean"

checkout:
	@if [ ! -d tools/sphinx ]; then \
		echo "Checking out Sphinx..."; \
			svn checkout $(SVNROOT)/external/Sphinx-0.6.2/sphinx tools/sphinx; \
	fi
	@if [ ! -d tools/docutils ]; then \
		echo "Checking out Docutils..."; \
		svn checkout $(SVNROOT)/external/docutils-0.5/docutils tools/docutils; \
	fi
	@if [ ! -d tools/jinja2 ]; then \
		echo "Checking out Jinja..."; \
		svn checkout $(SVNROOT)/external/Jinja-2.1.1/jinja2 tools/jinja2; \
	fi
	@if [ ! -d tools/pygments ]; then \
		echo "Checking out Pygments..."; \
		svn checkout $(SVNROOT)/external/Pygments-0.11.1/pygments tools/pygments; \
	fi

build: checkout
	mkdir -p build/$(BUILDER) build/doctrees
	python tools/sphinx-build.py $(SPHINXOPTS)
	@echo

html: BUILDER = html
html: build
	@echo "Build finished. The HTML pages are in build/html."

clean:
	rm -rf build/*


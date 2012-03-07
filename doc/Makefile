all:: html

.PHONY: help build clean

SPHINXOPTS = -b $(BUILDER) -d build/doctrees -D latex_paper_size= . build/$(BUILDER)

help:
	@echo "use \`make <target>\` where <target> is one of"
	@echo "	help"
	@echo "	html"
	@echo "	clean"

install:
	@python -c 'import sphinx' &>/dev/null; \
		if [ $$? -ne 0 ]; then \
			easy_install sphinx; \
		fi

build: install
	mkdir -p build/$(BUILDER) build/doctrees
	python build.py $(SPHINXOPTS)
	@echo

html: BUILDER = html
html: build
	@echo "Build finished. The HTML pages are in build/html."

clean:
	rm -rf build/*

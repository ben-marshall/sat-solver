
.PHONY: clean

all:
	$(MAKE) -C ./build

setup:
	cd ./build/ ; \
    cmake ../

clean:
	$(MAKE) -C ./build clean

code-docs:
	doxygen doxyfile

project-docs:
	mkdocs build --clean --site-dir ./build/docs

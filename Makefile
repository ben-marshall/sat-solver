
.PHONY: clean

all:
	$(MAKE) -C ./build

setup:
	cd ./build/ ; \
    cmake ../

clean:
	rm -rf ./build/*

code-docs:
	doxygen doxyfile

project-docs:
	mkdocs build --clean --site-dir ./build/docs


.PHONY: clean

setup:
	cd ./build/ ; \
    cmake ../src/c

clean:
	rm -rf ./build/*

project-docs:
	mkdocs build --clean --site-dir ./build/docs

libsatsolver:
	$(MAKE) -C ./build satsolver

sats: libsatsolver
	$(MAKE) -C ./build sats

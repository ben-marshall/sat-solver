
# Debug or release build?
BUILD_TYPE=DEBUG

# With or without coverage flags?
WITH_COVERAGE=YES


all:
	$(MAKE) -C ./build

setup:
	cd ./build/ ; \
    cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DWITH_COVERAGE=$(WITH_COVERAGE) ../

docs:
	doxygen doxyfile
	mkdocs build --clean --site-dir ./build/docs

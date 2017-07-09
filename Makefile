
.PHONY: clean

clean:
	rm -rf ./build/*

project-docs:
	mkdocs build --clean --site-dir ./build/docs

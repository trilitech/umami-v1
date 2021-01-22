all: build

node_modules:
	yarn

.PHONY: build
build: node_modules
	BS_VSCODE=1 yarn bsb:build

.PHONY: clean
clean:
	@rm -rf node_modules

.PHONY: run
run:
	yarn electron:dev

.PHONY: update-and-run
update-and-run: build
	yarn electron:dev


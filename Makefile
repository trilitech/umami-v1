all: build

node_modules:
	yarn

.PHONY: build
build: node_modules
	BS_VSCODE=1 yarn bsb:build

.PHONY: update
update:
	yarn
	BS_VSCODE=1 yarn bsb:build

.PHONY: clean
clean:
	@rm -rf node_modules

.PHONY: run
run:
	yarn electron:dev

.PHONY: update-and-run
update-and-run: update
	yarn electron:dev


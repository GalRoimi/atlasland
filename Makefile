include dev/docker.mk

.PHONY: setup
setup: build ## Generates Atlasland docker image
	docker build -t atlasland build/

.PHONY: _build
_build: ## Builds Atlasland stages
	@./dev/build.sh "generate_docker"

.PHONY: _clean
_clean: ## Cleans environment
	@./dev/build.sh "clean"

.PHONY: run
run: ## Runs docker
	docker run -d -p 2022:22 atlasland

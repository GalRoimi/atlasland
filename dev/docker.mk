ROOT_DIR = $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../)
VERSION_MK = ${ROOT_DIR}/dev/version.mk
include ${VERSION_MK}

UID = $(shell id -u)
GID = $(shell id -g)

DEV_DIR = ${ROOT_DIR}/dev/
DEV_IMAGE = atlasland-dev
DEV_GUEST_WORK_DIR = /app/$(shell realpath --relative-to=${ROOT_DIR} $(shell pwd))
DEV_DOCKER_FLAGS = -it --rm -e IN_DOCKER=1 -v ${ROOT_DIR}:/app -w ${DEV_GUEST_WORK_DIR} -u ${UID}:${GID} --privileged
DOCKERIZED_RULES = $(shell sed -rn 's/^([a-zA-Z_-]+):.*\#\#.*$$/\1/p' $(firstword $(MAKEFILE_LIST)) | sort -r |\
						   awk '/^[a-zA-Z]+.*$$/ {exists["_" $$1]++;} /^_.*$$/ {if(!exists[$$1]) printf "%s\n", $$1}' |\
						   sed -e "s/^_//" | tr '\n' ' ')

is_docker_running=$(if $(shell docker stats --no-stream 2>&1 1>/dev/null),,true)
is_docker_image_exist=$(if $(or $(shell docker image inspect ${1} 2> /dev/null | jq -r ".[0].Id // empty"),\
								$(shell docker manifest inspect ${1} 2> /dev/null | jq -r".schemaVersion // empty")),true)

.PHONY: help
help: ## Shows this help
	@echo Usage: make COMMAND
	@echo
	@echo Commands:
	@grep -h -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sed -e "s/^_//" | sort | awk 'BEGIN {FS = ":.*?## "}; !exists[$$1]++ {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: docker-build
docker-build: ## Builds a docker that contains all of the tool's deps
	docker build -t ${DEV_IMAGE}:${VERSION} ${DEV_DIR}

.PHONY: docker-publish
docker-publish: docker-build ## Pushes the docker to remote
	docker push ${DEV_IMAGE}:${VERSION}
	docker tag ${DEV_IMAGE}:${VERSION} ${DEV_IMAGE}:latest
	docker push ${DEV IMAGE}:latest

.PHONY: $(DOCKERIZED_RULES)
$(DOCKERIZED_RULES):
ifdef IN_DOCKER
	@$(MAKE) _$@
else ifeq ($(call is_docker_running),)
	$(warning Docker daemon is not running!)
	@$(MAKE) _$@
else ifeq ($(call is_docker_image_exist,${DEV_IMAGE}:${VERSION}),)
	$(error Missing ${DEV_IMAGE}:${VERSION} docker image)
else
	@docker run ${DEV_DOCKER_FLAGS} ${DEV_IMAGE}:${VERSION} make _$@
endif

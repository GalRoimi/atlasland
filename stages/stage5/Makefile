include ../../dev/docker.mk

OUT_DIR = outputs
STAGE_NAME = no_sweat

.PHONY: _setup
_setup: ## Builds stage
	mkdir -p $(OUT_DIR)
	$(MAKE) -C $(STAGE_NAME) build
	cp $(STAGE_NAME)/bin/$(STAGE_NAME).bin $(OUT_DIR)/

.PHONY: _clean
_clean: ## Cleans stage environment
	rm -rf $(OUT_DIR)
	$(MAKE) -C $(STAGE_NAME) clean

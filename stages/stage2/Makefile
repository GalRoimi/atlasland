include ../../dev/docker.mk

OUT_DIR = outputs
STAGE_NAME = piece_of_cake

.PHONY: _setup
_setup: ## Builds stage
	mkdir -p $(OUT_DIR)
	$(MAKE) -C $(STAGE_NAME) build
	cp $(STAGE_NAME)/bin/$(STAGE_NAME) $(OUT_DIR)/
	cp $(STAGE_NAME)/bin/$(STAGE_NAME).bin $(OUT_DIR)/
	python3 $(ROOT_DIR)/dev/invalidate_elf.py $(OUT_DIR)/$(STAGE_NAME)

.PHONY: _clean
_clean: ## Cleans stage environment
	rm -rf $(OUT_DIR)
	$(MAKE) -C $(STAGE_NAME) clean

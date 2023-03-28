include ../../dev/docker.mk

OUT_DIR = outputs
STAGE_NAME = feeling_the_burn

QEMU_ROOT_DIR = $(ROOT_DIR)/modules/qemu/

.PHONY: _setup
_setup: ## Builds stage
	mkdir -p $(OUT_DIR)
	$(MAKE) -C $(STAGE_NAME) build
	cp $(STAGE_NAME)/bin/$(STAGE_NAME).bin $(OUT_DIR)/
	cp -r dev/bootstrap $(OUT_DIR)/

	git -C $(QEMU_ROOT_DIR) reset --hard
	git -C $(QEMU_ROOT_DIR) apply $(abspath dev/qemu.patch)

	cd $(QEMU_ROOT_DIR) && ./configure --target-list=arm-softmmu
	$(MAKE) -C $(QEMU_ROOT_DIR)
	mkdir -p $(OUT_DIR)/.bin/
	cp $(QEMU_ROOT_DIR)/build/qemu-system-arm $(OUT_DIR)/.bin/

	echo "export PATH=\"\$$HOME/.bin/:\$$PATH\"" > $(OUT_DIR)/.loginrc

.PHONY: _clean
_clean: ## Cleans stage environment
	rm -rf $(OUT_DIR)
	$(MAKE) -C $(STAGE_NAME) clean
	$(MAKE) -C $(QEMU_ROOT_DIR) clean

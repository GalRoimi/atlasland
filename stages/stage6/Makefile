include ../../dev/docker.mk

OUT_DIR = outputs
STAGE_NAME = butterfingers

.PHONY: _setup
_setup: ## Builds stage
	mkdir -p $(OUT_DIR)
	$(MAKE) -C $(STAGE_NAME) build

	rm -r $(STAGE_NAME)/bin/
	rm $(STAGE_NAME)/.obj/startup.o
	rm $(STAGE_NAME)/src/message.gen.h

	python3 $(ROOT_DIR)/dev/clone_env.py $(STAGE_NAME) $(OUT_DIR)/$(STAGE_NAME)

	cp $(STAGE_NAME)/Makefile $(OUT_DIR)/$(STAGE_NAME)/
	cp $(STAGE_NAME)/dev/mbedtls_config.h $(OUT_DIR)/$(STAGE_NAME)/dev/
	cp $(STAGE_NAME)/dev/ldscript.ld $(OUT_DIR)/$(STAGE_NAME)/dev/
	cp $(STAGE_NAME)/src/startup.s $(OUT_DIR)/$(STAGE_NAME)/src/
	cp -r $(STAGE_NAME)/.obj/ $(OUT_DIR)/$(STAGE_NAME)/.obj/
	rm $(OUT_DIR)/$(STAGE_NAME)/src/auth.h
	rm $(OUT_DIR)/$(STAGE_NAME)/README.md

	echo "find \$$HOME/$(STAGE_NAME)/src/ -exec touch -t 197001010000.00 {} +" > $(OUT_DIR)/.loginrc
	echo "find \$$HOME/$(STAGE_NAME)/dev/ -exec touch -t 197001010000.00 {} +" >> $(OUT_DIR)/.loginrc

	sed -i -e '/^#START_REMOVE_SECTION/,/^\#END_REMOVE_SECTION/d;' $(OUT_DIR)/$(STAGE_NAME)/Makefile
	sed -i -e "s/_build/build/" $(OUT_DIR)/$(STAGE_NAME)/Makefile
	sed -i -e "s/_clean/clean/" $(OUT_DIR)/$(STAGE_NAME)/Makefile
	sed -i -e "s/  \| \$$(MESSAGE_FILE)//" $(OUT_DIR)/$(STAGE_NAME)/Makefile
	sed -i -e "s/\$$(ROOT_DIR)\/modules\/mbedtls\//..\/mbedtls\//" $(OUT_DIR)/$(STAGE_NAME)/Makefile
	TMP=$$(mktemp tmp.XXXXXXXX); \
	awk -v RS='\n\n\n' 1 $(OUT_DIR)/$(STAGE_NAME)/Makefile > $$TMP; \
	mv $$TMP $(OUT_DIR)/$(STAGE_NAME)/Makefile; \

	cp -r ../../modules/mbedtls $(OUT_DIR)/mbedtls
	rm -rf $(OUT_DIR)/mbedtls/.git

.PHONY: _clean
_clean: ## Cleans stage environment
	rm -rf $(OUT_DIR)
	$(MAKE) -C $(STAGE_NAME) clean

MODULES = module/freertos
BOARD = edu-ciaa-nxp
MUJU = ./muju


include $(MUJU)/module/base/makefile

OUT_DIR = ./build
DOC_DIR = $(OUT_DIR)/doc

doc: | $(DOC_DIR)
	@echo "generating documentation"
	@doxygen Doxyfile

$(DOC_DIR):
	@mkdir -p $(DOC_DIR)

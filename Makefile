.PHONY: clean All

All:
	@echo "----------Building project:[ netcap - Debug ]----------"
	@"$(MAKE)" -f  "netcap.mk"
clean:
	@echo "----------Cleaning project:[ netcap - Debug ]----------"
	@"$(MAKE)" -f  "netcap.mk" clean

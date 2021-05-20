# usage:
#	make -B pkg=${pkg} env=${env}
# example:
#   make -B env=dev pkg=web

environment=local
ifneq ($(env), )
environment=$(env)
endif

ws=$(abspath $(shell pwd))
ws=.
rst=$(ws)/build


dependence=smilex taxcalcer generator_pwd
all: init $(dependence)
	@echo "===> current $@ :  ready : dependence = $^"
	
	if [ $(shell ls $(rst) | wc -l) -ne $(shell echo $(dependence)|wc -w) ]; then \
		echo "!!! something error happened"; \
	fi
	@echo "===> current $@ : end"


init:
	rm -rf $(rst);
	@if [ ! -d "$(rst)" ]; then \
		mkdir -p $(rst);\
	fi
	@echo "===> $@ : env=$(environment) end"


web_path=$(ws)/web
smilex:
	@echo "===> $@ : ready : web_path=$(web_path)/$@"
	rm -rf $(rst)/$@ && mkdir -p $(rst)/$@
	cp -rf $(web_path)/$@/* $(rst)/$@


tax_rebuild=0
taxcalcer:
	@echo "===> $@ : ready : tax_path=$(web_path)/$@"
	if [ $(tax_rebuild) -ne 0 ]; then \
		cd $(web_path)/$@ && npm run build; \
	fi
	rm -rf $(rst)/$@ && mkdir -p $(rst)/$@
	cp -rf $(web_path)/$@/build/* $(rst)/$@


generator_pwd:
	@echo "===> $@ : ready : web_path=$(web_path)/$@"
	rm -rf $(rst)/$@ && mkdir -p $(rst)/$@
	cp -rf $(web_path)/$@/* $(rst)/$@

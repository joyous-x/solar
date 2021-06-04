# usage:
#	make -B pkg=${pkg} env=${env}
# example:
#   make -B env=dev

environment=local
ifneq ($(env), )
environment=$(env)
endif

ws=$(abspath $(shell pwd))
ws=..
rst=$(ws)/package
rst_tar=myapp.tar.gz
apps=myapp


dependence=krotas blog
all: init config $(dependence)
	@echo "===> $@ : ready : env=$(environment) dependence=$^"
	
	if [ $(shell ls $(rst)/$(apps) | wc -l) -ne $(shell echo $(dependence)|wc -w) ]; then \
		echo "!!! something error happened"; exit 1; \
	fi

	rm -rf $(rst)/$(rst_tar) && tar -czf $(rst)/$(rst_tar) $(rst)/*
	if [ "$(env)"=="local" ];then \
		echo "===> current is $(env): end"; \
	fi
	@echo "===> current $@ : end"

init:
	rm -rf $(rst);
	@if [ ! -d "$(rst)" ]; then \
		mkdir -p $(rst)/envs;\
	else \
		echo "$(rst) already exists"; \
	fi
	cp -f -r $(ws)/$(apps) $(rst)/ 
	cp -f -r $(ws)/env_scripts/biz $(rst)/envs/ 
	cp -f -r $(ws)/env_scripts/env $(rst)/envs/
	cp -f -r $(ws)/env_scripts/crts $(rst)/envs/
	cp -f -r $(ws)/env_scripts/deploy.sh $(rst)/eploy.sh
	@echo "===> $@ : env=$(environment) end"

private_config=$(ws)/env_scripts/private_config
config:
	@echo "===> $@ : ready : target=$@ env=$(environment)"
	bash $(private_config)/config_table.apply.sh -f $(rst)/envs/env/nginx.conf -p $(private_config)/config_table.$(environment).yaml
	bash $(private_config)/config_table.apply.sh -f $(rst)/envs/env/docker-compose.env.yaml -p $(private_config)/config_table.$(environment).yaml
	@echo "===> $@ : env=$(environment) end"

krotas: 
	@echo "===> $@ : ready : target=$@ env=$(environment)"
	rm -rf $(rst)/$(apps)/$@
	mkdir -p $(rst)/$(apps)/$@
#	TODO: 
# 	cd $(ws)/$@/src && make -B ws=. src=$@ env=$(environment)
#	mv $(ws)/$@/src/bin/* $(rst)/bin
#   rm -rf $(ws)/$@/src/bin

web_target=web
blog:
	@echo "===> $@ : ready : target=$@ env=$(environment)"
	rm -rf $(rst)/$(apps)/$@
	mkdir -p $(rst)/$(apps)/$@
#	TODO: 

clean:
	rm -rf $(rst)
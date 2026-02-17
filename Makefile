MAKE = make

all: build_engine build_client

build_engine: 
	$(MAKE) -C engine all

build_client: build_engine
	$(MAKE) -C client all

clean: clean_engine clean_client

clean_engine: 
	$(MAKE) -C engine clean

clean_client: 
	$(MAKE) -C client clean

.PHONY: clean_engine clean_client


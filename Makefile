include $(API_ROOT)/ComDrv.mk

INCLUDE = $(COMDRV_INCL) -I.

# GCOV flags + libs
#CXXFLAGS += -fprofile-arcs -ftest-coverage -O0 -fPIC
CXXFLAGS += -std=c++11 -ggdb -rdynamic -O3
#LIBS	= $(COMDRV_LIBS) $(LINKLIB) -pthread -lgcov
LIBS	= $(COMDRV_LIBS) $(LINKLIB) -pthread -lrdkafka -lz -lcppkafka

OBJS = $(COMDRV_OBJS)

DRV_NAME = WCCOAkafkaDrv
MYOBJS = kafkaDrv.o \
	kafkaHWMapper.o \
	kafkaHWService.o \
	kafkaResources.o \
	kafkaProducerFacade.o \
	kafkaConsumerFacade.o \
	kafkaMain.o

define INSTALL_BODY
	@if [[ -n "$(PVSS_PROJ_PATH)" ]]; then \
	; \
	fi;
endef

COMMON_SOURCE = $(wildcard Common/*.cxx)
COMMON_TYPES = $(COMMON_SOURCE:.cxx=.o)

TRANSFORMATIONS_SOURCE = $(wildcard Transformations/*.cxx)
TRANSFORMATIONS = $(TRANSFORMATIONS_SOURCE:.cxx=.o)


WCCOAkafkaDrv: $(MYOBJS) $(COMMON_TYPES) $(TRANSFORMATIONS)
	@rm -f addVerInfo.o
	@$(MAKE) addVerInfo.o
	$(LINK_CMD) -o $(DRV_NAME) *.o $(OBJS) $(LIBS)


installLibs:
	sudo yum install --assumeyes cyrus-sasl-gssapi cyrus-sasl-devel boost* cmake openssl-devel
	git submodule update --init --recursive
	cd ./libs/librdkafka && ./configure && make && sudo make install
	cd ./libs/cppkafka && mkdir -p build && cd build && cmake .. && make && sudo make install
	

install: WCCOAkafkaDrv
ifdef PVSS_PROJ_PATH
	@echo
	@echo "***************************************************************************************"
	@echo "Installing binaries in: $(PVSS_PROJ_PATH)"
	@echo "***************************************************************************************"
	@echo
	@cp -f $(DRV_NAME) $(PVSS_PROJ_PATH)/$(DRV_NAME)
else
	@echo
	@echo "*****************************************************************************************"
	@echo "ERROR !!!!!!!"
	@echo "You have to specified installation dir: PVSS_PROJ_PATH=<path_to_pvss_project_bin_folder>"
	@echo "*****************************************************************************************"
	@echo
endif

restart:
	pkill -9 -u $(USER) -f /$(DRV_NAME)

update: clean install restart

clean:
	@rm -f *.o $(DRV_NAME) WCCOAkafkaDrv

docs:
	doxygen Doxyfile

addVerInfo.cxx: $(API_ROOT)/include/Basics/Utilities/addVerInfo.hxx
	@cp -f $(API_ROOT)/include/Basics/Utilities/addVerInfo.hxx addVerInfo.cxx

addVerInfo.o: $(OFILES) addVerInfo.cxx


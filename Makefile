PACKAGE_NAME := srt-server_1.4.3-1
ROOT_DIR := tmp
BUILD_DIR := ${ROOT_DIR}/${PACKAGE_NAME}

.PHONY: help
help:
	@echo "Usage:"
	@echo ""
	@echo " make build: build binary"
	@echo " make package: build deb package"

.PHONY: build
build:
	@cd build; cmake ..
	@cd build; make

.PHONY: package
package: build
	@mkdir -p ${BUILD_DIR}/usr/local/bin
	@mkdir -p ${BUILD_DIR}/usr/local/include
	@mkdir -p ${BUILD_DIR}/usr/local/lib/pkgconfig
	@mkdir -p ${BUILD_DIR}/usr/local/etc
	@cp -r misc/DEBIAN ${BUILD_DIR}
	@cp build/srt-server ${BUILD_DIR}/usr/local/bin
	@cp /usr/local/bin/srt-ffplay ${BUILD_DIR}/usr/local/bin
	@cp /usr/local/bin/srt-live-transmit ${BUILD_DIR}/usr/local/bin
	@cp /usr/local/bin/srt-file-transmit ${BUILD_DIR}/usr/local/bin
	@cp -r /usr/local/include/srt ${BUILD_DIR}/usr/local/include
	@cp /usr/local/lib/libsrt.a ${BUILD_DIR}/usr/local/lib
	@cp /usr/local/lib/libsrt.so.1.4.3 ${BUILD_DIR}/usr/local/lib
	@cp /usr/local/lib/pkgconfig/haisrt.pc ${BUILD_DIR}/usr/local/lib/pkgconfig
	@cp /usr/local/lib/pkgconfig/srt.pc ${BUILD_DIR}/usr/local/lib/pkgconfig
	@cp misc/srt-server.cfg ${BUILD_DIR}/usr/local/etc
	@cd ${ROOT_DIR}; dpkg-deb --build --root-owner-group ${PACKAGE_NAME}

.PHONY: clean
clean:
	@cd build; make clean
	@rm -rf ${ROOT_DIR}

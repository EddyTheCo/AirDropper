cmake_minimum_required(VERSION 3.24)

if(NODE_JWT)
	file(WRITE qml/LocalConf.qml "pragma Singleton \n import QtQuick 2.15 \n QtObject { \n property string nodeaddr:\"https://3216aae.online-server.cloud\" \n property string jwt:\"${NODE_JWT}\"\n }" )
endif(NODE_JWT)

include(local_conf.cmake OPTIONAL)

include(FetchContent)
FetchContent_Declare(
	ccommon
	GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
	GIT_TAG main
	)
FetchContent_MakeAvailable(ccommon)
version_from_git(
	LOG       OFF
	TIMESTAMP "%Y%m%d%H%M%S"
	)
project(IOTAQml VERSION ${VERSION} LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(GNUInstallDirs)

find_package(Qt6 COMPONENTS Core Gui Quick Qml REQUIRED)

qt_standard_project_setup()

qt_add_executable(airdropper main.cpp)
FetchContent_Declare(
	MyDesigns
	GIT_REPOSITORY https://github.com/EddyTheCo/MyDesigns.git
	GIT_TAG v0.2.1
	FIND_PACKAGE_ARGS 0 CONFIG
	)
FetchContent_MakeAvailable(MyDesigns)
FetchContent_Declare(
	account
	GIT_REPOSITORY https://github.com/EddyTheCo/account.git
	GIT_TAG v0.1.1
	FIND_PACKAGE_ARGS 0 CONFIG
	)
FetchContent_MakeAvailable(account)
FetchContent_Declare(
	nodeConection
	GIT_REPOSITORY https://github.com/EddyTheCo/ConectionSettings.git
	GIT_TAG v0.1.3
	FIND_PACKAGE_ARGS 0 CONFIG
	)
FetchContent_MakeAvailable(nodeConection)

FetchContent_Declare(
	outMonitor
	GIT_REPOSITORY https://github.com/EddyTheCo/outMonitor.git
	GIT_TAG v0.1.2
	FIND_PACKAGE_ARGS 0 CONFIG
	)
FetchContent_MakeAvailable(outMonitor)

set_source_files_properties(qml/LocalConf.qml PROPERTIES
	QT_QML_SINGLETON_TYPE TRUE
	)

qt6_add_qml_module(airdropper
	URI  AirDropper
	VERSION 1.0
	SOURCES
	src/CreateDrop.cpp include/CreateDrop.hpp
	QML_FILES
	qml/BoxDrop.qml
	qml/BoxMenu.qml
	qml/ConfDrawer.qml
	qml/window.qml
	qml/LocalConf.qml
	RESOURCE_PREFIX
	"/esterVtech.com/imports"
	IMPORT_PATH ${QML_IMPORT_PATH}
	)
qt_add_resources(airdropper "airdropper.fonts"
        PREFIX "/esterVtech.com/imports/AirDropper"
	FILES
	"fonts/Anton/Anton-Regular.ttf"
	"fonts/Permanent_Marker/PermanentMarker-Regular.ttf"
	)

target_link_libraries(airdropper PRIVATE Qt::Core Qt::Gui Qt::Qml Qt::Quick)


FetchContent_Declare(
	qrCode
	GIT_REPOSITORY https://github.com/EddyTheCo/qrCode.git
	GIT_TAG v0.0.1
	FIND_PACKAGE_ARGS 0 CONFIG
	)
FetchContent_MakeAvailable(qrCode)


target_link_libraries(airdropper PRIVATE MyDesigns account nodeConection QtQrGen outMonitor)
set_target_properties(airdropper PROPERTIES
	WIN32_EXECUTABLE ON
	MACOSX_BUNDLE ON
	)
target_include_directories(airdropper PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/airdropper>)

install(TARGETS airdropper 
	BUNDLE  DESTINATION .
	DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
if(EMSCRIPTEN)
	add_custom_command(
		TARGET airdropper
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy "${CMAKE_CURRENT_BINARY_DIR}/airdropper.js" "${CMAKE_CURRENT_BINARY_DIR}/airdropper.wasm" "${CMAKE_CURRENT_BINARY_DIR}/qtloader.js" "${CMAKE_CURRENT_SOURCE_DIR}/wasm"
		)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v${SEMVER}-${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
	if(USE_THREADS)
		set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-MThread")
	endif(USE_THREADS)
	include(CPack)
endif()


aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" KCHMVIEWER_SRCS)
list(FILTER KCHMVIEWER_SRCS EXCLUDE REGEX "[^a-zA-Z0-9_\\.]*viewwindow_webkit\\.cpp")
list(FILTER KCHMVIEWER_SRCS EXCLUDE REGEX "[^a-zA-Z0-9_\\.]*dataprovider_qwebkit\\.cpp")
list(FILTER KCHMVIEWER_SRCS EXCLUDE REGEX "[^a-zA-Z0-9_\\.]*moc_.*")

file(GLOB KCHMVIEWER_UIS *.ui)
file(GLOB_RECURSE KCHMVIEWER_QRCS *.qrc)

file(GLOB KCHMVIEWER_HEADERS *.h)
list(FILTER KCHMVIEWER_HEADERS EXCLUDE REGEX "[^a-zA-Z0-9_\\.]*viewwindow_webkit\\.h")
list(FILTER KCHMVIEWER_HEADERS EXCLUDE REGEX "[^a-zA-Z0-9_\\.]*dataprovider_qwebkit\\.h")

qthelper_wrap_cpp(KCHMVIEWER_HEADER_MOCS ${KCHMVIEWER_HEADERS})

set(KCHMVIEWER_SRCS ${KCHMVIEWER_SRCS} ${KCHMVIEWER_HEADER_MOCS})
list(REMOVE_DUPLICATES KCHMVIEWER_SRCS)

add_executable(kchmviewer ${KCHMVIEWER_SRCS} ${KCHMVIEWER_UIS} ${KCHMVIEWER_QRCS})
target_link_libraries(kchmviewer
	chm
	ebook
	${LIBZIP_LIBRARY}
	Qt5::Widgets
	Qt5::WebEngine
	Qt5::WebView
	Qt5::WebEngineWidgets
	Qt5::PrintSupport
	Qt5::Xml
	Qt5::DBus
	Qt5::Network 
	)
	
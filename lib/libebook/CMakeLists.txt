
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} EBOOK_SRCS)
list(FILTER EBOOK_SRCS EXCLUDE REGEX "[^a-zA-Z0-9_\\.]*moc_.*")

file(GLOB EBOOK_HEADERS *.h)
qthelper_wrap_cpp(EBOOK_HEADER_MOCS ${EBOOK_HEADERS})

set(EBOOK_SRCS ${EBOOK_SRCS} ${EBOOK_HEADER_MOCS})
list(REMOVE_DUPLICATES EBOOK_SRCS)

add_library(ebook STATIC ${EBOOK_SRCS})
target_link_libraries(ebook chm)

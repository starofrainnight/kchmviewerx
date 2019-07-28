/***************************************************************************
 *             chm_lib.h - CHM archive manipulation routines               *
 *                           -------------------                           *
 *                                                                         *
 *  author:     Jed Wing <jedwin@ugcs.caltech.edu>                         *
 *  version:    0.3                                                        *
 *  notes:      These routines are meant for the manipulation of microsoft *
 *              .chm (compiled html help) files, but may likely be used    *
 *              for the manipulation of any ITSS archive, if ever ITSS     *
 *              archives are used for any other purpose.                   *
 *                                                                         *
 *              Note also that the section names are statically handled.   *
 *              To be entirely correct, the section names should be read   *
 *              from the section names meta-file, and then the various     *
 *              content sections and the "transforms" to apply to the data *
 *              they contain should be inferred from the section name and  *
 *              the meta-files referenced using that name; however, all of *
 *              the files I've been able to get my hands on appear to have *
 *              only two sections: Uncompressed and MSCompressed.          *
 *              Additionally, the ITSS.DLL file included with Windows does *
 *              not appear to handle any different transforms than the     *
 *              simple LZX-transform.  Furthermore, the list of transforms *
 *              to apply is broken, in that only half the required space   *
 *              is allocated for the list.  (It appears as though the      *
 *              space is allocated for ASCII strings, but the strings are  *
 *              written as unicode.  As a result, only the first half of   *
 *              the string appears.)  So this is probably not too big of   *
 *              a deal, at least until CHM v4 (MS .lit files), which also  *
 *              incorporate encryption, of some description.               *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef INCLUDED_CHMLIB_H
#define INCLUDED_CHMLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#ifdef WIN32
#include <wtypes.h>
#endif

/* the two available spaces in a CHM file                      */
/* N.B.: The format supports arbitrarily many spaces, but only */
/*       two appear to be used at present.                     */
#define CHM_UNCOMPRESSED 0
#define CHM_COMPRESSED 1

#define CHM_ENUMERATE_NORMAL 1
#define CHM_ENUMERATE_META 2
#define CHM_ENUMERATE_SPECIAL 4
#define CHM_ENUMERATE_FILES 8
#define CHM_ENUMERATE_DIRS 16

/*
chm_reader is a function that reads from an abstract reader interface (e.g. a file)
Reads len bytes from offset off and copies them into buf. Buf must be at least len in size.
Returns number of bytes read, -1 on error.
ctx is opaque object that allows different implementations */
typedef int64_t (*chm_reader)(void* ctx, void* buf, int64_t off, int64_t len);

typedef struct mem_reader_ctx {
    void* data;
    int64_t size;
} mem_reader_ctx;

void mem_reader_init(mem_reader_ctx* ctx, void* data, int64_t size);
int64_t mem_reader(void* ctx, void* buf, int64_t off, int64_t len);

typedef struct fd_reader_ctx { int fd; } fd_reader_ctx;

bool fd_reader_init(fd_reader_ctx* ctx, const char* path);
void fd_reader_close(fd_reader_ctx* ctx);
int64_t fd_reader(void* ctx, void* buf, int64_t off, int64_t len);

#ifdef WIN32
typedef struct win_reader_ctx { HANDLE fh; } win_reader_ctx;

bool win_reader_init(win_reader_ctx* ctx, const WCHAR* path);
void win_reader_close(win_reader_ctx* ctx);
int64_t win_reader(void* ctx, void* buf, int64_t off, int64_t len);
#endif

/* structure of ITSF headers */
typedef struct itsf_hdr {
    char signature[4];       /*  0 (ITSF) */
    int32_t version;         /*  4 */
    int32_t header_len;      /*  8 */
    int32_t unknown_000c;    /*  c */
    uint32_t last_modified;  /* 10 */
    uint32_t lang_id;        /* 14 */
    uint8_t dir_uuid[16];    /* 18 */
    uint8_t stream_uuid[16]; /* 28 */
    int64_t unknown_offset;  /* 38 */
    int64_t unknown_len;     /* 40 */
    int64_t dir_offset;      /* 48 */
    int64_t dir_len;         /* 50 */
    int64_t data_offset;     /* 58 (Not present before V3) */
} itsf_hdr;

/* structure of ITSP headers */
typedef struct itsp_hdr {
    char signature[4];        /*  0 (ITSP) */
    int32_t version;          /*  4 */
    int32_t header_len;       /*  8 */
    int32_t unknown_000c;     /*  c */
    uint32_t block_len;       /* 10 */
    int32_t blockidx_intvl;   /* 14 */
    int32_t index_depth;      /* 18 */
    int32_t index_root;       /* 1c */
    int32_t index_head;       /* 20 */
    int32_t unknown_0024;     /* 24 */
    uint32_t num_blocks;      /* 28 */
    int32_t unknown_002c;     /* 2c */
    uint32_t lang_id;         /* 30 */
    uint8_t system_uuid[16];  /* 34 */
    uint8_t unknown_0044[16]; /* 44 */
} itsp_hdr;

typedef struct lzxc_reset_table {
    uint32_t version;
    uint32_t block_count;
    uint32_t unknown;
    uint32_t table_offset;
    int64_t uncompressed_len;
    int64_t compressed_len;
    int64_t block_len;
} lzxc_reset_table;

typedef struct chm_entry {
    struct chm_entry* next;
    char* path;
    int64_t start;
    int64_t length;
    int space;
    int flags;
} chm_entry;

#define MAX_CACHE_BLOCKS 128

/* the structure used for chm file handles */
typedef struct chm_file {
    chm_reader read_func;
    void* read_ctx;

    itsf_hdr itsf;
    itsp_hdr itsp;

    int64_t dir_offset;
    int64_t dir_len;

    chm_entry* rt_unit;
    chm_entry* cn_unit;

    lzxc_reset_table reset_table;

    /* LZX control data */
    bool compression_enabled;
    uint32_t window_size;
    uint32_t reset_interval;
    uint32_t reset_blkcount;

    /* decompressor state */
    struct lzx_state* lzx_state;
    int lzx_last_block;
    uint8_t* lzx_last_block_data;

    /* cache for decompressed blocks */
    uint8_t* cache_blocks[MAX_CACHE_BLOCKS];
    int64_t cache_block_indices[MAX_CACHE_BLOCKS];
    int n_cache_blocks;

    chm_entry** entries;
    int n_entries;
    /* might be a partial failure i.e. might still have entries */
    bool parse_entries_failed;
} chm_file;

void chm_close(struct chm_file* h);

void chm_set_cache_size(struct chm_file* h, int nCacheBlocks);

bool chm_parse(struct chm_file* f, chm_reader read_func, void* read_ctx);

/* allow intercepting debug messages from the code */
typedef void (*dbgprintfunc)(const char* s);
void chm_set_dbgprint(dbgprintfunc f);

/* retrieve part of an entry from the archive */
int64_t chm_retrieve_entry(struct chm_file* h, chm_entry* e, unsigned char* buf, int64_t addr,
                           int64_t len);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_CHMLIB_H */

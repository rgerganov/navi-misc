/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * fidtool - C backend for working with Fast Interval Databases
 * Copyright (C) 2005 Micah Dowty <micah@navi.cx>
 *
 * The FID file format is made up of two types of pages:
 *
 *   - Level-0 (L0, or 'leaf') pages contain only timestamps.
 *     The stamps are all stored as variable-length integers.
 *     L0 pages need not be complete, but a complete page should
 *     have any unusable space padded with 0x00. All integers
 *     in an L0 page represent time deltas. The first item is
 *     relative to the page's entry in its parent L1 page,
 *     and all subsequent items are relative to the previous item.
 *
 *   - Level-1 (L1, or 'index') pages are used to quickly locate
 *     L0 pages. This works like a 2-level B*-tree in which all
 *     keys and values must be monotonically increasing. This concept
 *     could be extended to deeper trees, but 2 levels seems optimal
 *     for the real-world appliction this library was designed for.
 *
 *     Each completed L0 page is represented in the L1 page by two
 *     variable length integers, representing the total time delta
 *     and the total number of items in that page. Incomplete
 *     L0 pages have no entry in their L1 page yet.
 *
 *     In each completed L1 page, a pair of reversed variable-length
 *     integers at the end indicate, respectively, the total
 *     time delta encompassed by the L1 page and the total number
 *     of samples in all of its L0 pages. An L1 page is considered
 *     full when the forward-growing entries and backward-growing
 *     entries are about to collide.
 *
 *     Incomplete L1 pages, by definition, end with an 0x00 byte
 *     indicating space reserved for these two reversed integers.
 *
 * The variable length integers are formatted as follows, shown
 * in binary:
 *
 *   x < 0x80              1xxxxxxx
 *   x < 0x4000            01xxxxxx xxxxxxxx
 *   x < 0x200000          001xxxxx xxxxxxxx xxxxxxxx
 *   x < 0x10000000        0001xxxx xxxxxxxx xxxxxxxx xxxxxxxx
 *   ...
 *   End-of-page mark      00000000
 *
 * The largest integer length that can be represented is 56-bits,
 * which will be prefixed by 0x01.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <Python.h>

/************************************************************************/
/***************************************** Variable-length Integers *****/
/************************************************************************/

/* Valid samples can be up to 56 bits long */
typedef long long sample_t;
#define END_MARKER   -1
#define HIT_FENCE    -2
#define SAMPLE_INF   0x7FFFFFFFFFFFFFFFLL

/*
 * Read a sample forward, incrementing 'p' to point just past the end
 * of the sample on a successful read. If any byte in the sample would
 * have been read from 'fence', this does not change p and returns HIT_FENCE.
 * Memory will never be read from addresses greater than or equal to 'fence'.
 */
sample_t static inline sample_read(unsigned char **p, unsigned char *fence)
{
    unsigned char *cp = *p;
    unsigned char c;

    if (cp >= fence)
        return HIT_FENCE;
    c = *cp;

    if (c & 0x80) {
        *p = cp + 1;
        return c & 0x7F;
    }

    if (c & 0x40) {
        if (cp + 1 >= fence)
            return HIT_FENCE;
        *p = cp + 2;
        return ((c & 0x3F) << 8) | cp[1];
    }

    if (c == 0)
        return END_MARKER;

    if (c & 0x20) {
        if (cp + 2 >= fence)
            return HIT_FENCE;
        *p = cp + 3;
        return ((c & 0x1F) << 16) | (cp[1] << 8) | cp[2];
    }

    if (c & 0x10) {
        if (cp + 3 >= fence)
            return HIT_FENCE;
        *p = cp + 4;
        return ((c & 0x0F) << 24) | (cp[1] << 16) | (cp[2] << 8) | cp[3];
    }

    if (c & 0x08) {
        if (cp + 4 >= fence)
            return HIT_FENCE;
        *p = cp + 5;
        return (((sample_t) c & 0x07) << 32) | (cp[1] << 24) |
            (cp[2] << 16) | (cp[3] << 8) | cp[4];
    }

    if (c & 0x04) {
        if (cp + 5 >= fence)
            return HIT_FENCE;
        *p = cp + 6;
        return (((sample_t) c & 0x03) << 40) | (((sample_t) cp[1]) << 32) |
            (cp[2] << 24) | (cp[3] << 16) | (cp[4] << 8) | cp[5];
    }

    if (c & 0x02) {
        if (cp + 6 >= fence)
            return HIT_FENCE;
        *p = cp + 7;
        return (((sample_t) c & 0x01) << 48) | (((sample_t) cp[1]) << 40) |
            (((sample_t) cp[2]) << 32) | (cp[3] << 24) | (cp[4] << 16) |
            (cp[5] << 8) | cp[6];
    }

    if (cp + 7 >= fence)
        return HIT_FENCE;
    *p = cp + 8;
    return (((sample_t) cp[1]) << 48) | (((sample_t) cp[2]) << 40) |
        (((sample_t) cp[3]) << 32) | (cp[4] << 24) | (cp[5] << 16) |
        (cp[6] << 8) | cp[7];
}

/* A reversed version of sample_read, where memory addresses move
 * downward rather than upward.
 */
sample_t static inline sample_read_r(unsigned char **p, unsigned char *fence)
{
    unsigned char *cp = *p;
    unsigned char c;

    if (cp <= fence)
        return HIT_FENCE;
    c = *cp;

    if (c & 0x80) {
        *p = cp - 1;
        return c & 0x7F;
    }

    if (c & 0x40) {
        if (cp - 1 <= fence)
            return HIT_FENCE;
        *p = cp - 2;
        return ((c & 0x3F) << 8) | cp[-1];
    }

    if (c == 0)
        return END_MARKER;

    if (c & 0x20) {
        if (cp - 2 <= fence)
            return HIT_FENCE;
        *p = cp - 3;
        return ((c & 0x1F) << 16) | (cp[-1] << 8) | cp[-2];
    }

    if (c & 0x10) {
        if (cp - 3 <= fence)
            return HIT_FENCE;
        *p = cp - 4;
        return ((c & 0x0F) << 24) | (cp[-1] << 16) | (cp[-2] << 8) | cp[-3];
    }

    if (c & 0x08) {
        if (cp - 4 <= fence)
            return HIT_FENCE;
        *p = cp - 5;
        return (((sample_t) c & 0x07) << 32) | (cp[-1] << 24) |
            (cp[-2] << 16) | (cp[-3] << 8) | cp[-4];
    }

    if (c & 0x04) {
        if (cp - 5 <= fence)
            return HIT_FENCE;
        *p = cp - 6;
        return (((sample_t) c & 0x03) << 40) | (((sample_t) cp[-1]) << 32) |
            (cp[-2] << 24) | (cp[-3] << 16) | (cp[-4] << 8) | cp[-5];
    }

    if (c & 0x02) {
        if (cp - 6 <= fence)
            return HIT_FENCE;
        *p = cp - 7;
        return (((sample_t) c & 0x01) << 48) | (((sample_t) cp[-1]) << 40) |
            (((sample_t) cp[-2]) << 32) | (cp[-3] << 24) | (cp[-4] << 16) |
            (cp[-5] << 8) | cp[-6];
    }

    if (cp - 7 <= fence)
        return HIT_FENCE;
    *p = cp - 8;
    return (((sample_t) cp[-1]) << 48) | (((sample_t) cp[-2]) << 40) |
        (((sample_t) cp[-3]) << 32) | (cp[-4] << 24) | (cp[-5] << 16) |
        (cp[-6] << 8) | cp[-7];
}

/* Return the length, in bytes, necessary to store a sample.
 * This assumes a sample fits in our 56-bit limit.
 */
int static inline get_sample_len(sample_t s)
{
    if (s < 0x80) return 1; /* This case also works for END_MARKER */
    if (s < 0x4000) return 2;
    if (s < 0x200000) return 3;
    if (s < 0x10000000) return 4;
    if (s < 0x0800000000LL) return 5;
    if (s < 0x040000000000LL) return 6;
    if (s < 0x02000000000000LL) return 7;
    return 8;
}

/* Write a sample at the provided address. This does not increment
 * the pointer, or perform any EOF checking. The sample may not
 * be END_MARKER.
 */
void static inline sample_write(sample_t s, unsigned char *p)
{
    if (s < 0x80) {
        p[0] = 0x80 | s;
    }
    else if (s < 0x4000) {
        p[0] = 0x40 | (s >> 8);
        p[1] = s;
    }
    else if (s < 0x200000) {
        p[0] = 0x20 | (s >> 16);
        p[1] = s >> 8;
        p[2] = s;
    }
    else if (s < 0x10000000) {
        p[0] = 0x10 | (s >> 24);
        p[1] = s >> 16;
        p[2] = s >> 8;
        p[3] = s;
    }
    else if (s < 0x0800000000LL) {
        p[0] = 0x08 | (s >> 32);
        p[1] = s >> 24;
        p[2] = s >> 16;
        p[3] = s >> 8;
        p[4] = s;
    }
    else if (s < 0x040000000000LL) {
        p[0] = 0x04 | (s >> 40);
        p[1] = s >> 32;
        p[2] = s >> 24;
        p[3] = s >> 16;
        p[3] = s >> 8;
        p[4] = s;
    }
    else if (s < 0x02000000000000LL) {
        p[0] = 0x02 | (s >> 48);
        p[1] = s >> 40;
        p[2] = s >> 32;
        p[3] = s >> 24;
        p[4] = s >> 16;
        p[5] = s >> 8;
        p[6] = s;
    }
    else {
        p[0] = 0x01;
        p[1] = s >> 48;
        p[2] = s >> 40;
        p[3] = s >> 32;
        p[4] = s >> 24;
        p[5] = s >> 16;
        p[6] = s >> 8;
        p[7] = s;
    }
}

/* A reversed version of sample_write */
void static inline sample_write_r(sample_t s, unsigned char *p)
{
    if (s < 0x80) {
        p[0] = 0x80 | s;
    }
    else if (s < 0x4000) {
        p[0] = 0x40 | (s >> 8);
        p[-1] = s;
    }
    else if (s < 0x200000) {
        p[0] = 0x20 | (s >> 16);
        p[-1] = s >> 8;
        p[-2] = s;
    }
    else if (s < 0x10000000) {
        p[0] = 0x10 | (s >> 24);
        p[-1] = s >> 16;
        p[-2] = s >> 8;
        p[-3] = s;
    }
    else if (s < 0x0800000000LL) {
        p[0] = 0x08 | (s >> 32);
        p[-1] = s >> 24;
        p[-2] = s >> 16;
        p[-3] = s >> 8;
        p[-4] = s;
    }
    else if (s < 0x040000000000LL) {
        p[0] = 0x04 | (s >> 40);
        p[-1] = s >> 32;
        p[-2] = s >> 24;
        p[-3] = s >> 16;
        p[-3] = s >> 8;
        p[-4] = s;
    }
    else if (s < 0x02000000000000LL) {
        p[0] = 0x02 | (s >> 48);
        p[-1] = s >> 40;
        p[-2] = s >> 32;
        p[-3] = s >> 24;
        p[-4] = s >> 16;
        p[-5] = s >> 8;
        p[-6] = s;
    }
    else {
        p[0] = 0x01;
        p[-1] = s >> 48;
        p[-2] = s >> 40;
        p[-3] = s >> 32;
        p[-4] = s >> 24;
        p[-5] = s >> 16;
        p[-6] = s >> 8;
        p[-7] = s;
    }
}

/************************************************************************/
/*************************************************** Page Interface *****/
/************************************************************************/

#define PAGE_SHIFT 12
#define PAGE_SIZE  (1 << PAGE_SHIFT)
#define PAGE_MASK  (PAGE_SIZE - 1)

typedef struct {
    unsigned char data[PAGE_SIZE];
    off_t file_offset;
    ssize_t size;
} fid_page;

typedef struct {
    int fd;
    off_t size;
    off_t offset;
} fid_file;

typedef struct {
    fid_file file;
    struct {
        fid_page page;
        off_t loaded_offset;
        int offset;
        sample_t sample;
        unsigned long sample_number;
    } l0, l1;
} fid_cursor;

static int fid_file_init(fid_file *self, int fd)
{
    self->fd = fd;
    self->offset = self->size = lseek(fd, 0, SEEK_END);
    if (self->offset == (off_t)-1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return -1;
    }
    return 0;
}

static int fid_file_seek(fid_file *self, off_t offset)
{
    if (self->offset != offset) {
        if (lseek(self->fd, offset, SEEK_SET) == (off_t)-1) {
            PyErr_SetFromErrno(PyExc_IOError);
            return -1;
        }
    }
    return 0;
}

static int fid_page_read(fid_page *self, fid_file *file)
{
    if (fid_file_seek(file, self->file_offset) < 0)
        return -1;

    self->size = read(file->fd, self->data, PAGE_SIZE);
    if (self->size < 0) {
        PyErr_SetFromErrno(PyExc_IOError);
        return -1;
    }
    file->offset += self->size;

    /* Zero out anything not present in the file yet */
    memset(self->data + self->size, 0, PAGE_SIZE - self->size);

    return 0;
}

static int fid_page_write(fid_page *self, fid_file *file)
{
    if (fid_file_seek(file, self->file_offset) < 0)
        return -1;

    if (write(file->fd, self->data, self->size) != self->size) {
        PyErr_SetFromErrno(PyExc_IOError);
        return -1;
    }
    file->offset += self->size;

    return 0;
}

/* Initialize a fid_cursor, which represents a fid_file plus
 * our current position on all tree levels.
 */
static int fid_cursor_init(fid_cursor *self, int fd)
{
    self->l0.loaded_offset = (off_t)-1;
    self->l0.sample = 0;
    self->l0.sample_number = 0;
    self->l0.offset = 0;
    self->l0.page.file_offset = 0;
    
    self->l1.loaded_offset = (off_t)-1;
    self->l1.sample = 0;
    self->l1.sample_number = 0;
    self->l1.offset = 0;
    self->l1.page.file_offset = 0;
    
    return fid_file_init(&self->file, fd);
}

/* Seek a fid_cursor to the first sample equal to or greater
 * than the supplied key. The key SAMPLE_INF can be used to seek
 * to the end of the file, where no sample exists yet.
 */
static int fid_cursor_seek(fid_cursor *self, sample_t key)
{
    /* First, the level 1 seek */

    /* We can only seek forwards, discard saved state
     * (but not necessarily cached pages) if we have to go back.
     */
    if (key < self->l1.sample) {
        self->l1.sample = 0;
        self->l1.sample_number = 0;
        self->l1.offset = 0;
        self->l1.page.file_offset = 0;

        self->l0.sample = 0;
        self->l0.sample_number = 0;
        self->l0.offset = 0;
        self->l0.page.file_offset = PAGE_SIZE;
    }

    while (1) {
        sample_t page_sample_delta;
        unsigned long page_item_count;
        unsigned char *p;

        /* Bring this l1 page into core if it isn't there already */
        if (self->l1.loaded_offset != self->l1.page.file_offset) {
            if (fid_page_read(&self->l1.page, &self->file) < 0)
                return -1;
            self->l1.loaded_offset = self->l1.page.file_offset;
        }

        /* If this isn't a completed page, we can stop searching forward */
        p = &self->l1.page.data[PAGE_SIZE - 1];
        if (*p == 0x00)
            break;

        /* It's a completed page. We should be able to read its totals */
        page_sample_delta = sample_read_r(&p, self->l1.page.data);
        page_item_count = sample_read_r(&p, self->l1.page.data);
        if (self->l1.sample + page_sample_delta >= key)
            break;

        /* Skip this page */
        self->l1.sample += page_sample_delta;
        self->l1.sample_number += page_item_count;
        self->l1.offset = 0;
        self->l1.page.file_offset += (1 + page_item_count) << PAGE_SHIFT;

        /* Point l0 at the first child of this new l1 page */
        self->l0.sample = self->l1.sample;
        self->l0.sample_number = self->l1.sample_number;
        self->l0.offset = 0;
        self->l0.page.file_offset = self->l1.page.file_offset + PAGE_SIZE;
    }

    /* Now scan the level 1 page to find the proper level 0 page */
    {
        unsigned char *p = &self->l1.page.data[self->l1.offset];
        unsigned char *fence = &self->l1.page.data[self->l1.page.size-1];
        sample_t page_sample_delta;
        unsigned long page_item_count;
        
        while (1) {
            page_sample_delta = sample_read_r(&p, fence);
            if (page_sample_delta == END_MARKER) {
                /* End of page, leave the cursor here */
                break;
            }
            else if (page_sample_delta < 0) {
                PyErr_SetString(PyExc_ValueError, "Improperly terminated level-1 index page");
                return -1;
            }

            page_item_count = sample_read_r(&p, fence);
            if (page_item_count < 0) {
                PyErr_SetString(PyExc_ValueError, "Incomplete record in level-1 index");
                return -1;
            }

            if (self->l1.sample + page_sample_delta >= key)
                break;

            /* Skip the l0 page corresponding to this sample */
            self->l1.sample += page_sample_delta;
            self->l1.sample_number += page_item_count;
            self->l1.offset = p - self->l1.page.data;

            self->l0.sample = self->l1.sample;
            self->l0.sample_number = self->l1.sample_number;
            self->l0.offset = 0;
            self->l0.page.file_offset += PAGE_SIZE;
        }
    }

    /* Bring this l0 page into core if it isn't there already */
    if (self->l0.loaded_offset != self->l0.page.file_offset) {
        if (fid_page_read(&self->l0.page, &self->file) < 0)
            return -1;
        self->l0.loaded_offset = self->l0.page.file_offset;
    }

    /* Scan forward in the level 0 page */
    {
        unsigned char *p = &self->l0.page.data[self->l0.offset];
        unsigned char *fence = &self->l0.page.data[self->l0.page.size-1];
        sample_t sample_delta;
        
        while (1) {
            sample_delta = sample_read_r(&p, fence);
            if (sample_delta < 0) {
                /* End of page, leave the cursor here */
                break;
            }

            if (self->l0.sample + sample_delta >= key)
                break;

            /* Skip this sample */
            self->l0.sample += sample_delta;
            self->l0.sample_number++;
            self->l0.offset = p - self->l0.page.data;
        }
    }

    return 0;
}


/************************************************************************/
/************************************** High-Level Python Interface *****/
/************************************************************************/


/* Appends a list of new samples to a fid file */
static PyObject* fid_append_samples(PyObject *self, PyObject *args) {
    PyObject *sequence, *item, *iterator = NULL;
    int fd;
    fid_cursor cursor;

    if (!PyArg_ParseTuple(args, "iO", &fd, &sequence))
        goto error;
    iterator = PyObject_GetIter(sequence);
    if (!iterator)
        goto error;

    if (fid_cursor_init(&cursor, fd) < 0)
        goto error;
    if (fid_cursor_seek(&cursor, SAMPLE_INF) < 0)
        goto error;

    /* For each item in our sequence... */
    while ((item = PyIter_Next(iterator))) {
        /* Safely decode as a sample_t */
        sample_t sample = PyLong_AsLongLong(item);
        Py_DECREF(item);
        if (sample == -1 && PyErr_Occurred())
            goto error;

    }

    Py_DECREF(iterator);
    Py_RETURN_NONE;
error:
    Py_DECREF(iterator);
    return NULL;
}


static PyMethodDef module_methods[] = {
    { "append_samples", (PyCFunction) fid_append_samples, METH_VARARGS },
    {0}
};

PyMODINIT_FUNC init_fidtool(void)
{
    Py_InitModule("_fidtool", module_methods);
}

/* The End */
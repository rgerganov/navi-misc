/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_model.h -- Mutable data that is shared between the multiple views in THD.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __THD_MODEL_H
#define __THD_MODEL_H

#include <boost/signal.hpp>
#include <wx/string.h>
#include "log_index.h"
#include "log_reader.h"


struct THDModelCursor {
    typedef MemTransfer::ClockType ClockType;
    typedef MemTransfer::OffsetType OffsetType;
    typedef MemTransfer::AddressType AddressType;

    static const ClockType NO_TIME = (ClockType)-1;
    static const OffsetType NO_TRANSFER = (OffsetType)-1;
    static const AddressType NO_ADDRESS = (AddressType)-1;

    THDModelCursor()
        : time(NO_TIME),
          transferId(NO_TRANSFER),
          address(NO_ADDRESS)
    {}

    ClockType time;
    OffsetType transferId;
    AddressType address;
};


struct THDModel {
    typedef boost::signal0<void> signal_t;

    THDModel(LogIndex *_index)
        : index(_index),
          clockHz(LogReader::GetDefaultClockHZ())
    {}

    LogIndex *index;

    double clockHz;
    signal_t clockHzChanged;

    THDModelCursor cursor;
    signal_t cursorChanged;

    wxString formatClock(MemTransfer::ClockType clock)
    {
        return wxString::Format(wxT("%.06fs"), clock / clockHz);
    }
};

#endif /* __THD_MODEL_H */

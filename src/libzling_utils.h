/**
 * zling:
 *  light-weight lossless data compression utility.
 *
 * Copyright (C) 2012-2013 by Zhang Li <zhangli10 at baidu.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @author zhangli10<zhangli10@baidu.com>
 * @brief  libzling utils.
 */
#ifndef SRC_LIBZLING_UTILS_H
#define SRC_LIBZLING_UTILS_H

#include "libzling_inc.h"

namespace baidu {
namespace zling {

/* Interfaces:
 *  Inputer:       interface for an abstract inputer.
 *  Outputer:      interface for an abstract outputer.
 *  ActionHandler: interface for an abstract action handler (normally used for printing process.)
 */
struct Inputer {
    virtual size_t GetData(unsigned char* buf, size_t len) = 0;
    virtual bool IsEnd() = 0;
    virtual bool IsErr() = 0;

    int GetChar();
    uint32_t GetUInt32();
};
struct Outputer {
    virtual size_t PutData(unsigned char* buf, size_t len) = 0;
    virtual bool IsErr() = 0;

    int PutChar(int v);
    uint32_t PutUInt32(uint32_t v);
};

struct ActionHandler {
    virtual void OnInit() {}
    virtual void OnDone() {}
    virtual void OnProcess(unsigned char* orig_data, size_t orig_size) {}

    inline void SetInputerOutputer(Inputer* inputer, Outputer* outputer, bool is_encode) {
        m_is_encode = is_encode;
        m_inputer = inputer;
        m_outputer = outputer;
    }
    inline bool IsEncode() {
        return m_is_encode;
    }
    inline Inputer* GetInputer() {
        return m_inputer;
    }
    inline Outputer* GetOutputer() {
        return m_outputer;
    }
private:
    bool       m_is_encode;
    Inputer*  m_inputer;
    Outputer* m_outputer;
};

/* codebuf: manipulate code (u64) buffer.
 *  Input();
 *  Output();
 *  Peek();
 *  GetLength();
 */
struct ZlingCodebuf {
    ZlingCodebuf():
        m_buf(0),
        m_len(0) {}

    inline void Input(uint64_t code, int len) {
        m_buf |= code << m_len;
        m_len += len;
        return;
    }
    inline uint64_t Output(int len) {
        uint64_t out = Peek(len);
        m_buf >>= len;
        m_len  -= len;
        return out;
    }
    inline uint64_t Peek(int len) const {
        return m_buf & ~(-1ull << len);
    }
    inline int GetLength() const {
        return m_len;
    }
private:
    uint64_t m_buf;
    int m_len;
};

/* FileInputer/FileOutputer:
 *  FILE I/O implementation of Inputer/Outputer.
 */
struct FileInputer: public baidu::zling::Inputer {
    FileInputer(FILE* fp):
        m_fp(fp),
        m_total_read(0) {}

    size_t GetData(unsigned char* buf, size_t len);
    bool   IsEnd();
    bool   IsErr();
    size_t GetInputSize();

private:
    FILE*  m_fp;
    size_t m_total_read;
};

struct FileOutputer: public baidu::zling::Outputer {
    FileOutputer(FILE* fp):
        m_fp(fp),
        m_total_write(0) {}

    size_t PutData(unsigned char* buf, size_t len);
    bool   IsErr();
    size_t GetOutputSize();

private:
    FILE*  m_fp;
    size_t m_total_write;
};

}  // namespace zling
}  // namespace baidu
#endif  // SRC_LIBZLING_UTILS_H

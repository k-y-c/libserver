#pragma once

#include <vector>
#include <string>
#include <string.h>
#include <sys/uio.h>
#include "Log.hpp"
#include <assert.h>

class Buffer
{
public:
    Buffer():buf_(1024),readIdx_(8),writeIdx_(8){

    }
    ~Buffer(){

    }

    char* peek(){
        return begin()+readIdx_;
    }

    bool retrieveAll(){
        readIdx_ = writeIdx_ = 8;
        return true;
    }

    bool retrieve(int len){
        assert(readIdx_+len <= writeIdx_);
        readIdx_ += len;
        if(readIdx_ == writeIdx_){
            retrieveAll();
        }
        return true;
    }

    std::string retrieveAllAsString(){
        auto s = std::string(peek(),begin()+writeIdx_);
        retrieveAll();
        return s;
    }

    bool append(const void* data,int len){
        if(writable()<len){
            makeSpace(len);
        }
        memcpy((void*)(begin()+writeIdx_),data,len);
        writeIdx_ += len;
        return true;
    }

    int prependable(){
        return readIdx_;
    }

    int writable(){
        return buf_.size()-writeIdx_;
    }

    int readable(){
        return writeIdx_-readIdx_;
    }

    int readFd(int fd){
        char exbuf[65535];
        memset(exbuf,0,65535);
        iovec iov[2];
        iov[0].iov_base = begin()+writeIdx_;
        iov[0].iov_len = writable();
        iov[1].iov_base = exbuf;
        iov[1].iov_len = 65535;
        ssize_t n =  readv(fd,iov,2);
        LOG_INFO << "readv bytes:" << n;
        int writable_ = writable();
        if(n>writable()){
            writeIdx_ = buf_.size();
            append(exbuf,n-writable_);
            // buf_.insert(buf_.end(),exbuf,exbuf+(n-writable()));
        }
        else{
            writeIdx_ += n;
        }
        
        return n;
    }

private:
    char* begin(){
        return &*buf_.begin();
    }

    bool makeSpace(int len){
        if(prependable()+writable()<len){
            buf_.resize(writeIdx_+len);
        }
        else{
            memmove(begin()+8,peek(),readable());
            writeIdx_ = 8 + readable();
            readIdx_ = 8;
        }
        return true;
    }
    
    std::vector<char> buf_;
    int readIdx_;
    int writeIdx_;
};
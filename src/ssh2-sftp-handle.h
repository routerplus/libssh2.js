/*
 * Copyright (c) 2021 RouterPlus Networks
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _SSH2_SFTP_HANDLE_H_
#define _SSH2_SFTP_HANDLE_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <emscripten/bind.h>

#ifndef BUFF_LEN
#define BUFF_LEN 4096
#endif

class SFTP_HANDLE {
public:
	SFTP_HANDLE(emscripten::val handle)
	{
		fprintf(stderr, "SFTP_HANDLE CANNOT CREATE DIRECTLY!\n");
	}

	SFTP_HANDLE(LIBSSH2_SFTP *sf, LIBSSH2_SFTP_HANDLE *hand) :
		sftp(sf),
		handle(hand)
	{
		if(handle) {
			fprintf(stderr, "SFTP_HANDLE ok\n");
			active = true;
			memset(&attrs, '\0', sizeof(attrs));
			memset(&st, '\0', sizeof(st));
		}
	}

	int close()
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		return libssh2_sftp_close(handle);
	}

	int closedir()
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		return libssh2_sftp_closedir(handle);
	}

	LIBSSH2_SFTP_ATTRIBUTES fsetstat()
	{
		memset(&attrs, '\0', sizeof(attrs));
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return attrs;
		}

		if(libssh2_sftp_fsetstat(handle, &attrs)) {
		}
		return attrs;
	}

	LIBSSH2_SFTP_ATTRIBUTES fstat(int setstat = 0)
	{
		memset(&attrs, '\0', sizeof(attrs));

		if(!handle) {
			fprintf(stderr, "handle error\n");
			return attrs;
		}

		if(libssh2_sftp_fstat_ex(handle, &attrs, setstat)) {
			//TODO: 
		}
		return attrs;
	}

	LIBSSH2_SFTP_STATVFS fstatvfs() 
	{
		memset(&st, '\0', sizeof(st));
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return st;
		}

		if(libssh2_sftp_fstatvfs(handle,  &st)) {
			//TODO: 
		}

		return st;
	}

	int fsync()
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		return libssh2_sftp_fsync(handle);
	}

	std::string read() 
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return nodata;
		}

		ssize_t n = libssh2_sftp_read(handle, buffer, BUFF_LEN);
		return (n) ? std::string(buffer, n) : nodata;
	}

	std::string readdir() 
	{
		memset(&attrs, '\0', sizeof(attrs));
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return nodata;
		}

		int n = libssh2_sftp_readdir(handle, buffer, BUFF_LEN, &attrs);
		return (n) ? std::string(buffer, n) : nodata;
	}

	int readdir_ex() 
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		return libssh2_sftp_readdir_ex(handle, NULL, 0, 
				NULL, 0, NULL);
	}

	int rewind() 
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		libssh2_sftp_rewind(handle);
		return 0;
	}

	int seek(size_t offset) 
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		libssh2_sftp_seek(handle, offset);
		return 0;
	}

	int seek64(libssh2_uint64_t offset)
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		libssh2_sftp_seek64(handle, offset);
		return 0;
	}

	int shutdown()
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		return libssh2_sftp_shutdown(sftp);
	}

	int tell()
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		return libssh2_sftp_tell(handle);
	}

	int tell64()
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		return libssh2_sftp_tell64(handle);
	}

	int write(std::string buff)
	{
		if(!handle) {
			fprintf(stderr, "handle error\n");
			return -1;
		}

		return libssh2_sftp_write(handle, buff.c_str(), buff.length());
	}

	bool getActive() const {
                return active;
        }

private:
	emscripten::val & attrs_object(emscripten::val &v, 
			const LIBSSH2_SFTP_ATTRIBUTES *attrs)
	{
		v.set("flags", attrs->flags);
		v.set("filesize", attrs->filesize);
		v.set("uid", attrs->uid);
		v.set("gid", attrs->gid);
		v.set("perm", attrs->permissions);
		v.set("atime", attrs->atime);
		v.set("mtime", attrs->mtime);

		return v;
	}
private:
	LIBSSH2_SFTP_HANDLE *handle;
	LIBSSH2_SESSION     *session;
	LIBSSH2_SFTP        *sftp;

	LIBSSH2_SFTP_ATTRIBUTES attrs;
	LIBSSH2_SFTP_STATVFS st;

	char buffer[BUFF_LEN];
	std::string nodata;
	
	bool active = false;
};

#endif /* ~_SSH2_SFTP_HANDLE_H_ */


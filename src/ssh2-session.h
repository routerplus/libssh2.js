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

#ifndef _SSH2_SESSION_H_
#define _SSH2_SESSION_H_

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <sys/socket.h>

#include <string>

#include <iostream>
#include <queue>

#include <emscripten/bind.h>

#include "ssh2-channel.h"
#include "ssh2-sftp.h"

class SESSION {
public:
	SESSION(emscripten::val v) :
		error(0),
		has_opened(false)
	{
		fd = socket(AF_INET, SOCK_STREAM, 0);

		if(connect(fd, (struct sockaddr*)this, sizeof(*this))!= 0) {
			fprintf(stderr, "failed to connect!\n");
			error = 1;
			return;
		}

		session = libssh2_session_init();
		if(session) {
			libssh2_session_set_blocking(session, 0);
		}
		else {
			error = 2;
		}
	}

	~SESSION() 
	{
		if(session) {
			//TODO: free all channels

			libssh2_session_free(session);
		}
	}

	void pushdata(std::string data)
	{
		uint8_t *p = (uint8_t*)data.c_str();

		for(int i = 0; i < data.length(); i++) {
			incoming.push(*(p+i));
		}

		if(error && (error != LIBSSH2_ERROR_EAGAIN)) {
			error = 3;
		}
		else if(!has_opened)
			handshake();
	}

	ssize_t recv(void *buffer, size_t length, int flags) 
	{
		ssize_t  c = 0;
		uint8_t *p = static_cast<uint8_t*>(buffer);

		while(!incoming.empty() && (c++ < length)) {
			*p++ = incoming.front();
			incoming.pop();
		}

		if(!c) {
			errno = EAGAIN;
			c = -errno;
		}
		return c;
	}

	void clean() 
	{
		while(!incoming.empty()) {
			incoming.pop();
		}
	}

public:
	std::string userauth(std::string user);
	int login(std::string user, std::string passwd);

	CHANNEL channel();
	SFTP    sftp();

	emscripten::val send = emscripten::val::null();

	std::string getFingerprint() const { return fingerprint; }
	emscripten::val getSendCb() const { return send; }
	void setSendCb(emscripten::val cb)  { send = cb; }

private:
	int handshake();

private:
	int  fd;
	int  error = 0;
	bool has_opened = false;
	bool has_logined= false;

	LIBSSH2_SESSION *session;

	std::queue<unsigned char> incoming;

	std::string fingerprint;
	std::string methods;

	emscripten::val onerror   = emscripten::val::null();
	emscripten::val onclose   = emscripten::val::null();
};

#endif /* ~_SSH2_SESSION_H_ */

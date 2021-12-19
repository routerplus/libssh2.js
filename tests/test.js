const net = require('net')

var factory = require('../dist/libssh2.js');
var libssh2;

factory().then((wasm) => {
	libssh2 = wasm;
	libssh2.init(0);
	test();
});

let user  = 'user'
let passwd= 'xxxxxx'

function test() 
{
	let session;
	let socket = new net.Socket();

	session = libssh2.createSESSION(socket, (rc)=> {	
		session.login(user, passwd, (rc) => {
			if(rc !== libssh2.ERROR.NONE) {
				console.log(rc, libssh2.ERRMSG[rc]);
			}
			else {
				has_logined = true;
				console.log(user,'logined');
			}
		});
	});
	socket.connect(22, '127.0.0.1');
}

function sftp_test() {
	var sf;
	session.SFTP((rc, _sf) => {
		console.log('rc=', rc);
		// console.log('_sf', _sf);
		sf = _sf;
	})
}

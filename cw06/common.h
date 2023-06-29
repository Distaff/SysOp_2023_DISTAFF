#pragma once
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std::string_literals;

namespace common {
	constexpr int SERVER_QUEUE_PROJ_ID = 255;
	constexpr int MSG_MAX_LEN = 1024;


	enum MsgType {
		INIT = 1,
		STOP,
		LIST,
		LIST_ENTITY,
		TO_ALL,
		TO_ONE
	};


	struct MsgInit {
		long mtype;
		int client_PID;
		int client_queue_id;
	} __attribute((packed));

	struct MsgInitReply {
		long mtype;
		int clientID;
	} __attribute((packed));

	struct MsgStop {
		long mtype;
		int client_id;
	} __attribute((packed));

	struct MsgList {
		long mtype;
		union {
			int num_clients;
			int sender_id;
		};
	} __attribute((packed));

	struct MsgListEntity {
		long mtype;
		int client_ID;
		int client_PID;
		int client_queue_id;
	} __attribute((packed));


	template<int msgLen>
	struct MsgToOne	{
		long mtype;
		long sender_id;
		long target_id;
		std::time_t time;
		char msg[msgLen];
	};

	template<int msgLen>
	struct MsgToAll {
		long mtype;
		long sender_id;
		std::time_t time;
		char msg[msgLen];
	};
}
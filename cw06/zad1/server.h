#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <chrono>
#include <ctime>
#include <errno.h>
#include <string.h>

#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>

#include "common.h"
using namespace common;

namespace server {
	enum logMsgType {
		THROWING_ERROR,
		ERROR,
		INFO,
		DEBUG, 
		NEWLINE_RAW
	};

	struct ClientData {
		int PID;
		int queue_key;
	};


	std::map<int, ClientData> client_data;
	bool interrupt_flag = false;
	int queue_id = -1;
	std::ofstream log_file;
	key_t server_queue_key;


	void srv_init();
	void mainLoop();
	void registerNewClients();
	void deregisterTerminatedClients();
	void handleListing();
	void handleToOne();
	void handleToAll();
	void interruptHandler(int sig);
	void terminateClients();
	void message(logMsgType msg_type, std::string message);


	void srv_init() {
		log_file = std::ofstream("./server.log");


		//set handler for ctrl+c and sigterm
		struct sigaction sigact;
		sigact.sa_handler = &interruptHandler;
		sigaction(SIGINT, &sigact, nullptr);
		sigaction(SIGTERM, &sigact, nullptr);

		//create queue
		server_queue_key = ftok("/tmp/ssop_cw06_server_queue", SERVER_QUEUE_PROJ_ID);
		queue_id = msgget(server_queue_key, IPC_CREAT | IPC_EXCL | 0777 | IPC_RMID);

		if (queue_id == -1)
			message(THROWING_ERROR, "Failed to create a queue. Reason: "s + strerror(errno));
		else
			message(INFO, "Server queue created successfuly. Queue ID is: " + std::to_string(queue_id));
	}

	void mainLoop() {
		while (0) {
			if (interrupt_flag)
				break;

			registerNewClients();

			deregisterTerminatedClients();
			
			handleListing();

			handleToOne();

			//handleToAll();
		}
		terminateClients();
		log_file.close();
		return;
	}


	void registerNewClients() {
		MsgInit buffer;
		while (msgrcv(queue_id, &buffer, sizeof(buffer), static_cast<int>(MsgType::INIT), MSG_NOERROR | IPC_NOWAIT)) {	
			message(INFO, "Recieved init request from client with PID " + std::to_string(buffer.client_PID));
			message(NEWLINE_RAW, "Client queue key: " + std::to_string(buffer.client_queue_id));

			int new_client_id = 0;
			if (!client_data.empty())
				new_client_id = client_data.rbegin()->first + 1;

			client_data[new_client_id] = { buffer.client_PID , buffer.client_queue_id };
			MsgInitReply reply = { 
				static_cast<int>(MsgType::INIT),
				new_client_id 
			};
			msgsnd(buffer.client_queue_id, &reply, sizeof(reply) - sizeof(reply.mtype), 0);
		}
	}

	void deregisterTerminatedClients() {
		MsgStop buffer;
		while (msgrcv(queue_id, &buffer, sizeof(buffer), static_cast<int>(MsgType::STOP), MSG_NOERROR | IPC_NOWAIT)) {
			message(INFO, "Recieved stop request from client no. " + std::to_string(buffer.client_id));
			message(NEWLINE_RAW, 
				" (PID: " + std::to_string(client_data[buffer.client_id].PID) 
				+ ", queue key: " + std::to_string(client_data[buffer.client_id].queue_key) 
				+ ")"
			);
			client_data.erase(buffer.client_id);
		}
	}

	void handleListing() {
		MsgList buffer;
		while (msgrcv(queue_id, &buffer, sizeof(buffer), static_cast<int>(MsgType::LIST), MSG_NOERROR | IPC_NOWAIT)) {
			message(INFO, "Recieved list request from client no. " + std::to_string(buffer.sender_id));

			MsgList reply = {
				static_cast<int>(MsgType::LIST),
				client_data.size()
			};
			msgsnd(client_data[buffer.sender_id].queue_key, &reply, sizeof(reply) - sizeof(reply.mtype), 0);

			for (auto& i : client_data) {
				MsgListEntity reply = {
					static_cast<int>(MsgType::LIST_ENTITY),
					i.first, 
					i.second.PID, 
					i.second.queue_key
				};
				msgsnd(client_data[buffer.sender_id].queue_key, &reply, sizeof(reply) - sizeof(reply.mtype), 0);
			}
		}
	}

	void handleToOne() {
		MsgToOne<MSG_MAX_LEN> buffer;
		while (msgrcv(queue_id, &buffer, sizeof(buffer), static_cast<int>(MsgType::TO_ONE), MSG_NOERROR | IPC_NOWAIT)) {
			message(INFO, "Recieved message from client no. " + std::to_string(buffer.sender_id));
			message(NEWLINE_RAW, "Target client: " + std::to_string(buffer.target_id));

			msgsnd(buffer.sender_id, &buffer, sizeof(buffer) - sizeof(buffer.mtype), 0);
		}
	}

	void handleToAll() {
		MsgToAll<MSG_MAX_LEN> buffer;
		while (msgrcv(queue_id, &buffer, sizeof(buffer), static_cast<int>(MsgType::TO_ALL), MSG_NOERROR | IPC_NOWAIT)) {
			message(INFO, "Recieved broadcast message request from client no. " + std::to_string(buffer.sender_id));


			for (auto& i : client_data) {
				msgsnd(i.second.queue_key, &buffer, sizeof(buffer) - sizeof(buffer.mtype), 0);			
			}
		}
	}

	void interruptHandler(int sig) {
		interrupt_flag = true;
		//TODO: queue removal
	}

	void terminateClients() {
		for (auto& i : client_data) {
			kill(i.second.PID, SIGTERM);
		}
	}

	//write string to stdout + log file
	void message(logMsgType msg_type, std::string message) {
		std::string curr_time_str;
		std::time_t time = std::time(nullptr);
		curr_time_str.resize(std::size("yyyy-mm-ddThh:mm:ssZ"));
		std::strftime(curr_time_str.data(), curr_time_str.size(), "%FT%TZ", std::gmtime(&time));
		curr_time_str += ": ";

		std::string msg_type_str;
		switch (msg_type)
		{
		case THROWING_ERROR:
		case ERROR:
			msg_type_str = "ERROR: ";
			break;
		case INFO:
			msg_type_str = "INFO: ";
			break;
		case DEBUG:
			msg_type_str = "DEBUG: ";
			break;
		case NEWLINE_RAW:
			curr_time_str = "";
			msg_type_str = "";
			break;
		}

		//std::endl to flush after write
		std::cout << curr_time_str << msg_type_str << message << std::endl;
		log_file << curr_time_str << msg_type_str << message << std::endl;

		if (msg_type == THROWING_ERROR) {
			throw std::runtime_error(message);
		}
	}
}


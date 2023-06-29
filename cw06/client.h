#pragma once

#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <unistd.h>

namespace client {

	bool interrupt_flag = false;
	int this_id = -1;
	int server_queue_id = -1;
	int this_queue_id = -1;
	key_t server_queue_key;
	key_t this_queue_key;


	void client_init();
	void mainLoop();
	void list();
	void to_all();
	void to_one();
	void handleIncoming();
	void stop();
	void interruptHandler(int sig);


	void client_init() {
		//set handler for ctrl+c and sigterm
		struct sigaction sigact;
		sigact.sa_handler = interruptHandler;
		sigaction(SIGINT, &sigact, nullptr);
		sigaction(SIGTERM, &sigact, nullptr);

		//get server queue
		server_queue_key = ftok("/tmp/ssop_cw06_server_queue", SERVER_QUEUE_PROJ_ID);
		server_queue_id = msgget(server_queue_key, 0);

		if (server_queue_id == -1)
			throw std::runtime_error("Failed to obtain server queue id");
		
		srand(time(NULL));
		system("touch /tmp/client_queue");
		key_t this_queue_key = ftok("/tmp/client_queue", (char)(rand() % 255));		//Ugly. I know.
		this_queue_id = msgget(this_queue_key, IPC_CREAT | 0777 | IPC_RMID);
		
		if(this_queue_id == -1)
			throw std::runtime_error("Failed to create a queue. Reason: "s + strerror(errno));

		MsgInit msg{
			MsgType::INIT,
			getpid(),
			this_queue_id
		};
		msgsnd(server_queue_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0);

		MsgInitReply buffer;
		if (msgrcv(this_queue_id, &buffer, 12, MsgType::INIT, 0) == -1) 
			throw std::runtime_error("Wyjebao sie: "s + strerror(errno) + std::to_string(errno));		//TODO: refactor msgs
		this_id = buffer.clientID;

		std::cout << "Init successful. ClientID: " << this_id << "\n";
	}

	void mainLoop() {
		std::string input;

		while (1) {
			if (interrupt_flag) {
				stop();
				std::cout << "Stopping.\n";
				break;
			}

			std::cin >> input;

			if (input == "LIST") {
				list();
			}
			else if (input == "2ALL") {
				to_all();
			}
			else if (input == "2ONE") {
				to_one();
			}
			else if (input == "INCOMING") {
				handleIncoming();
			}
			else if (input == "STOP") {
				interrupt_flag = true;
			}
			else {
				std::cout << "Error: Unrecognized command.\n";
			}

			input.clear();
		}
		return;
	}

	void list() {
		std::cout << "Listing clients...\n";

		MsgList buffer{
			MsgType::LIST,
			this_id
		};
		msgsnd(server_queue_id, &buffer, sizeof(buffer) - sizeof(buffer.mtype), 0);
		msgrcv(this_queue_id, &buffer, sizeof(buffer), static_cast<int>(MsgType::LIST), 0);

		for (int i = 0; i < buffer.num_clients; i++) {
			MsgListEntity entity;
			msgrcv(this_queue_id, &entity, sizeof(entity), static_cast<int>(MsgType::LIST_ENTITY), 0);
			std::cout << "ClientID: " << entity.client_ID << ", ClientPID: " << entity.client_PID << ", ClientQueueID: " << entity.client_queue_id << "\n";
			std::cout.flush();
		}

		std::cout << "Listing complete.\n";
	};

	void to_one() {
		MsgToOne<MSG_MAX_LEN> buffer{
			MsgType::TO_ONE,
			this_id
		};
		std::cin >> buffer.target_id;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cin.getline(buffer.msg, MSG_MAX_LEN);

		msgsnd(server_queue_id, &buffer, sizeof(buffer) - sizeof(buffer.mtype), 0);
	};

	void to_all() {
		MsgToOne<MSG_MAX_LEN> buffer{
			MsgType::TO_ALL,
			this_id
		};
		std::cin.ignore();
		std::cin.getline(buffer.msg, MSG_MAX_LEN);

		msgsnd(server_queue_id, &buffer, sizeof(buffer) - sizeof(buffer.mtype), 0);
	};

	void handleIncoming() {
		std::cout << "Printing incoming messages...\n";
		MsgToOne<MSG_MAX_LEN> buffer;
		while (msgrcv(this_queue_id, &buffer, sizeof(buffer), static_cast<int>(MsgType::TO_ONE), MSG_NOERROR | IPC_NOWAIT) != -1) {
			std::cout << "[Unicast] ClientID: " << buffer.sender_id << ", Message: " << buffer.msg << "\n";
		}
		while (msgrcv(this_queue_id, &buffer, sizeof(buffer), static_cast<int>(MsgType::TO_ALL), MSG_NOERROR | IPC_NOWAIT) != -1) {
			std::cout << "[Broadcast] ClientID: " << buffer.sender_id << ", Message: " << buffer.msg << "\n";
		}
		std::cout << "No more messages.\n";
	};

	void stop() {
		MsgStop buffer{
			MsgType::STOP,
			this_id
		};
		msgsnd(server_queue_id, &buffer, sizeof(buffer) - sizeof(buffer.mtype), 0);
		msgctl(this_queue_id, IPC_RMID, nullptr);
	};



	void interruptHandler(int sig) {
		interrupt_flag = true;
	}
}


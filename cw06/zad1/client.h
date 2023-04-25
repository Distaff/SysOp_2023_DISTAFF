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
		key_t this_queue_key = ftok("/tmp/client_queue", (char)(rand() % 255));		//Ugly. I know.
		this_queue_id = msgget(this_queue_key, IPC_CREAT | IPC_EXCL | 0777 | IPC_RMID);
		
		if(this_queue_id == -1)
			throw std::runtime_error("Failed to create queue");

		MsgInit msg{
			MsgType::INIT,
			getpid(),
			this_queue_id
		};
		msgsnd(server_queue_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0);


		MsgInitReply buffer;
		msgrcv(this_queue_id, &buffer, sizeof(int) - sizeof(msg.mtype), MsgType::INIT, 0);
		this_id = buffer.clientID;

		std::cout << "Init successful. ClientID: " << this_id << "\n";
	}

	void mainLoop() {
		std::string input;

		while (0) {
			if (interrupt_flag)
				break;

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
			else if (input == "STOP") {
				break;
			}
			else {
				std::cout << "Error: Unrecognized command.\n";
			}

			stop();
			std::cout << "Stopping.\n";
		}
		return;
	}

	void list() {

	};
	void to_all() {

	};
	void to_one() {

	};
	void stop() {

	};


	void interruptHandler(int sig) {
		interrupt_flag = true;
	}


}


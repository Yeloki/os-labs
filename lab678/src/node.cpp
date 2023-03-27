#include <cstdio>
#include <zmq.hpp>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "kmp.h"
#include <string>

typedef struct {
  std::string str;
  std::string sub;
  int command;
  int node;
} data_for_node;

int main(int argc, char * argv[]){

  int CUR_NODE = atoi(argv[1]);
  int PAR_NODE = atoi(argv[2]);

  //printf("node: %d, parent: %d\n", CUR_NODE, PAR_NODE);

  void *context = zmq_ctx_new(); //сокет для коннекта с родительским узлом
  void *subscriber = zmq_socket(context, ZMQ_SUB);
  char adress[256];
  snprintf(adress, sizeof(adress), "tcp://localhost:%d", 4040 + PAR_NODE);
  if (PAR_NODE == -1){
    zmq_connect(subscriber, "tcp://localhost:4040");
  } else {
    zmq_connect(subscriber, adress);
  }
  zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, 0, 0);

  void *context_2 = zmq_ctx_new(); //сокет для коннекта с дочерними узлами
  void *publisher = zmq_socket(context, ZMQ_PUB);
  char adress_2[256];
  snprintf(adress_2, sizeof(adress_2), "tcp://*:%d", 4040 + CUR_NODE);
  zmq_bind (publisher, adress_2);

  for(;;){
    zmq_msg_t reply;
    zmq_msg_init(&reply);

    zmq_msg_recv (&reply, subscriber, 0);

    auto* data = reinterpret_cast<data_for_node *>(malloc(zmq_msg_size(&reply)));
    memcpy(data, zmq_msg_data(&reply), zmq_msg_size(&reply));

    //printf("node %d: str: %s, sub: %s, command: %d, node: %d\n", CUR_NODE, data->str, data->sub, data->command, data->node);

    if (data->command == 1){

      if(data->node != CUR_NODE){
        zmq_msg_send(&reply, publisher, 0);
        zmq_msg_close(&reply);

        free(data);
        continue;
      }
      zmq_msg_close(&reply);

      sleep(10);

      printf("Ok:%d:%d\n", CUR_NODE, KMP(data->str.c_str(), data->sub.c_str()));

    }
    else if (data->command == 2){

      printf("Ok:%d\n", CUR_NODE);

      zmq_msg_send(&reply, publisher, 0);
      zmq_msg_close(&reply);

    }
    else if (data->command == 3){

      if(data->node == CUR_NODE || data->node == -1){

        printf("Ok:%d\n", CUR_NODE);

        zmq_msg_close(&reply);
        free(data);

        auto* data2 = reinterpret_cast<data_for_node *>(malloc(sizeof(data_for_node)));
        data2->command = 3;
        data2->node = -1;

        zmq_msg_t message;
        zmq_msg_init_size(&message, sizeof(data_for_node));

        memcpy(zmq_msg_data(&message), data2, zmq_msg_size(&message));

        zmq_msg_send(&message, publisher, 0);

        zmq_msg_close(&message);

        free(data);
        break;

      } else {
        zmq_msg_send(&reply, publisher, 0);
        zmq_msg_close(&reply);
      }

    }

    free(data);
  }
  zmq_close(subscriber);
  zmq_close(publisher);
  zmq_ctx_destroy(context);
  zmq_ctx_destroy(context_2);

  return 0;
}
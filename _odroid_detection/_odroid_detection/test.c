#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib-json.h"
#include "ServerCom.h"

struct linked_list {
  int val;
  struct linked_list* next;
};

void cleanup_ll(struct linked_list* head);
void writeStuffToServer();

int main() {
  uint16_t* arr_back;
  char buffer[10];
  int i = 0, c = 0, idx/*, sockfd*/;
  int size = 0, retsize;
  double *arr;
  struct linked_list head, *cur = &head;
//  char* msg, *msg_back;
  printf("Voer een paar getallen in, gescheiden door enters\nMet q of EOF wordt het invoeren afgesloten\n");
  while(1) {
    for(idx = 0; idx < 10; idx++) buffer[idx] = '\0';
    idx = 0;
    while((c = getchar()) != EOF && c != '\n') {
      buffer[idx++] = c;
      if(c == 'q') break;
    }
    if(c == EOF || c == 'q') break;
    cur->val = atoi(buffer);
    cur->next = malloc(sizeof(struct linked_list));
    cur = cur->next;
  }
  cur->next = NULL;
  cur = &head;
  while(cur->next) {
    cur = cur->next;
    size++;
  }
  arr = malloc(sizeof(double) * size);
  for(cur = &head, i = 0; cur->next; cur = cur->next, i++) {
    arr[i] = cur->val;
  }

  cleanup_ll(&head);
  arr_back = writeDataToServer(arr, size, TEST, &retsize);
  for(i = 0; i < retsize; i++) {
    printf("%d\n", arr_back[i]);
  } 
  free(arr);
  free(arr_back);
  writeStuffToServer();
}

void cleanup_ll(struct linked_list* head) {
  if(head->next != NULL) {
    cleanup_ll(head->next);
    free(head->next);
  }
}

void writeStuffToServer() {
  int i;
  double data[46] = {
   21, 29,  8,  2,
    7, 38, 22, 36,
    3, 10, 43,  6,
    9, 17, 44, 27,
   33, 23,  4, 32,
   28, 20, 34, 14,
   13, 18, 12, 25,
   24,  1,  5, 45,
   19, 40, 37, 41,
   15, 11, 46, 42,
   39, 30, 35, 31,
   16, 26 
  };
  uint16_t solar_data[6];
  uint16_t *retData;
  for(i = 0; i < 6; i++) {
    solar_data[i] = (uint16_t)data[i];
  }
  writeWeatherToServer(data);
  writeSolarToServer(solar_data);
  writeVictron1ToServer(data);
  writeVictron2ToServer(data);
  writeVictron3ToServer(data);
  writeVictron4ToServer(data);
  retData = getChargerState(data[3]);
  for(i = 0; i < 1; i++) {
    printf("%d\n", retData[i]);
  }
  free(retData);
}

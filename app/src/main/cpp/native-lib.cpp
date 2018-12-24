#include <jni.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include "native-lib.h"
#include <android/log.h>
#include <errno.h>
#include <sys/un.h>

#define LOG_TAG  "huang"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
const char *PATH = "/data/data/com.david.verusalive/keep_alive.socket";
int m_child;
const  char *userId;
extern "C"
JNIEXPORT void JNICALL
Java_com_david_verusalive_Watcher_createWatcher(JNIEnv *env, jobject instance, jstring userId_) {
    userId = env->GetStringUTFChars(userId_, 0);
    int pid = fork();
    //子进程
    if(pid ==0){
        LOGD("启动子进程");
        child_do_work();
     //父进程
    }else if(pid >0){

    }
    env->ReleaseStringUTFChars(userId_, userId);
}

 void child_do_work(){
    if(create_socket_channel()){
        child_listener_msg();
    }
}

void child_listener_msg(){

    fd_set set;
    struct  timeval  timeval{10,0};
    FD_ZERO(&set);
    FD_SET(m_child,&set);
    while (1){
        int r = select(m_child+1,&set,NULL,NULL,&timeval);
        LOGD("读取消息%d",r);
        if(r >0){
            char pkg[256] = {0};
            if(FD_ISSET(m_child,&set)){
                read(m_child,&pkg, sizeof(pkg));
                LOGD("启动服务");
                execlp("am","am","startservice","--user",userId,"com.david.verusalive/com.david.verusalive.ProcessService",NULL);
                break;
            }

        }

    }
}

 int create_socket_channel(){
     int sockfd  = socket(AF_LOCAL,SOCK_STREAM,0);
     struct sockaddr_un  addr;
     int connId;
     if(sockfd <0){
         LOGE("create socket fail");
         return -1;
     }
     unlink(PATH);
     memset(&addr,0, sizeof(sockaddr_un));
     addr.sun_family = AF_LOCAL;
     strcpy(addr.sun_path,PATH);
     if(bind(sockfd,(struct sockaddr*)&addr, sizeof(sockaddr_un))<0){
         LOGE("bind socket fail");
         return -1;
     }
     listen(sockfd,5);
     while(1){
         if((connId= accept(sockfd,NULL,NULL))<0){
             if(errno == EINTR){
                 continue;
             }else{
                 LOGE("accept socket fail");
                 return -1;
             }
         }
         LOGD("create_socket_channel%d",connId);
         m_child = connId;
         break;
     }
     LOGD("create service socket suc");
     return 1;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_david_verusalive_Watcher_createConnect(JNIEnv *env, jobject instance) {
    int sockfd;
    struct sockaddr_un addr;
    while (1){
        sockfd= socket(AF_LOCAL,SOCK_STREAM,0);
      if(sockfd <0){
          LOGE("create socket fail");
          return;
      }
      memset(&addr,0, sizeof(sockaddr_un));
      addr.sun_family = AF_LOCAL;
      strcpy(addr.sun_path,PATH);
      if(connect(sockfd,(struct sockaddr*)&addr, sizeof(sockaddr_un))<0){
          close(sockfd);
          sleep(1);
          LOGE("connect socket fail");
          continue;
      }
      break;
  }

}
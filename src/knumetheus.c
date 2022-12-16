
#include "str_helper.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#define PORTNUM 8080
int server_fd = 0;
#define MAX_RESPONSE_SIZE 1024 * 10
char* html_content = NULL;
char* js_content = NULL;
void close_server_socket()
{
    free(html_content);
    free(js_content);
    printf("\nEnd KNU-metheus. Bye..!\r\n"); 
    close(server_fd);
}

int main(int ac, char*av[])
{
    signal(SIGINT, close_server_socket);
    char http_response[MAX_RESPONSE_SIZE];

    html_content = read_str_from_path("res/knufana.html");
    js_content = read_str_from_path("res/knufana.js");

    struct sockaddr_in server_addr;
    int client_fd;
    int recv_len;
    char buffer[1024];
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
        oops("socket");


    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORTNUM);
    server_addr.sin_family = AF_INET;
   

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
        oops("bind");
    printf("Started KNU-metheus! Open browser and Access http://127.0.0.1:8080\r\n");    
    printf("To end, Press Ctrl + C.\r\n"); 
    if(listen(server_fd, 50) != 0)
        oops("listen");
    while(1) {
        
        client_fd = accept(server_fd, NULL, NULL);
        if(client_fd == -1)
        {
            break;
        }

        //Get 뭐시기하는 HTTP 요청을 버퍼로 받는다.
        recv_len = recv(client_fd, buffer, 1024, 0);
        if(recv_len < 0)
        {
            perror("recv");
            break;
        }
        buffer[recv_len] = '\0';
        //Grafana랑 Prometheus의 경우는
        //HTML로 이쁘게 그려서 HTML로 보내주는 Grafana랑
        //파싱하기 쉽게 JSON 문자열 같은 거 주는 Prometheus가 구분 되어 있는데
        //KNUFANA는 서버 하나만 관찰하면 되서 
        //한 서버 프로세스에서 
        //그림 요청과 값 요청을 받게 하는 식으로 만들었다.  

       memset(http_response,0,MAX_RESPONSE_SIZE);
        if(strncmp(buffer, "GET /", 5) == 0)
        {
            // raw information 요청의 경우는 이렇게 json형태로 반환
            if(strncmp(buffer, "GET /memory_usage",17) == 0)
            {
                char content[512];
                build_meminfo_json(content);
                append_static_http_response(http_response, content, CONTENT_JSON);
            }
            else if(strncmp(buffer, "GET /cpu_usage",14) == 0)
            {  
                char content[512];
                build_cpuusage_json(content);
                append_static_http_response(http_response, content, CONTENT_JSON);
            }
            else if(strncmp(buffer, "GET /net_usage",14) == 0)
            {  
                char content[512];
                build_netusage_json(content);
                append_static_http_response(http_response, content, CONTENT_JSON);
            }
            else if(strncmp(buffer, "GET /knufana.js",15) == 0)
            {  

                append_static_http_response(http_response, js_content, CONTENT_JS);
            }
            else  // 일반 요청의 경우 HTML
            {
                append_static_http_response(http_response, html_content,CONTENT_HTML);
            }
           
            send(client_fd, http_response, strlen(http_response), 0);
        }

        close(client_fd);       
    }

   
    return 0;
}



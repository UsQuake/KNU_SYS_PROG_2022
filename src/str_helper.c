#include "str_helper.h"

unsigned int get_char_count(const char* source)
{
	unsigned int input_str_len = 0;

	while (source[input_str_len] != 0)
	{
		input_str_len++;
	}
	return input_str_len;
}

void copy_str_deep(const char* source, char* destination, unsigned int length)
{
	for (unsigned int i = 0; i <= length; i++)
	{
		destination[i] = source[i];
	}
}

char* build_str(const char* initial_string)
{
		unsigned int input_str_len = get_char_count(initial_string);
		char* new_string = malloc(input_str_len + 1);
		if (new_string != NULL)
		{
			copy_str_deep(initial_string, new_string, input_str_len);
		}
		return new_string;
}

char* append(char* in_string, const char* s)
{
		unsigned int inputStringLength = get_char_count(s);
		unsigned int thisStringLength = get_char_count(in_string);
		if (inputStringLength == 0)
			return in_string;
		char* pNewString = malloc(thisStringLength + inputStringLength + 1);
		if (pNewString != NULL)
		{
			copy_str_deep(in_string, pNewString, thisStringLength);
			copy_str_deep(s, pNewString + thisStringLength, inputStringLength);
		}

		free(in_string);
		return pNewString;
}

char* read_str_from_path(const char* file_path)
{
    int file_fd;
    ssize_t  buf_size;
    char* file_content = build_str("");
    char file_buf[1024];

    file_fd = open(file_path, O_RDONLY);
    if(file_fd == -1)
        oops("file"); 
    while((buf_size = read(file_fd, file_buf, 1024)) > 0)
    {
        file_buf[buf_size] = '\0';

        file_content = append(file_content, file_buf);
    }
    close(file_fd);
    return file_content;
}

void read_limited_str_from_path(const char* file_path, char out_str[], unsigned int limit)
{
    int file_fd;
    ssize_t  buf_size;
    file_fd = open(file_path, O_RDONLY);
    if(file_fd == -1)
        oops("file"); 
    buf_size = read(file_fd, out_str, limit);
    out_str[buf_size] = '\0';
    close(file_fd);
}

char* build_http_response(const char* content, unsigned int return_type)
{
    char* http_response;
    switch(return_type)
    {
    case CONTENT_HTML:
        http_response = build_str("HTTP/2.0 200 OK\r\n""Content-Type: text/html\r\n\r\n");
        break;
    case CONTENT_JSON:
        http_response = build_str("HTTP/2.0 200 OK\r\n""Content-Type: application/json\r\n\r\n");
        break; 
    case CONTENT_JS:
        http_response = build_str("HTTP/2.0 200 OK\r\n""Content-Type: text/js\r\n\r\n");
        break;
    case CONTENT_CSS:
        http_response = build_str("HTTP/2.0 200 OK\r\n""Content-Type: text/css\r\n\r\n");
        break;
    default:
        http_response = build_str("HTTP/2.0 200 OK\r\n""Content-Type: text/plain\r\n\r\n");
        break;
    }
    http_response = append(http_response, content);
    return http_response;
}

void append_static_http_response(char out_response[], char content[], unsigned int return_type)
{

    switch(return_type)
    {
    case CONTENT_HTML:
        sprintf(out_response, "%s%s", "HTTP/2.0 200 OK\r\n""Content-Type: text/html\r\n\r\n", content);
        break;
    case CONTENT_JSON:
         sprintf(out_response,"%s%s", "HTTP/2.0 200 OK\r\n""Content-Type: application/json\r\n\r\n", content);
        break; 
    case CONTENT_JS:
         sprintf(out_response,"%s%s", "HTTP/2.0 200 OK\r\n""Content-Type: text/js\r\n\r\n", content);
        break; 
    case CONTENT_CSS:
         sprintf(out_response,"%s%s", "HTTP/2.0 200 OK\r\n""Content-Type: text/css\r\n\r\n", content);
        break; 
    default:
         sprintf(out_response,"%s%s", "HTTP/2.0 200 OK\r\n""Content-Type: text/plain\r\n\r\n", content);
         break;
    }
}

void build_meminfo_json(char out_json[])
{
    char raw_info[128];
    read_limited_str_from_path("/proc/meminfo", raw_info, 128);
    unsigned int memtotal, memfree;
    sscanf(strstr(raw_info, "MemTotal:") + strlen("MemTotal:"), "%u", &memtotal);
    sscanf(strstr(raw_info, "MemFree:") + strlen("MemFree:"), "%u", &memfree);
    sprintf(out_json, "{\r\n\t\"MemoryUsagePercent\" : %lf\r\n}\r\n", ((memtotal - memfree)/ (double)(memtotal)) * 100.0);
}

unsigned long prev_usr_mode = 0, prev_nice_usr_mode = 0, prev_sys_mode = 0, prev_idle_mode = 0;

void build_cpuusage_json(char out_json[])
{
    char raw_info[128];
    unsigned long usr_mode, nice_usr_mode, sys_mode, idle_mode;
    read_limited_str_from_path("/proc/stat", raw_info, 128);
    sscanf(strstr(raw_info, "cpu") + strlen("cpu"), "%lu%lu%lu%lu", &usr_mode, &nice_usr_mode, &sys_mode, &idle_mode);
   
    unsigned  long diff_user_jiffle = usr_mode - prev_usr_mode;
    unsigned  long diff_nice_user_jiffle = nice_usr_mode -  prev_nice_usr_mode;
    unsigned  long diff_system_jiffle = sys_mode - prev_sys_mode;
    unsigned long diff_idle_jiffle = idle_mode - prev_idle_mode;
    unsigned  long total_jiffle_diff =  diff_user_jiffle + diff_nice_user_jiffle + diff_system_jiffle + diff_idle_jiffle;
    double system_usage = (diff_system_jiffle / (double)(total_jiffle_diff)) * 100.0;
    double user_usage = ((diff_user_jiffle + diff_nice_user_jiffle)  / (double)(total_jiffle_diff)) * 100.0;
    double idle_free = (diff_idle_jiffle / (double)(total_jiffle_diff)) * 100.0;


    sprintf(out_json, "{\r\n\t\"SystemUsagePercent\" : %lf,\r\n\t\"UserUsagePercent\" : %lf,\r\n\t\"IdlePercent\" : %lf\r\n}\r\n", system_usage, user_usage, idle_free);
    
    prev_usr_mode = usr_mode; 
    prev_nice_usr_mode= nice_usr_mode;
    prev_sys_mode = sys_mode;
    prev_idle_mode = idle_mode;
}

unsigned long prev_rx_byte_sum = 0;
unsigned long prev_tx_byte_sum = 0;
void build_netusage_json(char out_json[])
{
    char* raw_info = read_str_from_path("/proc/net/dev");
    
    unsigned long rx_byte_sum = 0;
    unsigned long tx_byte_sum = 0;
    unsigned chr_iter = 200;
    unsigned int all_chr_cnt = strlen(raw_info);
    while(chr_iter <= all_chr_cnt)
    {
        unsigned line_chr_cnt = 0;
        char line[1024];
        while(*(raw_info + chr_iter + line_chr_cnt) != '\n')
            line_chr_cnt++;
        *(raw_info + chr_iter + line_chr_cnt)= '\0';
        strcpy(line, raw_info + chr_iter);
        char tmp[64];
        unsigned long rx_bytes = 0, rx_packets = 0, rx_errs, rx_drop = 0, rx_fifo = 0, rx_frame = 0, rx_compressed = 0, rx_multicast = 0;
        unsigned long tx_bytes = 0, tx_packets = 0, tx_errs, tx_drop = 0, tx_fifo = 0, tx_colls = 0, tx_carrier = 0, tx_compressed = 0;
            sscanf(line, "%s %lu%lu%lu%lu %lu%lu%lu%lu %lu%lu%lu%lu %lu%lu%lu%lu", tmp, 
    &rx_bytes, &rx_packets, &rx_errs, &rx_drop, &rx_fifo, &rx_frame, &rx_compressed, &rx_multicast,
    &tx_bytes, &tx_packets, &tx_errs, &tx_drop, &tx_fifo, &tx_colls, &tx_carrier, &tx_compressed);
        rx_byte_sum += rx_bytes;
        tx_byte_sum += tx_bytes;
        chr_iter += line_chr_cnt + 1;
    };
    sprintf(out_json, "{\r\n\t\"RxByteSum\" : %lu,\r\n\t\"TxByteSum\" : %lu\r\n}\r\n", rx_byte_sum - prev_rx_byte_sum, tx_byte_sum - prev_tx_byte_sum);
    prev_rx_byte_sum = rx_byte_sum;
    prev_tx_byte_sum = tx_byte_sum;
    free(raw_info); 
}

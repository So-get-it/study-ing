#ifndef  _SERIALPORT_H_
#define  _SERIALPORT_H_

#define SERIALNAME_LEN 128

typedef struct attr_s{
	int 		flow_ctrl;			//流控制
	int 		baud_rate;			//波特率
	int 		data_bits;			//数据位
	char 		parity;				//奇偶校验位
	int 		stop_bits;			//停止位
}attr_t;

extern int serial_open(char *fname);
extern int serial_close(int fd, struct termios *termios_p);
extern int serial_init(int fd, struct termios *oldtermios, attr_t *attr);
extern int serial_send(int fd, char *msg, int msg_len);
extern int serial_recv(int fd, char *recv_msg, int size);

#endif


#include "serial_port.h"
#include "logger.h"


int serial_open(char *fname)
{
    int fd, rv;

    if(NULL == fname)
    {
        log_error("%s,Invalid parameter\n",__func__);
        return -1;
    }

    if((fd = open(fname,O_RDWR|O_NOCTTY|O_NDELAY)) < 0)
    {
        log_error("Open %s failed: %s\n",fname, strerror(errno));

        return -1;
    }

    /* 判断串口的状态是否处于阻塞态 */
    if((rv = fcntl(fd, F_SETFL, 0)) < 0)
    {
        log_error("fcntl failed!\n");

        return -2;
    }
    
    /* 是否为终端设备 */
    if(0 == isatty(fd))  
    {
        log_error("%s:[%d] is not a Terminal equipment.\n", fname, fd);
        return -3;
    }
    
    log_info("Open %s successfully!\n", fname);

    return fd;
} 


int serial_close (int fd, struct termios *termios_p)
{
    /* 清空串口通信的缓冲区 */
    if(tcflush(fd,TCIOFLUSH))
    {
        log_error("%s, tcflush() fail: %s\n", __func__, strerror(errno));
        return -1;
    }

    /* 将串口设置为原有属性, 立即生效 */
    if(tcsetattr(fd,TCSANOW,termios_p))
    {
        log_error("%s, set old options fail: %s\n",__func__,strerror(errno));
        return -2;
    }

    close(fd);
    log_info("close OK..............\n");

    return 0;
} 

int serial_init(attr_t *attr, struct termios *oldtermios)
{
    char                  baudrate[32] = {0};
    struct termios        newtermios;

    memset(&newtermios,0,sizeof(struct termios));
    memset(oldtermios,0,sizeof(struct termios));

    if(!attr)
    {
        log_error("%s invalid parameter.\n", __func__);
        return -1;
    }

    /* 获取默认串口属性 */
    if(tcgetattr(attr->fd, oldtermios))
    {
        log_error("%s, get termios to oldtermios failure:%s\n",__func__,strerror(errno));
        return -2;
    }

    /* 先获取默认属性，后在此基础上修改 */
    if(tcgetattr(attr->fd, &newtermios))
    {
        log_error("%s, get termios to newtermios failure:%s\n",__func__,strerror(errno));
        return -3;
    }
    

    /* 修改控制模式，保证程序不会占用串口 */
    newtermios.c_cflag |= CLOCAL;


    /* 启动接收器，能够从串口中读取输入数据 */
    newtermios.c_cflag |= CREAD;

    /* 将\n转换成\r */
    newtermios.c_oflag |= ONLCR;


    /*
     * ICANON: 标准模式
     * ECHO: 回显所输入的字符
     * ECHOE: 如果同时设置了ICANON标志，ERASE字符删除前一个所输入的字符，WERASE删除前一个输入的单词
     * ISIG: 当接收到INTR/QUIT/SUSP/DSUSP字符，生成一个相应的信号
     *
     * 在原始模式下，串口输入数据是不经过处理的，在串口接口接收的数据被完整保留。
     
     * */
    newtermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);


    /*
     * BRKINT: BREAK将会丢弃输入和输出队列中的数据(flush)，并且如果终端为前台进程组的控制终端，则BREAK将会产生一个SIGINT信号发送到这个前台进程组
     * ICRNL: 将输入中的CR转换为NL
     * INPCK: 允许奇偶校验
     * ISTRIP: 剥离第8个bits
     * IXON: 允许输出端的XON/XOF流控
     *
     * */
    newtermios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);


    /* --------设置数据流控制------- */
    switch(attr->flow_ctrl)
    {
        case 0:                         //不使用流控制
            newtermios.c_cflag &=~CRTSCTS;
            break;

        case 1:                         //使用硬件流控制
            newtermios.c_cflag |= CRTSCTS;
            break;

        case 2:                         //使用软件流控制
            newtermios.c_cflag |= IXON| IXOFF|IXANY;
            break;

        default:
            newtermios.c_cflag &=~CRTSCTS;
            break;
    }


    /* 设置波特率，否则默认设置其为B115200 */
    if(attr->baud_rate)
    {
        sprintf(baudrate,"B%d",attr->baud_rate);

        cfsetispeed(&newtermios, (int)baudrate); //设置输入输出波特率
        cfsetospeed(&newtermios, (int)baudrate);
    }
    else
    {
        cfsetispeed(&newtermios, B115200);
        cfsetospeed(&newtermios, B115200);
    }


    /* ------设置数据位-------*/
    newtermios.c_cflag &= ~CSIZE;   //先把数据位清零，然后再设置新的数据位

    switch(attr->data_bits)
    {
        case '5':
            newtermios.c_cflag |= CS5;
            break;

        case '6':
            newtermios.c_cflag |= CS6;
            break;

        case '7':
            newtermios.c_cflag |= CS7;
            break;

        case '8':
            newtermios.c_cflag |= CS8;
            break;

        default:
            newtermios.c_cflag |= CS8;  //默认数据位为8
            break;
    }

    /* -------设置校验方式------- */
    switch(attr->parity)
    {
        /* 无校验 */
        case 'n':
        case 'N':
            newtermios.c_cflag &= ~PARENB;
            newtermios.c_iflag &= ~INPCK;
            break;

        /* 偶校验 */
        case 'e':
        case 'E':
            newtermios.c_cflag |= PARENB;
            newtermios.c_cflag &= ~PARODD;
            newtermios.c_iflag |= INPCK;
            break;

        /* 奇校验 */
        case 'o':
        case 'O':
            newtermios.c_cflag |= (PARODD | PARENB);
            newtermios.c_iflag |= INPCK;

        /* 设置为空格 */
        case 's':
        case 'S':
            newtermios.c_cflag &= ~PARENB;
            newtermios.c_cflag &= ~CSTOPB;

        /* 默认无校验 */
        default:
            newtermios.c_cflag &= ~PARENB;
            newtermios.c_iflag &= ~INPCK;
            break;

    }

    /* -------设置停止位-------- */
    switch(attr->stop_bits)
    {
        case '1':
            newtermios.c_cflag &= ~CSTOPB;
            break;

        case '2':
            newtermios.c_cflag |= CSTOPB;
            break;

        default:
            newtermios.c_cflag &= ~CSTOPB;
            break;
    }

    /* OPOST: 表示处理后输出，按照原始数据输出 */
    newtermios.c_oflag &= ~(OPOST);


    newtermios.c_cc[VTIME] = 0;  //最长等待时间
    newtermios.c_cc[VMIN] = 0;  //最小接收字符

    //attr->mSend_Len = 128;  //若命令长度大于mSend_Len,则每次最多发送为mSend_Len

    /* 刷新串口缓冲区 / 如果发生数据溢出，接收数据，但是不再读取*/
    if(tcflush(attr->fd,TCIFLUSH))
    {
        log_error("%s, clear the cache failure:%s\n", __func__, strerror(errno));
        return -4;
    }

    /* 设置串口属性，立刻生效 */
    if(tcsetattr(attr->fd,TCSANOW,&newtermios) != 0)
    {
        log_error("%s, tcsetattr failure: %s\n", __func__, strerror(errno));
        return -5;
    }

    log_info("Serial port Init Successfully!\n");

    return 0;
} 

int serial_send (int fd, char *msg, int msg_len)
{
    int rv = 0;
    
    rv = write(fd, msg, msg_len);
    if(rv == msg_len)
    {
        return rv;
    }
    else
    {
        tcflush(fd, TCOFLUSH);
        log_error("%s write message failure: %s\n", __func__, strerror(errno));

        return -1;
    }

    return rv;
} 

int serial_recv(int fd, char *buf, int size)
{
    int                 len, rv;
    int                 flag = 0;
    char                save[256] = {0};
    fd_set              rdset;

    struct timeval      time;

    memset(buf, 0, size);

    while(!flag)
    {
        FD_ZERO(&rdset);
        FD_SET(fd, &rdset);

        time.tv_sec= 10;
        time.tv_usec= 0;

        //使用select实现串口的多路通信
        rv = select(fd+1,&rdset,NULL,NULL,&time);

        rv = read(fd, save, size);
        if(rv)
        {
            if(strstr(save, "OK") || strstr(save, "ERROR"))
            {
                snprintf(buf, size, "%s%s", buf, save);
                log_info("Receive: %s\n", buf);

                flag = 1;
            }
            else
            {
                snprintf(buf, size, "%s%s", buf, save);
            }
        }
        else
        {
            log_error("%s read message failure: %s\n", __func__, strerror(errno));
            return -1;
        }

    }
    return rv;
} 

